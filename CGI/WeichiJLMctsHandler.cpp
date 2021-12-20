#include "WeichiJLMctsHandler.h"
#include "concepts.h"

WeichiJLMctsHandler::WeichiJLMctsHandler() 
{
	genedMove = PASS_MOVE ; 
}

WeichiJLMctsHandler::WeichiJLMctsHandler(std::string& path) 
{
	transCoorJLMCTS(path);
	for( uint pos = 0 ; path.find_first_of("BW", pos) != string::npos ; pos += 5 ) {
		Color c = toColor(path[pos]) ;		
		string pos_str = path.substr(pos+2, 2) ;
		WeichiMove m(c, pos_str) ;
		mcts.play(m) ;
	}
	genedMove = PASS_MOVE ; 
}

void WeichiJLMctsHandler::transCoorJLMCTS(std::string& path) 
{
	for( int pos = 0 ; path.find_first_of("BW", pos) != std::string::npos ; pos += 5 ) {
		if( path[pos+3] != '@' )
			path[pos+3] = path[pos+3] - 'A' + '0' + 1 ; 
	}
}

WeichiMove WeichiJLMctsHandler::genmove()
{
	genedMove = mcts.genmove() ;
	return genedMove ;
}

string WeichiJLMctsHandler::getResult() const
{
	ostringstream oss ;	

	oss << toChar(genedMove.getColor()) << "[" << genedMove.toGtpPureString() << "]"
		<< "WR[" << WeichiGlobalInfo::getTreeInfo().m_bestSD.getMean() << "]"  
		<< "IC[" << WeichiGlobalInfo::getTreeInfo().m_nSimulation  << "]"
		<< "VC[" << WeichiGlobalInfo::getTreeInfo().m_bestSD.getCount() << "]" ;

	oss << "RWIN[0]" ;  

	oss << "CWIN" ;
	if( ( 1.0000000 - WeichiGlobalInfo::getTreeInfo().m_bestSD.getMean() ) < 0.001 ) 
		oss << "[" << WIN_STATUS <<  "]" ;
	else if( WeichiGlobalInfo::getTreeInfo().m_bestSD.getMean() < WeichiConfigure::ResignWinrateThreshold )
		oss << "[" << LOSS_STATUS <<  "]" ;
	else
		oss << "[" << UNKNOWN_STATUS <<  "]" ;

	oss << "LAST" ;

	if( WeichiGlobalInfo::get()->m_bIsLastCandidate ) 
		oss << "[1]" ;
	else
		oss << "[0]" ;

	oss << "AI_END" << endl ;

	return oss.str();
}
