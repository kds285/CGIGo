#include "WeichiThreadState.h"
#include <boost/format.hpp>
#include "UctChildIterator.h"
#include "arguments.h"
#include "WeichiPlayoutAgent.h"

void WeichiThreadState::resetThreadState()
{
	BaseThreadState<WeichiMove, WeichiPlayoutResult, WeichiUctNode>::resetThreadState();
	m_moves.clear();
	m_keys.clear();
	m_nSelectCNN = 0;
	m_board.reset();
	WeichiGlobalInfo::getSearchInfo().m_calTerritoryStat = false;
	// clear up board
	m_rootTurn = COLOR_BLACK ;
	m_ht.clear();

	if( WeichiConfigure::mcts_use_rave ) { clearUpdateRaveTable(); }
}

void WeichiThreadState::backup ( )
{
	BaseThreadState<WeichiMove, WeichiPlayoutResult, WeichiUctNode>::backup();
	m_board.setBackupFlag();
	m_moves.clear();
	m_keys.clear();
	m_nSelectCNN = 0;
	if( WeichiConfigure::mcts_use_rave ) { clearUpdateRaveTable(); }

	m_rootTurn = m_board.getToPlay();
	if ( WeichiConfigure::ResetSeedInBackup ) {
		Random::reset(Configure::RandomSeed) ;
	}
	/// TODO: clear some data for real move made
}

void WeichiThreadState::rollback ( )
{
	BaseThreadState<WeichiMove, WeichiPlayoutResult, WeichiUctNode>::rollback();
	m_moves.clear();
	m_nSelectCNN = 0;
	if( WeichiConfigure::mcts_use_rave ) { clearUpdateRaveTable(); }

	while ( !m_keys.empty() ) {		
		HashKey64 key = m_keys.back();
		m_ht.erase(key) ;
		m_keys.pop_back();
	}

#if DO_WINDOWS_TIMER
	m_timer.timeElapsed += m_board.getMicrosecondTimer().getTimerTotalMicroSeconds();
	m_timer.counter += m_board.getMicrosecondTimer().getTimerTotalCounts() ;
#endif
	m_board.restoreBackupFlag();
} 


void WeichiThreadState::startSimulation( int sim_count )
{
	if( WeichiConfigure::mcts_use_rave ) {
		if( (WeichiConfigure::RandomizeRaveFequency>0) && (sim_count%WeichiConfigure::RandomizeRaveFequency==0) ) { m_useRave = false; }
		else { m_useRave = true; }
	}
}

void WeichiThreadState::endPlayout()
{
	m_board.setPlayoutStatus(false);
}

void WeichiThreadState::setThreadID( uint threadID, uint default_gpu_device/*=WeichiConfigure::dcnn_default_gpu_device*/ )
{
	m_thread_id = threadID;
	m_nSelectCNN = 0;

	if( !isFirstThread() ) {
		vector<string> vParamString = splitToVector(WeichiConfigure::dcnn_net,':');
		m_dcnnNetCollection.initialize(threadID,vParamString);
	}
}

double WeichiThreadState::startPreSimulation( uint numPreSimulation/*=64*/, Territory& territory/*=WeichiInfo::getSearchInfo().m_territory*/ )
{
	m_board.setPreSimulationSataus(true);

	territory.clear();
	m_rootFilter.startFilter();
	backup();

	WeichiPlayoutAgent agent(*this);
	
	int iBlackWins = 0;
	for( uint times=0; times<numPreSimulation; times++ ) {
		startPlayout();
		WeichiPlayoutResult result = agent.run();

		if( result.getWinner()==COLOR_BLACK ) { iBlackWins++; }
		territory.addTerritory(m_board.getTerritory(),result.getWinner());
		endPlayout();
		rollback();
	}

	m_board.setPreSimulationSataus(false);

	return ((double)iBlackWins/numPreSimulation);
}

void WeichiThreadState::endPreSimulation()
{
}

void WeichiThreadState::startPlayout()
{
	if( static_cast<int>(m_moves.size())>WeichiGlobalInfo::getTreeInfo().m_nMaxTreeHeight ) {
		WeichiGlobalInfo::getTreeInfo().m_nMaxTreeHeight = static_cast<int>(m_moves.size());
	}
	WeichiGlobalInfo::getTreeInfo().m_treeHeight.add(m_moves.size());
	m_board.setPlayoutStatus(true);
}

