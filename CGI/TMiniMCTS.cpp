#include "TMiniMCTS.h"

void TMiniMCTS::newGame()
{
	newTree();
	m_vMoveComments.clear();
	m_state.resetThreadState();
	m_bEnableResign = (Random::nextReal() > WeichiConfigure::zero_disable_resign_prob);

	m_bFastEndGameStage = false;
	m_iEndGameMoveNumber = -1;
	m_minWinRate.m_black = m_minWinRate.m_white = 1.0f;
	m_fastEndGameSimulation = MAX_NUM_SIMULATION * 0.1;
	m_fastEndGameSimulation = (m_fastEndGameSimulation >= 1 ? m_fastEndGameSimulation : 1);
}

void TMiniMCTS::play(const WeichiMove &move)
{
	m_state.play(move, true);
	newTree();
}

void TMiniMCTS::runMCTSSimulationBeforeForward()
{
	if (m_simulation == 0) {
		newTree();
		m_state.backup();
	}
	selection();

	WeichiBitBoard bmLegal;
	float inputFeatures[MAX_CNN_CHANNEL_SIZE*MAX_NUM_GRIDS];
	WeichiCNNNet* cnnNet = getCNNNet(m_state.getRootTurn());
	WeichiCNNFeatureType cnnFeatureType = cnnNet->getCNNFeatureType();
	WeichiCNNFeatureGenerator::calculateDCNNFeatures(getState(), cnnFeatureType, inputFeatures, bmLegal);
	cnnNet->set_data(getBatchID(), inputFeatures, getState().m_board.getToPlay(), bmLegal);
}

void TMiniMCTS::runMCTSSimulationAfterForward()
{
	if (m_vPath.size() == 0) { return; }

	DCNNResult dcnnResult = getDCNNResult();

	expandAndEvaluate(dcnnResult);
	update(dcnnResult);
	m_state.rollback();

	++m_simulation;

	if (isSimulationEnd()) { handle_simulationEnd(); }
}

WeichiMove TMiniMCTS::selectMCTSMove()
{
	uint childNum = 0;
	double dSum = 0.0f;
	WeichiMove bestMove = PASS_MOVE;
	TMiniNode* pBest = nullptr;
	TMiniNode* pRoot = getRootNode();

	uint maxCount = 0;
	for (TMiniNode *pChild = pRoot->getFirstChild(); childNum < pRoot->getNumChild(); ++pChild, ++childNum) {
		const StatisticData& uctData = pChild->getUctData();
		if (uctData.getCount() > maxCount) { maxCount = uctData.getCount(); }
	}
	
	childNum = 0;
	m_MCTSSearchDistribution = "";
	bool bAddComma = false;
	double dMaxCount = 0;
	for (TMiniNode *pChild = pRoot->getFirstChild(); childNum < pRoot->getNumChild(); ++pChild, ++childNum) {
		const StatisticData& uctData = pChild->getUctData();
		if (uctData.getCount() == 0) { continue; }
		if (maxCount > 1 && uctData.getCount() == 1) { continue; }
		if (m_state.m_board.getMoveList().size() >= 30 && uctData.getCount() < maxCount * WeichiConfigure::zero_threshold_ratio) { continue; }

		if (m_bFastEndGameStage) {
			if (uctData.getCount() > dMaxCount) {
				dMaxCount = uctData.getCount();
				bestMove = pChild->getMove();
				pBest = pChild;
			}
		} else {
			double dCount = uctData.getCount();
			dSum += dCount;
			double dRand = Random::nextReal(dSum);
			if (dRand < dCount) {
				bestMove = pChild->getMove();
				pBest = pChild;
			}

			if (uctData.getCount() > 0) {
				m_MCTSSearchDistribution += (bAddComma ? "," : "");
				m_MCTSSearchDistribution += ToString(WeichiMove::toCompactPosition(pChild->getMove().getPosition())) + ":";
				m_MCTSSearchDistribution += ToString(dCount);
				bAddComma = true;
			}
		}
	}

	m_pBest = pBest;
	m_simulation = 0;

	// debug information
	WeichiCNNNet* cnnNet = getCNNNet(m_state.getRootTurn());
	m_MCTSSearchDistribution += getSearchDebugInformation();

	return bestMove;
}

