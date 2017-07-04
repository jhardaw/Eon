#include "Board.h"
#include "MoveList.h"

namespace Board
{
	MoveList::MoveList()
	{
		m_listLength = 0;
	}

	Move MoveList::GetMove(int index)
	{
		int max = index;
		for (int ii = index + 1; ii < m_listLength; ii++)
		{
			if (m_list[ii] > m_list[max])
				max = ii;
		}
		Move temp = m_list[index];
		m_list[index] = m_list[max];
		m_list[max] = temp;
		return m_list[index];
	}

	void MoveList::AddMove(Move &move)
	{
		m_list[m_listLength++] = move;
	}

	void MoveList::RemoveMove(int index)
	{
		m_list[index] = m_list[m_listLength - 1];
		m_listLength -= 1;
	}

	int MoveList::GetLength()
	{
		return m_listLength;
	}

	std::string MoveList::ToString()
	{
		std::string moves;
		for (int ii = 0; ii < m_listLength; ii++)
		{
			moves += m_list[ii].ToString() + "\n";
		}
		return moves;
	}
}