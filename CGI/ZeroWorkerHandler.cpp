#include "ZeroWorkerHandler.h"
#include "BaseCNNNet.h"
#include "SgfLoader.h"
#include "TimeSystem.h"

string ZeroWorkerSharedData::getOneSelfPlay()
{
	boost::lock_guard<boost::mutex> lock(m_selfPlayQueueMutex);
	if (m_selfPlayQueue.size() == 0) { return ""; }
	else {
		string sSelfPlay = m_selfPlayQueue.front();
		m_selfPlayQueue.pop_front();
		return sSelfPlay;
	}
}

string ZeroWorkerSharedData::getOneEvaluator()
{
	boost::lock_guard<boost::mutex> lock(m_evaluatorQueueMutex);
	if (m_evaluatorQueue.size() == 0) { return ""; }
	else {
		string sEvaluator = m_evaluatorQueue.front();
		m_evaluatorQueue.pop_front();
		return sEvaluator;
	}
}

string ZeroWorkerSharedData::getOneOptimization()
{
	boost::lock_guard<boost::mutex> lock(m_optimizationQueueMutex);
	if (m_optimizationQueue.size() == 0) { return ""; }
	else {
		string sOptimization = m_optimizationQueue.front();
		m_optimizationQueue.pop_front();
		return sOptimization;
	}
}

// ZeroWorkerStatus
void ZeroWorkerStatus::handle_msg(const std::string msg)
{
	std::vector<std::string> vArgs;
	boost::split(vArgs, msg, boost::is_any_of(" "), boost::token_compress_on);

	if (vArgs[0] == "Info") {
		// Info name num_gpu type
		m_sName = vArgs[1];
		m_sGPUList = vArgs[2];
		if (vArgs[3] == "Self-play") { m_type = ZERO_SELFPLAY; }
		else if (vArgs[3] == "Optimization") { m_type = ZERO_OPTIMIZAITON; }
		else if (vArgs[3] == "Evaluator") { m_type = ZERO_EVALUATOR; }
		else {
			std::cerr << "[error] receive error type \"" << vArgs[3] << "\" from worker, disconnect." << std::endl;
			do_close();
			return;
		}

		boost::lock_guard<boost::mutex> lock(m_sharedData.m_workerMutex);
		m_sharedData.m_fWorkerLog << "[Worker-Connection] "
			<< TimeSystem::getTimeString("Y/m/d_H:i:s.f ")
			<< m_sName << " " << m_sGPUList << " " << getZeroTypeString(m_type) << endl;
	} else if (vArgs[0] == "Train_DIR") {
		m_bIsIdle = true;
	} else if (vArgs[0] == "Self-play") {
		if (msg.find("Self-play", msg.find("Self-play", 0) + 1) != string::npos) { return; }
		if (m_type == ZERO_SELFPLAY) {
			string sSelfPlay = msg.substr(msg.find(vArgs[0]) + vArgs[0].length() + 1);
			boost::lock_guard<boost::mutex> lock(m_sharedData.m_selfPlayQueueMutex);
			m_sharedData.m_selfPlayQueue.push_back(sSelfPlay);
		} else if (m_type == ZERO_EVALUATOR) {
			string sEvaluator = msg.substr(msg.find(vArgs[0]) + vArgs[0].length() + 1);
			boost::lock_guard<boost::mutex> lock(m_sharedData.m_evaluatorQueueMutex);
			m_sharedData.m_evaluatorQueue.push_back(sEvaluator);
		}
	} else if (vArgs[0] == "Optimization") {
		boost::lock_guard<boost::mutex> lock(m_sharedData.m_optimizationQueueMutex);
		m_sharedData.m_optimizationQueue.push_back(vArgs[1]);
	} else {
		// unknown client, reject it
		string sMessage = msg;
		std::replace(sMessage.begin(), sMessage.end(), '\r', ' ');
		std::replace(sMessage.begin(), sMessage.end(), '\n', ' ');
		std::cerr << "[error] receive \"" << sMessage << "\" from worker, disconnect." << std::endl;
		do_close();
	}
}

void ZeroWorkerStatus::do_close()
{
	boost::lock_guard<boost::mutex> lock(m_sharedData.m_workerMutex);
	m_sharedData.m_fWorkerLog << "[Worker-Disconnection] " << TimeSystem::getTimeString("Y/m/d_H:i:s.f ") << m_sName << " " << m_sGPUList << endl;
	TBaseWorkerStatus::do_close();
}

// ZeroWorkerHandler (public function)
void ZeroWorkerHandler::run()
{
	initialize();
	start_accept();
	CERR() << TimeSystem::getTimeString("[Y/m/d H:i:s.f] ") << "Server initialize over." << endl;

	m_iteration--;
	setupSelfPlay();
	
	while (true) {
		handleSelfPlay();
		//handleOptimizationReadSgf();
		handleOptimization();
		handleEvaluator();
	}
}

