#ifndef H_JOBLEVELMCTS
#define H_JOBLEVELMCTS
#include <string>
#include "concepts.h"

#define UNKNOWN_STATUS 0
#define WIN_STATUS    1
#define LOSS_STATUS   2

class WeichiJLMctsHandler
{
	private:
		MCTS mcts;
		WeichiMove genedMove;
	public:
		WeichiJLMctsHandler() ;
		WeichiJLMctsHandler(std::string& path) ;
		WeichiMove genmove() ;
		static void transCoorJLMCTS(std::string& path) ;		
		string getResult() const ;
};

#endif