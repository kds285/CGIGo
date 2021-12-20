#include "WeichiUctAccessor.h"
#include "WeichiMoveDecisionMaker.h"
#include "WeichiJLMctsHandler.h"
#include "arguments.h"
#include <iostream>
#include <boost/asio.hpp>

using namespace std;

WeichiUctAccessor::WeichiUctAccessor( WeichiThreadState& state, node_manage::NodeAllocator<WeichiUctNode>& na )
	: BaseUctAccessor<WeichiMove, WeichiPlayoutResult, WeichiUctNode, WeichiThreadState> ( state, na )
	, m_uctCNNHandler(state,na)
{
}

void WeichiUctAccessor::updateAll( const WeichiRemoteCNNJob& currentJob )
{
	m_uctCNNHandler.updateAll(currentJob);
}

WeichiUctAccessor::UctNodePtr WeichiUctAccessor::selectNode()
{
	UctNodePtr pNode = m_pRoot;
	if (!isQueueHaveSpace()) { return pNode; }

	Vector<WeichiMove, MAX_TREE_DEPTH> vPaths;
	m_state.m_path.push_back(pNode);

	// selection
	int MMDeep = 0;
	while (pNode->hasChildren()) {
		pNode = selectChild(pNode);
		addVirtualLoss(pNode);
		m_state.m_path.push_back(pNode);
		vPaths.push_back(pNode->getMove());
		m_state.play(pNode->getMove());

		if (!pNode->isCNNNode()) { MMDeep++; }
	}

	// expansion
	if (!m_state.isTerminal()) {
		bool bSuccess = false;
		if (pNode->getExpandToken()) {
			const int MAX_MM_DEEP = 1;
			bSuccess = (expandChildren(pNode) || (WeichiConfigure::use_playout && MMDeep <= MAX_MM_DEEP));
			if (bSuccess) {
				bSuccess = addToCNNJobQueue(pNode, CNNJob(vPaths));

				if (WeichiConfigure::use_playout) {
					// select to next child
					pNode = selectChild(pNode);
					addVirtualLoss(pNode);
					m_state.m_path.push_back(pNode);
					vPaths.push_back(pNode->getMove());
					m_state.play(pNode->getMove());
				}
			}
			pNode->releaseExpandToken();
		}

		if (!bSuccess) {
			for (uint i = 1; i < m_state.m_path.size(); ++i) { removeVirutalLoss(m_state.m_path[i]); }
		}
	}

	return pNode;
}

WeichiUctAccessor::UctNodePtr WeichiUctAccessor::selectChild( UctNodePtr pNode )
{
	assert(pNode->hasChildren());

	double dBestScore = -DBL_MAX;
	UctNodePtr pBest = UctNodePtr::NULL_PTR;
	//UctNodePtr pPass = UctNodePtr::NULL_PTR;
	int iTotalCount = calChildSimulationCount(pNode);
	double dLogTotalCount = WeichiConfigure::mcts_use_ucb? log(iTotalCount): 0.0f;

	uint childIndex = 0;
	uint bestChildIndex = 0;
	float fSumOfChildWins = 0.0f;
	float fSumOfChildSims = 0.0f;
	for( UctChildIterator<WeichiUctNode> it(pNode); it; ++it ) {
		WeichiUctNode& child = *it;
		StatisticData UctData = child.getUctData();

		/*if( child.getMove().isPass() ) {
			if( !pPass.isValid() ) { pPass = it; }
			continue;
		}*/
		if( !isInWideningRange(iTotalCount,childIndex) ) { break; }

		// calculate move score & add bias
		//double dMoveScore = getMoveScore(child);
		double dAdjustChildVNValue = pNode->getAdjustChildVNValue();
		double dMoveScore = getMoveScore(child, dAdjustChildVNValue);
		dMoveScore += getUcbBias(dLogTotalCount,UctData);
		dMoveScore += getProgressiveBias(child);
		dMoveScore += getPUctBias(iTotalCount,child);

		if (child.hasChildren()) {
			fSumOfChildWins += child.getUctData().getMean();
			fSumOfChildSims += 1;
		}

		if( dMoveScore>dBestScore ) {
			dBestScore = dMoveScore;
			if( bestChildIndex < childIndex+1 ) { bestChildIndex = childIndex; }
			pBest = it;
		}

		childIndex++;
	}

	if( WeichiGlobalInfo::getTreeInfo().m_nMaxChildIndex < bestChildIndex ) {
		WeichiGlobalInfo::getTreeInfo().m_nMaxChildIndex = bestChildIndex;
	}
	WeichiGlobalInfo::getTreeInfo().m_childIndex[bestChildIndex]++;

	float fAdjustChildVNValue = fSumOfChildWins / (fSumOfChildSims + 1);
	pNode->setAdjustChildVNValue(fAdjustChildVNValue);

	//if (canEarlyPass(pNode, pBest, pPass)) { pBest = pPass; }
	//if( pBest.isNull() ) { pBest = pPass; }

	assertToFile( pBest.isValid(), const_cast<WeichiBoard*>(&m_state.m_board) );

	return pBest;
}

