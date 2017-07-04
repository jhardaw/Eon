/***********************************************************************************
 *  File: Eon.h
 *  Author: Ben Hardaway
 *  Purpose: Header file defining the components of the Eon Chess Engine
 **********************************************************************************/
 
#ifndef EON_H
#define EON_H

#include <cstdint.h>

#define CASTLE_NONE 0x00
#define CASTLE_BLACK_KING 0x01 
#define CASTLE_BLACK_QUEEN 0x02
#define CASTLE_WHITE_KING 0x04
#define CASTLE_WHITE_QUEEN 0x08

#define EP_NULL 64

// Piece values
#define EMPTY 0
#define WHITE 0
#define BLACK 1 
#define PAWN 2
#define KNIGHT 4
#define BISHOP 6
#define ROOK 8
#define QUEEN 10
#define KING 12

// Move flags
#define NO_FLAGS 0x00
#define PAWN_DOUBLE_PUSH 0x01
#define EP_CAPTURE 0x02
#define CASTLE 0x4
#define QUEEN_PROMOTION 0x80
#define ROOK_PROMOTION 0x40
#define BISHOP_PROMOTION 0x20
#define KNIGHT_PROMOTION 0x10
#define PROMOTION 0xF0

typedef uint64_t bitboard_t;

const char PIECENAMES[14];

class Board
{
	public:
		Board();
		void GetMoves();
		void MakeMove(Move move);
		void UnmakeMove(Move move);
		void ParseFEN(char *FEN);
		uint8_t PieceAt(int loc);
		uint8_t PieceAt(int row, int col);
		bitboard_t GetBitboard(int index);
		uint8_t GetPlayersTurn();
		uint8_t GetHalfMoveCount();
		uint8_t GetCastlingRights();
		uint8_t GetEPSquare();
		void Perft(int depth);
		char *ToString();
	
	protected:
		bitboard_t m_bitboards[14];
		uint8_t m_board_array[64];
		uint8_t m_half_move_count;
		uint8_t m_castling_rights;
		uint8_t m_ep_square;
		uint8_t m_color_to_move;
};

class Move
{
	public:
		Move(uint8_t from, uint8_t to, uint8_t flags, uint8_t captured);
		Move(char *LFAlgebra);
		uint8_t GetFrom();
		uint8_t GetTo();
		uint8_t GetFlags();
		uint8_t GetCaptured();
		void ToString();
		
	private:
		uint8_t m_from;
		uint8_t m_to;
		uint8_t m_flags;
		uint8_t m_captured;
};

class MoveGen
{
	public:
		MoveGen(Board board);
		int remove_checks(Move *move_list, int list_length);
		bitboard_t player_attacks(int color);
		int in_check(uint8_t color);
		//int in_checkmate(board_t *board, uint8_t color);
		int add_queen_moves(Move *move_list, int list_length);
		int add_rook_moves(Move *move_list, int list_length);
		int add_bishop_moves(Move *move_list, int list_length);
		int add_king_moves(Move *move_list, int lisT_length);
		int add_knight_moves(Move *move_list, int list_length);
		int add_pawn_moves(Move *move_list, int list_length);
	private:
		Board m_board;
		int add_pawn_pushes(Move *move_list, int list_length);
		int add_pawn_attacks(Move *move_list, int list_length);
		int add_move_with_diff(bitboard_t bitboard, Move *move_list, int list_length, int diff, uint8_t flags);
		int add_promotion_with_diff(bitboard_t bitboard, Move *move_list, int list_length, int diff, uint8_t flags);
		bitboard_t bit_scan_forward(bitboard_t bboard);
		bitboard_t circular_shift(bitboard_t num, int shift);
		bitboard_t vertical_attacks(bitboard_t o, int from);
		bitboard_t horizontal_attacks(bitboard_t o, int from);
		bitboard_t diaganal_attacks(bitboard_t o, int from);
		bitboard_t antidiaganal_attacks(bitboard_t o, int from);
};

#endif // EON_H