#pragma once
/***********************************************************************************
*  File: TranspoTable.h
*  Author: Ben Hardaway
*  Purpose: Header file for the C++ implementation of the Transposition Tab le
**********************************************************************************/

#define VAL_UNKNOWN 999999999
#define HASH_EXACT 0
#define HASH_ALPHA 1
#define HASH_BETA 2

void TranspoTable_Init();
void TranspoTable_Destruct();

uint64_t ZorbistKey(const Board::Board &board);
void Zorbist_RemovePiece(uint64_t &zorbist, uint8_t piece, int sqr);
void Zorbist_SetPiece(uint64_t &zorbist, uint8_t piece, int sqr);

int ProbeHash(const Board::Board &board, int depth, int alpha, int beta);
void RecordHash(const Board::Board &board, int depth, int value, int flag);