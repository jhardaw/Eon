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
	const int INFINITEVAL = 2000000;

	static int AlphaBeta(Board::Board& board, int alpha, int beta, int depthleft, int mate);
	static int Quiescene(Board::Board &board, int alpha, int beta, int depth);
	const int MaxQDepth = 4;

	int stats_nodes = 0;
	int stats_score = 0;
	int stats_hashhit = 0;

	// Iterative deepening alpha beta search framework
	void Search(Board::Board &board, UCI::SearchInfo &searchInfo)
	{
		int depth = 1;
		Board::Move best = BasicExtractPV(board, depth);;
		for (depth = 2; !searchInfo.DoneSearch(depth) && depth <= 12; depth++)
		{
			clock_t start = clock();
			best = BasicExtractPV(board, depth);
			double elapsed = double(clock() - start) / (double)CLOCKS_PER_SEC;
			if (stats_score >= Eval::MATE_SCORE - 1000)
				std::cout << "info depth " << depth << " score mate " << (Eval::MATE_SCORE - stats_score + 1) / 2;
			else if (stats_score <= -Eval::MATE_SCORE + 1000)
				std::cout << "info depth " << depth << " score mate " << -(Eval::MATE_SCORE - stats_score + 1) / 2;
			else
				std::cout << "info depth " << depth << " score cp " << -stats_score;
				
			std::cout << " nodes " << stats_nodes << " time " << elapsed << " pv " << best.ToString() << std::endl;
		}
		std::cout << "bestmove " << best.ToString() << std::endl;
	}

	void AnalyzeBoard(Board::Board &board, int depth)
	{
		Board::MoveList moveList(board);
		Board::GeneratePseudoLegalMoves(board, moveList);
		for (int ii = 0; ii < moveList.GetLength(); ii++)
		{
			Board::Move move = moveList.GetOrderedMove(ii);
			board.MakeMove(move);
			if (!Board::inCheck(board, !board.GetPlayersTurn()))
			{
				int score = -AlphaBeta(board, -INFINITEVAL, INFINITEVAL, depth - 1, Eval::MATE_SCORE);
				std::cout << "Move: " << move.ToString() << "\tScore: " << score << std::endl;
			}
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
			Board::Move move = moveList.GetOrderedMove(ii);
			board.MakeMove(move);
			if (!Board::inCheck(board, !board.GetPlayersTurn()))
			{
				int score = AlphaBeta(board, -INFINITEVAL, INFINITEVAL, depth - 1, Eval::MATE_SCORE);
				if (score < stats_score)
				{
					stats_score = score;
					bestMove = move;
				}
			}
			board.UnmakeMove(move);
		}

		return bestMove;
	}

	int AlphaBeta(Board::Board& board, int alpha, int beta, int depthleft, int mate)
	{
		int value;
		int legal = 0;

		if (depthleft == 0)
			return Eval::EvaluateFast(board);//Quiescene(board, alpha, beta, 0);//

		Board::MoveList moveList(board);
		Board::GeneratePseudoLegalMoves(board, moveList);
		for (int ii = 0; ii < moveList.GetLength(); ii++)
		{
			Board::Move move = moveList.GetOrderedMove(ii);
			board.MakeMove(move);
			if (!Board::inCheck(board, !board.GetPlayersTurn()))
			{
				legal += 1;
				value = -AlphaBeta(board, -beta, -alpha, depthleft - 1,mate - 1);
			}
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

		if (!legal)
		{
			if (!Board::inCheck(board, board.GetPlayersTurn()))
				return Eval::CONTEMPT_SCORE;
			return -mate;
		}
		return alpha;
	}

	int Quiescene(Board::Board &board, int alpha, int beta, int depth)
	{
		stats_nodes++;
		int val = Eval::EvaluateFast(board);
		if (val >= beta || depth > MaxQDepth)
			return beta;
		if (val > alpha)
			alpha = val;

		Board::MoveList moveList(board);
		Board::GeneratePseudoLegalMoves(board, moveList);

		for (int ii = 0; ii < moveList.GetLength(); ii++)
		{
			Board::Move move = moveList.GetOrderedMove(ii);
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