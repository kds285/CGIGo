#ifndef WEICHIUCTACCESSOR_H
#define WEICHIUCTACCESSOR_H

#include "WeichiMove.h"
#include "WeichiPlayoutResult.h"
#include "WeichiUctNode.h"
#include "WeichiThreadState.h"
#include "BaseUctAccessor.h"
#include "WeichiBoard.h"
#include "WeichiUctCNNHandler.h"

class WeichiUctAccessor : public BaseUctAccessor<WeichiMove, WeichiPlayoutResult, WeichiUctNode, WeichiThreadState>
{
    typedef NodePtr<WeichiUctNode> UctNodePtr ;
	typedef int DTIndex;
private:
	WeichiUctCNNHandler m_uctCNNHandler;
	Vector<int, MAX_NUM_GRIDS> m_blacks, m_whites;

public:
	void doUctCNNJob() { m_uctCNNHandler.doUctCNNJob(); }
	std::string runRemoteJob(WeichiCNNNetType type, const vector<CNNJob>& vJobs) { return m_uctCNNHandler.runRemoteJob(type, vJobs); }
	void updateAll( const WeichiRemoteCNNJob& currentJob );

	WeichiUctAccessor ( WeichiThreadState& state, node_manage::NodeAllocator<WeichiUctNode>& na ) ;
	UctNodePtr selectNode();
    void update ( WeichiPlayoutResult result ) ;
	void beforeChangeTree()
	{
		m_uctCNNHandler.putBackAllToQueue();
		m_state.rollback();
	}

	void setRoot ( UctNodePtr pRoot )
	{
		BaseUctAccessor::setRoot(pRoot);
		m_uctCNNHandler.setRoot(pRoot);
	}

protected:
    virtual bool expandChildren ( UctNodePtr pNode ) ;
    virtual UctNodePtr selectChild ( UctNodePtr pNode ) ;

private:
    void collectMovePlayed () ;
	void ignoreMoves(UctNodePtr node, const Vector<CandidateEntry,MAX_NUM_GRIDS>& vUctCandidateList) ;
	void updateCache( WeichiPlayoutResult result );

	// for queue
	bool isQueueHaveSpace() {
		if (WeichiGlobalInfo::getCNNJobQueue().getUnfinishedJob() >= WeichiConfigure::dcnn_max_queue_size) {
			boost::this_thread::sleep(boost::posix_time::microseconds(100));
			return false;
		}

		return true;
	}

	bool addToCNNJobQueue(UctNodePtr pNode, const CNNJob& job)
	{
		if (!WeichiConfigure::use_playout) {
			if (!isQueueHaveSpace() || pNode->isInCNNQueue()) { return false; }
		}

		pNode->setInCNNQueue();
		WeichiGlobalInfo::getCNNJobQueue().addJob(job);
		return true;
	}

	/*
		mcts selection technique function, include the following:
			* virtual loss
			* score with rave / value network / weighted ucb
			* progressive widening
			* progressive bias
			* UCB
			* PUCT
			* replace move by early pass
	*/
	inline int calChildSimulationCount( UctNodePtr pNode ) {
		int iTotalCount = 1.0f;
		for( UctChildIterator<WeichiUctNode> it(pNode); it; ++it ) {
			iTotalCount += it->getUctData().getCount();
		}
		return iTotalCount;
	}
	inline void addVirtualLoss( UctNodePtr pNode ) {
		if( !WeichiConfigure::use_virtual_loss ) { return; }
		pNode->addVirtualLoss(WeichiConfigure::virtual_loss_count);
	}
	inline void removeVirutalLoss( UctNodePtr pNode ) {
		if( !WeichiConfigure::use_virtual_loss ) { return; }
		pNode->removeVirtualLoss(WeichiConfigure::virtual_loss_count);
	}
	inline double getMoveScore(const WeichiUctNode& node, const double dBaselineValue) {
		double dScore = 0.0f;
		
		dScore = (node.getUctData().getCount() > 0) ? node.getUctData().getMean() : dBaselineValue;
		//dScore = (node.getUctData().getCount() > 0) ? node.getUctData().getMean() : 0.0f;

		// mixed with value network if using playout
		if (WeichiConfigure::use_playout) {
			const double dAlpha = 0.5f;
			double dVNValue = node.hasVNValue() ? node.getVNValueData().getMean() : 0.5f;
			dScore = dScore*dAlpha + dVNValue*(1 - dAlpha);
		}

		if( WeichiConfigure::mcts_use_rave && m_state.m_useRave ) {
			const StatisticData& RaveData = node.getRaveData();
			double raveCnt = (double)RaveData.getCount(), uctCnt = node.getUctData().getCount();
			double raveWeight = (double)(raveCnt / (raveCnt+uctCnt+(raveCnt*uctCnt/3000.0f)));
			dScore = (double)(raveWeight*RaveData.getMean() + (1-raveWeight)*dScore);
		}

		return dScore;
	}
	inline bool isInWideningRange( const double dTotalCount, const int childIndex ) {
		if( !WeichiConfigure::mcts_use_progressive_widening ) { return true; }
		return dTotalCount>=StaticBoard::getWideningIndex(childIndex);
	}
	inline double getUcbBias( const double dLogTotalCount, const StatisticData& uctData ) {
		if( !WeichiConfigure::mcts_use_ucb ) { return 0; }
		return Configure::UCBWeight*sqrt(dLogTotalCount/uctData.getCount());
	}
	inline double getProgressiveBias( const WeichiUctNode& node ) {
		if( !WeichiConfigure::mcts_use_progressive_bias ) { return 0; }
		return 2*node.getProb()*sqrt(0.06/(600+node.getUctData().getCount()));
	}
	inline double getPUctBias( const int iTotalCount, const WeichiUctNode& node ) {
		if( !WeichiConfigure::mcts_use_puct ) { return 0; }
		double dPUCTBias = log((1 + iTotalCount + 19652) / 19652) + WeichiConfigure::mcts_puct_bias;
		return dPUCTBias*node.getProb()*sqrt(iTotalCount) / (1 + node.getUctData().getCount());
	}
	inline bool canEarlyPass( UctNodePtr pParent, UctNodePtr pBest, UctNodePtr pPass ) {
		if ( !WeichiConfigure::EarlyPass ) { return false; }
		if ( m_pRoot != pParent ) { return false; }
		if ( !pPass.isValid() ) { return false; }

		// Case 1: Winning, make sure it's still win after play pass
		if ( pBest->getUctData().getMean() > WeichiConfigure::EarlyPassWinrateThreshold &&
			 pBest->getUctData().getCount() > WeichiConfigure::EarlyPassCountThreshold )
		{
			StatisticData passUctData = pPass->getUctData();
			passUctData.remove(0, pPass->getVirtualLossCount());
			if ( passUctData.getCount() < WeichiConfigure::EarlyPassCountThreshold ||
				 passUctData.getMean() > WeichiConfigure::EarlyPassWinrateThreshold )
			{
				return true;
			}
		}

		// Case 2: Losing, collect pass territory to determine early pass
		if ( pBest->getUctData().getMean() < WeichiConfigure::ResignWinrateThreshold &&
			 pBest->getUctData().getCount() > WeichiConfigure::ResignCountThreshold )
		{
			StatisticData passUctData = pPass->getUctData();
			passUctData.remove(0, pPass->getVirtualLossCount());
			if ( passUctData.getCount() < WeichiConfigure::EarlyPassCountThreshold ) { return true; }
		}

		return false;
	}
};

#endif //WEICHIUCTACCESSOR_H
