#pragma once

/***********************************************************************************
*  File: Board.h
*  Author: Ben Hardaway
*  Purpose: Header file for the C++ implementation of the Board class using bitboards
**********************************************************************************/

#include <stdint.h>
#include <vector>
#include <string>
#include "Move.h"
#include "Bitboard.h"

namespace Board
{
	// Castling rights
	enum ECastleRights : uint8_t { CASTLE_NONE = 0x00, BLACK_KING = 0x01, BLACK_QUEEN = 0x02, WHITE_KING = 0x04, WHITE_QUEEN = 0x08 };

	inline ECastleRights operator |(ECastleRights a, ECastleRights b)
	{
		return static_cast<ECastleRights>(static_cast<int>(a) | static_cast<int>(b));
	}

	inline ECastleRights operator &(ECastleRights a, ECastleRights b)
	{
		return static_cast<ECastleRights>(static_cast<int>(a) & static_cast<int>(b));
	}

	inline ECastleRights operator ~(ECastleRights a)
	{
		return static_cast<ECastleRights>(~static_cast<int>(a));
	}

	inline ECastleRights& operator &=(ECastleRights &a, ECastleRights b)
	{
		return a = a & b;
	}

	inline ECastleRights& operator |=(ECastleRights &a, ECastleRights b)
	{
		return a = a | b;
	}

	// No en passant attack possible
	const ESquare EP_NULL = NULL_SQR;

	const char PIECENAMES[14] = { ' ', ' ', 'P', 'p', 'N', 'n', 'B', 'b', 'R', 'r', 'Q', 'q', 'K', 'k' };

	class UndoHelper
	{
	public:
		UndoHelper(uint8_t half_move_count, ECastleRights castling_rights, ESquare ep_square);
		uint8_t GetHalfMoveCount();
		ECastleRights GetCastlingRights();
		ESquare GetEPSquare();

	private:
		uint8_t m_half_move_count;
		ECastleRights m_castling_rights;
		ESquare m_ep_square;
	};

	class Board
	{
	public:
		Board();

		void MakeMove(Move &move);
		void UnmakeMove(Move &move);
		void ParseFEN(std::string &FEN);

		EPiece PieceAt(ESquare sqr) const;
		EPiece PieceAt(int row, int col) const;
		bitboard_t GetBitboard(int index) const;
		EColor GetPlayersTurn() const;
		uint8_t GetHalfMoveCount() const;
		ECastleRights GetCastlingRights() const;
		ESquare GetEPSquare() const;
		int GetEval() const;
		uint64_t GetZorbist() const;

		std::string ToString();
		bool Validate();

	private:
		// Private methods
		void Clear();
		void SetPiece(EPiece piece, ESquare sqr);
		EPiece RemovePiece(ESquare sqr);

		// Member variables
		std::vector<UndoHelper> UndoStack;
		bitboard_t m_bitboards[15];
		EPiece m_board_array[64];
		int m_move_count;
		uint8_t m_half_move_count;
		ECastleRights m_castling_rights;
		ESquare m_ep_square;
		EColor m_ToMove;
		int m_Eval; // Incrementally updated MB + Positional
		uint64_t m_Zorbist; // Incrementally updated zorbist key for transposition table
	};
}