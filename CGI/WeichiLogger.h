#ifndef WEICHILOGGER_H
#define WEICHILOGGER_H

#include "BasicType.h"
#include "WeichiBoard.h"
#include "WeichiGlobalInfo.h"
#include "WeichiUctNode.h"

class WeichiLogger
{
private:
	static string m_sDirectory;
	static string m_sBestSequence[MAX_GAME_LENGTH];
	static string m_sWinRate[MAX_GAME_LENGTH];
	static WeichiMove m_bestMove;
	static Vector<WeichiMove, MAX_GAME_LENGTH> m_moves;

public:
	WeichiLogger();

	static void initialize( string sDirectory );
	static void cleanUp( const WeichiBoard& board );
	static void logSgfRecord( const WeichiBoard& board, bool bWithBestSequence );
	static void logWinRate( const WeichiBoard& board, WeichiMove move );
	static void logTreeInfo( const WeichiBoard& board, WeichiMove move, bool bWriteComment, bool isImportantChild(int deep, int index, const WeichiUctNode* parent, const WeichiUctNode* child) );
	static string getTreeInfo(const WeichiBoard& board, WeichiMove move, bool bWriteComment, bool isImportantChild(int deep, int index, const WeichiUctNode* parent, const WeichiUctNode* child));

	static bool isSaveTreeImportantChild(int deep, int index, const WeichiUctNode* parent, const WeichiUctNode* child);
	static bool isBestSequenceImportantChild(int deep, int index, const WeichiUctNode* parent, const WeichiUctNode* child);

private:
	static void logSgfRecordWithBestSequence(const string& sFullGame);
	static string logTreeInfo_r( NodePtr<WeichiUctNode> pNode, bool bWriteComment, int deep, bool isImportantChild(int deep, int index, const WeichiUctNode* parent, const WeichiUctNode* child));
};

#endif