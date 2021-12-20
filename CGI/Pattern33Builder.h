#ifndef PATTERN33BUILDER_H
#define PATTERN33BUILDER_H

#include "PatternUtils.h"
#include "Pattern33Entry.h"
#include "Pattern33Table.h"
#include <string>
#include <map>
#define TEST_PATTERN_BUILD 0
using std::string;

namespace pattern33 {
#define RegisterSetter(name) \
    RegSetter ( #name, &Pattern33Table::set##name )  

class Pattern33Builder 
{
    typedef void (*PatternSetter)(Pattern33Entry&, uint, Symbol[13]);
private:
    string m_error ;
    Pattern33Entry* m_entries ;
    std::map<string, PatternSetter> m_mapFn ;
    bool useByProcedure ;
public:
    Pattern33Builder(); 
    inline void initByGenerator();
    bool build(const string& dirname);
    string getError() const { return m_error ; }
    void self_test() ; 

private:
	bool loadPreBuilt() ;
	void storePreBuilt() ;
	void RegSetter(const string& ID, PatternSetter fn);
	bool setError(const string& msg) { m_error=msg; return false ; }
	void buildByProcedure ();
	static void buildProcedure (Pattern33Entry&, uint, Symbol[]) ;
	static int buildCutArea( Pattern33Entry& , uint , Symbol [], Symbol ) ;
	bool buildPattern(const PatternFeature& feature) ;
	void buildPattern_r(Symbol symbols[13], uint value, PatternSetter fn, int depth = 0) ;
	int getRadius3PatternIndex(Symbol symbols[13]);
	void RotateReflect (Symbol symbols[13], uint method) ;
	uint RotateReflect (DataType dt, uint value, uint method) ;
	void printPattern(Symbol[]); 
	PatternFeature getDual ( const PatternFeature& feature ) ; 
};

inline void Pattern33Builder::initByGenerator()
{
    RegisterSetter(CompleteEmpty) ; 
    RegisterSetter(EmptyGridCount) ; 
    RegisterSetter(BlackGridCount) ; 
    RegisterSetter(WhiteGridCount) ; 
    RegisterSetter(EmptyAdjGridCount) ; 
    RegisterSetter(EmptyDiagGridCount) ; 
    RegisterSetter(BlackAdjGridCount) ; 
    RegisterSetter(WhiteAdjGridCount) ; 
    RegisterSetter(BlackDiagGridCount) ; 
    RegisterSetter(WhiteDiagGridCount) ; 
    RegisterSetter(BlackCutArea) ; 
    RegisterSetter(WhiteCutArea) ; 
    RegisterSetter(Black2LibJump) ; 
    RegisterSetter(White2LibJump) ; 
    RegisterSetter(BlackAnyAdjNeighbor) ; 
    RegisterSetter(WhiteAnyAdjNeighbor) ; 
    RegisterSetter(BlackApproachLib) ; 
    RegisterSetter(WhiteApproachLib) ; 
    RegisterSetter(BlackAtariLadder) ; 
    RegisterSetter(WhiteAtariLadder) ; 
    RegisterSetter(BlackAtariLadderCheckPoint) ; 
    RegisterSetter(WhiteAtariLadderCheckPoint) ; 
    RegisterSetter(BlackBamboo) ; 
    RegisterSetter(WhiteBamboo) ; 
    RegisterSetter(BlackCloseDoor) ; 
    RegisterSetter(WhiteCloseDoor) ; 
    RegisterSetter(BlackCloseDoorDir) ; 
    RegisterSetter(WhiteCloseDoorDir) ; 
    RegisterSetter(BlackLine1CloseDoor) ; 
    RegisterSetter(WhiteLine1CloseDoor) ; 
    RegisterSetter(BlackConnectCutPoint) ; 
    RegisterSetter(WhiteConnectCutPoint) ; 
    RegisterSetter(ConnectWithOneEye) ; 
    RegisterSetter(BlackDeadOnlyOneEye3NodesSet1) ; 
    RegisterSetter(WhiteDeadOnlyOneEye3NodesSet1) ; 
    RegisterSetter(BlackDeadOnlyOneEye3NodesSetRotation) ; 
    RegisterSetter(WhiteDeadOnlyOneEye3NodesSetRotation) ; 
    RegisterSetter(BlackDeadOnlyOneEye4NodesSet1) ; 
    RegisterSetter(WhiteDeadOnlyOneEye4NodesSet1) ; 
    RegisterSetter(BlackDeadOnlyOneEye4NodesSetRotation) ; 
    RegisterSetter(WhiteDeadOnlyOneEye4NodesSetRotation) ; 
    RegisterSetter(BlackDeadOnlyOneEye5NodesSet1) ; 
    RegisterSetter(WhiteDeadOnlyOneEye5NodesSet1) ; 
    RegisterSetter(BlackDeadOnlyOneEye5NodesSetRotation) ; 
    RegisterSetter(WhiteDeadOnlyOneEye5NodesSetRotation) ; 
    RegisterSetter(BlackDeadOnlyOneEye6NodesSet1) ; 
    RegisterSetter(WhiteDeadOnlyOneEye6NodesSet1) ; 
    RegisterSetter(BlackDeadOnlyOneEye6NodesSetRotation) ; 
    RegisterSetter(WhiteDeadOnlyOneEye6NodesSetRotation) ; 
    RegisterSetter(BlackDiagonal) ; 
    RegisterSetter(WhiteDiagonal) ; 
    RegisterSetter(BlackDiagonalClosedArea) ; 
    RegisterSetter(WhiteDiagonalClosedArea) ; 
    RegisterSetter(BlackDiagonalProduce) ; 
    RegisterSetter(WhiteDiagonalProduce) ; 
    RegisterSetter(BlackDoubleKnightMove) ; 
    RegisterSetter(WhiteDoubleKnightMove) ; 
    RegisterSetter(Black1Diagonal_RD) ; 
    RegisterSetter(White1Diagonal_RD) ; 
    RegisterSetter(Black2Diagonal_LU) ; 
    RegisterSetter(White2Diagonal_LU) ; 
    RegisterSetter(Black2Diagonal_LD) ; 
    RegisterSetter(White2Diagonal_LD) ; 
    RegisterSetter(Black3Diagonal_RU) ; 
    RegisterSetter(White3Diagonal_RU) ; 
    RegisterSetter(Black0Diagonal_RD) ; 
    RegisterSetter(White0Diagonal_RD) ; 
    RegisterSetter(Black3Diagonal_LU) ; 
    RegisterSetter(White3Diagonal_LU) ; 
    RegisterSetter(Black0Diagonal_RU) ; 
    RegisterSetter(White0Diagonal_RU) ; 
    RegisterSetter(Black1Diagonal_LD) ; 
    RegisterSetter(White1Diagonal_LD) ; 
    RegisterSetter(Black0Jump_R) ; 
    RegisterSetter(White0Jump_R) ; 
    RegisterSetter(Black2Jump_L) ; 
    RegisterSetter(White2Jump_L) ; 
    RegisterSetter(Black1Jump_D) ; 
    RegisterSetter(White1Jump_D) ; 
    RegisterSetter(Black3Jump_U) ; 
    RegisterSetter(White3Jump_U) ; 
    RegisterSetter(Black0Knight_RRU) ; 
    RegisterSetter(White0Knight_RRU) ; 
    RegisterSetter(Black5Knight_LLD) ; 
    RegisterSetter(White5Knight_LLD) ; 
    RegisterSetter(Black0Knight_RRD) ; 
    RegisterSetter(White0Knight_RRD) ; 
    RegisterSetter(Black6Knight_LLU) ; 
    RegisterSetter(White6Knight_LLU) ; 
    RegisterSetter(Black1Knight_RDD) ; 
    RegisterSetter(White1Knight_RDD) ; 
    RegisterSetter(Black6Knight_LUU) ; 
    RegisterSetter(White6Knight_LUU) ; 
    RegisterSetter(Black1Knight_LDD) ; 
    RegisterSetter(White1Knight_LDD) ; 
    RegisterSetter(Black7Knight_RUU) ; 
    RegisterSetter(White7Knight_RUU) ; 
    RegisterSetter(Black4Knight_RRD) ; 
    RegisterSetter(White4Knight_RRD) ; 
    RegisterSetter(Black2Knight_LLU) ; 
    RegisterSetter(White2Knight_LLU) ; 
    RegisterSetter(Black2Knight_LLD) ; 
    RegisterSetter(White2Knight_LLD) ; 
    RegisterSetter(Black7Knight_RRU) ; 
    RegisterSetter(White7Knight_RRU) ; 
    RegisterSetter(Black5Knight_LDD) ; 
    RegisterSetter(White5Knight_LDD) ; 
    RegisterSetter(Black3Knight_RUU) ; 
    RegisterSetter(White3Knight_RUU) ; 
    RegisterSetter(Black4Knight_RDD) ; 
    RegisterSetter(White4Knight_RDD) ; 
    RegisterSetter(Black3Knight_LUU) ; 
    RegisterSetter(White3Knight_LUU) ; 
    RegisterSetter(Black1Bamboo_RD) ; 
    RegisterSetter(White1Bamboo_RD) ; 
    RegisterSetter(Black3Bamboo_RU) ; 
    RegisterSetter(White3Bamboo_RU) ; 
    RegisterSetter(Black1Bamboo_LD) ; 
    RegisterSetter(White1Bamboo_LD) ; 
    RegisterSetter(Black3Bamboo_LU) ; 
    RegisterSetter(White3Bamboo_LU) ; 
    RegisterSetter(Black0Bamboo_UR) ; 
    RegisterSetter(White0Bamboo_UR) ; 
    RegisterSetter(Black2Bamboo_UL) ; 
    RegisterSetter(White2Bamboo_UL) ; 
    RegisterSetter(Black0Bamboo_DR) ; 
    RegisterSetter(White0Bamboo_DR) ; 
    RegisterSetter(Black2Bamboo_DL) ; 
    RegisterSetter(White2Bamboo_DL) ; 
    RegisterSetter(Black1hBamboo_SLD) ; 
    RegisterSetter(White1hBamboo_SLD) ; 
    RegisterSetter(Black3hBamboo_LLU) ; 
    RegisterSetter(White3hBamboo_LLU) ; 
    RegisterSetter(Black1hBamboo_SRD) ; 
    RegisterSetter(White1hBamboo_SRD) ; 
    RegisterSetter(Black3hBamboo_LRU) ; 
    RegisterSetter(White3hBamboo_LRU) ; 
    RegisterSetter(Black0hBamboo_LDR) ; 
    RegisterSetter(White0hBamboo_LDR) ; 
    RegisterSetter(Black2hBamboo_SDL) ; 
    RegisterSetter(White2hBamboo_SDL) ; 
    RegisterSetter(Black0hBamboo_LUR) ; 
    RegisterSetter(White0hBamboo_LUR) ; 
    RegisterSetter(Black2hBamboo_SUL) ; 
    RegisterSetter(White2hBamboo_SUL) ; 
    RegisterSetter(Black1hBamboo_LRD) ; 
    RegisterSetter(White1hBamboo_LRD) ; 
    RegisterSetter(Black3hBamboo_SRU) ; 
    RegisterSetter(White3hBamboo_SRU) ; 
    RegisterSetter(Black1hBamboo_LLD) ; 
    RegisterSetter(White1hBamboo_LLD) ; 
    RegisterSetter(Black3hBamboo_SLU) ; 
    RegisterSetter(White3hBamboo_SLU) ; 
    RegisterSetter(Black0hBamboo_SUR) ; 
    RegisterSetter(White0hBamboo_SUR) ; 
    RegisterSetter(Black2hBamboo_LUL) ; 
    RegisterSetter(White2hBamboo_LUL) ; 
    RegisterSetter(Black0hBamboo_SDR) ; 
    RegisterSetter(White0hBamboo_SDR) ; 
    RegisterSetter(Black2hBamboo_LDL) ; 
    RegisterSetter(White2hBamboo_LDL) ; 
    RegisterSetter(Black0LHalfBamboo_SUR) ; 
    RegisterSetter(White0LHalfBamboo_SUR) ; 
    RegisterSetter(Black2LHalfBamboo_LUL) ; 
    RegisterSetter(White2LHalfBamboo_LUL) ; 
    RegisterSetter(Black0LHalfBamboo_SDR) ; 
    RegisterSetter(White0LHalfBamboo_SDR) ; 
    RegisterSetter(Black2LHalfBamboo_LDL) ; 
    RegisterSetter(White2LHalfBamboo_LDL) ; 
    RegisterSetter(Black1LHalfBamboo_SLD) ; 
    RegisterSetter(White1LHalfBamboo_SLD) ; 
    RegisterSetter(Black3LHalfBamboo_LLU) ; 
    RegisterSetter(White3LHalfBamboo_LLU) ; 
    RegisterSetter(Black1LHalfBamboo_SRD) ; 
    RegisterSetter(White1LHalfBamboo_SRD) ; 
    RegisterSetter(Black3LHalfBamboo_LRU) ; 
    RegisterSetter(White3LHalfBamboo_LRU) ; 
    RegisterSetter(Black0LHalfBamboo_LUR) ; 
    RegisterSetter(White0LHalfBamboo_LUR) ; 
    RegisterSetter(Black2LHalfBamboo_SUL) ; 
    RegisterSetter(White2LHalfBamboo_SUL) ; 
    RegisterSetter(Black0LHalfBamboo_LDR) ; 
    RegisterSetter(White0LHalfBamboo_LDR) ; 
    RegisterSetter(Black2LHalfBamboo_SDL) ; 
    RegisterSetter(White2LHalfBamboo_SDL) ; 
    RegisterSetter(Black1LHalfBamboo_LRD) ; 
    RegisterSetter(White1LHalfBamboo_LRD) ; 
    RegisterSetter(Black3LHalfBamboo_SRU) ; 
    RegisterSetter(White3LHalfBamboo_SRU) ; 
    RegisterSetter(Black1LHalfBamboo_LLD) ; 
    RegisterSetter(White1LHalfBamboo_LLD) ; 
    RegisterSetter(Black3LHalfBamboo_SLU) ; 
    RegisterSetter(White3LHalfBamboo_SLU) ; 
    RegisterSetter(Black1ZHalfBamboo_RD) ; 
    RegisterSetter(White1ZHalfBamboo_RD) ; 
    RegisterSetter(Black3ZHalfBamboo_LU) ; 
    RegisterSetter(White3ZHalfBamboo_LU) ; 
    RegisterSetter(Black1ZHalfBamboo_LD) ; 
    RegisterSetter(White1ZHalfBamboo_LD) ; 
    RegisterSetter(Black3ZHalfBamboo_RU) ; 
    RegisterSetter(White3ZHalfBamboo_RU) ; 
    RegisterSetter(Black0ZHalfBamboo_UR) ; 
    RegisterSetter(White0ZHalfBamboo_UR) ; 
    RegisterSetter(Black2ZHalfBamboo_DL) ; 
    RegisterSetter(White2ZHalfBamboo_DL) ; 
    RegisterSetter(Black0ZHalfBamboo_DR) ; 
    RegisterSetter(White0ZHalfBamboo_DR) ; 
    RegisterSetter(Black2ZHalfBamboo_UL) ; 
    RegisterSetter(White2ZHalfBamboo_UL) ; 
    RegisterSetter(Black0THalfBamboo_SR) ; 
    RegisterSetter(White0THalfBamboo_SR) ; 
    RegisterSetter(Black2THalfBamboo_LL) ; 
    RegisterSetter(White2THalfBamboo_LL) ; 
    RegisterSetter(Black1THalfBamboo_SD) ; 
    RegisterSetter(White1THalfBamboo_SD) ; 
    RegisterSetter(Black3THalfBamboo_LU) ; 
    RegisterSetter(White3THalfBamboo_LU) ; 
    RegisterSetter(Black0THalfBamboo_LR) ; 
    RegisterSetter(White0THalfBamboo_LR) ; 
    RegisterSetter(Black2THalfBamboo_SL) ; 
    RegisterSetter(White2THalfBamboo_SL) ; 
    RegisterSetter(Black1THalfBamboo_LD) ; 
    RegisterSetter(White1THalfBamboo_LD) ; 
    RegisterSetter(Black3THalfBamboo_SU) ; 
    RegisterSetter(White3THalfBamboo_SU) ; 
    RegisterSetter(Black3WallJump_U) ; 
    RegisterSetter(White3WallJump_U) ; 
    RegisterSetter(Black0WallJump_R) ; 
    RegisterSetter(White0WallJump_R) ; 
    RegisterSetter(Black1WallJump_D) ; 
    RegisterSetter(White1WallJump_D) ; 
    RegisterSetter(Black2WallJump_L) ; 
    RegisterSetter(White2WallJump_L) ; 
    RegisterSetter(Black3WallBamboo_RU) ; 
    RegisterSetter(White3WallBamboo_RU) ; 
    RegisterSetter(Black3WallBamboo_LU) ; 
    RegisterSetter(White3WallBamboo_LU) ; 
    RegisterSetter(Black0WallBamboo_DR) ; 
    RegisterSetter(White0WallBamboo_DR) ; 
    RegisterSetter(Black0WallBamboo_UR) ; 
    RegisterSetter(White0WallBamboo_UR) ; 
    RegisterSetter(Black1WallBamboo_RD) ; 
    RegisterSetter(White1WallBamboo_RD) ; 
    RegisterSetter(Black1WallBamboo_LD) ; 
    RegisterSetter(White1WallBamboo_LD) ; 
    RegisterSetter(Black2WallBamboo_DL) ; 
    RegisterSetter(White2WallBamboo_DL) ; 
    RegisterSetter(Black2WallBamboo_UL) ; 
    RegisterSetter(White2WallBamboo_UL) ; 
    RegisterSetter(BlackElephant) ; 
    RegisterSetter(WhiteElephant) ; 
    RegisterSetter(BlackElephantThreat1) ; 
    RegisterSetter(WhiteElephantThreat1) ; 
    RegisterSetter(BlackElephantThreat2) ; 
    RegisterSetter(WhiteElephantThreat2) ; 
    RegisterSetter(BlackElephantThreat3) ; 
    RegisterSetter(WhiteElephantThreat3) ; 
    RegisterSetter(BlackElephantThreat4) ; 
    RegisterSetter(WhiteElephantThreat4) ; 
    RegisterSetter(EmptyPosition) ; 
    RegisterSetter(BlackEye) ; 
    RegisterSetter(WhiteEye) ; 
    RegisterSetter(BlackFalseEye) ; 
    RegisterSetter(WhiteFalseEye) ; 
    RegisterSetter(BlackFixTrueEye) ; 
    RegisterSetter(WhiteFixTrueEye) ; 
    RegisterSetter(BlackFixPotentialTrueEye) ; 
    RegisterSetter(WhiteFixPotentialTrueEye) ; 
    RegisterSetter(BlackFixFalseEye) ; 
    RegisterSetter(WhiteFixFalseEye) ; 
    RegisterSetter(BlackGoodPatternOrder) ; 
    RegisterSetter(WhiteGoodPatternOrder) ; 
    RegisterSetter(BlackGoodPattern) ; 
    RegisterSetter(WhiteGoodPattern) ; 
    RegisterSetter(BlackHalfBamboo) ; 
    RegisterSetter(WhiteHalfBamboo) ; 
    RegisterSetter(BlackHalfBambooThreatPatternDirection) ; 
    RegisterSetter(WhiteHalfBambooThreatPatternDirection) ; 
    RegisterSetter(BlackHalfBambooThreats) ; 
    RegisterSetter(WhiteHalfBambooThreats) ; 
    RegisterSetter(BlackJump) ; 
    RegisterSetter(WhiteJump) ; 
    RegisterSetter(BlackKnight) ; 
    RegisterSetter(WhiteKnight) ; 
    RegisterSetter(BlackKnightThreat1) ; 
    RegisterSetter(WhiteKnightThreat1) ; 
    RegisterSetter(BlackKnightThreat2) ; 
    RegisterSetter(WhiteKnightThreat2) ; 
    RegisterSetter(BlackIsLadder) ; 
    RegisterSetter(WhiteIsLadder) ; 
    RegisterSetter(BlackLadderDir) ; 
    RegisterSetter(WhiteLadderDir) ; 
    RegisterSetter(BlackLadderType) ; 
    RegisterSetter(WhiteLadderType) ; 
    RegisterSetter(BlackMakeTrueEye) ; 
    RegisterSetter(WhiteMakeTrueEye) ; 
    RegisterSetter(BlackMakePotentialTrueEye) ; 
    RegisterSetter(WhiteMakePotentialTrueEye) ; 
    RegisterSetter(BlackMakeFalseEye) ; 
    RegisterSetter(WhiteMakeFalseEye) ; 
    RegisterSetter(BlackMakeKo) ; 
    RegisterSetter(WhiteMakeKo) ; 
    RegisterSetter(BlackMakeKoCheckDir) ; 
    RegisterSetter(WhiteMakeKoCheckDir) ; 
    RegisterSetter(BlackMoGoHane) ; 
    RegisterSetter(WhiteMoGoHane) ; 
    RegisterSetter(BlackMoGoCut1) ; 
    RegisterSetter(WhiteMoGoCut1) ; 
    RegisterSetter(BlackMoGoCut1Exclude) ; 
    RegisterSetter(WhiteMoGoCut1Exclude) ; 
    RegisterSetter(BlackMoGoCut2) ; 
    RegisterSetter(WhiteMoGoCut2) ; 
    RegisterSetter(BlackMoGoEdge) ; 
    RegisterSetter(WhiteMoGoEdge) ; 
    RegisterSetter(BlackMultipleDiagonal) ; 
    RegisterSetter(WhiteMultipleDiagonal) ; 
    RegisterSetter(BlackNakade) ; 
    RegisterSetter(WhiteNakade) ; 
    RegisterSetter(BlackSZNakade) ; 
    RegisterSetter(WhiteSZNakade) ; 
    RegisterSetter(Black2LineNakade) ; 
    RegisterSetter(White2LineNakade) ; 
    RegisterSetter(BlackOneOfAdjPos) ; 
    RegisterSetter(WhiteOneOfAdjPos) ; 
    RegisterSetter(BlackOnlyOneLiberty) ; 
    RegisterSetter(WhiteOnlyOneLiberty) ; 
    RegisterSetter(BlackOppEmpty) ; 
    RegisterSetter(WhiteOppEmpty) ; 
    RegisterSetter(BlackPerfectClosedArea) ; 
    RegisterSetter(WhitePerfectClosedArea) ; 
    RegisterSetter(BlackPotentialTrueEye) ; 
    RegisterSetter(WhitePotentialTrueEye) ; 
    RegisterSetter(BlackPushCenter3Nodes) ; 
    RegisterSetter(WhitePushCenter3Nodes) ; 
    RegisterSetter(BlackPushCenter4Nodes) ; 
    RegisterSetter(WhitePushCenter4Nodes) ; 
    RegisterSetter(BlackPushCenter5Nodes) ; 
    RegisterSetter(WhitePushCenter5Nodes) ; 
    RegisterSetter(BlackPushCenter6Nodes) ; 
    RegisterSetter(WhitePushCenter6Nodes) ; 
    RegisterSetter(BlackReduceEye) ; 
    RegisterSetter(WhiteReduceEye) ; 
    RegisterSetter(BlackSplit) ; 
    RegisterSetter(WhiteSplit) ; 
    RegisterSetter(BlackStartCAPosition) ; 
    RegisterSetter(WhiteStartCAPosition) ; 
    RegisterSetter(BlackIsSplitCA) ; 
    RegisterSetter(WhiteIsSplitCA) ; 
    RegisterSetter(BlackEdge) ; 
    RegisterSetter(WhiteEdge) ; 
    RegisterSetter(BlackStrongCutTwoSidePoint) ; 
    RegisterSetter(WhiteStrongCutTwoSidePoint) ; 
    RegisterSetter(BlackEdgeIsBorder) ; 
    RegisterSetter(WhiteEdgeIsBorder) ; 
    RegisterSetter(BlackStrongCut) ; 
    RegisterSetter(WhiteStrongCut) ; 
    RegisterSetter(BlackSequentialBamboo) ; 
    RegisterSetter(WhiteSequentialBamboo) ; 
    RegisterSetter(BlackStrongCutTwoSidePoint1) ; 
    RegisterSetter(WhiteStrongCutTwoSidePoint1) ; 
    RegisterSetter(BlackStrongCutTwoSidePoint2) ; 
    RegisterSetter(WhiteStrongCutTwoSidePoint2) ; 
    RegisterSetter(BlackMultipleStrongCut1) ; 
    RegisterSetter(WhiteMultipleStrongCut1) ; 
    RegisterSetter(BlackMultipleStrongCut2) ; 
    RegisterSetter(WhiteMultipleStrongCut2) ; 
    RegisterSetter(BlackStrongCutTwoSidePoint3) ; 
    RegisterSetter(WhiteStrongCutTwoSidePoint3) ; 
    RegisterSetter(BlackMultipleStrongCut3) ; 
    RegisterSetter(WhiteMultipleStrongCut3) ; 
    RegisterSetter(BlackStrongCutTwoSidePoint4) ; 
    RegisterSetter(WhiteStrongCutTwoSidePoint4) ; 
    RegisterSetter(BlackMultipleStrongCut4) ; 
    RegisterSetter(WhiteMultipleStrongCut4) ; 
    RegisterSetter(BlackStrongCutSecondMainPoint) ; 
    RegisterSetter(WhiteStrongCutSecondMainPoint) ; 
    RegisterSetter(BlackStupidPattern) ; 
    RegisterSetter(WhiteStupidPattern) ; 
    RegisterSetter(BlackTerritory) ; 
    RegisterSetter(WhiteTerritory) ; 
    RegisterSetter(BlackTigerMouth) ; 
    RegisterSetter(WhiteTigerMouth) ; 
    RegisterSetter(BlackTigerThreats) ; 
    RegisterSetter(WhiteTigerThreats) ; 
    RegisterSetter(BlackTofuFour) ; 
    RegisterSetter(WhiteTofuFour) ; 
    RegisterSetter(BlackTrueEye) ; 
    RegisterSetter(WhiteTrueEye) ; 
    RegisterSetter(BlackTwoDiagonalClosedArea) ; 
    RegisterSetter(WhiteTwoDiagonalClosedArea) ; 
    RegisterSetter(BlackWeakCutTwoSidePoint) ; 
    RegisterSetter(WhiteWeakCutTwoSidePoint) ; 
    RegisterSetter(BlackWeakCut) ; 
    RegisterSetter(WhiteWeakCut) ; 
    RegisterSetter(BlackWeakCutSecondMainPoint) ; 
    RegisterSetter(WhiteWeakCutSecondMainPoint) ; 
    RegisterSetter(BlackCornerWeakCut) ; 
    RegisterSetter(WhiteCornerWeakCut) ; 
}
}

#endif