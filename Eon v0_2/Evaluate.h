#pragma once

#include "Board.h"

void Evaluate_Init();
int Evaluate(const Board::Board &board);
int EvaluateFast(const Board::Board &board);
void Eval_RemovePiece(int &eval, uint8_t piece, Board::ESquare sqr);
void Eval_SetPiece(int &eval, uint8_t piece, Board::ESquare sqr);
//void Evaluate_Test(char *filename);