// ZeroWorkerHandler (private function)
void ZeroWorkerHandler::initialize()
{
	uint seed = Configure::UseTimeSeed ? static_cast<uint>(time(NULL)) : Configure::RandomSeed;
	Random::reset(seed);

	m_iteration = WeichiConfigure::zero_start_iteration;
	m_logger.createLogDirectoryAndFiles();

	// add agent pool
	CNNNetParam blackParam(WeichiConfigure::zero_selfplay_black_net);
	CNNNetParam whiteParam(WeichiConfigure::zero_selfplay_white_net);
	m_vBestAgentPool.push_back(make_pair("iter_8922563.pb", StatisticData()));
	m_vBestAgentPool.push_back(make_pair("worker10_iter_1200.pb", StatisticData()));
	m_vBestAgentPool.push_back(make_pair("worker11_iter_1200.pb", StatisticData()));
	m_vBestAgentPool.push_back(make_pair("worker16_iter_1200.pb", StatisticData()));
	m_vBestAgentPool.push_back(make_pair("worker16_iter_3600.pb", StatisticData()));
	m_vBestAgentPool.push_back(make_pair("worker8_iter_3600.pb", StatisticData()));
	m_vBestAgentPool.push_back(make_pair("worker2_iter_4800.pb", StatisticData()));
	m_vBestAgentPool.push_back(make_pair(blackParam.m_sCaffeModel.substr(blackParam.m_sCaffeModel.find_last_of("/") + 1), StatisticData()));

	// self-play configure
	m_sSelfPlayConfigure = "";
	m_sSelfPlayConfigure += "USE_TIME_SEED=false:BOARD_SIZE=" + ToString(WeichiConfigure::BoardSize);
	m_sSelfPlayConfigure += ":KOMI=" + ToString(WeichiConfigure::komi);
	m_sSelfPlayConfigure += ":PUCT_BIAS=" + ToString(WeichiConfigure::mcts_puct_bias);
	m_sSelfPlayConfigure += ":ZERO_NUM_SIMULATION=" + ToString(WeichiConfigure::zero_num_simulation);
	m_sSelfPlayConfigure += ":ZERO_THRESHOLD_RATIO=" + ToString(WeichiConfigure::zero_threshold_ratio);
	m_sSelfPlayConfigure += ":ZERO_ROOT_NODE_NOISE=" + ToString(WeichiConfigure::zero_root_node_noise);
	m_sSelfPlayConfigure += ":ZERO_NOISE_ALPHA=" + ToString(WeichiConfigure::zero_noise_alpha);
	m_sSelfPlayConfigure += ":ZERO_NOISE_EPSILON=" + ToString(WeichiConfigure::zero_noise_epsilon);
	m_sSelfPlayConfigure += ":ZERO_DISABLE_RESIGN_PROB=" + ToString(WeichiConfigure::zero_disable_resign_prob);

	// customized configure
	m_sSelfPlayConfigure += ":RANDOM_SEED=@RANDOMSEED@";
	m_sSelfPlayConfigure += ":DCNN_TRAIN_GPU_LIST=@GPULIST@";
	m_sSelfPlayConfigure += ":ZERO_RESIGN_THRESHOLD=@RESIGN_THRESHOLD@";
	m_sSelfPlayConfigure += ":ZERO_SELFPLAY_MACHINE_NAME=@MACHINENAME@";
	m_sSelfPlayConfigure += ":ZERO_SELFPLAY_BLACK_NET=NET|" + ToString(blackParam.m_iRandomNum);
	m_sSelfPlayConfigure += "|" + getWeichiCNNFeatureTypeString(blackParam.m_featureType) + "|";
	m_sSelfPlayConfigure += "../../" + WeichiConfigure::zero_train_directory + "/model/";
	m_sSelfPlayConfigure += blackParam.m_sPrototxt.substr(blackParam.m_sPrototxt.find_last_of("/") + 1) + "|@BLACKMODEL@|0";
	m_sSelfPlayConfigure += ":ZERO_SELFPLAY_WHITE_NET=NET|" + ToString(whiteParam.m_iRandomNum);
	m_sSelfPlayConfigure += "|" + getWeichiCNNFeatureTypeString(whiteParam.m_featureType) + "|";
	m_sSelfPlayConfigure += "../../" + WeichiConfigure::zero_train_directory + "/model/";
	m_sSelfPlayConfigure += whiteParam.m_sPrototxt.substr(whiteParam.m_sPrototxt.find_last_of("/") + 1) + "|@WHITEMODEL@|0";

	// evaluator configure
	m_sEvaluatorConfigure = "";
	m_sEvaluatorConfigure += "USE_TIME_SEED=false:BOARD_SIZE=" + ToString(WeichiConfigure::BoardSize);
	m_sEvaluatorConfigure += ":KOMI=" + ToString(WeichiConfigure::komi);
	m_sEvaluatorConfigure += ":PUCT_BIAS=" + ToString(WeichiConfigure::mcts_puct_bias);
	m_sEvaluatorConfigure += ":ZERO_NUM_SIMULATION=" + ToString(WeichiConfigure::zero_num_simulation);	
	m_sEvaluatorConfigure += ":ZERO_THRESHOLD_RATIO=1:ZERO_ROOT_NODE_NOISE=false:ZERO_DISABLE_RESIGN_PROB=0:ZERO_RESIGN_THRESHOLD=-0.8";

	// customized configure
	m_sEvaluatorConfigure += ":RANDOM_SEED=@RANDOMSEED@";
	m_sEvaluatorConfigure += ":DCNN_TRAIN_GPU_LIST=@GPULIST@";
	m_sEvaluatorConfigure += ":ZERO_SELFPLAY_BLACK_NET=NET|" + ToString(blackParam.m_iRandomNum);
	m_sEvaluatorConfigure += "|" + getWeichiCNNFeatureTypeString(blackParam.m_featureType) + "|";
	m_sEvaluatorConfigure += "../../" + WeichiConfigure::zero_train_directory + "/model/";
	m_sEvaluatorConfigure += blackParam.m_sPrototxt.substr(blackParam.m_sPrototxt.find_last_of("/") + 1) + "|@BLACKMODEL@|0";
	m_sEvaluatorConfigure += ":ZERO_SELFPLAY_WHITE_NET=NET|" + ToString(whiteParam.m_iRandomNum);
	m_sEvaluatorConfigure += "|" + getWeichiCNNFeatureTypeString(whiteParam.m_featureType) + "|";
	m_sEvaluatorConfigure += "../../" + WeichiConfigure::zero_train_directory + "/model/";
	m_sEvaluatorConfigure += whiteParam.m_sPrototxt.substr(whiteParam.m_sPrototxt.find_last_of("/") + 1) + "|@WHITEMODEL@|0";

	// optimization configure
	m_sOptimizationConfigure = "";
	m_sOptimizationConfigure += "USE_TIME_SEED=false:BOARD_SIZE=" + ToString(WeichiConfigure::BoardSize);
	m_sOptimizationConfigure += ":KOMI=" + ToString(WeichiConfigure::komi);
	m_sOptimizationConfigure += ":ZERO_TRAIN_DIRECTORY=" + ToString(WeichiConfigure::zero_train_directory);
	m_sOptimizationConfigure += ":ZERO_REPLAY_BUFFER_SIZE=" + ToString(WeichiConfigure::zero_replay_buffer_size);
	m_sOptimizationConfigure += ":ZERO_OPTIMIZATION_NUM_WORKER=" + ToString(WeichiConfigure::zero_optimization_num_worker);
	m_sOptimizationConfigure += ":ZERO_OPTIMIZATION_LEARNING_RATE=" + ToString(WeichiConfigure::zero_optimization_learning_rate);
	CNNNetParam optimizationParam(WeichiConfigure::zero_optimization_net);
	m_sOptimizationConfigure += ":ZERO_OPTIMIZATION_NET=NET|" + ToString(optimizationParam.m_iRandomNum);
	m_sOptimizationConfigure += "|" + getWeichiCNNFeatureTypeString(optimizationParam.m_featureType) + "|";
	m_sOptimizationConfigure += "../../" + WeichiConfigure::zero_train_directory + "/model/";
	m_sOptimizationConfigure += optimizationParam.m_sPrototxt.substr(optimizationParam.m_sPrototxt.find_last_of("/") + 1) + "|";
	m_sOptimizationConfigure += "../../" + WeichiConfigure::zero_train_directory + "/model/";
	m_sOptimizationConfigure += optimizationParam.m_sCaffeModel.substr(optimizationParam.m_sCaffeModel.find_last_of("/") + 1) + "|0";
	m_sOptimizationConfigure += ":ZERO_OPTIMIZATION_SNAPSHOT_ITER=" + ToString(WeichiConfigure::zero_optimization_snapshot_iter);
	m_sOptimizationConfigure += ":ZERO_OPTIMIZAIOTN_SGF_DIRECTORY=" + ToString(WeichiConfigure::zero_optimization_sgf_directory);
	m_sOptimizationConfigure += ":DISPLAY=" + ToString(WeichiConfigure::display);
	m_sOptimizationConfigure += ":SNAPSHOT=" + ToString(WeichiConfigure::snapshot);
	m_sOptimizationConfigure += ":SNAPSHOT_PREFIX=" + WeichiConfigure::snapshot_prefix;
	m_sOptimizationConfigure += ":NFS_PATH=" + WeichiConfigure::NFS_path;
	m_sOptimizationConfigure += ":RUN_ID=" + WeichiConfigure::run_id;

	// customized configure
	m_sOptimizationConfigure += ":RANDOM_SEED=@RANDOMSEED@";
	m_sOptimizationConfigure += ":ZERO_OPTIMIZATION_SGF_MAX_ITERATION=@SGFMAXITERATION@";
}

