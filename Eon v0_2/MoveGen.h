#pragma once

/***********************************************************************************
 *  File: MoveGen.h
 *  Author: Ben Hardaway
 *  Purpose: Header file for the C++ implementation of the MoveGen class
 **********************************************************************************/

#include "Board.h"
#include "Move.h"
#include "MoveList.h"

namespace Board
{
	extern void MoveGen_Init();
	bool inCheck(Board &board, EColor color);

	void GeneratePseudoLegalMoves(Board &board, MoveList &list);
	void GenerateLegalMoves(Board &board, MoveList &list);
	void GenerateTacticalMoves(Board &board, MoveList &list);
}