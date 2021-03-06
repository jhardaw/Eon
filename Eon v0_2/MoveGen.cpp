/***********************************************************************************
 *  File: MoveGen.cpp
 *  Author: Ben Hardaway
 *  Purpose: Generate Chess Moves using bitboards and Hyperbola Quintessence for sliding pieces
 **********************************************************************************/
 
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <cassert>
#include "MoveGen.h"
#include "MoveList.h"

//#pragma intrinsic(_BitScanForward64) 

namespace Board
{
	bitboard_t AllAttacks(Board &board, EColor color);
	void add_queen_moves(MoveList &list, bitboard_t queens, bitboard_t friendly, bitboard_t occupied);
	void add_rook_moves(MoveList &list, bitboard_t rooks, bitboard_t friendly, bitboard_t occupied);
	void add_bishop_moves(MoveList &list, bitboard_t bishops, bitboard_t friendly, bitboard_t occupied);
	void add_king_moves(MoveList &list, bitboard_t kings, bitboard_t friendly);
	void add_castle_moves(MoveList &list, Board &board, bitboard_t occupied);
	void add_knight_moves(MoveList &list, bitboard_t knights, bitboard_t friendly);
	void add_pawn_pushes(MoveList &list, bitboard_t pawns, bitboard_t occupied, EColor toMove);
	void add_pawn_attacks(MoveList &list, bitboard_t pawns, bitboard_t oponent, EColor toMove);
	void add_pawn_enpassant(MoveList &list, bitboard_t pawns, bitboard_t ep_sqr, EColor toMove);
	double Distance(ESquare from, ESquare to);

	bitboard_t king_moves(ESquare from);
	bitboard_t knight_moves(ESquare from);
	bitboard_t bishop_moves(ESquare from, bitboard_t occupied);
	bitboard_t rook_moves(ESquare from, bitboard_t occupied);
	bitboard_t queen_moves(ESquare from, bitboard_t occupied);
	bitboard_t pawn_pushes(bitboard_t pawns, EColor toMove);
	bitboard_t pawn_Lattacks(bitboard_t pawns, EColor toMove);
	bitboard_t pawn_Rattacks(bitboard_t pawns, EColor toMove);

	// Hyperbola Quintessence
	void HyperbolaQuintessence_Init();
	bitboard_t vertical_attacks(bitboard_t o, int from);
	bitboard_t horizontal_attacks(bitboard_t o, int from);
	bitboard_t diaganal_attacks(bitboard_t o, int from);
	bitboard_t antidiaganal_attacks(bitboard_t o, int from);
	bitboard_t rankAttacks(bitboard_t o, int from);

	const bitboard_t fileAt[64] = {	FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H,
								FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H,
								FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H,
								FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H,
								FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H,
								FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H,
								FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H,
								FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H };
	const bitboard_t rowAt[64] = {	ROW_1, ROW_1, ROW_1, ROW_1, ROW_1, ROW_1, ROW_1, ROW_1,
								ROW_2, ROW_2, ROW_2, ROW_2, ROW_2, ROW_2, ROW_2, ROW_2,
								ROW_3, ROW_3, ROW_3, ROW_3, ROW_3, ROW_3, ROW_3, ROW_3,
								ROW_4, ROW_4, ROW_4, ROW_4, ROW_4, ROW_4, ROW_4, ROW_4,
								ROW_5, ROW_5, ROW_5, ROW_5, ROW_5, ROW_5, ROW_5, ROW_5,
								ROW_6, ROW_6, ROW_6, ROW_6, ROW_6, ROW_6, ROW_6, ROW_6,
								ROW_7, ROW_7, ROW_7, ROW_7, ROW_7, ROW_7, ROW_7, ROW_7,
								ROW_8, ROW_8, ROW_8, ROW_8, ROW_8, ROW_8, ROW_8, ROW_8 };
	bitboard_t diagAt[64];
	bitboard_t antidiagAt[64];
	uint8_t arrFirstRankAttacks64x8[64 * 8];

