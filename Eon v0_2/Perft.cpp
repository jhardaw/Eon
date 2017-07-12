#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <time.h>
#include <cassert>
#include "Board.h"
#include "Move.h"
#include "MoveGen.h"
#include "MoveList.h"

#define MAX_BUFF_LEN 100

using namespace std;

uint64_t Perft_PseudoLegal(Board::Board &board, int depth)
{
	if (depth == 0)
		return 1;

	uint64_t nodes = 0;
	Board::MoveList moveList(board);
	Board::GeneratePseudoLegalMoves(board, moveList);

	for (int ii = 0; ii < moveList.GetLength(); ii++)
	{
		Board::Move move = moveList.GetMove(ii);
		board.MakeMove(move);
		assert(board.Validate());
		if (!inCheck(board, !board.GetPlayersTurn()))
			nodes += Perft_PseudoLegal(board, depth - 1);
		board.UnmakeMove(move);
		assert(board.Validate());
	}

	return nodes;
}

uint64_t Perft_Legal(Board::Board &board, int depth)
{
	uint64_t nodes = 0;
	Board::MoveList moveList(board);
	Board::GenerateLegalMoves(board, moveList);

	if (depth == 0)
		return 1;

	if (depth == 1)
	{
		return moveList.GetLength();
	}	
	
	for (int ii = 0; ii < moveList.GetLength(); ii++)
	{
		Board::Move move = moveList.GetMove(ii);
		board.MakeMove(move);
		assert(board.Validate());
		nodes += Perft_Legal(board, depth-1);
		board.UnmakeMove(move);
		assert(board.Validate());
	}
	
	return nodes;
}

void Divide(Board::Board &board, int depth)
{
	Board::MoveList moveList(board);
	Board::GenerateLegalMoves(board, moveList);
	uint64_t nodes = 0;
	for (int ii = 0; ii < moveList.GetLength(); ii++)
	{
		Board::Move move = moveList.GetMove(ii);
		board.MakeMove(move);
		assert(board.Validate());
		uint64_t subNodes = Perft_Legal(board, depth - 1);
		nodes += subNodes;
		cout << move.ToString() << "\t" << subNodes << endl;
		board.UnmakeMove(move);
		assert(board.Validate());
	}
	
	cout << "Moves: " << moveList.GetLength() << endl;
	cout << "Nodes: " << nodes << endl;
}

void split(const std::string &s, char delim, std::vector<std::string> &elems) 
{
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim)) 
	{
		elems.push_back(item);
	}
}

void Perft_Test(void)
{
	const int MAXLINE = 100;
	ifstream file;
	bool noError = true;
	file.open("C:\\Users\\Ben\\Documents\\Visual Studio 2015\\Projects\\Eon v0_2\\Eon v0_2\\PerftTestPositions.txt", ios::out);
	if (file.is_open())
	{
		string line;
		Board::MoveGen_Init();
		Board::Board board = Board::Board();
		while (getline(file, line) && noError)
		{
			if (line.find("perft") != string::npos && line.find("ignore") == string::npos)
			{
				vector<string> getFEN;
				split(line, ';', getFEN);

				string fen = getFEN[0];
				vector<string> tokens;
				split(getFEN[1], ' ', tokens);

				int depth = stoi(tokens[2]);
				uint64_t answer = stoull(tokens[4]);

				cout << "Loading FEN: " << fen << endl;
				board.ParseFEN(fen);
				cout << "\tDepth: " << depth << endl;
				cout << "\tAnswer: " << answer << endl;

				clock_t startTime = clock();
				uint64_t nodes = Perft_PseudoLegal(board, depth);
				cout << "\tNodes: " << nodes << endl;
				if (nodes != answer)
				{
					noError = false;
					cout << "Error: Perft result does not match expected answer. . ." << endl;
				}
				double time = double(clock() - startTime) / (double)CLOCKS_PER_SEC;
				cout << "time: " << time << " seconds\t nps: " << nodes/time << endl << endl;
			}
			else if (line.find("//--") != string::npos)
			{
				cout << endl << line << endl;
			}
		}
	}
	file.close();
}

