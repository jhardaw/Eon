/***********************************************************************************
 *  File: Move.cpp
 *  Author: Ben Hardaway
 *  Purpose: C++ implementation of the Move class
 **********************************************************************************/

#include <stdio.h>
#include <iostream>
#include <cassert>
#include "Move.h"
#include "Board.h"

//=================================================================================
 //  Move_t layout: Total 32 bits
 //  ----------------------------------------------------
 //  from:  6 bits
 //  to:  6 bits
 //  captured:  4 bits
 //  flags:  3 bits?
 //  ----------------- 19 bits --------------------------
 //  value:  13 bits
 //=================================================================================

namespace Board
{
	Move::Move(ESquare from, ESquare to, EMoveFlag flags, EPiece captured)
	{
		m_move = (flags << flags_shift) | (captured << captured_shift) | (to << to_shift) | (from << from_shift);
	}

	Move::Move(ESquare from, ESquare to, EMoveFlag flags, EPiece captured, uint8_t score)
	{
		m_move = (flags << flags_shift) | (captured << captured_shift) | (to << to_shift) | (from << from_shift) | (score << score_shift);
	}

	Move::Move()
	{
		m_move = 0;
	}

	std::string Move::ToString()
	{
		std::string lfalgebra = std::string(translate(GetFrom())) + std::string(translate(GetTo()));
		/*if (m_flags&QUEEN_PROMOTION)
			lfalgebra += "=q";
		else if (m_flags&ROOK_PROMOTION)
			lfalgebra += "r";
		else if (m_flags&BISHOP_PROMOTION)
			lfalgebra += "b";
		else if (m_flags&KNIGHT_PROMOTION)
			lfalgebra += "n";
		if (m_captured != EMPTY)
			lfalgebra += "x" + PIECENAMES[m_captured];*/
		return lfalgebra;
	}

	std::string Move::ToVerboseString()
	{
		std::string lfalgebra = std::string(translate(GetFrom())) + std::string(translate(GetTo()));
		if (GetFlags() & QUEEN_PROMOTION)
			lfalgebra += "=q";
		else if (GetFlags()&ROOK_PROMOTION)
			lfalgebra += "r";
		else if (GetFlags()&BISHOP_PROMOTION)
			lfalgebra += "b";
		else if (GetFlags()&KNIGHT_PROMOTION)
			lfalgebra += "n";
		//if (GetCaptured() != EMPTY)
		lfalgebra += "x" + PIECENAMES[GetCaptured()];

		return lfalgebra;
	}

	//bool Move::Validate(Board::Board &board)
	//{
	//	if (GetFrom() < A1 || GetFrom() > H8)
	//		return false;
	//	if (GetTo() < A1 || GetTo() > H8)
	//		return false;
	//	EPiece piece = board.PieceAt(GetFrom());
	//	if (GetColor(piece) != board.GetPlayersTurn())
	//		return false;
	//	if (board.PieceAt(GetTo()) != GetCaptured())
	//		return false;
	//	if ((GetFlags() == QUEEN_PROMOTION || GetFlags() == ROOK_PROMOTION || GetFlags() == BISHOP_PROMOTION || GetFlags() == KNIGHT_PROMOTION) && GetType(piece) != PAWN)
	//		return false;
	//	if (GetFlags() == EP_CAPTURE && GetType(piece) != PAWN)
	//		return false;
	//	if (GetFlags() == CASTLE && GetType(piece) != KING)
	//		return false;
	//
	//	return true;
	//}

	std::string translate(ESquare sqr)
	{
		int row = static_cast<int>(sqr) / 8;
		int col = static_cast<int>(sqr) % 8;
		return std::string(1, (char)('a' + col)) + std::string(1, (char)('1' + row));
	}

	ESquare translate(std::string sqr)
	{
		int col = sqr[0] - 'a';
		int row = sqr[1] - '1';
		return static_cast<ESquare>(row * 8 + col);
	}
}