	const bitboard_t EMPTY_CASTLE_BLACK_KING =  bitAt[F8] | bitAt[G8];				//0x6000000000000000ULL;	//
	const bitboard_t EMPTY_CASTLE_BLACK_QUEEN = bitAt[B8] | bitAt[C8] | bitAt[D8];	//0x0E00000000000000ULL;	//
	const bitboard_t EMPTY_CASTLE_WHITE_KING =  bitAt[F1] | bitAt[G1];				// 0x60ULL;				// 
	const bitboard_t EMPTY_CASTLE_WHITE_QUEEN = bitAt[B1] | bitAt[C1] | bitAt[D1];	//0x0EULL;				//

	const bitboard_t CASTLE_BLACK_KING_THRU =  bitAt[E8] | bitAt[F8] | bitAt[G8]; //0x7000000000000000ULL;	//
	const bitboard_t CASTLE_BLACK_QUEEN_THRU = bitAt[C8] | bitAt[D8] | bitAt[E8]; //0x1C00000000000000ULL;	//
	const bitboard_t CASTLE_WHITE_KING_THRU =  bitAt[E1] | bitAt[F1] | bitAt[G1]; //0x70ULL;					//
	const bitboard_t CASTLE_WHITE_QUEEN_THRU = bitAt[C1] | bitAt[D1] | bitAt[E1]; //0x1CULL;					//

	bitboard_t knight_moves_table[64];
	bitboard_t king_moves_table[64];
	
	// Initilize lookup tables used by MoveGen
	extern void MoveGen_Init(void)
	{
		// Fill knight_moves lookup table
		int knight_diff[8] = { 15, 17, 6, 10, -15, -17, -6, -10 };
		for (ESquare ii = A1; ii <= H8; ii++)
		{
			knight_moves_table[ii] = 0ULL;
			ESquare dest;
			for (int jj = 0; jj < 8; jj++)
			{
				dest = ii + knight_diff[jj];
				double dist = Distance(ii, dest);
				if (dist > 1 && dist < 3 && dest >= 0 && dest < 64)
					knight_moves_table[ii] |= bitAt[dest];
			}
		}

		// Fill king_moves lookup table
		int king_diffs[8] = { -1, 1, -8, 8, -7, 7, -9, 9 };
		for (ESquare ii = A1; ii <= H8; ii++)
		{
			king_moves_table[ii] = 0ULL;
			ESquare dest;
			for (int jj = 0; jj < 8; jj++)
			{
				dest = ii + king_diffs[jj];
				double dist = Distance(ii, dest);
				if (dist >= 1 && dist < 2 && dest >= 0 && dest < 64)
					king_moves_table[ii] |= bitAt[dest];
			}
		}

		HyperbolaQuintessence_Init();
	}

	// Check if color's king is being attacked
	bool inCheck(Board &board, EColor color)
	{
		bitboard_t occupied = board.GetBitboard(WHITE) | board.GetBitboard(BLACK);
		bitboard_t king = board.GetBitboard(color | KING);
		ESquare from = static_cast<ESquare>(bit_scan_forward(king));
		bitboard_t bishopMoves = bishop_moves(from, occupied);
		bitboard_t rookMoves = rook_moves(from, occupied);
		
		bitboard_t bishops = board.GetBitboard((!color) | BISHOP);
		bitboard_t rooks = board.GetBitboard((!color) | ROOK);
		bitboard_t queens = board.GetBitboard((!color) | QUEEN);
		bitboard_t BoQAttacks = (bishopMoves & (bishops | queens));
		bitboard_t RoQAttacks = (rookMoves & (rooks | queens));

		if ((BoQAttacks | RoQAttacks) != BITBOARD_EMPTY)
			return true;

		bitboard_t NAttacks = knight_moves(from) & board.GetBitboard((!color) | KNIGHT);
		bitboard_t KAttacks = king_moves(from) & board.GetBitboard((!color) | KING);

		if ((NAttacks | KAttacks) != BITBOARD_EMPTY)
			return true;

		// Lastly pawn attacks
		bitboard_t PAttacks = BITBOARD_EMPTY;
		if (color == WHITE)
		{
			PAttacks = (king & ~FILE_A) << 7;
			PAttacks |= (king & ~FILE_H) << 9;
		}
		else
		{
			PAttacks = (king & ~FILE_H) >> 7;
			PAttacks |= (king & ~FILE_A) >> 9;
		}
		PAttacks &= board.GetBitboard((!color) | PAWN);

		return PAttacks != BITBOARD_EMPTY;
	}

