#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <string.h>
#include <windows.h>
#include "UCI.h"
#include "Perft.h"
#include "Search.h"
#include "MoveGen.h"
#include "Evaluate.h"
#include "TranspoTable.h"

#define VERSION 0
#define REVISION 2
#define MAXLINE 100

#define MOVES_REMAINING 30

namespace UCI
{
	EEngineState g_engineState;

	EEngineState GetEngineState()
	{
		return g_engineState;
	}

	void SetEngineState(EEngineState engineState)
	{
		g_engineState = engineState;
	}

	UCI::UCI()
	{
		m_board = new Board::Board();
		m_searchInfo = new SearchInfo(Board::WHITE, INFINITE_SEARCH);
		SetEngineState(IDLE);		
	}

	void UCI::Start(void)
	{
		m_thread = std::thread(&UCI::HandleInput, this);
		EngineLoop();
		m_thread.join();
	}

	void UCI::EngineLoop(void)
	{
		while (GetEngineState() != QUIT)
		{
			if (GetEngineState() == START_THINKING)
			{
				Search::Search(*m_board, *m_searchInfo);
				//std::cout << "bestmove " << bestMove.ToString() << std::endl;
				if (GetEngineState() != QUIT)
					SetEngineState(IDLE);
			}
			Sleep(1);
		}
	}

	void UCI::HandleInput(void)
	{
		std::string line;
		while (GetEngineState() != QUIT)
		{
			getline(std::cin, line, '\n');
			if (line.find("uci") != std::string::npos && line.find("ucinewgame") == std::string::npos)
				inputUCI();
			else if (line.find("debug") != std::string::npos)
				inputDebug();
			else if (line.find("isready") != std::string::npos)
				inputIsReady();
			else if (line.find("setoption") != std::string::npos)
				inputSetOption();
			else if (line.find("register") != std::string::npos)
				inputRegister();
			else if (line.find("ucinewgame") != std::string::npos)
				inputUCINewGame();
			else if (line.find("position") != std::string::npos)
				inputPosition(line);
			else if (line.find("go") != std::string::npos)
				inputGo(line);
			else if (line.find("stop") != std::string::npos)
				inputStop();
			else if (line.find("ponderhit") != std::string::npos)
				inputPonderHit();
			else if (line.find("quit") != std::string::npos)
				inputQuit();
			else if (line.find("print") != std::string::npos)
			{
				std::cout << m_board->ToString() << std::endl;
			}
			else if (line.find("perft test") != std::string::npos)
			{
				Perft_Test();
			}
			else if (line.find("perft") != std::string::npos)
			{
				std::vector<std::string> tokens;
				split(line, ' ', tokens);
				std::cout << Perft_PseudoLegal(*m_board, std::stoi(tokens[1])) << std::endl;
			}
			else if (line.find("divide") != std::string::npos)
			{
				std::vector<std::string> tokens;
				split(line, ' ', tokens);
				Divide(*m_board, stoi(tokens[1]));
			}
			else if (line.find("unmake") != std::string::npos)
			{
				std::vector<std::string> tokens;
				split(line, ' ', tokens);
				Board::ESquare from = Board::translate(tokens[1]);
				Board::ESquare to = Board::translate(tokens[1]);
				Board::MoveFlags flags = Board::MoveFlags::NO_FLAGS;
				Board::EPiece captured = m_board->PieceAt(to);
				m_board->UnmakeMove(Board::Move(from, to, flags, captured));
			}
			else if (line.find("make") != std::string::npos)
			{
				std::vector<std::string> tokens;
				split(line, ' ', tokens);
				Board::ESquare from = Board::translate(tokens[1]);
				Board::ESquare to = Board::translate(tokens[1]);
				Board::MoveFlags flags = Board::MoveFlags::NO_FLAGS;
				Board::EPiece captured = m_board->PieceAt(to);
				m_board->MakeMove(Board::Move(from, to, flags, captured));
			}
			else if (line.find("analyze depth") != std::string::npos)
			{
				std::vector<std::string> tokens;
				split(line, ' ', tokens);
				Search::AnalyzeBoard(*m_board, stoi(tokens[2]));
			}
			else
			{
				//std::cout << "Ignored" << std::endl;
			}
			Sleep(1);
		}
	}

	void UCI::inputUCI(void)
	{
		std::cout << "id name Eon " << VERSION << "." << REVISION << std::endl;
		std::cout << "id author Ben Hardaway" << std::endl;
		// send the "option" commands
		std::cout << "uciok" << std::endl;
	}

	void UCI::inputDebug(void)
	{
		// check for "on" or "off" and set debug appropriately
	}