WeichiPlayoutResult TMiniMCTS::getGameResult(bool bResign)
{
	WeichiPlayoutResult result(0);
	if (bResign) {
		Color turnColor = m_state.m_board.getToPlay();
		result = (turnColor == COLOR_BLACK) ? WeichiPlayoutResult(-MAX_NUM_GRIDS) : WeichiPlayoutResult(MAX_NUM_GRIDS);
	} else { result = m_state.eval(); }

	return result;
}

bool TMiniMCTS::isResign()
{
	const StatisticData& rootData = getRootNode()->getUctData();
	const StatisticData& bestData = m_pBest->getUctData();
	const float RESIGN_THRESHOLD = WeichiConfigure::zero_resign_threshold;
	return (rootData.getMean() > -RESIGN_THRESHOLD && bestData.getMean() < RESIGN_THRESHOLD);
}

bool TMiniMCTS::isSimulationEnd()
{
	if (m_bFastEndGameStage) {
		int childNum = 0;
		StatisticData best, second;
		TMiniNode* pRoot = getRootNode();
		for (TMiniNode *child = pRoot->getFirstChild(); childNum < pRoot->getNumChild(); ++child, ++childNum) {
			if (child->getUctData().getCount() > best.getCount()) {
				second = best;
				best = child->getUctData();
			} else if (child->getUctData().getCount() > second.getCount()) {
				second = child->getUctData();
			}
		}

		// do early abort
		return (best.getCount() - second.getCount() > m_fastEndGameSimulation - m_simulation);
	} else { return m_simulation >= MAX_NUM_SIMULATION; }
}

// private function
void TMiniMCTS::newTree()
{
	m_nodeIndex = 1;
	m_simulation = 0;

	// initialize root node
	Color rootColor = AgainstColor(m_state.m_board.getToPlay());
	m_nodes->reset(WeichiMove(rootColor));
}

void TMiniMCTS::selection()
{
	TMiniNode* pNode = getRootNode();

	m_vPath.clear();
	m_vPath.push_back(pNode);
	while (pNode->hasChildren()) {
		pNode = selectChild(pNode);
		m_state.play(pNode->getMove());
		m_vPath.push_back(pNode);
	}
}

void TMiniMCTS::expandAndEvaluate(const DCNNResult& dcnnResult)
{
	if (m_state.m_board.hasTwoPass() || m_state.m_board.getMoveList().size() > 2 * WeichiConfigure::TotalGrids) { return; }

	Color trunColor = m_state.m_board.getToPlay();
	const Vector<CandidateEntry, MAX_NUM_GRIDS>& vCandidates = dcnnResult.m_vProbability;
	uint size = static_cast<uint>(vCandidates.size());

	TMiniNode* pFirstChild = allocateNewNodes(size);
	TMiniNode* pParent = m_vPath.back();
	TMiniNode* pChild = pFirstChild;
	for (uint i = 0; i < vCandidates.size(); i++) {
		WeichiMove move(trunColor, vCandidates[i].getPosition());
		float fScore = static_cast<float>(vCandidates[i].getScore());

		pChild->reset(move);
		pChild->setProbability(fScore);
		pChild->setOriginalProbability(fScore);
		pChild++;
	}

	pParent->setNumChild(size);
	pParent->setFirstChild(pFirstChild);

	// add noise to root node
	if (m_bAddNoiseToRoot && !m_bFastEndGameStage) {
		TMiniNode* pRootNode = getRootNode();
		if (pParent == pRootNode) {
			const float fEpsilon = WeichiConfigure::zero_noise_epsilon;
			vector<float> vDirichletNoise = calculateDirichletNoise(pRootNode->getNumChild(), WeichiConfigure::zero_noise_alpha);
			uint childNum = 0;
			for (TMiniNode *pChild = pRootNode->getFirstChild(); childNum < pRootNode->getNumChild(); ++pChild, ++childNum) {
				pChild->setProbability((1 - fEpsilon) * pChild->getProbability() + fEpsilon * vDirichletNoise[childNum]);
			}
		}
	}
}

