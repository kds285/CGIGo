#include "WeichiLogger.h"
#include "UctChildIterator.h"
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

string WeichiLogger::m_sDirectory;
string WeichiLogger::m_sBestSequence[MAX_GAME_LENGTH];
string WeichiLogger::m_sWinRate[MAX_GAME_LENGTH];
WeichiMove WeichiLogger::m_bestMove;
Vector<WeichiMove, MAX_GAME_LENGTH> WeichiLogger::m_moves;

WeichiLogger::WeichiLogger()
{
}

void WeichiLogger::initialize( string sDirectory )
{
	m_sDirectory = sDirectory;
	boost::filesystem::create_directories(m_sDirectory);
}

void WeichiLogger::cleanUp( const WeichiBoard& board )
{
	uint upperIndex = 0;
	for ( ; upperIndex < board.getMoveList().size() && upperIndex < m_moves.size(); ++upperIndex ) {
		if ( board.getMoveList()[upperIndex] != m_moves[upperIndex] ) { break; }
	}
	++upperIndex;
	m_moves = board.getMoveList();

	for ( uint i = upperIndex + 1; i < MAX_GAME_LENGTH; ++i ) {
		m_sBestSequence[i].clear();
		m_sWinRate[i].clear();
	}
}

void WeichiLogger::logSgfRecord( const WeichiBoard& board, bool bWithBestSequence )
{
	string sFileName = m_sDirectory + "/FullGame.sgf";
	fstream fSgfRecord(sFileName.c_str(),ios::out);
	fSgfRecord << board.toSgfFileString() << endl;
	fSgfRecord.close();

	if ( bWithBestSequence ) { logSgfRecordWithBestSequence(board.toSgfFileString()); }
}

void WeichiLogger::logWinRate( const WeichiBoard& board, WeichiMove move )
{
	cleanUp(board);

	ostringstream oss;
	oss << ToString(board.getMoveList().size() + 1) << ","
		<< move.toGtpString() << ","
		<< ToString(WeichiGlobalInfo::getTreeInfo().m_bestSD.getMean()) << ","
		<< ToString(WeichiGlobalInfo::getTreeInfo().m_bestValueSD.getMean()) << ","
		<< (WeichiGlobalInfo::getTreeInfo().m_bestSD.getMean() + WeichiGlobalInfo::getTreeInfo().m_bestValueSD.getMean()) / 2;
	m_sWinRate[board.getMoveList().size() + 1] = oss.str();

	string sFileName = m_sDirectory + "/win_rate.csv";
	fstream fWinRate(sFileName.c_str(), ios::out);
	fWinRate << "No,Move,PO,VN,AVG" << endl;

	for ( uint i = 1; i < MAX_GAME_LENGTH ; ++i ) {
		if ( i <= board.getMoveList().size() + 1 ) {
			if ( !m_sWinRate[i].empty() ) {
				fWinRate << m_sWinRate[i] << endl;
			}
		} else {
			m_sWinRate[i].clear();
		}
	}

	fWinRate.close();
}

void WeichiLogger::logTreeInfo( const WeichiBoard& board, WeichiMove move, bool bWriteComment, bool isImportantChild(int deep, int index, const WeichiUctNode* parent, const WeichiUctNode* child) )
{
	cleanUp(board);
	m_bestMove = move;

	// generate best sequence string into m_sBestSequence
	m_sBestSequence[board.getMoveList().size() + 1] = getTreeInfo(board, move, bWriteComment, isImportantChild);

	// delete old best sequence sgf file
	boost::filesystem::path path(m_sDirectory);
	const boost::regex my_filter("^\\d+_.*\\.sgf");
	if ( boost::filesystem::exists(path) && boost::filesystem::is_directory(path) ) {
		boost::filesystem::directory_iterator end;
		for ( boost::filesystem::directory_iterator it(path); it != end; ++it ) {
			if ( !boost::filesystem::is_regular_file(it->status()) ) { continue; }

			boost::smatch match;
			if ( boost::regex_match(it->path().filename().string(), match, my_filter) ) {
				boost::filesystem::remove(it->path());
			}
		}
	}

	// generate best sequence sgf file
	for ( uint i = 1; i < MAX_GAME_LENGTH; ++i ) {
		if ( i <= board.getMoveList().size() + 1 ) {
			if ( !m_sBestSequence[i].empty() ) {
				string sMove = (i == board.getMoveList().size() + 1) ? move.toGtpString() : m_moves[i-1].toGtpString();
				string sFileName = m_sDirectory + "/" + ToString(i) + "_" + sMove + ".sgf";
				fstream fLog(sFileName.c_str(), ios::out);
				fLog << m_sBestSequence[i] << endl;
				fLog.close();
			}
		} else {
			m_sBestSequence[i].clear();
		}
	}
}

