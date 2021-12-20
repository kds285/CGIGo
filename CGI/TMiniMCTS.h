#ifndef TCNNSEARCHTREE_H
#define TCNNSEARCHTREE_H

#include "BasicType.h"
#include "WeichiMove.h"
#include "WeichiCNNNet.h"
#include "StatisticData.h"
#include "WeichiThreadState.h"
#include "OpenAddressHashTable.h"

class DCNNResult {
public:
	float m_fValue;
	Vector<CandidateEntry, MAX_NUM_GRIDS> m_vProbability;

	void clear() {
		m_fValue = 0.0f;
		m_vProbability.clear();
	}
	void storeProbability(const vector<CandidateEntry>& vProbability ) {
		for (uint i = 0; i < vProbability.size(); i++) { m_vProbability.push_back(vProbability[i]); }
	}
};

class TMiniNode {
private:
	uint m_nChildren;
	WeichiMove m_move;
	float m_fValue;
	float m_fProbabilty;
	float m_fOriginalProbabilty;
	float m_fAdjustChildVNValue;
	StatisticData m_uctData;
	TMiniNode* m_pFirstChild;

public:
	TMiniNode() {}
	~TMiniNode() {}

	void reset(WeichiMove move)
	{
		m_nChildren = 0;
		m_move = move;
		m_fValue = 0.0f;
		m_fProbabilty = 0.0f;
		m_fOriginalProbabilty = 0.0f;
		m_fAdjustChildVNValue = 0.0f;
		m_pFirstChild = NULL;
		m_uctData.reset();
	}
	
	inline void setNumChild(uint nChild) { m_nChildren = nChild; }
	inline void setValue(float fValue) { m_fValue = fValue; }
	inline void setProbability(float fProbability) { m_fProbabilty = fProbability; }
	inline void setOriginalProbability(float fOriginalProbability) { m_fOriginalProbabilty = fOriginalProbability; }
	inline void setAdjustChildVNValue(float fAdjustChildVNValue) { m_fAdjustChildVNValue = fAdjustChildVNValue; }
	inline void setFirstChild(TMiniNode* pFirstChild) { m_pFirstChild = pFirstChild; }

	inline bool hasChildren() const { return (m_nChildren != 0); }
	inline uint getNumChild() const { return m_nChildren; }
	inline WeichiMove getMove() const { return m_move; }
	inline float getValue() const { return m_fValue; }
	inline float getProbability() const { return m_fProbabilty; }
	inline float getOriginalProbability() const { return m_fOriginalProbabilty; }
	inline float getAdjustChildVNValue() const { return m_fAdjustChildVNValue; }
	inline StatisticData& getUctData() { return m_uctData; }
	inline const StatisticData& getUctData() const { return m_uctData; }
	inline TMiniNode* getFirstChild() { return m_pFirstChild; }
};

class TMiniMCTS
{
private:
	bool m_bDisplayBoard;
	bool m_bEnableResign;
	bool m_bAddNoiseToRoot;
	bool m_bFastEndGameStage;

	int m_simulation;
	int m_fastEndGameSimulation;
	int m_iEndGameMoveNumber;
	uint m_batchID;
	uint m_nodeIndex;
	boost::mutex& m_mutex;
	TMiniNode* m_nodes;
	TMiniNode* m_pBest;
	Dual<WeichiCNNNet*> m_cnnNet;
	Dual<float> m_minWinRate;
	WeichiThreadState m_state;
	vector<TMiniNode*> m_vPath;
	vector<string> m_vMoveComments;
	string m_MCTSSearchDistribution;

	const int MAX_NUM_SIMULATION;

public:
	TMiniMCTS(int nSimulation, boost::mutex& threadMutex)
		: MAX_NUM_SIMULATION(nSimulation), m_pBest(nullptr)
		, m_bDisplayBoard(false)
		, m_batchID(-1)
		, m_mutex(threadMutex)
	{
		m_nodes = new TMiniNode[MAX_NUM_SIMULATION * MAX_NUM_GRIDS];
		m_cnnNet.m_black = m_cnnNet.m_white = nullptr;
	}

	~TMiniMCTS()
	{
		delete[] m_nodes;
	}
	
	void newGame();
	void play(const WeichiMove &move);
	void runMCTSSimulationBeforeForward();
	void runMCTSSimulationAfterForward();
	WeichiMove selectMCTSMove();
	WeichiPlayoutResult getGameResult(bool bResign);
	bool isResign();
	bool isSimulationEnd();

	inline uint getBatchID() { return m_batchID; }
	inline TMiniNode* getRootNode() { return m_nodes; }
	inline TMiniNode* getBestNode() { return m_pBest; }
	inline WeichiCNNNet* getCNNNet(Color netColor) { return m_cnnNet.get(netColor); }
	inline bool isEnableResign() const { return m_bEnableResign; }
	inline string getMCTSSearchDistribution() const { return m_MCTSSearchDistribution; }
	inline bool isEndGame() const { return (m_state.m_board.hasTwoPass()) || (m_state.m_board.getMoveList().size() > 2 * WeichiConfigure::TotalGrids); }
	inline bool setDisplayBoard(bool bDisplayBoard) { m_bDisplayBoard = bDisplayBoard; }
	inline void setBatchID(uint batchID) { m_batchID = batchID; }
	inline void setNoise(bool bAddNoiseToRoot) { m_bAddNoiseToRoot = bAddNoiseToRoot; }
	inline void setCNNNet(WeichiCNNNet* cnnNet, Color netColor) { m_cnnNet.set(netColor, cnnNet); }
	inline WeichiThreadState& getState() { return m_state; }
	inline const WeichiThreadState& getState() const { return m_state; }

private:
	void newTree();

	void selection();
	void expandAndEvaluate(const DCNNResult& dcnnResult);
	void update(const DCNNResult& dcnnResult);

	string getGameSgf();
	void handle_simulationEnd();
	void displayBoard(const WeichiMove& move);
	DCNNResult getDCNNResult();
	string getSearchDebugInformation();

	TMiniNode* selectChild(TMiniNode* pNode);
	vector<float> calculateDirichletNoise(int size, float fAlpha);

	inline void addMoveComment(int move_number, string sMoveComment) {
		m_vMoveComments.resize(move_number);
		m_vMoveComments.back() = sMoveComment;
	}
	inline TMiniNode* allocateNewNodes(uint size) {
		TMiniNode* pNode = m_nodes + m_nodeIndex;
		m_nodeIndex += size;
		return pNode;
	}
};

#endif
