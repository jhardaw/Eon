#include <stdlib.h>
#include <stdio.h>
#include <intrin.h>
#include <iostream>
#include "Bitboard.h"

namespace Board
{
	uint64_t bitAt[65];

	void Bitboard_Init()
	{
		uint64_t bit = 1;
		for (int ii = 0; ii < 64; ii++)
		{
			bitAt[ii] = bit;
			bit = bit << 1;
		}
		bitAt[64] = 0;
	}

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