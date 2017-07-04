/***********************************************************************************
 *  File: Evaluate.c
 *  Author: Ben Hardaway
 *  Purpose: Score a board position for Search purposes.  Implements the Simplified 
 *           Evaluation Function proposed by .....
 *			 ref: https://chessprogramming.wikispaces.com/Simplified+evaluation+function
 **********************************************************************************/
 
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cassert>
#include "Board.h"
#include "Evaluate.h"

#define MAXLINE 100

const int MATE = 100000;

const int PAWN_VALUE = 100;
const int KNIGHT_VALUE = 320;
const int BISHOP_VALUE = 330;
const int ROOK_VALUE = 500;
const int QUEEN_VALUE = 900;
const int KING_VALUE = 20000;

static void flip_piece_square(int *piece_square, int *rev);

// Piece squares
int Empty[64] = { 0, 0, 0, 0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0, 0, 0, 0 };
							    

int Black_Pawn[64] = { 0,   0,   0,   0,   0,   0,   0,   0,
					 -50, -50, -50, -50, -50, -50, -50, -50,
					 -10, -10, -20, -30, -30, -20, -10, -10,
				   	  -5,  -5, -10, -25, -25, -10,  -5,  -5,
					   0,   0,   0, -20, -20,   0,   0,   0,
					  -5,   5,  10,   0,   0,  10,   5,  -5,
					  -5, -10, -10,  20,  20, -10, -10,  -5, 
					   0,   0,   0,   0,   0,   0,   0,   0 };
int White_Pawn[64];

int Black_Knight[64] = {50, 40, 30, 30, 30, 30, 40, 50,
						40, 20,  0,  0,  0,  0, 20, 40,
						30,  0,-10,-15,-15,-10,  0, 30,
						30, -5,-15,-20,-20,-15, -5, 30,
						30,  0,-15,-20,-20,-15,  0, 30,
						30, -5,-10,-15,-15,-10, -5, 30,
						40, 20,  0, -5, -5,  0, 20, 40,
						50, 40, 30, 30, 30, 30, 40, 50 };
int White_Knight[64];
							
int Black_Bishop[64] = {20, 10, 10, 10, 10, 10, 10, 20,
						10,  0,  0,  0,  0,  0,  0, 10,
						10,  0, -5,-10,-10, -5,  0, 10,
						10, -5, -5,-10,-10, -5, -5, 10,
						10,  0,-10,-10,-10,-10,  0, 10,
						10,-10,-10,-10,-10,-10,-10, 10,
						10, -5,  0,  0,  0,  0, -5, 10,
						20, 10, 10, 10, 10, 10, 10, 20 };
int White_Bishop[64];
							  
int Black_Rook[64] = { 0,  0,  0,  0,  0,  0,  0, 0,
					  -5,-10,-10,-10,-10,-10,-10,-5,
					   5,  0,  0,  0,  0,  0,  0, 5,
					   5,  0,  0,  0,  0,  0,  0, 5,
					   5,  0,  0,  0,  0,  0,  0, 5,
					   5,  0,  0,  0,  0,  0,  0, 5,
					   5,  0,  0,  0,  0,  0,  0, 5,
					   0,  0,  0, -5, -5,  0,  0, 0 };
int White_Rook[64];
							
int Black_Queen[64] = { 20, 10, 10,  5,  5, 10, 10, 20,
					    10,  0,  0,  0,  0,  0,  0, 10,
					    10,  0, -5, -5, -5, -5,  0, 10,
						 5,  0, -5, -5, -5, -5,  0,  5,
						 0,  0, -5, -5, -5, -5,  0,  5,
						10, -5, -5, -5, -5, -5,  0, 10,
						10,  0, -5,  0,  0,  0,  0, 10,
						20, 10, 10,  5,  5, 10, 10, 20 };
int White_Queen[64];
							  
int Black_King[64] = {  30, 40, 40, 50, 50, 40, 40, 30,
					    30, 40, 40, 50, 50, 40, 40, 30,
					    30, 40, 40, 50, 50, 40, 40, 30,
					    30, 40, 40, 50, 50, 40, 40, 30,
					    20, 30, 30, 40, 40, 30, 30, 20,
					    10, 20, 20, 20, 20, 20, 20, 10,
					   -20,-20,  0,  0,  0,  0,-20,-20,
					   -20,-30,-10,  0,  0,-10,-30,-20 };