void WeichiThreadState::startExpansion()
{
	m_board.setExpansionSataus(true);
}

void WeichiThreadState::endExpansion()
{
	m_board.setExpansionSataus(false);
}

void WeichiThreadState::endSimulation()
{
}

void WeichiThreadState::playoutPolicyGradientUpdate( WeichiPlayoutResult result, double dBaseLineB )
{
	/*const double dAlpha0 = 0.01;
	const double dLamda = 0.001;
	const double dAlpha = (dAlpha0/(1+m_path[0]->getUctData().getCount()*dLamda*dAlpha0));
	const WeichiProbabilityPlayoutHandler& probabilityHandler = m_board.m_probabilityHandler;
	
	Dual<double> dResult;
	if( result.getWinner()==COLOR_BLACK ) { dResult.black = 1.0f-dBaseLineB; dResult.white = 0.0f-(1.0f-dBaseLineB); }
	else if( result.getWinner()==COLOR_WHITE ) { dResult.white = 1.0f-(1.0f-dBaseLineB); dResult.black = 0.0f-dBaseLineB; }
	else { dResult.black = 0.5f-dBaseLineB; dResult.white = 0.5f-(1.0f-dBaseLineB); }

	const Vector<ProbabilityGradient,MAX_NUM_GRIDS>& vBlackProbGradient = probabilityHandler.getProbGradient(COLOR_BLACK);
	const Vector<ProbabilityGradient,MAX_NUM_GRIDS>& vWhiteProbGradient = probabilityHandler.getProbGradient(COLOR_WHITE);
	for( StaticBoard::iterator it=StaticBoard::getIterator(); it; ++it ) {
		const ProbabilityGradient& blackGradient = vBlackProbGradient[*it];
		for( int iIndex=0; iIndex<blackGradient.getNumGradientPolicy(); iIndex++ ) {
			const int iFeature = blackGradient.getGradientPolicyIndex(iIndex);
			double dGradient = blackGradient.getGradientPolicy(iIndex);
			double dOnlineFeatureValue = WeichiGammaTable::getOnlinePlayoutFeatureValue(*it,iFeature,COLOR_BLACK);
			dOnlineFeatureValue += dAlpha * (dResult.black*dGradient-dLamda*dOnlineFeatureValue);
			WeichiGammaTable::setOnlinePlayoutFeatureValue(*it,iFeature,COLOR_BLACK,dOnlineFeatureValue);
		}

		const ProbabilityGradient& whiteGradient = vWhiteProbGradient[*it];
		for( int iIndex=0; iIndex<whiteGradient.getNumGradientPolicy(); iIndex++ ) {
			const int iFeature = whiteGradient.getGradientPolicyIndex(iIndex);
			double dGradient = whiteGradient.getGradientPolicy(iIndex);
			double dOnlineFeatureValue = WeichiGammaTable::getOnlinePlayoutFeatureValue(*it,iFeature,COLOR_WHITE);
			dOnlineFeatureValue += dAlpha * (dResult.white*dGradient-dLamda*dOnlineFeatureValue);
			WeichiGammaTable::setOnlinePlayoutFeatureValue(*it,iFeature,COLOR_WHITE,dOnlineFeatureValue);
		}
	}*/
}

