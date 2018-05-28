#pragma once
#include "Board.h"

namespace Board
{
	const int MAXMOVES = 200;

	class MoveList
	{
	public:
		MoveList(const Board &board);
		Move GetOrderedMove(int index);
		Move GetUnorderedMove(int index);
		void AddMove(Move move);
		void RemoveMove(int index);
		int GetLength();
		void MoveList::Serialize(ESquare from, bitboard_t moves);
		void MoveList::Serialize_Pawns(bitboard_t moves, int diff, EMoveFlag flag);
		std::string ToString();

		/** Properties **/
		const Board& m_board;
		Move m_list[MAXMOVES];
		int m_listLength;
	};
}