void ZeroWorkerHandler::handleSelfPlay()
{
	// send selfplay command
	{
		boost::lock_guard<boost::mutex> lock(m_workerMutex);
		for (auto worker : m_vWorkers) {
			if (worker->getType() != ZERO_SELFPLAY || !worker->isIdle()) { continue; }
			worker->setIsIdle(false);
			worker->write(getSelfPlayConfigure(worker));
		}
	}

	// receive selfplay games
	while (m_total_games < WeichiConfigure::zero_num_game) {
		string sSelfPlay = m_sharedData.getOneSelfPlay();
		if (sSelfPlay == "") { break; }

		string sMoveNumber = sSelfPlay.substr(0, sSelfPlay.find("(") - 1);
		string sSgfString = sSelfPlay.substr(sSelfPlay.find("("));
		string sSimpleSgf = deleteUnusedSgfTag(sSgfString);

		m_logger.m_fSelfPlayGame << m_total_games << " " << sMoveNumber << " " << sSimpleSgf << endl;
		m_logger.m_fSelfPlayDebugGame << m_total_games << " " << sMoveNumber << " " << sSgfString << endl;
		addResignThreshold(sSimpleSgf);
		addSelfPlayModelWinRate(sSimpleSgf);
		m_total_games++;

		if (m_total_games % static_cast<int>(WeichiConfigure::zero_num_game * 0.25) == 0) {
			m_logger.m_fTrainingLog << TimeSystem::getTimeString("[Y/m/d H:i:s.f] ") << "[SelfPlay Progress] "
				<< m_total_games << " / " << WeichiConfigure::zero_num_game << endl;
		}
	}

	if (m_total_games >= WeichiConfigure::zero_num_game) {
		m_logger.m_fTrainingLog << TimeSystem::getTimeString("[Y/m/d H:i:s.f] ") << "[SelfPlay] Finished." << endl;

		vector< pair<string, StatisticData> > vBestAgentPool;
		for (int i = 0; i < m_vBestAgentPool.size(); i++) {
			bool bRemovePool = (m_vBestAgentPool[i].second.getCount() >= 60 && m_vBestAgentPool[i].second.getMean() <= WeichiConfigure::zero_selfplay_model_pool_threshold);
			m_logger.m_fTrainingLog << TimeSystem::getTimeString("[Y/m/d H:i:s.f] ") << "[SelfPlay WinRate] "
				<< getModelIteration(m_vBestAgentPool[i].first) << " vs. "
				<< getModelIteration(m_vBestAgentPool.back().first) << " : "
				<< m_vBestAgentPool[i].second.toString() << " "
				<< (bRemovePool ? "x" : "") << endl;

			if (bRemovePool) { continue; }
			vBestAgentPool.push_back(m_vBestAgentPool[i]);
			vBestAgentPool.back().second.reset();
		}
		m_vBestAgentPool = vBestAgentPool;

		m_logger.m_fSelfPlayGame.close();
		m_logger.m_fSelfPlayDebugGame.close();
		adjustResignThreshold();

		setupSelfPlay();
	}
}

