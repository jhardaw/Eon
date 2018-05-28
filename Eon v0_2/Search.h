#pragma once

#include "Board.h"
#include "UCI.h"

namespace Search
{
	void Search(Board::Board &board, UCI::SearchInfo &searchInfo);
	void AnalyzeBoard(Board::Board &board, int depth);
	Board::Move BasicExtractPV(Board::Board &board, int depth);
}