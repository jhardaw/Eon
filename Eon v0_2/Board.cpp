/***********************************************************************************
*  File: Board.cpp
*  Author: Ben Hardaway
*  Purpose: C++ implementation of the Board class using bitboards
**********************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <cassert>
#include <iostream>
#include <string.h>
#include <sstream>
#include <time.h>
#include "Board.h"
#include "TranspoTable.h"
#include "Evaluate.h"
#include "Perft.h"

uint64_t bitAt[65];

namespace Board
{
	Board::Board(void)
	{
		Clear();
	}

	void Board::Clear()
	{
		int ii;
		for (ii = 0; ii < 14; ii++)
		{
			m_bitboards[ii] = BITBOARD_EMPTY;
		}

		for (ii = 0; ii < 64; ii++)
		{
			m_board_array[ii] = WHITE|EMPTY; // yeah its a hack for now
		}

		m_move_count = 1;
		m_half_move_count = 0;
		m_castling_rights = CASTLE_NONE;
		m_ep_square = EP_NULL;
		m_ToMove = WHITE;
		UndoStack = std::vector<UndoHelper>();
	}

	int numMakeMoves = 0;
	void Board::MakeMove(Move &move)
	{
		numMakeMoves++;
		//assert(move.Validate(*this));
		UndoStack.push_back(UndoHelper(m_half_move_count, m_castling_rights, m_ep_square));
		EPiece piece = RemovePiece(move.GetFrom());
		RemovePiece(move.GetTo());
		SetPiece(piece, move.GetTo());
		m_ep_square = EP_NULL;

		if (move.GetFlags() == PAWN_DOUBLE_PUSH)
		{
			// Possible En Passant capture in next ply
			if (m_ToMove == WHITE)
				m_ep_square = move.GetFrom() + 8;
			else
				m_ep_square = move.GetFrom() - 8;
		}
		else if (move.GetFlags() == EP_CAPTURE)
		{
			// Remove Pawn captured En Passant
			if (m_ToMove == WHITE)
				RemovePiece(move.GetTo() - 8);
			else
				RemovePiece(move.GetTo() + 8);
		}
		else if (move.GetFlags() == QUEEN_PROMOTION)
		{
			// Replace Pawn with Queen
			RemovePiece(move.GetTo());
			SetPiece(m_ToMove | QUEEN, move.GetTo());
		}
		else if (move.GetFlags() == ROOK_PROMOTION)
		{
			// Replace Pawn with Rook
			RemovePiece(move.GetTo());
			SetPiece(m_ToMove | ROOK, move.GetTo());
		}
		else if (move.GetFlags() == BISHOP_PROMOTION)
		{
			// Replace Pawn with Bishop
			RemovePiece(move.GetTo());
			SetPiece(m_ToMove | BISHOP, move.GetTo());
		}
		else if (move.GetFlags() == KNIGHT_PROMOTION)
		{
			// Replace Pawn with Knight
			RemovePiece(move.GetTo());
			SetPiece(m_ToMove | KNIGHT, move.GetTo());
		}
		else if (move.GetFlags() == CASTLE)
		{
			// Move the Rook
			if (move.GetTo() > move.GetFrom())  // kingside castle
			{
				EPiece rook = RemovePiece(move.GetTo() + 1);
				SetPiece(rook, move.GetTo() - 1);
			}
			else // queenside castle
			{
				EPiece rook = RemovePiece(move.GetTo() - 2);
				SetPiece(rook, move.GetTo() + 1);
			}

			// Update castling rights
			if (m_ToMove == WHITE)
				m_castling_rights &= ~WHITE_KING & ~WHITE_QUEEN;
			else
				m_castling_rights &= ~BLACK_KING & ~BLACK_QUEEN;
		}

		if (m_castling_rights)
		{
			if (move.GetFrom() == E1)
				m_castling_rights &= ~WHITE_KING & ~WHITE_QUEEN;
			else if (move.GetFrom() == E8)
				m_castling_rights &= ~BLACK_KING & ~BLACK_QUEEN;

			if (move.GetFrom() == H8 || move.GetTo() == H8)
				m_castling_rights &= ~BLACK_KING;
			else if (move.GetFrom() == A8 || move.GetTo() == A8)
				m_castling_rights &= ~BLACK_QUEEN;

			if (move.GetFrom() == H1 || move.GetTo() == H1)
				m_castling_rights &= ~WHITE_KING;
			else if (move.GetFrom() == A1 || move.GetTo() == A1)
				m_castling_rights &= ~WHITE_QUEEN;
		}

		m_half_move_count += 1;
		m_ToMove = !m_ToMove;
	}

	void Board::UnmakeMove(Move &move)
	{
		UndoHelper irreversible = UndoStack.back();
		m_half_move_count = irreversible.GetHalfMoveCount();
		m_castling_rights = irreversible.GetCastlingRights();
		m_ep_square = irreversible.GetEPSquare();
		UndoStack.pop_back();
		EPiece piece = RemovePiece(move.GetTo());
		SetPiece(piece, move.GetFrom());
		SetPiece(move.GetCaptured(), move.GetTo());
		m_ToMove = !m_ToMove;

		if (move.GetFlags() == EP_CAPTURE)
		{
			// Replace Pawn captured during En Passant
			if (m_ToMove == WHITE)
			{
				SetPiece((BLACK | PAWN), move.GetTo() - 8);
			}
			else
			{
				SetPiece((WHITE | PAWN), move.GetTo() + 8);
			}
		}
		else if (move.GetFlags() == KNIGHT_PROMOTION || move.GetFlags() == BISHOP_PROMOTION || move.GetFlags() == ROOK_PROMOTION || move.GetFlags() == QUEEN_PROMOTION)
		{
			// Replace promoted piece with Pawn
			RemovePiece(move.GetFrom());
			SetPiece(m_ToMove | PAWN, move.GetFrom());
		}
		else if (move.GetFlags() == CASTLE)
		{
			// Move the Rook
			if (move.GetTo() > move.GetFrom())  // kingside castle
			{
				EPiece rook = RemovePiece(move.GetTo() - 1);
				SetPiece(rook, move.GetTo() + 1);
			}
			else // queenside castle
			{
				EPiece rook = RemovePiece(move.GetTo() + 1);
				SetPiece(rook, move.GetTo() - 2);
			}
		}
	}

	void Board::ParseFEN(std::string &FENstring)
	{
		Clear();
		std::vector<std::string> tokens;
		split(FENstring, ' ', tokens);

		// fill the board
		int row = 7;
		int col = 0;
		for (char chr : tokens[0])
		{
			if (chr == '/')
			{
				row -= 1;
				col = 0;
			}
			else if (chr >= '0' && chr <= '9')
			{
				col += atoi(&chr);
			}
			else
			{
				for (int piece = 0; piece < 14; piece++)
				{
					if (chr == PIECENAMES[piece])
					{
						SetPiece(static_cast<EPiece>(piece), static_cast<ESquare>(row * 8 + col));
					}
				}
				col += 1;
			}
		}

		// set next move
		if (tokens[1] == "w")
			m_ToMove = WHITE;
		else
			m_ToMove = BLACK;

		// set castling rights
		if (tokens[2].find("K") != std::string::npos)
			m_castling_rights |= WHITE_KING;
		if (tokens[2].find("Q") != std::string::npos)
			m_castling_rights |= WHITE_QUEEN;
		if (tokens[2].find("k") != std::string::npos)
			m_castling_rights |= BLACK_KING;
		if (tokens[2].find("q") != std::string::npos)
			m_castling_rights |= BLACK_QUEEN;

		// set ep square
		if (tokens.size() >= 4)
		{
			if (tokens[3].compare("-") == 0)
				m_ep_square = EP_NULL;
			else
				m_ep_square = static_cast<ESquare>((tokens[3][0] - 'a') + (tokens[3][1] - '1') * 8);
		}

		// set half move clock
		if (tokens.size() >= 5)
		{
			if (tokens[4].compare("-") == 0)
				m_half_move_count = 0;
			else
				m_half_move_count = static_cast<uint8_t>(std::stoi(tokens[4]));
		}

		// set move 
		if (tokens.size() >= 6)
		{
			if (tokens[5].compare("-") == 0)
				m_move_count = 1;
			else
				m_move_count = std::stoi(tokens[5]);
		}

		m_Eval = m_ToMove == WHITE ? Evaluate(*this) : -Evaluate(*this);
		m_Zorbist = ZorbistKey(*this);
	}

	bool Board::Validate()
	{
		bitboard_t white = BITBOARD_EMPTY;
		bitboard_t black = BITBOARD_EMPTY;
		for (EPiece piece = W_PAWN; piece <= (B_KING); piece++)
		{
			if (GetColor(piece) == WHITE)
				white |= m_bitboards[piece];
			else
				black |= m_bitboards[piece];
		}
		if (white != m_bitboards[WHITE] || black != m_bitboards[BLACK])
			return false;

		for (ESquare sqr = A1; sqr <= H8; sqr++)
		{
			EPiece piece = m_board_array[sqr];
			if (piece != EMPTY && ((m_bitboards[piece] & bitAt[sqr]) == BITBOARD_EMPTY))
			{
				std::cout << ToString();
				return false;
			}
		}

		if (EvaluateFast(*this) != Evaluate(*this) || GetZorbist() != ZorbistKey(*this))
			return false;

		return true;
	}

	EPiece	Board::PieceAt(ESquare loc) const
	{
		return m_board_array[loc];
	}

	EPiece Board::PieceAt(int row, int col) const
	{
		return m_board_array[row * 8 + col];
	}

	bitboard_t Board::GetBitboard(int index) const
	{
		return m_bitboards[index];
	}

	EColor Board::GetPlayersTurn() const
	{
		return m_ToMove;
	}

	uint8_t Board::GetHalfMoveCount() const
	{
		return m_half_move_count;
	}

	ECastleRights Board::GetCastlingRights() const
	{
		return m_castling_rights;
	}

	ESquare Board::GetEPSquare() const
	{
		return m_ep_square;
	}

	int Board::GetEval() const
	{
		return m_Eval;
		/*if (m_color_to_move == WHITE)
		{
			return m_Eval;
		}
		else
		{
			return -m_Eval;
		}*/
	}

	uint64_t Board::GetZorbist() const
	{
		return m_Zorbist;
	}

	inline void Board::SetPiece(EPiece piece, ESquare sqr)
	{
		if (piece != EMPTY)
		{
			m_board_array[sqr] = piece;
			m_bitboards[piece] |= bitAt[sqr];
			m_bitboards[GetColor(piece)] |= bitAt[sqr];
			Zorbist_SetPiece(m_Zorbist, piece, sqr);
			Eval_SetPiece(m_Eval, piece, sqr);
		}
	}

	inline EPiece Board::RemovePiece(ESquare sqr)
	{
		EPiece piece = m_board_array[sqr];
		if (piece != EMPTY)
		{
			m_board_array[sqr] = WHITE|EMPTY; // like mentioned above, this is a hack for now
			m_bitboards[piece] &= ~bitAt[sqr];
			m_bitboards[GetColor(piece)] &= ~bitAt[sqr];
			Zorbist_RemovePiece(m_Zorbist, piece, sqr);
			Eval_RemovePiece(m_Eval, piece, sqr);
		}

		return piece;
	}

	std::string Board::ToString()
	{
		std::string asciiBoard;
		std::string border = std::string("\n  +---+---+---+---+---+---+---+---+\n");
		asciiBoard += border;
		for (int row = 7; row >= 0; row--)
		{
			asciiBoard += std::to_string(row + 1) + " |";
			for (int col = 0; col < 8; col++)
			{
				if (row * 8 + col == m_ep_square)
					asciiBoard += " : |";
				else
					asciiBoard += " " + std::string(1, PIECENAMES[PieceAt(row, col)]) + " |";
			}
			if (row == 7)
			{
				if (m_ToMove == WHITE)
					asciiBoard += "\tTo Move: White";
				else
					asciiBoard += "\tTo Move: Black";
			}
			if (row == 6)
			{
				asciiBoard += "\tCastling: ";
				if (m_castling_rights & WHITE_KING)
					asciiBoard += "K";
				if (m_castling_rights & WHITE_QUEEN)
					asciiBoard += "Q";
				if (m_castling_rights & BLACK_KING)
					asciiBoard += "k";
				if (m_castling_rights & BLACK_QUEEN)
					asciiBoard += "q";

				asciiBoard += "\tEP Square: ";
				if (m_ep_square == EP_NULL)
					asciiBoard += "-";
				else
					asciiBoard += m_ep_square;
			}
			if (row == 5)
			{
				asciiBoard += "\tMove: " + std::to_string(m_move_count);
				asciiBoard += "\tHalf Move: " + std::to_string(m_half_move_count);
			}
			if (row == 4)
			{
				asciiBoard += "\tEval: " + std::to_string(EvaluateFast(*this)) + "  (" + std::to_string(Evaluate(*this)) + ")";
			}
			if (row == 3)
			{
				asciiBoard += "\tZorbist: " + std::to_string(GetZorbist()) + "  (" + std::to_string(ZorbistKey(*this)) + ")";
			}
			if (row == 2)
			{
				asciiBoard += "\tnum makemoves: " + std::to_string(numMakeMoves);
			}
			asciiBoard += border;
		}
		asciiBoard += "    A   B   C   D   E   F   G   H\n";

		return asciiBoard;
	}

	UndoHelper::UndoHelper(uint8_t half_move_count, ECastleRights castling_rights, ESquare ep_square)
	{
		m_half_move_count = half_move_count;
		m_castling_rights = castling_rights;
		m_ep_square = ep_square;
	}

	uint8_t UndoHelper::GetHalfMoveCount()
	{
		return m_half_move_count;
	}

	ECastleRights UndoHelper::GetCastlingRights()
	{
		return m_castling_rights;
	}

	ESquare UndoHelper::GetEPSquare()
	{
		return m_ep_square;
	}
}