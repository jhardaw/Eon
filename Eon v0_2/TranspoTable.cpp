/***********************************************************************************
*  File: TranspoTable.cpp
*  Author: Ben Hardaway
*  Purpose: C++ implementation of transposition table using Zorbist Hashing and 
*           "always replace" replacement scheme
**********************************************************************************/
#include <random>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "Board.h"
#include "TranspoTable.h"

#define HASHSIZE 1048583

std::random_device rd;
std::mt19937_64 gen(rd());
std::uniform_int_distribution<uint64_t> dis;

struct HashEntry
{
	uint64_t lock; // Zorbist Key
	int depth;
	int flag;
	int value;
	Board::Move move;
};

uint64_t hash_keys[12][64];
HashEntry *hash_table;

void TranspoTable_Init()
{
	for (int piece = 0; piece < 12; piece++)
	{
		for (int sqr = 0; sqr < 64; sqr++)
		{
			hash_keys[piece][sqr] = dis(gen);
		}
	}
	hash_table = new HashEntry[HASHSIZE]();
}

void TranspoTable_Destruct()
{
	delete hash_table;
}

uint64_t ZorbistKey(const Board::Board &board)
{
	uint64_t zorbist = 0;
	for (Board::ESquare sqr = Board::ESquare::A1; sqr <= Board::ESquare::H8; sqr++)
	{
		Board::EPiece piece = board.PieceAt(sqr);
		if (piece != Board::EMPTY)
		{
			//std::cout << std::to_string(hash_keys[piece - 2][sqr]) + "\n";
			zorbist ^= hash_keys[piece - 2][sqr];
		}
	}
	return zorbist;
}

void Zorbist_RemovePiece(uint64_t &zorbist, uint8_t piece, int sqr)
{
	zorbist ^= hash_keys[piece - 2][sqr];
}

void Zorbist_SetPiece(uint64_t &zorbist, uint8_t piece, int sqr)
{
	zorbist ^= hash_keys[piece - 2][sqr];
}

int ProbeHash(const Board::Board &board, int depth, int alpha, int beta)
{
	uint64_t zorbist_key = board.GetZorbist();
	int hash_key = (int)(zorbist_key % HASHSIZE);
	if (hash_table[hash_key].lock == zorbist_key)
	{
		if (hash_table[hash_key].depth >= depth)
		{
			if (hash_table[hash_key].flag == HASH_EXACT)
				return hash_table[hash_key].value;
			else if (hash_table[hash_key].flag == HASH_ALPHA && hash_table[hash_key].value <= alpha)
				return alpha;
			else if (hash_table[hash_key].flag == HASH_BETA && hash_table[hash_key].value >= beta)
				return beta;
		}
		// mark move as best guess move
	}
	return VAL_UNKNOWN;
}

void RecordHash(const Board::Board &board, int depth, int value, int flag)
{
	uint64_t zorbist = ZorbistKey(board);
	int key = (int)(zorbist % HASHSIZE);
	hash_table[key].lock = zorbist;
	hash_table[key].depth = depth;
	hash_table[key].flag = flag;
	hash_table[key].value = value;
	hash_table[key].move = Board::Move();
}