	// Generate list of pseudo legal Chess moves, ignores leaving own King in check
	void GeneratePseudoLegalMoves(Board &board, MoveList &list)
	{
		EColor toMove = board.GetPlayersTurn();
		bitboard_t friendly = board.GetBitboard(toMove);
		bitboard_t oponent = board.GetBitboard(!toMove);
		bitboard_t occupied = friendly | oponent;

		add_knight_moves(list, board.GetBitboard(toMove|KNIGHT), friendly);
		add_bishop_moves(list, board.GetBitboard(toMove | BISHOP), friendly, occupied);
		add_pawn_attacks(list, board.GetBitboard(toMove | PAWN), oponent, toMove);
		add_pawn_pushes(list, board.GetBitboard(toMove | PAWN), occupied, toMove);
		add_rook_moves(list, board.GetBitboard(toMove | ROOK), friendly, occupied);
		add_queen_moves(list, board.GetBitboard(toMove | QUEEN), friendly, occupied);
		add_king_moves(list, board.GetBitboard(toMove | KING), friendly);
		add_castle_moves(list, board, occupied);
		add_pawn_enpassant(list, board.GetBitboard(toMove | PAWN), bitAt[board.GetEPSquare()], toMove);
	}

	// Remove moves that leave the side to move in check
	void RemoveChecks(Board &board, MoveList &list)
	{
		EColor toMove = board.GetPlayersTurn();
		for (int ii = 0; ii < list.GetLength(); ii++)
		{
			Move move = list.GetUnorderedMove(ii);
			board.MakeMove(move);
			if (inCheck(board, toMove))
			{
				board.UnmakeMove(move);
				list.RemoveMove(ii);
				ii -= 1;
			}
			else
			{
				board.UnmakeMove(move);
			}
		}
	}

	// Generate list of legal Chess moves
	void GenerateLegalMoves(Board &board, MoveList &list)
	{
		GeneratePseudoLegalMoves(board, list);
		RemoveChecks(board, list);
	}

	// Generate list of capturing moves
	void GenerateTacticalMoves(Board &board, MoveList &list)
	{
		EColor toMove = board.GetPlayersTurn();
		bitboard_t oponent = board.GetBitboard(!toMove);
		bitboard_t friendly = ~oponent; // Trick pieces into only capturing
		bitboard_t occupied = board.GetBitboard(toMove) | oponent;

		add_knight_moves(list, board.GetBitboard(toMove | KNIGHT), friendly);
		add_bishop_moves(list, board.GetBitboard(toMove | BISHOP), friendly, occupied);
		add_pawn_attacks(list, board.GetBitboard(toMove | PAWN), oponent, toMove);
		add_rook_moves(list, board.GetBitboard(toMove | ROOK), friendly, occupied);
		add_queen_moves(list, board.GetBitboard(toMove | QUEEN), friendly, occupied);
		add_king_moves(list, board.GetBitboard(toMove | KING), friendly);
	}

	// TODO: Generate list of quiet moves
	void GenerateQuietMoves(Board &board, MoveList &list)
	{
		EColor toMove = board.GetPlayersTurn();
		bitboard_t oponent = board.GetBitboard(!toMove);
		bitboard_t friendly = ~oponent; // Trick pieces into only capturing
		bitboard_t occupied = board.GetBitboard(toMove) | oponent;

		add_knight_moves(list, board.GetBitboard(toMove | KNIGHT), friendly);
		add_bishop_moves(list, board.GetBitboard(toMove | BISHOP), friendly, occupied);
		add_pawn_attacks(list, board.GetBitboard(toMove | PAWN), oponent, toMove);
		add_rook_moves(list, board.GetBitboard(toMove | ROOK), friendly, occupied);
		add_queen_moves(list, board.GetBitboard(toMove | QUEEN), friendly, occupied);
		add_king_moves(list, board.GetBitboard(toMove | KING), friendly);
	}