bool WeichiUctAccessor::expandChildren( UctNodePtr pNode )
{
	if (WeichiConfigure::use_playout && pNode->getUctData().getCount() < Configure::ExpandThreshold) { return false; }

	m_state.startExpansion();

	if (WeichiConfigure::use_playout) {
		// for MM
		Color turnColor = m_state.m_board.getToPlay();
		m_state.m_moveFeatureHandler.calculateFullBoardCandidates(turnColor);
		const Vector<CandidateEntry, MAX_NUM_GRIDS>& vUctCandidateList = m_state.m_moveFeatureHandler.getUCTCandidateList(true);

		ignoreMoves(pNode, vUctCandidateList);

		uint size = (vUctCandidateList.size() > MAX_EXPAND_CHILD_NODE) ? MAX_EXPAND_CHILD_NODE : vUctCandidateList.size();
		assert(size != 0);

		// allocation child node && check allocation is fail
		UctNodePtr firstChild = m_nodeAllocator.allocateNodes(size);
		if (firstChild.isNull()) { return false; }

		UctChildIterator<WeichiUctNode> childIterator(firstChild, size);
		for (uint i = 0; i < size; i++) {
			WeichiMove move(turnColor, vUctCandidateList[i].getPosition());
			assertToFile(childIterator, const_cast<WeichiBoard*>(&m_state.m_board));
			assertToFile(!m_state.m_board.isIllegalMove(move, m_state.m_ht), const_cast<WeichiBoard*>(&m_state.m_board));

			childIterator->reset(move);
			childIterator->setProb(vUctCandidateList[i].getScore());

			++childIterator;
		}

		pNode->getUpdateToken();
		if (!pNode->hasChildren()) { pNode->setChildren(firstChild, size); }
		pNode->releaseUpdateToken();
	}

	m_state.endExpansion();

	return true;
}

void WeichiUctAccessor::update( WeichiPlayoutResult result )
{
	Color winner = result.getWinner();
	if (WeichiConfigure::use_playout || (!WeichiConfigure::use_playout && m_state.isTerminal())) {
		float fBonus = ((winner == COLOR_NONE) ? 0.0f : ((winner != m_state.getRootTurn()) ? 1.0f : -1.0f));
		for (uint i = 0; i < m_state.m_path.size(); ++i) {
			UctNodePtr pNode = m_state.m_path[i];
			if (i > 0) { removeVirutalLoss(pNode); }
			pNode->getUctData().add(fBonus, 1.0f);
			fBonus = -fBonus;
		}

		if (!WeichiConfigure::use_playout) { WeichiGlobalInfo::getTreeInfo().m_nDCNNJob++; }
	}
	if (!WeichiConfigure::use_playout) { return; }

	// update value network's value
	bool bFindValue = false;
	float fValue = 0.0f;
	for (int i = m_state.m_path.size() - 1; i >= 0; i--) {
		UctNodePtr pNode = m_state.m_path[i];

		if (!bFindValue) {
			if (!pNode->isCNNNode()) { continue; }
			else {
				bFindValue = true;
				fValue = pNode->getCNNVNValue();
			}
		}

		pNode->getVNValueData().add(fValue);
		fValue = 1 - fValue;
	}

	WeichiGlobalInfo::getTreeInfo().m_nHandMove += m_state.m_moves.size();
	WeichiGlobalInfo::getSearchInfo().m_territory.addTerritory(m_state.m_board.getTerritory(), winner);
	if (m_state.m_path.size() > 1 && m_state.m_path[1]->getMove().isPass()) { WeichiGlobalInfo::getSearchInfo().m_passTerritory.addTerritory(m_state.m_board.getTerritory(), winner); }

	if( WeichiConfigure::DoPlayoutCacheLog ) { WeichiGlobalInfo::getLogInfo().m_cacheLogger.writeCacheLogFile(m_state.m_board,m_state.m_board.getTerritory()); }

	updateCache(result);
}

