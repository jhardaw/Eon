#include <stdlib.h>
#include <stdio.h>
#include <intrin.h>
#include <iostream>
#include "Bitboard.h"
#include <cassert>

namespace Board
{
	const uint64_t bitAt[65] = {	0x0000000000000001, 0x0000000000000002, 0x0000000000000004, 0x0000000000000008, 0x0000000000000010, 0x0000000000000020, 0x0000000000000040, 0x0000000000000080,
									0x0000000000000100, 0x0000000000000200, 0x0000000000000400, 0x0000000000000800, 0x0000000000001000, 0x0000000000002000, 0x0000000000004000, 0x0000000000008000,
									0x0000000000010000, 0x0000000000020000, 0x0000000000040000, 0x0000000000080000, 0x0000000000100000, 0x0000000000200000, 0x0000000000400000, 0x0000000000800000,
									0x0000000001000000, 0x0000000002000000, 0x0000000004000000, 0x0000000008000000, 0x0000000010000000, 0x0000000020000000, 0x0000000040000000, 0x0000000080000000,
									0x0000000100000000, 0x0000000200000000, 0x0000000400000000, 0x0000000800000000, 0x0000001000000000, 0x0000002000000000, 0x0000004000000000, 0x0000008000000000,
									0x0000010000000000, 0x0000020000000000, 0x0000040000000000, 0x0000080000000000, 0x0000100000000000, 0x0000200000000000, 0x0000400000000000, 0x0000800000000000,
									0x0001000000000000, 0x0002000000000000, 0x0004000000000000, 0x0008000000000000, 0x0010000000000000, 0x0020000000000000, 0x0040000000000000, 0x0080000000000000,
									0x0100000000000000, 0x0200000000000000, 0x0400000000000000, 0x0800000000000000, 0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x8000000000000000,
									0x0000000000000000, };

	int bit_scan_forward(bitboard_t bboard)
	{
		/*  Left over from Linux code
		int index = __builtin_ffsl(bboard);
		if (index == 0)
		return 64;
		return index - 1;*/

		unsigned long index;
		uint8_t isNonzero = _BitScanForward64(&index, bboard);
		if (!isNonzero)
			return 64;
		return index;
	}

	bitboard_t circular_shift(bitboard_t num, int shift)
	{
		return (num << shift) | (num >> (64 - shift));
	}

	bitboard_t Reverse(bitboard_t board)
	{
		bitboard_t rev = board;
		int s = sizeof(board) * 8 - 1;
		for (board >>= 1; board; board >>= 1)
		{
			rev <<= 1;
			rev |= board & 1;
			s--;
		}
		rev <<= s;
		return rev;
	}



	extern void print_bitboard(bitboard_t bitboard)
	{
		int ii = 0;
		char *border = (char *)"\n  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _\n";
		printf("%s", border);
		for (int row = 0; row < 8; row++)
		{
			printf("%d |", row + 1);
			for (int col = 0; col < 8; col++)
			{
				char *p;
				if ((bitboard >> ii) & 0x1)
				{
					p = (char *)"*";
				}
				else
				{
					p = (char *)" ";
				}
				printf(" %s |", p);
				ii++;
			}
			printf("%s", border);
		}
		printf("    A   B   C   D   E   F   G   H\n");
	}
}