	void add_queen_moves(MoveList &list, bitboard_t queens, bitboard_t friendly, bitboard_t occupied)
	{
		while (queens != 0)
		{
			ESquare from = static_cast<ESquare>(bit_scan_forward(queens));
			bitboard_t moves = queen_moves(from, occupied) & ~friendly;
			list.Serialize(from, moves);
			queens &= queens - 1;
		}
	}	

	void add_bishop_moves(MoveList &list, bitboard_t bishops, bitboard_t friendly, bitboard_t occupied)
	{
		while (bishops != 0)
		{
			ESquare from = static_cast<ESquare>(bit_scan_forward(bishops));
			bitboard_t moves = bishop_moves(from, occupied) & ~friendly;
			list.Serialize(from, moves);
			bishops &= bishops - 1;
		}
	}

	void add_rook_moves(MoveList &list, bitboard_t rooks, bitboard_t friendly, bitboard_t occupied)
	{
		while (rooks != 0)
		{
			ESquare from = static_cast<ESquare>(bit_scan_forward(rooks));
			bitboard_t moves = rook_moves(from, occupied) & ~friendly;
			list.Serialize(from, moves);
			rooks &= rooks - 1;
		}
	}

	void add_king_moves(MoveList &list, bitboard_t king, bitboard_t friendly)
	{
		if (king != 0)
		{
			ESquare from = static_cast<ESquare>(bit_scan_forward(king));
			bitboard_t moves = king_moves(from) & ~friendly;
			list.Serialize(from, moves);
		}
	}

	void add_castle_moves(MoveList &list, Board &board, bitboard_t occupied)
	{
		ECastleRight castling_rights = board.GetCastlingRights();
		if (castling_rights)
		{
			EColor toMove = board.GetPlayersTurn();
			bitboard_t attacks = AllAttacks(board, !toMove);
			if (toMove == BLACK)// &&) !(bitAt[E8] & attacks))
			{
				if (castling_rights&CASTLE_BLACK_KING && !(EMPTY_CASTLE_BLACK_KING&occupied) && !(CASTLE_BLACK_KING_THRU&attacks))
				{
					list.AddMove(Move(E8, G8, CASTLE, WHITE | EMPTY));
				}
				if (castling_rights&CASTLE_BLACK_QUEEN && !(EMPTY_CASTLE_BLACK_QUEEN&occupied) && !(CASTLE_BLACK_QUEEN_THRU&attacks))
				{
					list.AddMove(Move(E8, C8, CASTLE, WHITE | EMPTY));
				}
			}
			else //if (toMove == WHITE) && !(bitAt[E1] & attacks))
			{
				if (castling_rights&CASTLE_WHITE_KING && !(EMPTY_CASTLE_WHITE_KING&occupied) && !(CASTLE_WHITE_KING_THRU&attacks))
				{
					list.AddMove(Move(E1, G1, CASTLE, WHITE | EMPTY));
				}
				if (castling_rights&CASTLE_WHITE_QUEEN && !(EMPTY_CASTLE_WHITE_QUEEN&occupied) && !(CASTLE_WHITE_QUEEN_THRU&attacks))
				{
					list.AddMove(Move(E1, C1, CASTLE, WHITE | EMPTY));
				}
			}
		}
	}

	void add_knight_moves(MoveList &list, bitboard_t knights, bitboard_t friendly)
	{
		while (knights != 0)
		{
			ESquare from = static_cast<ESquare>(bit_scan_forward(knights));
			bitboard_t moves = knight_moves(from) & ~friendly;
			list.Serialize(from, moves);
			knights &= knights - 1;
		}
	}