void TMiniMCTS::update(const DCNNResult& dcnnResult)
{
	float fValue = dcnnResult.m_fValue;
	if (m_state.m_board.hasTwoPass() || m_state.m_board.getMoveList().size() > 2 * WeichiConfigure::TotalGrids) {
		WeichiPlayoutResult result = m_state.eval();
		Color winner = result.getWinner();
		fValue = (winner == COLOR_NONE) ? 0.0f : ((winner == m_state.m_board.getToPlay()) ? 1.0f : -1.0f);
	}

	for (int i = static_cast<int>(m_vPath.size()) - 1; i >= 0; i--) {
		TMiniNode* pNode = m_vPath[i];

		// since the first update node is parent node, we should reverse the value first
		fValue = -fValue;
		pNode->getUctData().add(fValue);

		// baseline value for all non-expand children
		if (i == static_cast<int>(m_vPath.size()) - 1) { pNode->setValue(fValue); }
	}
}

string TMiniMCTS::getGameSgf()
{
	SgfTag sgfTag;
	string sBlackModel = getCNNNet(COLOR_BLACK)->getNetParam().m_sCaffeModel;
	string sWhiteModel = getCNNNet(COLOR_WHITE)->getNetParam().m_sCaffeModel;
	sBlackModel = sBlackModel.substr(sBlackModel.find_last_of("/") + 1);
	sWhiteModel = sWhiteModel.substr(sWhiteModel.find_last_of("/") + 1);

	ostringstream oss;
	WeichiCNNNet* cnnNet = getCNNNet(m_state.getRootTurn());
	bool bResign = (cnnNet->getVNLabelSize() == 1 && !cnnNet->hasBVOutput() && isResign());
	WeichiPlayoutResult result = getGameResult(bResign);
	oss << (isEnableResign() ? "" : ("Mini-winrate: " + ToString(m_minWinRate.get(result.getWinner()))) + "; ");
	oss << (isEnableResign() ? "" : "disable resign; ");
	oss << "Machine: " << WeichiConfigure::zero_selfplay_machine_name << "; ";
	sgfTag.setSgfTag("EV", oss.str());
	sgfTag.setSgfTag("DT", TimeSystem::getTimeString("Y/m/d_H:i:s.f"));
	sgfTag.setSgfTag("RE", result.toString());
	sgfTag.setSgfTag("PB", sBlackModel);
	sgfTag.setSgfTag("PW", sWhiteModel);

	return m_state.m_board.toSgfFileString(sgfTag, m_vMoveComments);
}

