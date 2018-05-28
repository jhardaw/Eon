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

namespace Board
{
	ECastleRight update_rights[64];

	Board::Board(void)
	{
		Clear();

		for (ESquare sqr = A1; sqr <= H8; sqr++)
		{
			if (sqr == E1)
				update_rights[sqr] = ~CASTLE_WHITE_KING & ~CASTLE_WHITE_QUEEN;
			else if (sqr == E8)
				update_rights[sqr] = ~CASTLE_BLACK_KING & ~CASTLE_BLACK_QUEEN;
			else if (sqr == H8)
				update_rights[sqr] = ~CASTLE_BLACK_KING;
			else if (sqr == A8)
				update_rights[sqr] = ~CASTLE_BLACK_QUEEN;
			else if (sqr == H1)
				update_rights[sqr] = ~CASTLE_WHITE_KING;
			else if (sqr == A1)
				update_rights[sqr] = ~CASTLE_WHITE_QUEEN;
			else
				update_rights[sqr] = ~CASTLE_NONE;
		}
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
		//m_StackIndex = 0;
	}

	int numMakeMoves = 0;
	void Board::MakeMove(Move move)
	{
		numMakeMoves++;
		
		// store undo info
		UndoStack.push_back(UndoHelper(m_half_move_count, m_castling_rights, m_ep_square));
		/*m_UndoStack[m_StackIndex++].half_move_count = m_half_move_count;
		m_UndoStack[m_StackIndex].castling_rights = m_castling_rights;
		m_UndoStack[m_StackIndex].ep_square = m_ep_square;*/
		
		// init
		EColor me = m_ToMove;
		EColor opp = !m_ToMove;
		ESquare from = move.GetFrom();
		ESquare to = move.GetTo();
		EPiece captured = move.GetCaptured();
		EPiece piece = PieceAt(from);
		EMoveFlag flag = move.GetFlags();
		assert(GetColor(piece) == me);
		assert(piece >= W_PAWN && piece <= B_KING);

		// update turn
		m_ToMove = opp;

		// update castling rights
		m_castling_rights &= update_rights[from] & update_rights[to];

		// update en passant
		m_ep_square = EP_NULL;

		// update move number (handle captures later)
		m_half_move_count++;
		if (GetType(piece) == PAWN)
			m_half_move_count = 0; // reset clock

		// remove captured piece
		if (captured != EMPTY)
		{
			assert(GetColor(captured) == opp);
			assert(captured >= W_PAWN && captured <= B_KING);
			RemovePiece(to);
			m_half_move_count = 0; // reset clock
		}

		// move the piece
		RemovePiece(from);
		SetPiece(piece, to);

		if (flag == PAWN_DOUBLE_PUSH)
		{
			m_ep_square = from + 8 - me * 16;
		}
		else if (isPromotion(move.GetFlags()))
		{
			RemovePiece(to);
			EPiece promoted = PromoteType(flag, me);
			SetPiece(promoted, to);
		}
		else if (flag == EP_CAPTURE)
		{
			ESquare sqr = to - 8 + me * 16;
			RemovePiece(sqr);
		}
		else if (flag == CASTLE) // Move the rook
		{
			if (to > from)  // kingside castle
			{
				EPiece rook = RemovePiece(to + 1);
				assert(rook == (me | ROOK));
				SetPiece(rook, to - 1);
			}
			else // queenside castle
			{
				EPiece rook = RemovePiece(to - 2);
				assert(rook == (me | ROOK));
				SetPiece(rook, to + 1);
			}
		}
	}