	void add_pawn_attacks(MoveList &list, bitboard_t pawns, bitboard_t oponent, EColor toMove)
	{
		bitboard_t targets = pawn_Lattacks(pawns, toMove);
		int diff = 9 - 16 * toMove;

		list.Serialize_Pawns(targets & oponent & ~(ROW_8 | ROW_1), diff, NO_FLAGS);
		// Add promotions
		list.Serialize_Pawns(targets & oponent & (ROW_8 | ROW_1), diff, QUEEN_PROMOTION);
		list.Serialize_Pawns(targets & oponent & (ROW_8 | ROW_1), diff, ROOK_PROMOTION);
		list.Serialize_Pawns(targets & oponent & (ROW_8 | ROW_1), diff, BISHOP_PROMOTION);
		list.Serialize_Pawns(targets & oponent & (ROW_8 | ROW_1), diff, KNIGHT_PROMOTION);

		targets = pawn_Rattacks(pawns, toMove);
		diff = 7 - 16 * toMove;
		list.Serialize_Pawns(targets & oponent & ~(ROW_8 | ROW_1), diff, NO_FLAGS);
		list.Serialize_Pawns(targets & oponent & (ROW_8 | ROW_1), diff, QUEEN_PROMOTION);
		list.Serialize_Pawns(targets & oponent & (ROW_8 | ROW_1), diff, ROOK_PROMOTION);
		list.Serialize_Pawns(targets & oponent & (ROW_8 | ROW_1), diff, BISHOP_PROMOTION);
		list.Serialize_Pawns(targets & oponent & (ROW_8 | ROW_1), diff, KNIGHT_PROMOTION);
	}

	void add_pawn_enpassant(MoveList &list, bitboard_t pawns, bitboard_t ep_sqr, EColor toMove)
	{
		bitboard_t targets = pawn_Lattacks(pawns, toMove);
		int diff = 9 - 16 * toMove;
		list.Serialize_Pawns(targets & ep_sqr, diff, EP_CAPTURE);

		targets = pawn_Rattacks(pawns, toMove);
		diff = 7 - 16 * toMove;
		list.Serialize_Pawns(targets & ep_sqr, diff, EP_CAPTURE);
	}

	void add_pawn_pushes(MoveList &list, bitboard_t pawns, bitboard_t occupied, EColor toMove)
	{
		const bitboard_t double_push_mask[2] = { ROW_3, ROW_6 };
		
		bitboard_t pushes2 = pawn_pushes(pawns, toMove) & ~occupied;
		int diff = 8 - 16 * toMove;
		list.Serialize_Pawns(pushes2 & ~(ROW_8 | ROW_1), diff, NO_FLAGS);
		list.Serialize_Pawns(pawn_pushes(pushes2 & double_push_mask[toMove], toMove) & ~occupied, 2 * diff, PAWN_DOUBLE_PUSH);
		list.Serialize_Pawns(pushes2 & (ROW_8 | ROW_1), diff, QUEEN_PROMOTION);
		list.Serialize_Pawns(pushes2 & (ROW_8 | ROW_1), diff, ROOK_PROMOTION);
		list.Serialize_Pawns(pushes2 & (ROW_8 | ROW_1), diff, BISHOP_PROMOTION);
		list.Serialize_Pawns(pushes2 & (ROW_8 | ROW_1), diff, KNIGHT_PROMOTION);
	}

	// Returns the distance between too squares on a Chess board in units of squares
	double Distance(ESquare from, ESquare to)
	{
		int srcRow, srcCol, dstRow, dstCol;
		int diffRow, diffCol;
		srcRow = ((int)from) / 8;
		srcCol = ((int)from) % 8;
		dstRow = ((int)to) / 8;
		dstCol = ((int)to) % 8;
		diffRow = dstRow - srcRow;
		diffCol = dstCol - srcCol;
		return sqrt(diffRow*diffRow + diffCol*diffCol);
	}