void TMiniMCTS::handle_simulationEnd()
{
	WeichiMove move = selectMCTSMove();

	if (!m_bFastEndGameStage) {
		// record min winrate
		const StatisticData& rootData = getRootNode()->getUctData();
		const StatisticData& bestData = m_pBest->getUctData();
		float fMinWinRate = (bestData.getMean() > -rootData.getMean() ? bestData.getMean() : -rootData.getMean());
		if (fMinWinRate < m_minWinRate.get(m_state.getRootTurn())) { m_minWinRate.get(m_state.getRootTurn()) = fMinWinRate; }

		WeichiCNNNet* cnnNet = getCNNNet(m_state.getRootTurn());
		if ((cnnNet->getVNLabelSize() > 1 || cnnNet->hasBVOutput()) && isResign()) {
			m_iEndGameMoveNumber = m_state.m_board.getMoveList().size();
			m_bFastEndGameStage = true;
		}
	} else {
		// check to change sim*0.01
		WeichiCNNNet* cnnNet = getCNNNet(m_state.getRootTurn());
		if ((cnnNet->getVNLabelSize() > 1 || cnnNet->hasBVOutput()) && m_fastEndGameSimulation == MAX_NUM_SIMULATION * 0.1) {
			const StatisticData& rootData = getRootNode()->getUctData();
			const StatisticData& bestData = m_pBest->getUctData();
			const int MIN_MOVES = 200;
			const float SIM001_RESIGN_THRESHOLD = 0.05f;
			if (rootData.getMean() > -SIM001_RESIGN_THRESHOLD && bestData.getMean() < SIM001_RESIGN_THRESHOLD && m_state.m_board.getMoveList().size() >= MIN_MOVES) {
				m_fastEndGameSimulation = MAX_NUM_SIMULATION * 0.01;
				m_fastEndGameSimulation = (m_fastEndGameSimulation >= 1 ? m_fastEndGameSimulation : 1);
			}
		}
	}

	WeichiCNNNet* cnnNet = getCNNNet(m_state.getRootTurn());
	bool bResign = (cnnNet->getVNLabelSize() == 1 && !cnnNet->hasBVOutput() && isResign());
	if (!bResign) {
		play(move);
		int move_number = m_state.m_board.getMoveList().size();
		addMoveComment(move_number, getMCTSSearchDistribution());
	}
	displayBoard(move);

	bool bEndGame = (bResign || isEndGame());
	if (bEndGame) {
		boost::lock_guard<boost::mutex> lock(m_mutex);
		int total_moves = (m_iEndGameMoveNumber == -1 ? m_state.m_board.getMoveList().size() : m_iEndGameMoveNumber);
		COUT() << "Self-play " << total_moves << " " << getGameSgf() << endl;
		newGame();
	}
}

void TMiniMCTS::displayBoard(const WeichiMove& move)
{
	if (!m_bDisplayBoard) { return; }

	string sBlackModel = getCNNNet(COLOR_BLACK)->getNetParam().m_sCaffeModel;
	string sWhiteModel = getCNNNet(COLOR_WHITE)->getNetParam().m_sCaffeModel;
	sBlackModel = sBlackModel.substr(sBlackModel.find_last_of("/") + 1);
	sWhiteModel = sWhiteModel.substr(sWhiteModel.find_last_of("/") + 1);

	m_state.m_board.showColorBoard();
	CERR() << "Black Model: " << sBlackModel << endl;
	CERR() << "White Model: " << sWhiteModel << endl;
	CERR() << TimeSystem::getTimeString("[Y/m/d H:i:s.f] ")
		<< "[" << m_state.m_board.getMoveList().size() << "] "
		<< "Best Move: " << move.toGtpString() << " ";
	CERR() << m_pBest->getUctData().toString() << endl << endl;
}

DCNNResult TMiniMCTS::getDCNNResult()
{
	DCNNResult dcnnResult;
	WeichiCNNNet* cnnNet = getCNNNet(m_state.getRootTurn());
	
	dcnnResult.clear();
	dcnnResult.storeProbability(cnnNet->getSLCandidates(getBatchID()));
	dcnnResult.m_fValue = cnnNet->getVNResult(getBatchID(), WeichiConfigure::komi);
	
	return dcnnResult;
}

