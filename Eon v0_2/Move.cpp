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
	Move::Move(ESquare from, ESquare to, MoveFlags flags, EPiece captured)
	{
		m_move = (flags << flags_shift) | (captured << captured_shift) | (to << to_shift) | (from << from_shift);
	}

	Move::Move(ESquare from, ESquare to, MoveFlags flags, EPiece captured, uint8_t score)
	{
		m_move = (flags << flags_shift) | (captured << captured_shift) | (to << to_shift) | (from << from_shift) | (score << score_shift);
	}

	Move::Move()
	{
		m_move = 0;
	}

	//Move::Move(std::string &lfAlgebra, Board::Board &board)
	//{
	//	m_move = 0;
	//	int toCol = lfAlgebra[2] - 'a';
	//	int toRow = lfAlgebra[3] - '1';
	//	m_move |= (uint32_t)(fromRow * 8 + fromCol) << from_shift;
	//	m_move |= (uint32_t)(toRow * 8 + toCol) << to_shift;
	//	m_move |= NO_FLAGS << flags_shift;
	//	m_move |= EMPTY << captured_shift;

	//	//if ((board.PieceAt(m_from) & 0xFE) == PAWN) // Check for promotion
	//	//{
	//	//	if (lfAlgebra[4] == 'q')
	//	//		m_flags |= QUEEN_PROMOTION;
	//	//	else if (lfAlgebra[4] == 'r')
	//	//		m_flags |= ROOK_PROMOTION;
	//	//	else if (lfAlgebra[4] == 'b')
	//	//		m_flags |= BISHOP_PROMOTION;
	//	//	else if (lfAlgebra[4] == 'n')
	//	//		m_flags |= KNIGHT_PROMOTION;
	//	//}
	//	if (board.PieceAt(GetFrom()) == B_KING)
	//	{
	//		if ((GetTo() - GetFrom()) == 2)
	//			m_move |= CASTLE << flags_shift;
	//		else if (GetFrom() - GetTo() == 2)
	//			m_move |= CASTLE << flags_shift;
	//	}
	//	else if (board.PieceAt(GetFrom()) == (W_KING))
	//	{
	//		if ((GetTo() - GetFrom()) == 2)
	//			m_move |= CASTLE << flags_shift;
	//		else if (GetFrom() - GetTo() == 2)
	//			m_move |= CASTLE << flags_shift;
	//	}
	//	std::cout << ToString();
	//}

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