	void Board::UnmakeMove(Move move)
	{
		// Retrieve saved info
		/*m_half_move_count = m_UndoStack[m_StackIndex].half_move_count;
		m_castling_rights = m_UndoStack[m_StackIndex].castling_rights;
		m_ep_square = m_UndoStack[m_StackIndex--].ep_square;*/
		UndoHelper irreversible = UndoStack.back();
		m_half_move_count = irreversible.GetHalfMoveCount();
		m_castling_rights = irreversible.GetCastlingRights();
		m_ep_square = irreversible.GetEPSquare();
		UndoStack.pop_back();

		// init
		EColor opp = m_ToMove;
		EColor me = !m_ToMove;
		ESquare from = move.GetFrom();
		ESquare to = move.GetTo();
		EMoveFlag flag = move.GetFlags();
		EPiece piece = PieceAt(to);
		assert(GetColor(piece) == me);
		assert(piece >= W_PAWN && piece <= B_KING);

		RemovePiece(to);
		SetPiece(piece, from);
		if (move.GetCaptured() != EMPTY)
			SetPiece(move.GetCaptured(), move.GetTo());

		m_ToMove = me;

		if (flag == EP_CAPTURE)	// Replace Pawn captured during En Passant
		{
			SetPiece((opp | PAWN), to - 8 + me * 16);
		}
		else if (isPromotion(flag)) // Replace promoted piece with Pawn
		{
			RemovePiece(from);
			SetPiece(me | PAWN, from);
		}
		else if (flag == CASTLE) // Move the Rook
		{
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
				for (EPiece piece = W_PAWN; piece <= B_KING; piece++)
				{
					if (chr == PIECENAMES[piece])
					{
						SetPiece(piece, static_cast<ESquare>(row * 8 + col));
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
			m_castling_rights |= CASTLE_WHITE_KING;
		if (tokens[2].find("Q") != std::string::npos)
			m_castling_rights |= CASTLE_WHITE_QUEEN;
		if (tokens[2].find("k") != std::string::npos)
			m_castling_rights |= CASTLE_BLACK_KING;
		if (tokens[2].find("q") != std::string::npos)
			m_castling_rights |= CASTLE_BLACK_QUEEN;

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

		m_Eval = m_ToMove == WHITE ? Eval::Evaluate(*this) : -Eval::Evaluate(*this);
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

		if (Eval::EvaluateFast(*this) != Eval::Evaluate(*this))
		{
			std::cout <<"Eval doesn't match\n" << ToString();
			return false;
		}

		/*if (GetZorbist() != ZorbistKey(*this))
			return false;*/

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

	ECastleRight Board::GetCastlingRights() const
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
	}

	uint64_t Board::GetZorbist() const
	{
		return m_Zorbist;
	}

	void Board::SetPiece(EPiece piece, ESquare sqr)
	{
		assert(piece >= W_PAWN && piece <= B_KING);
		assert(sqr >= A1 && sqr <= H8);
		m_board_array[sqr] = piece;
		m_bitboards[piece] |= bitAt[sqr];
		m_bitboards[GetColor(piece)] |= bitAt[sqr];
		//Zorbist_SetPiece(m_Zorbist, piece, sqr);
		Eval::Eval_SetPiece(m_Eval, piece, sqr);
	}

	EPiece Board::RemovePiece(ESquare sqr)
	{
		assert(sqr >= A1 && sqr <= H8);
		EPiece piece = m_board_array[sqr];
		assert(piece >= W_PAWN && piece <= B_KING);
		m_board_array[sqr] = WHITE|EMPTY; // like mentioned above, this is a hack for now
		m_bitboards[piece] &= ~bitAt[sqr];
		m_bitboards[GetColor(piece)] &= ~bitAt[sqr];
		//Zorbist_RemovePiece(m_Zorbist, piece, sqr);
		Eval::Eval_RemovePiece(m_Eval, piece, sqr);

		return piece;
	}

	/*void Board::MovePiece(ESquare from, ESquare to)
	{
		assert(from >= A1 && from <= H8);
		assert(to >= A1 && to <= H8);
		EPiece piece = m_board_array[from];
	}*/

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
				if (m_castling_rights & CASTLE_WHITE_KING)
					asciiBoard += "K";
				if (m_castling_rights & CASTLE_WHITE_QUEEN)
					asciiBoard += "Q";
				if (m_castling_rights & CASTLE_BLACK_KING)
					asciiBoard += "k";
				if (m_castling_rights & CASTLE_BLACK_QUEEN)
					asciiBoard += "q";

				asciiBoard += "\tEP Square: ";
				if (m_ep_square == EP_NULL)
					asciiBoard += "-";
				else
					asciiBoard += std::to_string(m_ep_square);
			}
			if (row == 5)
			{
				asciiBoard += "\tMove: " + std::to_string(m_move_count);
				asciiBoard += "\tHalf Move: " + std::to_string(m_half_move_count);
			}
			if (row == 4)
			{
				asciiBoard += "\tEval: " + std::to_string(Eval::EvaluateFast(*this)) + "  (" + std::to_string(Eval::Evaluate(*this)) + ")";
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

	UndoHelper::UndoHelper(uint8_t half_move_count, ECastleRight castling_rights, ESquare ep_square)
	{
		m_half_move_count = half_move_count;
		m_castling_rights = castling_rights;
		m_ep_square = ep_square;
	}

	uint8_t UndoHelper::GetHalfMoveCount()
	{
		return m_half_move_count;
	}

	ECastleRight UndoHelper::GetCastlingRights()
	{
		return m_castling_rights;
	}

	ESquare UndoHelper::GetEPSquare()
	{
		return m_ep_square;
	}
}