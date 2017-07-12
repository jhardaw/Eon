#pragma once
/***********************************************************************************
 *  File: Move.h
 *  Author: Ben Hardaway
 *  Purpose: Header file for the C++ implementation of the Move class
 **********************************************************************************/

 
#include <stdint.h>
#include <string>
//#include "Board.h"

namespace Board
{
	enum ESquare : uint8_t {
		A1, B1, C1, D1, E1, F1, G1, H1,
		A2, B2, C2, D2, E2, F2, G2, H2,
		A3, B3, C3, D3, E3, F3, G3, H3,
		A4, B4, C4, D4, E4, F4, G4, H4,
		A5, B5, C5, D5, E5, F5, G5, H5,
		A6, B6, C6, D6, E6, F6, G6, H6,
		A7, B7, C7, D7, E7, F7, G7, H7,
		A8, B8, C8, D8, E8, F8, G8, H8, NULL_SQR
	};

	inline ESquare operator +(ESquare sqr, int a)
	{
		return static_cast<ESquare>(static_cast<int>(sqr) + a);
	}

	inline ESquare operator -(ESquare sqr, int a)
	{
		return static_cast<ESquare>(static_cast<int>(sqr) - a);
	}

	inline ESquare operator ++(ESquare &sqr, int)
	{
		return sqr = sqr + 1;
	}

	std::string translate(ESquare sqr);
	ESquare translate(std::string sqr);

	enum EColor : uint8_t { WHITE, BLACK };
	enum EPieceType : uint8_t { EMPTY= 0, PAWN = 1, KNIGHT = 2, BISHOP = 3, ROOK = 4, QUEEN = 5, KING = 6 };
	enum EPiece : uint8_t { W_PAWN = 2, B_PAWN = 3, W_KNIGHT = 4, B_KNIGHT = 5, W_BISHOP = 6, B_BISHOP = 7, W_ROOK = 8, B_ROOK = 9, W_QUEEN = 10, B_QUEEN = 11, W_KING = 12, B_KING = 13};
	inline EPiece operator | (EColor color, EPieceType type)
	{
		return static_cast<EPiece>((static_cast<int>(type) << 1) | static_cast<int>(color));
	}

	inline EColor operator !(EColor color)
	{
		return static_cast<EColor>(!static_cast<int>(color));
	}

	inline EColor GetColor(EPiece piece)
	{
		return static_cast<EColor>(static_cast<int>(piece) & 0x01);
	}

	inline EPieceType GetType(EPiece piece)
	{
		return static_cast<EPieceType>(static_cast<int>(piece) >> 1);
	}

	inline EPiece operator ++(EPiece &piece, int)
	{
		return piece = static_cast<EPiece>(piece + 1);
	}

	// Move flags
	enum MoveFlags : uint8_t {
		NO_FLAGS,
		PAWN_DOUBLE_PUSH,
		EP_CAPTURE,
		CASTLE,
		QUEEN_PROMOTION,
		ROOK_PROMOTION,
		BISHOP_PROMOTION,
		KNIGHT_PROMOTION
	};

	const int from_shift = 0;
	const int to_shift = 6;
	const int captured_shift = 12;
	const int flags_shift = 16;
	const int score_shift = 19;
	const int from_mask = 0x3F;
	const int to_mask = 0x3F;
	const int captured_mask = 0x0F;
	const int flags_mask = 0x07;
	const int score_mask = 0x1FFF;

	class Move
	{
	public:
		Move(ESquare from, ESquare to, MoveFlags flags, EPiece captured, uint8_t score);
		Move(ESquare from, ESquare to, MoveFlags flags, EPiece captured);
		Move();
		ESquare GetFrom()
		{
			return static_cast<ESquare>((m_move >> from_shift) & from_mask);
		}
		ESquare GetTo()
		{
			return static_cast<ESquare>((m_move >> to_shift) & to_mask);
		}
		MoveFlags GetFlags()
		{
			return static_cast<MoveFlags>((m_move >> flags_shift) & flags_mask);
		}
		EPiece GetCaptured()
		{
			return static_cast<EPiece>((m_move >> captured_shift) & captured_mask);
		}
		uint8_t GetScore()
		{
			return static_cast<uint8_t>((m_move >> score_shift) & score_mask);
		}
		uint32_t GetInt()
		{
			return m_move;
		}
		std::string ToString();
		std::string Move::ToVerboseString();

	private:
		uint32_t m_move;
	};

	inline bool operator >(Move a, Move b)
	{
		return a.GetInt() > b.GetInt();
	}
}