void ZeroWorkerHandler::handleOptimization()
{
	/*if (!m_bOptimization) { return; }

	{
		boost::lock_guard<boost::mutex> lock(m_workerMutex);
		for (auto worker : m_vWorkers) {
			if (worker->getType() != ZERO_OPTIMIZAITON || !worker->isIdle()) { continue; }
			worker->setIsIdle(false);
			worker->write(getOptimizationConfigure(worker));
		}
	}*/

	// receive optimization model
	string sOptimization = "";
	while (true) {
		sOptimization = m_sharedData.getOneOptimization();
		if (sOptimization == "ReadSgf_Done") { continue; }
		if (sOptimization == "" || sOptimization == "Job_Done") { break; }
		if (sOptimization.find("delete") != string::npos) {
			bool bHasDeleteModel = false;
			string sModel = sOptimization.substr(sOptimization.find("delete") + string("delete").length());
			for (uint i = 0; i < m_vBestAgentPool.size(); i++) {
				if (m_vBestAgentPool[i].first != sModel) { continue; }
				int poolSize = m_vBestAgentPool.size();
				m_vBestAgentPool[i] = m_vBestAgentPool[poolSize - 2];
				m_vBestAgentPool[poolSize - 2] = m_vBestAgentPool[poolSize - 1];
				m_vBestAgentPool.pop_back();
				bHasDeleteModel = true;
				m_logger.m_fTrainingLog << TimeSystem::getTimeString("[Y/m/d H:i:s.f] ") << "[Optimization Delete Model] " << sOptimization << endl;
				break;
			}
			if (bHasDeleteModel) {
				for (uint i = 0; i < m_vBestAgentPool.size(); i++) {
					CERR() << m_vBestAgentPool[i].first << endl;
				}
			}
			continue;
		} else if (sOptimization.find("add") != string::npos) {
			string sModel = sOptimization.substr(sOptimization.find("add") + string("add").length());
			m_logger.m_fTrainingLog << TimeSystem::getTimeString("[Y/m/d H:i:s.f] ") << "[Optimization Add Model] " << sModel << endl;
			m_vBestAgentPool.push_back(make_pair(sModel, StatisticData()));
			continue;
		} else if (sOptimization.find("best") != string::npos) {
			string sModel = sOptimization.substr(sOptimization.find("best") + string("best").length());
			m_logger.m_fTrainingLog << TimeSystem::getTimeString("[Y/m/d H:i:s.f] ") << "[Optimization Add Best Model] " << sModel << endl;
			m_vBestAgentPool.push_back(make_pair(sModel, StatisticData()));
			boost::lock_guard<boost::mutex> lock(m_workerMutex);
			for (auto worker : m_vWorkers) {
				if (worker->getType() != ZERO_SELFPLAY) { continue; }
				worker->setIsIdle(true);
			}
			continue;
		}

		/*m_logger.m_fTrainingLog << TimeSystem::getTimeString("[Y/m/d H:i:s.f] ") << "[Optimization Add Best Model] " << sOptimization << endl;
		m_vBestAgentPool.push_back(make_pair(sOptimization, StatisticData()));
		boost::lock_guard<boost::mutex> lock(m_workerMutex);
		for (auto worker : m_vWorkers) {
			if (worker->getType() != ZERO_SELFPLAY) { continue; }
			worker->setIsIdle(true);
		}*/
		//m_evaluatorModelPool.push_back(sOptimization);
	}

	if (sOptimization == "Job_Done") {
		m_bOptimization = false;
		boost::lock_guard<boost::mutex> lock(m_workerMutex);
		for (auto worker : m_vWorkers) {
			if (worker->getType() != ZERO_OPTIMIZAITON) { continue; }
			worker->write("Optimization Finished");
			worker->setIsIdle(true);
		}
		m_logger.m_fTrainingLog << TimeSystem::getTimeString("[Y/m/d H:i:s.f] ") << "[Optimization] Finished." << endl;
	}
}

