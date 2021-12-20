#ifndef WEICHIRUNINFO_H
#define WEICHIRUNINFO_H

#include "BasicType.h"
#include "NodePtr.h"
#include "WeichiUctNode.h"
#include "WeichiMove.h"
#include "Dual.h"
#include "StatisticData.h"
#include "Territory.h"
#include "WeichiLGRTable.h"
#include "WeichiMoveCache.h"
#include "WeichiCacheLogger.h"
#include "UctNodeOrder.h"
#include "CNNJobQueue.h"
#include "WeichiRemoteCNNJobTable.h"

class WeichiGlobalInfo
{
	typedef NodePtr<WeichiUctNode> UctNodePtr;

private:
	struct TreeInfo
	{
		static const int OUT_OF_ORDER = -1;

		bool m_bIsEarlyAbort;
		int m_nMove;
		int m_nDCNNJob;
		int m_nMaxTreeWidth;
		int m_nMaxTreeHeight;
		int m_nMaxChildIndex;
		int m_nHandMove;
		int m_nSimulation;
		int m_predictOrder;
		float m_dThinkTime;
		Dual<float> m_fTerritory;
		Dual<float> m_dRemainTime;
		UctNodePtr m_pRoot;
		WeichiMove m_bestMove;
		StatisticData m_bestSD;
		StatisticData m_bestValueSD;
		StatisticData m_lastSD;
		StatisticData m_predictSD;
		StatisticData m_treeWidth;
		StatisticData m_treeHeight;
		Vector<uint,MAX_NUM_GRIDS> m_vPredictOrder;
		Vector<StatisticData,MAX_NUM_GRIDS> m_vPredictInfoOrder;
		Vector<int, MAX_NUM_GRIDS> m_childIndex;

		TreeInfo()
			: m_predictOrder(OUT_OF_ORDER)
			, m_nMove(0)
		{}

		string getNodeString( const UctNodeOrder& nodeOrder, int deep );
		vector<UctNodeOrder> getChildOrder( UctNodePtr pNode );
	};

	struct SearchInfo
	{
		bool m_outOfOpeningBook;
		bool m_calTerritoryStat;
		Territory m_territory;
		Territory m_passTerritory;
		WeichiMoveCache m_moveCache;
		Dual<uint> m_policyStat[POLICY_SIZE];

		void clearPolicyStat();
	};

	struct LogInfo
	{
		bool m_bLogDirUsed;
		string m_sLogfilePrefix;
		WeichiCacheLogger m_cacheLogger;
	};

	struct RemoteInfo
	{
		WeichiRemoteCNNJobTable m_cnnJobTable;
		boost::atomic<uint> m_jobSessionId;
	};

	LogInfo m_logInfo;
	TreeInfo m_treeInfo;
	SearchInfo m_searchInfo;
	RemoteInfo m_remoteInfo;
	CNNJobQueue m_CNNJobQueue;
	static WeichiGlobalInfo *s_instance;
public:
	bool m_bIsLastCandidate;

public:
	WeichiGlobalInfo() {}
	~WeichiGlobalInfo() { if( s_instance ) { delete s_instance; } }

	static WeichiGlobalInfo *get()
	{
		if( s_instance==NULL ) { initialize(); }
		return s_instance;
	}
	static TreeInfo& getTreeInfo() { return get()->m_treeInfo; }
	static SearchInfo& getSearchInfo() { return get()->m_searchInfo; }
	static LogInfo& getLogInfo() { return get()->m_logInfo; }
	static RemoteInfo& getRemoteInfo() { return get()->m_remoteInfo; }
	static CNNJobQueue& getCNNJobQueue() { return get()->m_CNNJobQueue; }
	void cleanSummaryInfo();
	void logInfo( const WeichiMove& move );
	void logPredictData();
	string getSearchSummaryInfoString( const WeichiBoard& board );
	vector<string> getSearchTreeSummaryString( const WeichiBoard& board );
	string getPolicyStatString( vector<WeichiPlayoutPolicy> vPrintPolicy );
	string getBestSequenceString()
	{
		Vector<uint,MAX_NUM_GRIDS> vSequenceNumber;
		vector<string> vBestSequence = getBestSequenceString(vSequenceNumber);
		string sBestSequence = "";
		for( uint i=0; i<vBestSequence.size(); i++ ) {
			sBestSequence += vBestSequence[i] + "\n";
		}

		return sBestSequence;
	}
	void calculateSureTerritory();
	
private:
	static void initialize() { s_instance = new WeichiGlobalInfo(); }

	vector<string> getBoardString( const WeichiBoard& board );
	vector<string> getBoardStringWithTerritory( const WeichiBoard& board );
	vector<string> getBoardStringWithSequence( const WeichiBoard& board, const Vector<uint,MAX_NUM_GRIDS>& vSequenceNumber );
	vector<string> getBoardStringWithSLCandidates( const WeichiBoard& board, const Vector<uint,MAX_NUM_GRIDS>& vSLCandidates );
	vector<string> getBestSequenceString( Vector<uint,MAX_NUM_GRIDS>& vSequenceNumber );
	Vector<uint,MAX_NUM_GRIDS> getRootSLCandidates();
	uint predictOpponentMoveOrder( const WeichiMove& move );
	StatisticData predictOppoentMoveInfo( const WeichiMove& move );
	void traverseBestSequence( vector<string>& vBestSequence, UctNodePtr root, int deep, Vector<uint,MAX_NUM_GRIDS>& vSequenceNumber, bool bIsMainSequence=false );
};

#endif