string WeichiLogger::getTreeInfo(const WeichiBoard & board, WeichiMove move, bool bWriteComment, bool isImportantChild(int deep, int index, const WeichiUctNode *parent, const WeichiUctNode *child))
{
	ostringstream oss;
	oss << board.toSgfFilePrefix();
	uint index;
	WeichiBitBoard blackBitBoard = board.getStoneBitBoard(COLOR_BLACK);
	if (blackBitBoard.bitCount() > 0) {
		oss << "AB";
		while ((index = blackBitBoard.bitScanForward()) != -1) {
			oss << "[" << WeichiMove(COLOR_BLACK, index).toSgfString(false) << "]";
		}
	}
	WeichiBitBoard whiteBitBoard = board.getStoneBitBoard(COLOR_WHITE);
	if (whiteBitBoard.bitCount() > 0) {
		oss << "AW";
		while ((index = whiteBitBoard.bitScanForward()) != -1) {
			oss << "[" << WeichiMove(COLOR_WHITE, index).toSgfString(false) << "]";
		}
	}
	deque<WeichiUctNode*> child;
	NodePtr<WeichiUctNode> pNode = WeichiGlobalInfo::getTreeInfo().m_pRoot;
	if (pNode->hasChildren()) {
		for (UctChildIterator<WeichiUctNode> it(pNode); it; ++it) {
			child.push_back(&(*it));
		}
		sort(child.begin(), child.end(),
			[](const WeichiUctNode* lhs, const WeichiUctNode* rhs) {
			return lhs->getUctData().getCount() > rhs->getUctData().getCount();
		}
		);

		oss << "LB";
		for (int i = 0; i < 5; ++i) {
			if (child[i]->getMove().isPass()) { continue; }
			if (!isBestSequenceImportantChild(1, i, &(*pNode), child[i])) { continue; }
			oss << "[" << child[i]->getMove().toSgfString(false) << ":" << round((child[i]->getUctData().getMean() + 1) / 2 * 100) << "]";
		}
	}

	oss << logTreeInfo_r(WeichiGlobalInfo::getTreeInfo().m_pRoot, bWriteComment, 1, isImportantChild);
	oss << ")";

	return oss.str();
}

bool WeichiLogger::isSaveTreeImportantChild(int deep, int index, const WeichiUctNode* parent, const WeichiUctNode* child)
{
	if ( child->getUctData().getCount() < parent->getUctData().getCount() / 100 ) { return false; }
	if ( child->getUctData().getCount() < WeichiGlobalInfo::getTreeInfo().m_pRoot->getUctData().getCount() / 1000 ) { return false; }

	return true;
}

bool WeichiLogger::isBestSequenceImportantChild(int deep, int index, const WeichiUctNode* parent, const WeichiUctNode* child)
{
	if ( child->getUctData().getCount() < 100 ) { return false; }
	if (deep == 1 && index < 5) { return true; }
	if (deep == 2 && index < 5 && parent->getMove() == m_bestMove) { return true; }
	if (index == 0 && child->getUctData().getCount() > parent->getUctData().getCount() / 100) { return true; }

	return false;
}

void WeichiLogger::logSgfRecordWithBestSequence( const string& sFullGame )
{
	string sSgfRecordWithBestSequence = sFullGame;
	sSgfRecordWithBestSequence.pop_back(); // pop ')'
	sSgfRecordWithBestSequence += ";B[tt]"; // add a pass to prevent confusing full game with best sequences

	// iterate every file to get best sequence string
	boost::filesystem::path p(m_sDirectory);
	boost::filesystem::directory_iterator end_it;
	string sBestSequence[MAX_GAME_LENGTH];

	// collect best sequence for each move from best sequence file
	for ( boost::filesystem::directory_iterator it(p); it != end_it; ++it ) {
		if ( boost::filesystem::is_regular_file(it->path()) ) {
			const string currentFilepath = it->path().string();
			const string currentFilename = it->path().filename().string();
			// here we want to find file like "101_C2.sgf"
			{
				// match .sgf in filename
				boost::smatch result;
				boost::regex pattern("\\.sgf");
				if ( !boost::regex_search(currentFilename, result, pattern) ) { continue; }
			}
			int moveNum = -1;
			{
				// match number in filename
				boost::smatch result;
				boost::regex pattern("^(\\d+)");
				if ( !boost::regex_search(currentFilename, result, pattern) ) { continue; }
				moveNum = boost::lexical_cast<int>(result[1]);
			}
			if ( moveNum < 0 || moveNum >= MAX_GAME_LENGTH ) {
				CERR() << "logSgfRecordWithBestSequence: moveNum error with file \"" << currentFilepath << "\"" << std::endl;
				continue;
			}

			// get all file content to sFileContent
			ifstream fFile(currentFilepath);
			string sFileContent;
			if ( !fFile.is_open() ) {
				CERR() << "logSgfRecordWithBestSequence: open error with file \"" << currentFilepath << "\"" << std::endl;
				continue;
			}

			fFile.seekg(0, std::ios::end);
			sFileContent.reserve(fFile.tellg());
			fFile.seekg(0, std::ios::beg);
			sFileContent.assign((std::istreambuf_iterator<char>(fFile)), std::istreambuf_iterator<char>());
			fFile.close();

			{
				// match best sequence in file content (from second '(;' to last ')', contain some best sequence and additional ')')
				boost::smatch result;
				boost::regex pattern("\\(.+?(\\(;(?:.|\\r|\\n)+\\))");
				if ( !boost::regex_search(sFileContent, result, pattern) ) {
					CERR() << "logSgfRecordWithBestSequence: best sequence file error with file \"" << currentFilepath << "\"" << std::endl;
					return;
				}
				sBestSequence[moveNum] = result.str(1);
				sBestSequence[moveNum].pop_back(); // delete additional ')'
			}
		}
	}

	// add best sequence
	for ( int i = MAX_GAME_LENGTH - 1; i >= 0; --i ) {
		if ( sBestSequence[i].size() == 0 ) { continue; }
		int pos;
		int occurrence = -1;
		for ( pos = 0; pos < sSgfRecordWithBestSequence.size(); ++pos ) {
			if ( sSgfRecordWithBestSequence[pos] == ';' ) { ++occurrence; }
			if ( occurrence == i ) { break; }
		}
		sSgfRecordWithBestSequence = sSgfRecordWithBestSequence.substr(0, pos) + "(" + sSgfRecordWithBestSequence.substr(pos) + ")";
		sSgfRecordWithBestSequence += sBestSequence[i];
	}

	sSgfRecordWithBestSequence.push_back(')');

	// save to file
	string sFileName = m_sDirectory + "/FullGameWithBestSequence.sgf";
	fstream fSgfRecordWithBestSequence(sFileName.c_str(), ios::out);
	fSgfRecordWithBestSequence << sSgfRecordWithBestSequence;
	fSgfRecordWithBestSequence.close();
}