void ZeroWorkerHandler::handleOptimizationReadSgf()
{
	if (!m_bOptimizationReadSgf) { return; }

	{
		boost::lock_guard<boost::mutex> lock(m_workerMutex);
		for (auto worker : m_vWorkers) {
			if (worker->getType() != ZERO_OPTIMIZAITON || !worker->isIdle()) { continue; }
			worker->setIsIdle(false);
			worker->write("Optimization ReadSgf " + ToString(m_iteration - 1));
		}
	}

	// receive optimization model
	string sOptimization = "";
	while (true) {
		sOptimization = m_sharedData.getOneOptimization();
		if (sOptimization == "") { break; }
		if (sOptimization == "ReadSgf_Done") { ++m_optimizationCounter; }
	}

	if (m_optimizationCounter == WeichiConfigure::zero_optimization_num_worker) {
		m_bOptimizationReadSgf = false;
		m_bOptimization = true;
		boost::lock_guard<boost::mutex> lock(m_workerMutex);
		for (auto worker : m_vWorkers) {
			if (worker->getType() != ZERO_OPTIMIZAITON) { continue; }
			worker->setIsIdle(true);
		}
		m_logger.m_fTrainingLog << TimeSystem::getTimeString("[Y/m/d H:i:s.f] ") << "[Optimization] ReadSgf Finished." << endl;
	}
}

void ZeroWorkerHandler::handleEvaluator()
{
	if (!m_bEvaluator) {
		if (m_evaluatorModelPool.size() > 0) { setupEvaluator(); }
		return;
	}

	{
		boost::lock_guard<boost::mutex> lock(m_workerMutex);
		for (auto worker : m_vWorkers) {
			if (worker->getType() != ZERO_EVALUATOR || !worker->isIdle()) { continue; }
			worker->setIsIdle(false);
			worker->write(getEvaluatorConfigure(worker));
		}
	}

	// receive evaluator games
	while (true) {
		string sEvaluator = m_sharedData.getOneEvaluator();
		if (sEvaluator == "") { break; }

		string sSgfString = sEvaluator.substr(sEvaluator.find("("));
		string sSimpleSgf = deleteUnusedSgfTag(sSgfString);
		if (!addEvaluatorModelWinRate(sSimpleSgf)) { continue; }

		string sMoveNumber = sEvaluator.substr(0, sEvaluator.find("(") - 1);
		int total_game = m_evaluatorWinRate.m_black.getCount() + m_evaluatorWinRate.m_white.getCount();
		m_logger.m_fEvaluatorGame << total_game << " " << sMoveNumber << " " << sSimpleSgf << endl;
		m_logger.m_fEvaluatorDebugGame << total_game << " " << sMoveNumber << " " << sSgfString << endl;
	}

	if (canStopEvaluatorEarly()) {
		int bestIter = getModelIteration(m_vBestAgentPool.back().first);
		int evaluatorIter = getModelIteration(m_evaluatorModelPool.front());
		StatisticData totalWinRate = m_evaluatorWinRate.m_black;
		totalWinRate.add(m_evaluatorWinRate.m_white);
		m_logger.m_fTrainingLog << TimeSystem::getTimeString("[Y/m/d H:i:s.f] ") << "[Evaluator WinRate] "
			<< (totalWinRate.getMean() > WeichiConfigure::zero_evaluator_pass_threshold ? "* ": "")
			<< evaluatorIter << " vs. " << bestIter << " : "
			<< totalWinRate.toString(true) << " "
			<< "(Black: " << m_evaluatorWinRate.m_black.toString(true) << ","
			<< " White: " << m_evaluatorWinRate.m_white.toString(true) << ")" << endl;
		
		if (totalWinRate.getMean() >= WeichiConfigure::zero_evaluator_pass_threshold) {
			m_vBestAgentPool.push_back(make_pair(m_evaluatorModelPool.front(), StatisticData()));
			boost::lock_guard<boost::mutex> lock(m_workerMutex);
			for (auto worker : m_vWorkers) {
				if (worker->getType() != ZERO_SELFPLAY) { continue; }
				worker->setIsIdle(true);
			}
		}

		boost::lock_guard<boost::mutex> lock(m_workerMutex);
		for (auto worker : m_vWorkers) {
			if (worker->getType() != ZERO_EVALUATOR) { continue; }
			worker->write("Evaluator Finished");
			worker->setIsIdle(true);
		}

		m_logger.m_fEvaluatorGame.close();
		m_logger.m_fEvaluatorDebugGame.close();
		m_evaluatorModelPool.pop_front();
		m_bEvaluator = false;
	}
}

