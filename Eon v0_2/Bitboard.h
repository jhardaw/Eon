#pragma once
#include <stdint.h>

namespace Board
{
	extern uint64_t bitAt[65];

	typedef uint64_t bitboard_t;
	const bitboard_t BITBOARD_EMPTY = 0ULL;

	const bitboard_t ROW_1 = 0x00000000000000FFULL;
	const bitboard_t ROW_2 = 0x000000000000FF00ULL;
	const bitboard_t ROW_3 = 0x0000000000FF0000ULL;
	const bitboard_t ROW_4 = 0x00000000FF000000ULL;
	const bitboard_t ROW_5 = 0x000000FF00000000ULL;
	const bitboard_t ROW_6 = 0x0000FF0000000000ULL;
	const bitboard_t ROW_7 = 0x00FF000000000000ULL;
	const bitboard_t ROW_8 = 0xFF00000000000000ULL;

	const bitboard_t FILE_A = 0x0101010101010101ULL;
	const bitboard_t FILE_B = 0x0202020202020202ULL;
	const bitboard_t FILE_C = 0x0404040404040404ULL;
	const bitboard_t FILE_D = 0x0808080808080808ULL;
	const bitboard_t FILE_E = 0x1010101010101010ULL;
	const bitboard_t FILE_F = 0x2020202020202020ULL;
	const bitboard_t FILE_G = 0x4040404040404040ULL;
	const bitboard_t FILE_H = 0x8080808080808080ULL;

	void Bitboard_Init();

	int bit_scan_forward(bitboard_t bboard);
	bitboard_t circular_shift(bitboard_t num, int shift);
	bitboard_t Reverse(bitboard_t board);
	void print_bitboard(bitboard_t bitboard);
}