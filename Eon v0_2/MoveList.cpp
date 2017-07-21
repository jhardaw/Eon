/***********************************************************************************
*  File: MoveList.cpp
*  Author: Ben Hardaway
*  Purpose: Implementation of the MoveList data structure
**********************************************************************************/

#include "Board.h"
#include "MoveList.h"
#include <cassert>

namespace Board
{
	uint8_t MVV_LVA(const Board &board, ESquare from, ESquare to);

	// Constructor9
	MoveList::MoveList(const Board &board) : m_board(board)
	{
		//m_board = board;
		m_listLength = 0;
	}

	// Selection sort for retreiving moves from list
	Move MoveList::GetMove(int index)
	{
		assert(index >= 0 && index < m_listLength);

		// Find the next "best" move
		int max = index;
		for (int ii = index + 1; ii < m_listLength; ii++)
		{
			if (m_list[ii] > m_list[max])
				max = ii;
		}

		// Swap moves to partially sort the list
		Move temp = m_list[index];
		m_list[index] = m_list[max];
		m_list[max] = temp;

		return m_list[index];
	}

	// Add move to the MoveList data structure
	void MoveList::AddMove(Move move)
	{
		assert(m_listLength < MAXMOVES);

		m_list[m_listLength++] = move;
	}

	// Remove move from the MoveList data structure
	void MoveList::RemoveMove(int index)
	{
		assert(index >= 0);
		assert(m_listLength > 0);

		m_list[index] = m_list[m_listLength - 1];
		m_listLength -= 1;
	}

	// Return the number of moves in the list
	int MoveList::GetLength()
	{
		return m_listLength;
	}

	void MoveList::Serialize(ESquare from, bitboard_t moves)
	{
		while (moves != 0)
		{
			ESquare to = static_cast<ESquare>(bit_scan_forward(moves));
			uint8_t score = MVV_LVA(m_board, from, to);
			AddMove(Move(from, to, NO_FLAGS, m_board.PieceAt(to), score));
			moves &= moves - 1;
		}
	}

	void MoveList::Serialize_Pawns(bitboard_t moves, int diff, EMoveFlag flag)
	{
		while (moves != 0)
		{
			ESquare to = static_cast<ESquare>(bit_scan_forward(moves));
			ESquare from = to - diff;
			uint8_t score = MVV_LVA(m_board, from, to);
			AddMove(Move(from, to, flag, m_board.PieceAt(to), score));
			moves &= moves - 1;
		}
	}

	uint8_t MVV_LVA(const Board &board, ESquare from, ESquare to)
	{
		int score = KING + GetType(board.PieceAt(to)) - GetType(board.PieceAt(from));
		assert(score >= 0 && score <= 255);
		return static_cast<uint8_t>(score);
	}

	// Represent the MoveList data structure as a string
	std::string MoveList::ToString()
	{
		std::string moves;
		for (int ii = 0; ii < m_listLength; ii++)
		{
			moves += m_list[ii].ToString() + "\n";
		}
		return moves;
	}
}