void WeichiThreadState::summarize( UctNodePtr root, float seconds, uint simulateCount, bool period/*=true*/, WeichiMove move/*=WeichiMove()*/ ) const
{
	WeichiGlobalInfo::getTreeInfo().m_pRoot = root;
	WeichiGlobalInfo::getTreeInfo().m_dThinkTime = seconds;
	WeichiGlobalInfo::getTreeInfo().m_nSimulation = simulateCount;

	UctNodePtr best = UctNodePtr::NULL_PTR ;
	if( root->hasChildren() ) {
		UctChildIterator<WeichiUctNode> it (root) ;
		best = it ;
		for ( ++it ; it ; ++it ) {
			if ( it->getUctData().getCount() > best->getUctData().getCount() ) {
				best = it ;			
			}
		}

		if( WeichiConfigure::LiveGfx ) {
			cerr << "gogui-gfx:\n";

			//display best child
			cerr << "VAR ";
			cerr << (best->getColor() == COLOR_BLACK ? "B" : "W") << ' ';
			cerr << best->getMove().toGtpString();
			cerr << '\n';

			// display count
			cerr << "LABEL";
			UctChildIterator<WeichiUctNode> it ( root ) ;
			for ( ; it ; ++it ) {
				WeichiUctNode& child = *it;
				cerr << ' ' << child.getMove().toGtpString();
				cerr << ' ' << child.getUctData().getCount();
			}
			cerr << '\n';
			cerr << '\n';
		}
	}

	if( arguments::mode != "jlmcts" ) {
		if( !period ) {
			if( WeichiConfigure::DynamicTimeControl ) { WeichiGlobalInfo::getTreeInfo().m_dRemainTime.get(move.getColor()) -= seconds; }
		} else {
			if ( best.isValid() ) {
				CERR() << boost::format("[%1%:%2$02d] best: %3% (%4$.3f/%5%), speed: %6$.2f\n") 
						% ((int)(seconds)/60) % ((int)(seconds)%60)
						% (best->getMove().toGtpString()) % (best->getUctData().getMean()) % (best->getUctData().getCount()) 
						% (simulateCount/seconds) ;

				CERR() << WeichiGlobalInfo::get()->getBestSequenceString() << endl;
			} else {
				CERR() << boost::format("[%1%:%2%] (no child), speed: %3$.2f\n") 
						% ((int)(seconds)/60) % ((int)(seconds)%60)
						% (simulateCount/seconds) ;
			}
		}
		//CERR() << WeichiGlobalInfo::getTreeInfo().getTreeNodeDetail();
	}
}

bool WeichiThreadState::play( WeichiMove move, bool real_game /*= false */ )
{
	if ( real_game ) {
		if ( m_board.isIllegalMove(move, m_ht) ) {
			return false ;
		}
		m_rootTurn = AgainstColor(move.getColor());
	}

	m_moves.push_back(move) ;
	if( WeichiConfigure::mcts_use_rave ) { m_shouldUpdateRaveMoves[move.getPosition()] = checkForUpdateRaveMove(move); }
    
	bool res = m_board.play(move) ; 
	assert ( res ) ; 

	HashKey64 key = m_board.getHash();
	if ( !move.isPass() ) {
		m_keys.push_back(key) ; 
		m_ht.store(key) ;
	}
	else if( WeichiConfigure::superko_rule == "situational" ) { 
		// TO DO: How to use Natural Situational Superko? Not used!		
		// 若是 Situational 才要對 pass 做處理, pass 後, 如果 hashkey 已存在就不用儲存
		// 解決盤末連續pass兩手的問題以及 Pass 後盤面之前有出現過的問題(Pass 一定是 legal)
		// 之前沒出現過就存
		if( !m_ht.lookup(key) ) {
			m_keys.push_back(key) ; 			
			m_ht.store(key) ;
		}
	}

	if( real_game && isFirstThread() ) { WeichiGlobalInfo::getTreeInfo().m_nMove++; }
	return true ;
}

void WeichiThreadState::clearUpdateRaveTable()
{
	for( StaticBoard::iterator it=StaticBoard::getIterator(); it; ++it ) {
		m_shouldUpdateRaveMoves[*it] = false;
	}
}

bool WeichiThreadState::checkForUpdateRaveMove( const WeichiMove move )
{
	return true;
	/*bool shouldUpdateRave = true;

	assertToFile( move.getColor()!=COLOR_NONE, const_cast<WeichiBoard*>(&m_board) );

	WeichiMove opponentMove( AgainstColor(move.getColor()), move.getPosition() );
	bool isLegalForOpponent = !m_board.isIllegalMove(opponentMove, m_ht);
	if ( !isLegalForOpponent )
		shouldUpdateRave = false;

	return shouldUpdateRave;*/
}

std::string WeichiThreadState::getDeadStones() const
{
	const Territory& territory = WeichiGlobalInfo::getSearchInfo().m_territory;
	ostringstream oss ;

	for( StaticBoard::iterator it=StaticBoard::getIterator(); it; ++it ) {
		Color c = m_board.getGrid(*it).getColor();
		double prob = territory.getTerritory(*it) ;
		if ( prob >  WeichiConfigure::TerritoryThreshold && c == COLOR_WHITE )
			oss << WeichiMove(c, *it).toGtpString() << ' ';
		if ( prob < -WeichiConfigure::TerritoryThreshold && c == COLOR_BLACK )
			oss << WeichiMove(c, *it).toGtpString() << ' ';
	}

	return oss.str();
}