void ZeroWorkerHandler::setupSelfPlay()
{
	{
		boost::lock_guard<boost::mutex> lock(m_workerMutex);
		for (auto worker : m_vWorkers) {
			if (worker->getType() != ZERO_SELFPLAY) { continue; }
			worker->setIsIdle(true);
		}
	}

	m_iteration++;
	m_total_games = 0;
	if (m_iteration >= WeichiConfigure::zero_end_iteration) { exit(0); }

	string sSelfPlayGameFileName = WeichiConfigure::zero_train_directory + "/sgf/" + ToString(m_iteration) + ".sgf";
	m_logger.m_fSelfPlayGame.open(sSelfPlayGameFileName.c_str(), ios::out);
	string sSelfPlayDebugGameFileName = WeichiConfigure::zero_train_directory + "/sgf/debug/" + ToString(m_iteration) + ".sgf";
	m_logger.m_fSelfPlayDebugGame.open(sSelfPlayDebugGameFileName.c_str(), ios::out);

	// logger
	m_logger.m_fTrainingLog << TimeSystem::getTimeString("[Y/m/d H:i:s.f] ") << "[Iteration] =====" << m_iteration << "=====" << endl;
	m_logger.m_fTrainingLog << TimeSystem::getTimeString("[Y/m/d H:i:s.f] ") << "[SelfPlay] Start "
		<< getModelIteration(m_vBestAgentPool.back().first) << endl;

	//setupOptimization();
}

void ZeroWorkerHandler::setupOptimization()
{
	m_optimizationCounter = 0;
	m_bOptimizationReadSgf = true;
	string sConfigure = "Optimization " + m_sOptimizationConfigure;
	sConfigure = sConfigure.replace(sConfigure.find("@RANDOMSEED@"), sizeof("@RANDOMSEED@") - 1, ToString(getNextSeed()));

	// logger
	m_logger.m_fTrainingLog << TimeSystem::getTimeString("[Y/m/d H:i:s.f] ") << "[Optimization] Start." << endl;
}

void ZeroWorkerHandler::setupEvaluator()
{
	m_bEvaluator = true;
	m_evaluatorCounter = 0;
	m_evaluatorWinRate.reset();

	int bestIter = getModelIteration(m_vBestAgentPool.back().first);
	int evaluatorIter = getModelIteration(m_evaluatorModelPool.front());
	string sEvaluatorGameFileName = WeichiConfigure::zero_train_directory + "/evsgf/" + ToString(bestIter) + "vs" + ToString(evaluatorIter) + ".sgf";
	m_logger.m_fEvaluatorGame.open(sEvaluatorGameFileName.c_str(), ios::out);
	string sEvaluatorDebugGameFileName = WeichiConfigure::zero_train_directory + "/evsgf/debug/" + ToString(bestIter) + "vs" + ToString(evaluatorIter) + ".sgf";
	m_logger.m_fEvaluatorDebugGame.open(sEvaluatorDebugGameFileName.c_str(), ios::out);

	// logger
	m_logger.m_fTrainingLog << TimeSystem::getTimeString("[Y/m/d H:i:s.f] ") << "[Evaluator] Start "
		<< evaluatorIter << " vs. " << bestIter << endl;
}

Dual<string> ZeroWorkerHandler::randomSelfPlayAgentFromPool()
{
	Dual<string> agent;
	string sRandomAgent = m_vBestAgentPool[Random::nextInt(m_vBestAgentPool.size())].first;

	if (Random::nextInt(2) == 0) {
		agent.m_black = agent.m_white = m_vBestAgentPool.back().first;
	} else {
		if (Random::nextInt(2) == 0) {
			agent.m_black = m_vBestAgentPool.back().first;
			agent.m_white = sRandomAgent;
		} else {
			agent.m_black = sRandomAgent;
			agent.m_white = m_vBestAgentPool.back().first;
		}
	}

	return agent;
}

