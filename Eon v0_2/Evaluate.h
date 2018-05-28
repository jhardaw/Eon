#pragma once

#include "Board.h"
namespace Eval
{
	const int MATE_SCORE = 100000000;
	const int CONTEMPT_SCORE = 0;

	void Evaluate_Init();
	int Evaluate(const Board::Board &board);
	int EvaluateFast(const Board::Board &board);

	void Eval_RemovePiece(int &eval, uint8_t piece, Board::ESquare sqr);
	void Eval_SetPiece(int &eval, uint8_t piece, Board::ESquare sqr);
}