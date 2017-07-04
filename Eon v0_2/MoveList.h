#pragma once
#include "Board.h"

namespace Board
{
	const int MAXMOVES = 200;

	class MoveList
	{
	public:
		MoveList();
		Move GetMove(int index);
		void AddMove(Move &move);
		void RemoveMove(int index);
		int GetLength();
		std::string ToString();

		/** Properties **/
		Move m_list[MAXMOVES];
		int m_listLength;
	};
}