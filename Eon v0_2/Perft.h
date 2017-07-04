#pragma once

/***********************************************************************************
*  File: Perft.h
*  Author: Ben Hardaway
*  Purpose: Header file for the C++ implementation of Perft testing functions
**********************************************************************************/

#include <stdint.h>
#include "Board.h"

uint64_t Perft_RemoveChecks(Board::Board &board, int depth);
uint64_t Perft_PseudoLegal(Board::Board &board, int depth);
void Divide(Board::Board &board, int depth);
void Perft_Test(void);
void split(const std::string &s, char delim, std::vector<std::string> &elems);