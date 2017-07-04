#pragma once

#include <thread>
#include <string>
#include <time.h>
#include "Board.h"

namespace UCI
{
	enum EEngineState { IDLE, START_THINKING, THINKING, STOP, QUIT };

	enum ESearchType { INFINITE_SEARCH, DEPTH_SEARCH, TIME_SEARCH };

	EEngineState GetEngineState();
	void SetEngineState(EEngineState engineState);

	class SearchInfo
	{
	public:
		SearchInfo(Board::EColor searcher, ESearchType searchType); // Infinite Search
		SearchInfo(Board::EColor searcher, ESearchType searchType, int depth); // Depth Search
		SearchInfo(Board::EColor searcher, ESearchType searchType, int bTime, int wTime); // Timed Search

		bool DoneSearch(int depth);

	private:
		Board::EColor m_toMove;
		ESearchType m_searchType;
		int m_depth;
		int m_bTime;
		int m_wTime;
		clock_t m_startTime;
		clock_t m_lastPly;
	};

	class UCI
	{
	public:
		UCI();
		void Start(void);

	private:
		void HandleInput(void);
		void EngineLoop(void);
		void inputUCI(void);
		void inputDebug(void);
		void inputIsReady(void);
		void inputSetOption(void);
		void inputRegister(void);
		void inputUCINewGame(void);
		void inputPosition(std::string &line);
		void inputGo(std::string &line);
		void inputStop(void);
		void inputPonderHit(void);
		void inputQuit(void);

		// Properties
		std::thread m_thread;
		Board::Board *m_board;
		SearchInfo *m_searchInfo;
	};
}