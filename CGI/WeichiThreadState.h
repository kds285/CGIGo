#ifndef WEICHITHREADSTATE_H
#define WEICHITHREADSTATE_H

#include "arguments.h"
#include "BaseThreadState.h"
#include "WeichiUctNode.h"
#include "WeichiPlayoutResult.h"
#include "WeichiBoard.h"
#include "HashKey64.h"
#include "WeichiGlobalInfo.h"
#include "WeichiRootFilter.h"
#include "WeichiKnowledgeBase.h"
#include "Territory.h"
#include "WeichiLGRTable.h"
#include "WeichiMiniSearch.h"
#include "WeichiMoveFeatureHandler.h"
#include "WeichiLocalSearchHandler.h"
#include "WeichiCNNFeatureGenerator.h"
#include "WeichiMoveCache.h"
#include "WeichiDynamicKomi.h"
#include "DCNNNetCollection.h"
#include <cmath>
#include <boost/asio.hpp>

class WeichiThreadState :
	public BaseThreadState<WeichiMove, WeichiPlayoutResult, WeichiUctNode>
{
private:
	typedef NodePtr<WeichiUctNode> UctNodePtr ;

public:
	Vector<WeichiMove, MAX_GAME_LENGTH> m_moves ;
	Vector<HashKey64, MAX_GAME_LENGTH> m_keys ;

	Vector<bool, MAX_NUM_GRIDS> m_shouldUpdateRaveMoves ;
	bool m_useRave;

	WeichiBoard m_board ;
	OpenAddrHashTable m_ht ;
	WeichiKnowledgeBase m_kb ;
	WeichiMiniSearch m_miniSearch ;
	WeichiMoveFeatureHandler m_moveFeatureHandler ;
	WeichiLocalSearchHandler m_localSearchHandler ;
	WeichiRootFilter m_rootFilter ;
	
	int m_nSelectCNN;

	Color m_rootTurn ;
	DCNNNetCollection m_dcnnNetCollection;

	struct Timer {
		string timerName;
		double timeElapsed;
		uint counter;
		Timer(){ timerName=""; timeElapsed=0.0; counter = 0; }
	} m_timer;

private:
	class UctNodeOrder {
	public:
		int m_order;
		UctNodePtr m_pNode;
		UctNodeOrder() {}
		UctNodeOrder( int order, UctNodePtr pNode )
		{
			m_order = order;
			m_pNode = pNode;
		}
		inline bool operator<( const UctNodeOrder& rhs ) const { return m_pNode->getUctData().getCount()>rhs.m_pNode->getUctData().getCount(); }
	};

public:
	WeichiThreadState ()
		: BaseThreadState()
		, m_kb(m_board, m_ht)
		, m_rootTurn(COLOR_BLACK)
		, m_miniSearch(m_board,m_ht)
		, m_rootFilter(m_board)
		, m_localSearchHandler(m_board,m_ht)
		, m_moveFeatureHandler(m_board,m_ht,m_localSearchHandler)
		, m_nSelectCNN(0)
	{
		m_shouldUpdateRaveMoves.resize(MAX_NUM_GRIDS);
	}

	void resetThreadState() ;
	void backup ( );

	void rollback ( );

	bool play ( WeichiMove move, bool real_game = false );

	inline bool isTerminal ()
	{
		m_board.setEarlyEndGameStatus(false);
		if( !WeichiGlobalInfo::getSearchInfo().m_calTerritoryStat && m_board.isEarlyEndGame() ) {
			m_board.setEarlyEndGameStatus(true);
			return true;
		}
		//if( m_moves.size()<2 ) { return false; }

		return m_board.hasTwoPass() || m_board.getMoveList().size() > 2 * WeichiConfigure::TotalGrids;
	}
	inline void preCalculateForEval() { m_board.preCalculateForEval(); }
	inline WeichiPlayoutResult eval() { return WeichiPlayoutResult(m_board.eval()); }

	WeichiPlayoutResult abortEval() const { return WeichiPlayoutResult ( 0.0 ) ; }

	inline Color getRootTurn () const { return m_rootTurn; }

	void summarize (UctNodePtr root, float seconds, uint simulateCount, bool period=true, WeichiMove move=WeichiMove()) const ;

	void setThreadID( uint threadId, uint default_gpu_device=WeichiConfigure::dcnn_default_gpu_device );
	double startPreSimulation( uint numPreSimulation=64, Territory& territory=WeichiGlobalInfo::getSearchInfo().m_territory );
	void endPreSimulation();
	void startExpansion();
	void endExpansion();
	void startSimulation (int sim_count) ;
	void startPlayout() ;
	void endPlayout() ;
	void endSimulation() ;

	bool isServerThread() const { return arguments::mode == "cnnserver"; }
	bool isWorkerThread() const { return arguments::mode == "cnnworker"; }
	bool isCNNServerThread() const { return (isCNNThread() || isCNNCacheThread()) && isServerThread(); }
	bool isCNNWorkerThread() const { return (isCNNThread() || isCNNCacheThread()) && isWorkerThread(); }
	bool isCNNCacheThread() const {
		/*return (m_thread_id>1+WeichiConfigure::dcnn_sl_gpu_thread_num &&
				m_thread_id<=1+WeichiConfigure::dcnn_sl_gpu_thread_num+WeichiConfigure::dcnn_cache_gpu_thread_num);*/
		return false;
	}
	bool isRemoteUpdateThread() const { return m_thread_id>MAX_NUM_THREADS; }

	inline bool isFirstThread() const { return m_thread_id==1; }
	inline bool isCNNThread() const { return !isFirstThread() && m_dcnnNetCollection.hasNet(); }
	inline WeichiCNNNet* getCNNNet() { return m_dcnnNetCollection.getCNNNet(); }
	inline WeichiCNNSLNet* getSLNet() { return m_dcnnNetCollection.getSLNet(); }
	inline WeichiCNNBVVNNet* getBVVNNet() { return m_dcnnNetCollection.getBVVNNet(); }

	void playoutPolicyGradientUpdate( WeichiPlayoutResult result, double dBaseLineB );

	std::string getDeadStones () const;

private:
	// explicitly disable copy constructor
	WeichiThreadState (const WeichiThreadState& rhs);

	void clearUpdateRaveTable();
	bool checkForUpdateRaveMove ( const WeichiMove move );
};

#endif