	void UCI::inputIsReady(void)
	{
		Board::MoveGen_Init();
		Evaluate_Init();
		//TranspoTable_Init();
		m_board->ParseFEN(std::string("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
		std::cout << "readyok" << std::endl;
	}

	void UCI::inputSetOption(void)
	{
		// ignore for now
	}

	void UCI::inputRegister(void)
	{
		// ignore for now
	}

	void UCI::inputUCINewGame(void)
	{
		// prepare for new game
	}

	void UCI::inputPosition(std::string &line)
	{
		if (line.find("fen") != std::string::npos)
		{
			std::string fen;
			std::vector<std::string> tokens;
			split(line, ' ', tokens);
			fen = tokens[2] + " " + tokens[3] + " " + tokens[4] + " " + tokens[5] + " " + tokens[6] + " " + tokens[7];
			m_board->ParseFEN(fen);
		}
		else if (line.find("startpos") != std::string::npos)
		{
			m_board->ParseFEN(std::string("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
		}

		if (line.find("moves") != std::string::npos)
		{
			std::vector<std::string> tokens;
			split(line.substr(line.find("moves") + 6), ' ', tokens);
			for (int ii = 0; ii < tokens.size(); ii++)
			{
				Board::MoveList list(*m_board);
				Board::ESquare from = Board::translate(tokens[ii].substr(0,2));
				Board::ESquare to = Board::translate(tokens[ii].substr(2,2));
				Board::GenerateLegalMoves(*m_board, list);
				for (int jj = 0; jj < list.GetLength(); jj++)
				{
					Board::Move move = list.GetMove(jj);
					if (from == move.GetFrom() && to == move.GetTo())
					{
						m_board->MakeMove(move);
					}
				}
			}
		}
	}

	void UCI::inputGo(std::string &line)
	{
		if (line.find("infinite") != std::string::npos)
		{
			m_searchInfo = new SearchInfo(m_board->GetPlayersTurn(), INFINITE_SEARCH);
		}
		else if (line.find("depth") != std::string::npos)
		{
			std::vector<std::string> tokens;
			split(line, ' ', tokens);
			m_searchInfo = new SearchInfo(m_board->GetPlayersTurn(), DEPTH_SEARCH, stoi(tokens[2]));
		}
		else if (line.find("wtime") != std::string::npos && line.find("btime") != std::string::npos)
		{
			std::vector<std::string> tokens;
			split(line, ' ', tokens);
			int bTime, wTime;
			for (int ii = 0; ii < tokens.size(); ii++)
			{
				if (tokens[ii].compare("wtime") == 0)
					wTime = stoi(tokens[ii + 1]);
				else if (tokens[ii].compare("btime") == 0)
					bTime = stoi(tokens[ii + 1]);
			}
			m_searchInfo = new SearchInfo(m_board->GetPlayersTurn(), TIME_SEARCH, bTime, wTime);
		}
		SetEngineState(START_THINKING);
	}

	void UCI::inputStop(void)
	{
		SetEngineState(STOP);
	}

	void UCI::inputPonderHit(void)
	{
		// ignore
	}

	void UCI::inputQuit(void)
	{
		SetEngineState(QUIT);
		TranspoTable_Destruct();
	}

	SearchInfo::SearchInfo(Board::EColor searcher, ESearchType searchType)
	{
		assert(searchType == INFINITE_SEARCH);
		m_toMove = searcher;
		m_searchType = searchType;
	}

	SearchInfo::SearchInfo(Board::EColor searcher, ESearchType searchType, int depth)
	{
		assert(searchType == DEPTH_SEARCH);
		m_toMove = searcher;
		m_searchType = searchType;
		m_depth = depth;
	}

	SearchInfo::SearchInfo(Board::EColor searcher, ESearchType searchType, int bTime, int wTime)
	{
		assert(searchType == TIME_SEARCH);
		m_toMove = searcher;
		m_searchType = searchType;
		m_bTime = bTime;
		m_wTime = wTime;
		m_startTime = m_lastPly = clock();
	}
	bool SearchInfo::DoneSearch(int depth)
	{
		if (m_searchType == INFINITE_SEARCH)
			return GetEngineState() == STOP || GetEngineState() == QUIT;
		else if (m_searchType == DEPTH_SEARCH)
			return depth > m_depth;
		else if (m_searchType == TIME_SEARCH)
		{
			double totalDelta = double(clock() - m_startTime) / (double)CLOCKS_PER_SEC * 1000;
			int bFactor = 8;
			double plyDelta = double(clock() - m_lastPly) / (double)CLOCKS_PER_SEC * 1000;
			m_lastPly = clock();
			if (m_toMove == Board::EColor::WHITE)
			{
				//cout << totalDelta << " " << totalDelta + plyDelta*bFactor << " " << m_wTime / MOVES_REMAINING << endl;
				return totalDelta + plyDelta * bFactor > m_wTime / MOVES_REMAINING;
			}
			else
			{
				return totalDelta + plyDelta * bFactor > m_wTime / MOVES_REMAINING;
			}
		}
		return false;
	}
}