int White_King[64];
							  
int pieceValues[15]{ 0, 0, PAWN_VALUE, -PAWN_VALUE, KNIGHT_VALUE, -KNIGHT_VALUE, BISHOP_VALUE, -BISHOP_VALUE, ROOK_VALUE, -ROOK_VALUE, QUEEN_VALUE, -QUEEN_VALUE, KING_VALUE, -KING_VALUE, 0 };
int *pieceSquareTables[15] = {Empty, Empty, White_Pawn, Black_Pawn, White_Knight, Black_Knight, White_Bishop, Black_Bishop, White_Rook, Black_Rook, White_Queen, Black_Queen, White_King, Black_King, Empty};

extern void Evaluate_Init()
{
	flip_piece_square(Black_Pawn, White_Pawn);
	flip_piece_square(Black_Knight, White_Knight);
	flip_piece_square(Black_Bishop, White_Bishop);
	flip_piece_square(Black_Rook, White_Rook);
	flip_piece_square(Black_Queen, White_Queen);
	flip_piece_square(Black_King, White_King);
}

int Evaluate(const Board::Board &board)
{
	int materialValue = 0;
	int positionalValue = 0;
	for (Board::ESquare sqr = Board::ESquare::A1; sqr <= Board::ESquare::H8; sqr++)
	{
		assert(sqr >= Board::ESquare::A1 && sqr <= Board::ESquare::H8);
		materialValue += pieceValues[board.PieceAt(sqr)];
		positionalValue += pieceSquareTables[board.PieceAt(sqr)][sqr];
	}
	
	//std::cout << "Material: " << materialValue << std::endl << "Position: " << positionalValue << std::endl;
	int eval = materialValue + positionalValue;
	
	if (board.GetPlayersTurn() == Board::EColor::WHITE)
		return eval;
	else
		return -eval;
}

int EvaluateFast(const Board::Board &board)
{
	int eval = board.GetEval();
	
	if (board.GetPlayersTurn() == Board::EColor::WHITE)
		return eval;
	else
		return -eval;
}

void Eval_RemovePiece(int &eval, uint8_t piece, Board::ESquare sqr)
{
	assert(piece >= Board::EPiece::W_PAWN && piece <= Board::EPiece::B_KING);
	assert(sqr >= Board::ESquare::A1 && sqr <= Board::ESquare::H8);
	eval -= pieceValues[piece];
	eval -= pieceSquareTables[piece][sqr];
}

void Eval_SetPiece(int &eval, uint8_t piece, Board::ESquare sqr)
{
	assert(piece >= Board::EPiece::W_PAWN && piece <= Board::EPiece::B_KING);
	assert(sqr >= Board::ESquare::A1 && sqr <= Board::ESquare::H8);
	eval += pieceValues[piece];
	eval += pieceSquareTables[piece][sqr];
}

//void Board::UpdateEval(Move &move)
//{
//	m_Eval -= pieceSquareTables[PieceAt(move.GetTo())][move.GetFrom()];
//	m_Eval += pieceSquareTables[PieceAt(move.GetTo())][move.GetTo()];
//	m_Eval -= pieceValues[move.GetCaptured()];
//	m_Eval -= pieceSquareTables[move.GetCaptured()][move.GetTo()];
//}

//extern void Evaluate_Test(char *filename)
//{
//	/*FILE *fp = fopen(filename, "r");
//	if (fp == NULL) 
//	{
//		fprintf(stderr, "Can't open input file %s!\n", filename);
//		exit(1);
//	}
//	char line[MAXLINE];
//	while(fgets(line, sizeof(line), fp) != 0)
//	{
//		Board board = Board(line);
//		int val = Evaluate(board);
//		printf("FEN: %s\tvalue: %d\n", line, val);
//	}*/
//}

static void flip_piece_square(int *piece_square, int *rev)
{
	int ii;
	for (ii = 0; ii < 32; ii++)
	{
		int row = ii / 8;
		int col = ii % 8;
		rev[ii] = -piece_square[(7-row)*8+col];
		rev[(7-row)*8+col] = -piece_square[ii];
	}
}