Dual<string> ZeroWorkerHandler::getEvaluatorAgentFromPool()
{
	Dual<string> agent;
	string sEvaluatorAgent = m_evaluatorModelPool.front();

	if (++m_evaluatorCounter % 2 == 0) {
		agent.m_black = m_vBestAgentPool.back().first;
		agent.m_white = sEvaluatorAgent;
	} else {
		agent.m_black = sEvaluatorAgent;
		agent.m_white = m_vBestAgentPool.back().first;
	}

	return agent;
}

string ZeroWorkerHandler::getSelfPlayConfigure(boost::shared_ptr<ZeroWorkerStatus> worker)
{
	string sConfigure = "Self-play " + m_sSelfPlayConfigure;
	sConfigure = sConfigure.replace(sConfigure.find("@RANDOMSEED@"), sizeof("@RANDOMSEED@") - 1, ToString(getNextSeed()));
	sConfigure = sConfigure.replace(sConfigure.find("@GPULIST@"), sizeof("@GPULIST@") - 1, worker->getGPUList());
	sConfigure = sConfigure.replace(sConfigure.find("@RESIGN_THRESHOLD@"), sizeof("@RESIGN_THRESHOLD@") - 1, ToString(WeichiConfigure::zero_resign_threshold));
	
	// machine name
	string sMachineName = worker->getName().substr(0, worker->getName().find("_"));
	sConfigure = sConfigure.replace(sConfigure.find("@MACHINENAME@"), sizeof("@MACHINENAME@") - 1, sMachineName);

	// model
	Dual<string> agent = randomSelfPlayAgentFromPool();
	string sBlackModel = "../../" + WeichiConfigure::zero_train_directory + "/model/" + agent.m_black;
	string sWhiteModel = "../../" + WeichiConfigure::zero_train_directory + "/model/" + agent.m_white;
	sConfigure = sConfigure.replace(sConfigure.find("@BLACKMODEL@"), sizeof("@BLACKMODEL@") - 1, sBlackModel);
	sConfigure = sConfigure.replace(sConfigure.find("@WHITEMODEL@"), sizeof("@WHITEMODEL@") - 1, sWhiteModel);

	return sConfigure;
}

string ZeroWorkerHandler::getEvaluatorConfigure(boost::shared_ptr<ZeroWorkerStatus> worker)
{
	string sConfigure = "Evaluator " + m_sEvaluatorConfigure;
	sConfigure = sConfigure.replace(sConfigure.find("@RANDOMSEED@"), sizeof("@RANDOMSEED@") - 1, ToString(getNextSeed()));
	sConfigure = sConfigure.replace(sConfigure.find("@GPULIST@"), sizeof("@GPULIST@") - 1, worker->getGPUList());

	// model
	Dual<string> agent = getEvaluatorAgentFromPool();
	string sBlackModel = "../../" + WeichiConfigure::zero_train_directory + "/model/" + agent.m_black;
	string sWhiteModel = "../../" + WeichiConfigure::zero_train_directory + "/model/" + agent.m_white;
	sConfigure = sConfigure.replace(sConfigure.find("@BLACKMODEL@"), sizeof("@BLACKMODEL@") - 1, sBlackModel);
	sConfigure = sConfigure.replace(sConfigure.find("@WHITEMODEL@"), sizeof("@WHITEMODEL@") - 1, sWhiteModel);

	return sConfigure;
}

string ZeroWorkerHandler::getOptimizationConfigure(boost::shared_ptr<ZeroWorkerStatus> worker)
{
	string sConfigure = "Optimization " + m_sOptimizationConfigure;
	string sModelFileNamePrefix = WeichiConfigure::zero_train_directory + "/model/iter_";
	sConfigure = sConfigure.replace(sConfigure.find("@RANDOMSEED@"), sizeof("@RANDOMSEED@") - 1, ToString(getNextSeed()));
	sConfigure = sConfigure.replace(sConfigure.find("@SGFMAXITERATION@"), sizeof("@SGFMAXITERATION@") - 1, ToString(m_iteration - 1));

	return sConfigure;
}

void ZeroWorkerHandler::adjustResignThreshold()
{
	std::sort(m_vResignThreshold.begin(), m_vResignThreshold.end());
	
	float fOldResignThreshold = WeichiConfigure::zero_resign_threshold;
	if (m_vResignThreshold.size() > 0) {
		const float FALSE_POSITIVES_RATIO = 0.05f;
		int index = m_vResignThreshold.size() * FALSE_POSITIVES_RATIO;
		WeichiConfigure::zero_resign_threshold = m_vResignThreshold[index];
		m_logger.m_fTrainingLog << TimeSystem::getTimeString("[Y/m/d H:i:s.f] ") << "[SelfPlay Disable Resign Games] "
			<< m_vResignThreshold.size() << endl;
	}

	m_logger.m_fTrainingLog << TimeSystem::getTimeString("[Y/m/d H:i:s.f] ") << "[SelfPlay Resign Threshold] "
		<< fOldResignThreshold << " -> " << WeichiConfigure::zero_resign_threshold << endl;

	m_vResignThreshold.clear();
}

