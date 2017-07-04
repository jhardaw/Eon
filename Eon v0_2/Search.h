#pragma once

#include "Board.h"
#include "UCI.h"
//#include "MoveGen.h"
//
//class Search
//{
//	public:
//		Search(Board& board);
//		Move BestMove();
//		
//	private:
//		int AlphaBeta(int alpha, int beta, int depth);
//		
//		// Properties
//		Board m_board;
//		MoveGen m_moveGen;
//};

namespace Search
{
	void Search(Board::Board &board, UCI::SearchInfo &searchInfo);
	void AnalyzeBoard(Board::Board &board, int depth);
	Board::Move BasicExtractPV(Board::Board &board, int depth);
}