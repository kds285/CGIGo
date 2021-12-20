#ifndef WEICHI_GTP_UTIL
#define WEICHI_GTP_UTIL

#include "BasicType.h"
#include <vector>
#include "WeichiMove.h"
#include <string>
#include "MoveLocation.h"
#include "DbMove.h"

using namespace std ;

namespace WeichiGtpUtil
{
	vector<WeichiMove> getHandiCapStones(uint boardSize, uint stoneNum);
	MoveLocation WeichiMoveToLocation( WeichiMove wMove );
	WeichiMove dbMoveToWeichiMove( Color c, DbMove dMove );
}

#endif