	// Generates a bitboard showing squares attacked by the player who's turn it is
	bitboard_t AllAttacks(Board &board, EColor color_to_move)
	{
		bitboard_t attacks = BITBOARD_EMPTY;
		bitboard_t occupied = board.GetBitboard(WHITE) | board.GetBitboard(BLACK);
		bitboard_t friendly = board.GetBitboard(color_to_move);

		// Add pawn attacks
		const int diffs[2][2] = { {7, 9}, {64 - 7, 64 - 9} };
		const bitboard_t file_masks[2][2] = { {~FILE_A, ~FILE_H}, {~FILE_H, ~FILE_A} };
		int attack_dir;

		bitboard_t pawns = board.GetBitboard(color_to_move | PAWN);
		for (attack_dir = 0; attack_dir < 2; attack_dir++)
		{
			int diff = diffs[color_to_move][attack_dir];
			bitboard_t targets = circular_shift(pawns & file_masks[color_to_move][attack_dir], diff);

			// Add attacks
			attacks |= targets & ~friendly;

			// Add En Passant attacks
			attacks |= targets & bitAt[board.GetEPSquare()] & (ROW_3 | ROW_6);
		}

		// Add king attacks
		bitboard_t king = board.GetBitboard(color_to_move | KING);
		ESquare from = static_cast<ESquare>(bit_scan_forward(king));
		attacks |= king_moves(from) & ~friendly;
		
		// Add knight attacks
		bitboard_t knights = board.GetBitboard(color_to_move | KNIGHT);
		while (knights != 0)
		{
			from = static_cast<ESquare>(bit_scan_forward(knights));
			attacks |= knight_moves(from) & ~friendly;
			knights &= knights - 1;
		}

		// Add bishop attacks
		bitboard_t bishops = board.GetBitboard(color_to_move | BISHOP);
		while (bishops != 0)
		{
			from = static_cast<ESquare>(bit_scan_forward(bishops));
			attacks |= bishop_moves(from, occupied)  & ~friendly;
			bishops &= bishops - 1;
		}

		// Add rook attacks
		bitboard_t rooks = board.GetBitboard(color_to_move | ROOK);
		while (rooks != 0)
		{
			from = static_cast<ESquare>(bit_scan_forward(rooks));
			attacks |= rook_moves(from, occupied)  & ~friendly;
			rooks &= rooks - 1;
		}

		// Add queen attacks
		bitboard_t queens = board.GetBitboard(color_to_move | QUEEN);
		while (queens != 0)
		{
			from = static_cast<ESquare>(bit_scan_forward(queens));
			attacks |= queen_moves(from, occupied)  & ~friendly;
			queens &= queens - 1;
		}

		return attacks;
	}

	// Returns a bitboard showing squares attacked by a King from a particular square
	bitboard_t king_moves(ESquare from)
	{
		return king_moves_table[from];
	}

	// Returns a bitboard showing squares attacked by a Knight from a particular square
	bitboard_t knight_moves(ESquare from)
	{
		return knight_moves_table[from];
	}

	// Returns a bitboard showing squares attacked by a Bishop from a particular square
	bitboard_t bishop_moves(ESquare from, bitboard_t occupied)
	{
		bitboard_t diaganol = diaganal_attacks(occupied, from);
		bitboard_t antidiaganol = antidiaganal_attacks(occupied, from);
		return diaganol | antidiaganol;
	}

	// Returns a bitboard showing squares attacked by a Rook from a particular square
	bitboard_t rook_moves(ESquare from, bitboard_t occupied)
	{
		bitboard_t horizontal = horizontal_attacks(occupied, from);
		bitboard_t vertical = vertical_attacks(occupied, from);
		return horizontal | vertical;
	}

	// Returns a bitboard showing squares attacked by a Queen from a particular square
	bitboard_t queen_moves(ESquare from, bitboard_t occupied)
	{
		bitboard_t bishop = bishop_moves(from, occupied);
		bitboard_t rook = rook_moves(from, occupied);
		return bishop | rook;
	}

	// Returns a bitboard showing squares Pawns can be advanced to
	bitboard_t pawn_pushes(bitboard_t pawns, EColor toMove)
	{
		return (pawns << 8) >> (toMove * 16);
	}