string WeichiLogger::logTreeInfo_r( NodePtr<WeichiUctNode> pNode, bool bWriteComment, int deep, bool isImportantChild(int deep, int index, const WeichiUctNode* parent, const WeichiUctNode* child))
{
	ostringstream oss;
	if ( !pNode->hasChildren() ) {
		if (deep != 1 && bWriteComment) { oss << "]"; } // end of C[
		return oss.str();
	}

	if (deep == 1 && bWriteComment) { oss << "C["; }
	deque<WeichiUctNode*> child;
	WeichiUctNode* pChoose = nullptr;
	for ( UctChildIterator<WeichiUctNode> it(pNode); it; ++it ) {
		if (deep == 1 && it->getMove() == m_bestMove) { pChoose = &(*it); continue; }
		child.push_back(&(*it));
	}
	sort(child.begin(), child.end(),
		[](const WeichiUctNode* lhs, const WeichiUctNode* rhs) {
			return lhs->getUctData().getCount() > rhs->getUctData().getCount();
	}
	);
	if (deep == 1 && pChoose != nullptr) { child.push_front(pChoose); }

	// log all children to pNode's comment
	for ( int i = 0; i < child.size(); ++i ) {
		if ( child[i]->getMove().isPass() ) { continue; }

		if (bWriteComment) {
			//oss << "Move " << child[i]->getMove().toGtpString()
			//	<< ": PO(" << child[i]->getUctData().getMean() << "/" << child[i]->getUctData().getCount() << "), "
			//	<< "VN(" << child[i]->getVNValueData().getMean() << "/" << child[i]->getVNValueData().getCount() << ")\n";
			if ( deep == 1 && i == 0 ) {
				double dBlackWinRate = (child[i]->getUctData().getMean() + 1) / 2;
				if ( child[i]->getColor() == COLOR_WHITE ) { dBlackWinRate = 1.0 - dBlackWinRate; }
				oss << fixed << setprecision(1);
				oss << "B: " << dBlackWinRate*100.0 << "%, W: " << (1.0 - dBlackWinRate)*100.0 << "%";
			}
		}
	}
	if (bWriteComment) { oss << "]"; } // end of C[

	// log important children to its comment and call recursively
	for ( int i = 0; i < child.size(); ++i ) {
		if ( child[i]->getMove().isPass() ) { continue; }
		if ( !isImportantChild(deep, i, &(*pNode), child[i]) ) { continue; }

		for ( UctChildIterator<WeichiUctNode> it(pNode); it; ++it ) {
			if ( it->getMove() != child[i]->getMove() ) { continue; }

			oss << "(;" << it->getMove().toSgfString(true);
			if (bWriteComment) {
				//oss << "C[Win rate: " << it->getUctData().getMean() << ", Simulation count: " << it->getUctData().getCount() << "\n";
				//oss << "VN Win rate: " << it->getVNValueData().getMean() << ", VN count: " << it->getVNValueData().getCount() << "\n\n";
				double dBlackWinRate = (child[i]->getUctData().getMean() + 1) / 2;
				if ( it->getColor() == COLOR_WHITE ) { dBlackWinRate = 1.0 - dBlackWinRate; }
				oss << fixed << setprecision(1);
				oss << "C[B: " << dBlackWinRate*100.0 << "%, W: " << (1.0 - dBlackWinRate)*100.0 << "%";
			}

			oss << logTreeInfo_r(it, bWriteComment, deep+1, isImportantChild);
			oss << ")";
		}
	}

	return oss.str();
}
