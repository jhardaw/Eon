#pragma once
#include "Board.h"

namespace Board
{
	const int MAXMOVES = 200;

	class MoveList
	{
	public:
		MoveList(Board &board);
		Move GetMove(int index);
		void AddMove(Move &move);
		void RemoveMove(int index);
		int GetLength();
		void MoveList::Serialize(ESquare from, bitboard_t moves);
		void MoveList::Serialize_Pawns(bitboard_t moves, int diff, MoveFlags flag);
		std::string ToString();

		/** Properties **/
		Board m_board;
		Move m_list[MAXMOVES];
		int m_listLength;
	};
}