void WeichiUctAccessor::collectMovePlayed()
{
	Vector<WeichiMove, MAX_GAME_LENGTH>& moves = m_state.m_moves;
	m_blacks.setAllAs(-1, MAX_NUM_GRIDS);
	m_whites.setAllAs(-1, MAX_NUM_GRIDS);

	for ( uint i=0;i<moves.size();++i ) {
		const WeichiMove & m = moves[i];

		Vector<int, MAX_NUM_GRIDS>& mylist = ( m.getColor()==COLOR_BLACK?m_blacks:m_whites );
		Vector<int, MAX_NUM_GRIDS>& opplist = ( m.getColor()==COLOR_BLACK?m_whites:m_blacks );
		uint pos = m.getPosition() ;
		if ( mylist[pos] == -1 && opplist[pos] == -1 )
			mylist[pos] = i ;
	}
}

void WeichiUctAccessor::ignoreMoves( UctNodePtr node , const Vector<CandidateEntry,MAX_NUM_GRIDS>& vUctCandidateList )
{
	if( arguments::mode != "jlmcts" ) return ;

	if( node == m_pRoot ) {
		string ignores = WeichiConfigure::Ignore ;		
		WeichiJLMctsHandler::transCoorJLMCTS(ignores) ;
		for( uint pos = 0 ; ignores.find_first_of("BW", pos) != string::npos ; pos += 5 ) {
			Color c = toColor(ignores[pos]) ;	
			WeichiMove m( c, ignores.substr(pos+2, 2) ) ;	
			for( uint i=0; i<vUctCandidateList.size(); i++ ) {
				if( m.getPosition() == vUctCandidateList[i].getPosition() ) {
					const_cast<Vector<CandidateEntry,MAX_NUM_GRIDS>&>(vUctCandidateList).erase_no_order(i) ;
				}
			}//end for			
		}//end for

		// 仍舊需要判斷一次
		if( vUctCandidateList.size() == 1 ) 
			WeichiGlobalInfo::get()->m_bIsLastCandidate = true ;

		if( vUctCandidateList.size() == 0 ) {
			const_cast<Vector<CandidateEntry,MAX_NUM_GRIDS>&>(vUctCandidateList).push_back(CandidateEntry(PASS_MOVE.getPosition(), 0.00001) ) ;
			WeichiGlobalInfo::get()->m_bIsLastCandidate = true ;
		}		
	}
}

void WeichiUctAccessor::updateCache( WeichiPlayoutResult result )
{
	/*Color winner = result.getWinner();

	const Vector<WeichiMoveCacheContext,MAX_GAME_LENGTH>& vCacheMoveSequences = m_state.m_vCacheMoveSequences;
	for( uint i=0; i<vCacheMoveSequences.size(); i++ ) {
		const WeichiMoveCacheContext& moveCacheContext = vCacheMoveSequences[i];
		const WeichiMove& move = moveCacheContext.m_replyMove;

		bool bIsWin = (winner==move.getColor());
		if( bIsWin ) {
			WeichiGlobalInfo::getSearchInfo().m_moveCache.insert(moveCacheContext);
		} else {
			//WeichiInfo::getSearchInfo().m_moveCache.remove(sequences.m_prevKey,sequences.m_context);
		}

		//WeichiInfo::getSearchInfo().m_moveCache.addPlayoutInfo(sequences.m_prevKey,sequences.m_context,bIsWin);
	}

	m_state.m_vCacheMoveSequences.clear();*/
}
