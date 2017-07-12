/***********************************************************************************
 *  File: Search.c
 *  Author: Ben Hardaway
 *  Purpose: Searches the game tree to find the Best Move
 **********************************************************************************/

#include <iostream>
#include "Search.h"
#include "Evaluate.h"
#include "MoveList.h"
#include "MoveGen.h"
#include "Board.h"
#include "UCI.h"
#include "TranspoTable.h"

namespace Search
{
	#define INFINITEVAL 2000000

	static int AlphaBeta(Board::Board& board, int alpha, int beta, int depthleft);
	static int Quiescene(Board::Board &board, int alpha, int beta, int depth);
	const int MaxQDepth = 4;

	int stats_nodes = 0;
	int stats_score = 0;
	int stats_hashhit = 0;

	// Iterative deepening alpha beta search framework
	void Search(Board::Board &board, UCI::SearchInfo &searchInfo)
	{
		Board::Move best;
		for (int depth = 1; !searchInfo.DoneSearch(depth) && depth <= 12; depth++)
		{
			clock_t start = clock();
			best = BasicExtractPV(board, depth);
			double elapsed = double(clock() - start) / (double)CLOCKS_PER_SEC;
			std::cout << "info depth " << depth << " score cp " << -stats_score << " nodes " << stats_nodes << " time " << elapsed << " pv " << best.ToString() << std::endl;
		}
		std::cout << "bestmove " << best.ToString() << std::endl;
	}

	void AnalyzeBoard(Board::Board &board, int depth)
	{
		Board::MoveList moveList(board);
		Board::GeneratePseudoLegalMoves(board, moveList);
		for (int ii = 0; ii < moveList.GetLength(); ii++)
		{
			Board::Move move = moveList.GetMove(ii);
			board.MakeMove(move);
			int score = -AlphaBeta(board, -INFINITEVAL, INFINITEVAL, depth - 1);
			std::cout << "Move: " << move.ToString() << "\tScore: " << score << std::endl;
			board.UnmakeMove(move);
		}
	}

	// Basic find best move using AlphaBeta
	extern Board::Move BasicExtractPV(Board::Board& board, int depth)
	{
		Board::MoveList moveList(board);
		Board::GeneratePseudoLegalMoves(board, moveList);
		Board::Move bestMove;
		stats_nodes = 0;
		stats_score = INFINITEVAL;
		for (int ii = 0; ii < moveList.GetLength(); ii++)
		{
			Board::Move move = moveList.GetMove(ii);
			board.MakeMove(move);
			int score = AlphaBeta(board, -INFINITEVAL, INFINITEVAL, depth - 1);
			board.UnmakeMove(move);
			if (score < stats_score)
			{
				stats_score = score;
				bestMove = move;
			}
		}

		return bestMove;
	}

	int AlphaBeta(Board::Board& board, int alpha, int beta, int depthleft)
	{
		int value;

		if (depthleft == 0)
		{
			value = EvaluateFast(board);//Quiescene(board, alpha, beta, 0);//
			return value;
		}

		Board::MoveList moveList(board);
		Board::GeneratePseudoLegalMoves(board, moveList);
		for (int ii = 0; ii < moveList.GetLength(); ii++)
		{
			Board::Move move = moveList.GetMove(ii);
			board.MakeMove(move);
			value = -AlphaBeta(board, -beta, -alpha, depthleft - 1);
			board.UnmakeMove(move);

			if (value >= beta)
			{
				return beta;   //  fail hard beta-cutoff
			}
			if (value > alpha)
			{
				alpha = value; // alpha acts like max in MiniMax
			}
		}
		return alpha;
	}

	int Quiescene(Board::Board &board, int alpha, int beta, int depth)
	{
		stats_nodes++;
		int val = EvaluateFast(board);
		if (val >= beta || depth > MaxQDepth)
			return beta;
		if (val > alpha)
			alpha = val;

		Board::MoveList moveList(board);
		Board::GeneratePseudoLegalMoves(board, moveList);

		for (int ii = 0; ii < moveList.GetLength(); ii++)
		{
			Board::Move move = moveList.GetMove(ii);
			board.MakeMove(move);
			val = -Quiescene(board, -beta, -alpha, depth+1);
			board.UnmakeMove(move);
			if (val >= beta)
				return beta;
			if (val > alpha)
				alpha = val;
		}
		return alpha;
	}
}