void ZeroWorkerHandler::addResignThreshold(string sSgfString)
{
	if (sSgfString.find("Mini-winrate:") == string::npos) { return; }

	string sResignThreshold = sSgfString.substr(sSgfString.find("Mini-winrate: ") + ToString("Mini-winrate: ").length());
	sResignThreshold = sResignThreshold.substr(0, sResignThreshold.find(";"));

	m_vResignThreshold.push_back(atof(sResignThreshold.c_str()));
}

void ZeroWorkerHandler::addSelfPlayModelWinRate(string sSgfString)
{
	SgfLoader sgfLoader;
	sgfLoader.parseFromString(sSgfString);
	
	Color winner = sgfLoader.getWinner();
	string sBestModel = m_vBestAgentPool.back().first;
	for (int i = 0; i < m_vBestAgentPool.size() - 1; i++) {
		if (sgfLoader.getPlayerName(COLOR_BLACK) == m_vBestAgentPool[i].first) {
			if (sgfLoader.getPlayerName(COLOR_WHITE) != sBestModel) { return; }
			m_vBestAgentPool[i].second.add(winner == COLOR_BLACK);
		} else if (sgfLoader.getPlayerName(COLOR_WHITE) == m_vBestAgentPool[i].first) {
			if (sgfLoader.getPlayerName(COLOR_BLACK) != sBestModel) { return; }
			m_vBestAgentPool[i].second.add(winner == COLOR_WHITE);
		}
	}
}

bool ZeroWorkerHandler::addEvaluatorModelWinRate(string sSgfString)
{
	SgfLoader sgfLoader;
	sgfLoader.parseFromString(sSgfString);

	string sBestModel = m_vBestAgentPool.back().first;
	string sEvaluatorModel = m_evaluatorModelPool.front();
	if (sgfLoader.getPlayerName(COLOR_BLACK) == sEvaluatorModel) {
		if (sgfLoader.getPlayerName(COLOR_WHITE) != sBestModel) { return false; }
		if (m_evaluatorWinRate.m_black.getCount() >= WeichiConfigure::zero_evaluator_num_game / 2) { return false; }
		m_evaluatorWinRate.m_black.add(sgfLoader.getWinner() == COLOR_BLACK);
	} else if (sgfLoader.getPlayerName(COLOR_WHITE) == sEvaluatorModel) {
		if (sgfLoader.getPlayerName(COLOR_BLACK) != sBestModel) { return false; }
		if (m_evaluatorWinRate.m_white.getCount() >= WeichiConfigure::zero_evaluator_num_game / 2) { return false; }
		m_evaluatorWinRate.m_white.add(sgfLoader.getWinner() == COLOR_WHITE);
	} else { return false; }

	return true;
}

bool ZeroWorkerHandler::canStopEvaluatorEarly()
{
	const double dNGameThreshold = 60;
	const double dMaxEvaluatorGame = WeichiConfigure::zero_evaluator_num_game;
	const double dWinRateThreshold = WeichiConfigure::zero_evaluator_pass_threshold;

	StatisticData sCurrentWinRate;
	sCurrentWinRate.add(m_evaluatorWinRate.m_black);
	sCurrentWinRate.add(m_evaluatorWinRate.m_white);

	// upper/lower bound rule
	StatisticData sUpperBoundWinRate, sLowerBoundWinRate;
	sUpperBoundWinRate = sLowerBoundWinRate = sCurrentWinRate;
	sUpperBoundWinRate.add(1, dMaxEvaluatorGame - sCurrentWinRate.getCount());
	sLowerBoundWinRate.add(0, dMaxEvaluatorGame - sCurrentWinRate.getCount());
	if (sUpperBoundWinRate.getMean() < dWinRateThreshold || sLowerBoundWinRate.getMean() >= dWinRateThreshold) { return true; }

	// confidence rule (we check this every 10 games)
	if (static_cast<int>(sCurrentWinRate.getCount()) % 10 == 0
		&& sCurrentWinRate.getMean() < dWinRateThreshold
		&& sCurrentWinRate.getCount() >= dNGameThreshold) {
		if (sCurrentWinRate.getMean() == 0) { return true; }
		double dConfidence = calculateConfidence(sCurrentWinRate);
		if (sCurrentWinRate.getMean() + dConfidence < dWinRateThreshold) { return true; }
	}

	return false;
}

string ZeroWorkerHandler::deleteUnusedSgfTag(string sSgfString)
{
	int end = 0;
	string sNewSgfString = "";

	while ((end = sSgfString.find("*")) != string::npos) {
		sNewSgfString += sSgfString.substr(0, end);
		sSgfString = sSgfString.substr(end);
		sSgfString = sSgfString.substr(sSgfString.find("]"));
	}
	sNewSgfString += sSgfString;

	return sNewSgfString;
}

void ZeroWorkerHandler::keepAlive()
{
	broadcast("keep_alive");
	keepAliveTimer();
}

void ZeroWorkerHandler::keepAliveTimer()
{
	m_keepAliveTimer.expires_from_now(boost::posix_time::minutes(1));
	m_keepAliveTimer.async_wait(boost::bind(&ZeroWorkerHandler::keepAlive, this));
}