	// Returns a bitboard showing squares Pawns can attack to the left
	bitboard_t pawn_Lattacks(bitboard_t pawns, EColor toMove)
	{
		return ((pawns&~FILE_H) << 9) >> (toMove * 16);
	}

	// Returns a bitboard showing squares Pawns can attack to the right
	bitboard_t pawn_Rattacks(bitboard_t pawns, EColor toMove)
	{
		return ((pawns&~FILE_A) << 7) >> (toMove * 16);
	}

	
	////////////////////////////////////////////////////////////////////////////////////////
	// Hyperbola Quintessence functions
	////////////////////////////////////////////////////////////////////////////////////////
	void HyperbolaQuintessence_Init()
	{
		// Fill diagAt, antidiagAt, and rankAttack lookup tables
		for (int ii = 0; ii < 64; ii++)
		{
			diagAt[ii] = 0ULL;
			int row = ii / 8;
			int col = ii % 8;
			for (int jj = -7; jj < 8; jj++)
			{
				if (jj != 0 && row + jj >= 0 && row + jj < 8 && col + jj >= 0 && col + jj < 8)
				{
					int dest = (row + jj) * 8 + col + jj;
					diagAt[ii] |= bitAt[dest];
				}
				if (jj != 0 && row - jj >= 0 && row - jj < 8 && col + jj >= 0 && col + jj < 8)
				{
					int dest = (row - jj) * 8 + col + jj;
					antidiagAt[ii] |= bitAt[dest];
				}
			}
			for (int from = 0; from < 8; from++)
			{
				bitboard_t occupied = BITBOARD_EMPTY | static_cast<uint64_t>(ii) << 1;
				arrFirstRankAttacks64x8[8 * ii + from] = static_cast<uint8_t>(rankAttacks(occupied, from));
			}
		}
	}

	bitboard_t vertical_attacks(bitboard_t o, int from)
	{
		bitboard_t s = bitAt[from];
		bitboard_t m = fileAt[from];
		//bitboard_t om_r = bswap_64(o&m);
		bitboard_t om_r = _byteswap_uint64(o&m);
		bitboard_t s_r = _byteswap_uint64(s);
		return (((o&m) - 2 * s) ^ _byteswap_uint64(om_r - 2 * s_r))&m;
	}

	// Slow, used to fill rank attack lookup table used by horizontal_attacks()
	bitboard_t rankAttacks(bitboard_t o, int from)
	{
		bitboard_t s = bitAt[from];
		bitboard_t m = rowAt[from];
		bitboard_t om_r = Reverse(o&m);
		bitboard_t s_r = Reverse(s);
		return (((o&m) - 2 * s) ^ Reverse(om_r - 2 * s_r))&m;
	}

	bitboard_t horizontal_attacks(bitboard_t occ, int sq)
	{
		uint32_t file = sq & 7;
		uint32_t rkx8 = sq & 56; // rank * 8
		occ = (occ >> rkx8) & 2 * 63;
		bitboard_t attacks = static_cast<bitboard_t>(arrFirstRankAttacks64x8[4 * occ + file]);
		return attacks << rkx8;
	}

	bitboard_t diaganal_attacks(bitboard_t o, int from)
	{
		bitboard_t s = bitAt[from];
		bitboard_t m = diagAt[from];
		bitboard_t om_r = _byteswap_uint64(o&m);
		bitboard_t s_r = _byteswap_uint64(s);
		return (((o&m) - 2 * s) ^ _byteswap_uint64(om_r - 2 * s_r))&m;
	}

	bitboard_t antidiaganal_attacks(bitboard_t o, int from)
	{
		bitboard_t s = bitAt[from];
		bitboard_t m = antidiagAt[from];
		bitboard_t om_r = _byteswap_uint64(o&m);
		bitboard_t s_r = _byteswap_uint64(s);
		return (((o&m) - 2 * s) ^ _byteswap_uint64(om_r - 2 * s_r))&m;
	}
}