string TMiniMCTS::getSearchDebugInformation()
{
	TMiniNode* pRoot = getRootNode();

	ostringstream debugLog;
	debugLog << "*@";
	debugLog << "Root: " << getRootNode()->getUctData().toString() << "@";
	debugLog << "Best: " << getBestNode()->getUctData().toString() << "@";
	debugLog << "Sequence: ";
	TMiniNode* pNode = pRoot;
	while (pNode->hasChildren()) {
		int childNum = 0;
		TMiniNode* pBest = pNode->getFirstChild();
		for (TMiniNode *child = pNode->getFirstChild(); childNum < pNode->getNumChild(); ++child, ++childNum) {
			if (child->getUctData().getCount() <= pBest->getUctData().getCount()) { continue; }
			pBest = child;
		}
		pNode = pBest;
		debugLog << pBest->getMove().toGtpString() << "(" << pBest->getUctData().toString() << ")";
		if (pNode->hasChildren()) { debugLog << " => "; }
	}
	debugLog << "@";

	debugLog << "move rank:  action    Q     U     P   P-Dir    N  soft-N@";
	uint childNum = 0;
	uint nParentSimulation = static_cast<uint>(pRoot->getUctData().getCount());
	for (TMiniNode *pChild = pRoot->getFirstChild(); childNum < pRoot->getNumChild(); ++pChild, ++childNum) {
		if (pChild->getUctData().getCount() == 0 && childNum >= 3) { continue; }

		const StatisticData& childUctData = pChild->getUctData();
		double dValuePa = pChild->getProbability();
		double dPUCTBias = log((1 + nParentSimulation + 19652) / 19652) + WeichiConfigure::mcts_puct_bias;
		double dValueU = dPUCTBias * dValuePa * sqrt(nParentSimulation) / (1 + childUctData.getCount());
		double dValueQ = (childUctData.getCount() > 0) ? childUctData.getMean() : pNode->getAdjustChildVNValue();
		double move_score = dValueQ + dValueU;

		debugLog << left << setw(4) << pChild->getMove().toGtpString() << " "
			<< right << setw(4) << (childNum + 1) << ": "
			<< fixed << setprecision(3) << setw(7) << move_score << " "
			<< setw(6) << dValueQ << " "
			<< setw(5) << dValueU << " "
			<< setw(5) << pChild->getOriginalProbability() << " "
			<< setw(5) << dValuePa << " ";
		debugLog.unsetf(ios::fixed);
		debugLog << setw(5) << childUctData.getCount() << " "
			<< fixed << setprecision(3) << setw(5) << childUctData.getCount() / (nParentSimulation - 1)
			<< (pChild == m_pBest ? " *" : "") << "@";
	}

	return debugLog.str();
}

TMiniNode* TMiniMCTS::selectChild(TMiniNode* pNode)
{
	double dBestScore = -DBL_MAX;
	TMiniNode* pBest = nullptr;

	uint childNum = 0;
	float fSumOfChildWins = 0.0f;
	float fSumOfChildSims = 0.0f;
	uint nParentSimulation = static_cast<uint>(pNode->getUctData().getCount());
	for (TMiniNode *child = pNode->getFirstChild(); childNum < pNode->getNumChild(); ++child, ++childNum) {
		const StatisticData& childUctData = child->getUctData();
		double dValuePa = child->getProbability();
		double dPUCTBias = log((1 + nParentSimulation + 19652) / 19652) + WeichiConfigure::mcts_puct_bias;
		double dValueU = dPUCTBias * dValuePa * sqrt(nParentSimulation) / (1 + childUctData.getCount());
		double dValueQ = (childUctData.getCount() > 0) ? childUctData.getMean() : pNode->getAdjustChildVNValue();

		if (child->hasChildren()) {
			fSumOfChildWins += child->getUctData().getMean();
			fSumOfChildSims += 1;
		}

		double move_score = dValueQ + dValueU;
		if (move_score <= dBestScore) { continue; }

		dBestScore = move_score;
		pBest = child;
	}

	float fAdjustChildVNValue = fSumOfChildWins / (fSumOfChildSims + 1);
	pNode->setAdjustChildVNValue(fAdjustChildVNValue);

	return pBest;
}

vector<float> TMiniMCTS::calculateDirichletNoise(int size, float fAlpha)
{
	boost::random::gamma_distribution<> pdf(fAlpha);
	boost::variate_generator<Random::IntegerGenerator&, boost::gamma_distribution<> >
		generator(*Random::integer_generator, pdf);

	vector<float> vDirichlet;
	for (int i = 0; i < size; i++) { vDirichlet.push_back(static_cast<float>(generator())); }
	float fSum = accumulate(vDirichlet.begin(), vDirichlet.end(), 0.0f);
	if (fSum < boost::numeric::bounds<float>::smallest()) { return vDirichlet; }
	for (int i = 0; i < vDirichlet.size(); i++) { vDirichlet[i] /= fSum; }

	return vDirichlet;
}
