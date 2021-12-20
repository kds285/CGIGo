#include "WeichiEdgeHandler.h"
#include "WeichiBoard.h"

void WeichiEdgeHandler::findFullBoardEdges()
{
	for( StaticBoard::iterator it=StaticBoard::getIterator(); it; ++it ) {
		const WeichiGrid& center_grid = m_board->getGrid(*it);
		if( !center_grid.isEmpty() ) { continue; }

		assertToFile( center_grid.isEmpty(), m_board );

		Color center_color = center_grid.getColor();
		const uint patternIndex = center_grid.getPattern().getIndex();
		const Vector<WeichiEdgeBits,8>& vEdgeBits = StaticBoard::getEdgeBits(patternIndex,center_color);
		const Vector<uint,8>& vEdgeDirection = StaticBoard::getEdgeDirection(patternIndex,center_color);
		for( uint dir=0; dir<vEdgeDirection.size(); dir++ ) {
			WeichiGrid& grid = m_board->getGrid(center_grid,vEdgeDirection[dir]);
			setEachEdgeBits(grid,vEdgeBits[dir]);
		}
	}
}

void WeichiEdgeHandler::removeAllEdges()
{
	for( StaticBoard::iterator it=StaticBoard::getIterator(); it; ++it ) {
		WeichiGrid& grid = m_board->getGrid(*it);
		grid.setEdge(NULL);
	}

	clear();
}

void WeichiEdgeHandler::setEachEdgeBits( WeichiGrid& grid, WeichiEdgeBits edgeBits )
{
	const StaticEdge& staticEdge = grid.getStaticEdge();

	WeichiEdge* gridEdge = grid.getEdge();
	if( gridEdge==NULL ) {
		gridEdge = m_edges.NewOne();
		gridEdge->init();
	}

	uint edgeType;
	while( (edgeType=edgeBits.bitScanForward())!=-1 ) {
		const WeichiEdge& edge = staticEdge.getEdge(WeichiEdgeType(edgeType));

		gridEdge->addRealStone(edge.getRealStone());
		gridEdge->addEndStone(edge.getEndStone());
		gridEdge->addErodedStone(edge.getErodedStone());
		gridEdge->addVirtualCAStone(edge.getVirtualCAStone());
		gridEdge->addCAStone(edge.getCAStone());
		gridEdge->addThreatStone(edge.getThreatStone());
	}

	grid.setEdge(gridEdge);
}