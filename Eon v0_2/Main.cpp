#include "Search.h"
#include "Board.h"
#include "UCI.h"

//#define _CRTDBG_MAP_ALLOC  
//#include <stdlib.h>  
//#include <crtdbg.h>

#define MAX_BUFF_LEN 100

int main(int argc, char **argv)
{
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	UCI::UCI eon = UCI::UCI();
	eon.Start();
	
	return 0;
}


//void makeUserMove(Board& board)
//{
//	cout << "Move: ";
//	string lfAlgebra;
//	getline(cin, lfAlgebra);
//	Move userMove = Move(lfAlgebra);
//	cout << "Move read: " << userMove.ToString() << endl;
//	
//	MoveList moveList(board);
//	moveList.RemoveChecks();
//	bool userMoveFound = false;
//	for (int ii = 0; ii < moveList.GetLength(); ii++)
//	{
//		if (moveList.GetMove(ii).GetFrom() == userMove.GetFrom() && moveList.GetMove(ii).GetTo() == userMove.GetTo())
//		{
//			userMoveFound = true;
//		}
//	}
//
//	if (userMoveFound)
//	{
//		board.MakeMove(userMove);
//	}
//	else
//	{
//		cout << "Invalid move: " << userMove.ToString() << endl;
//		makeUserMove(board);
//	}
//}

//void EngineVsUser()
//{
//	string initial = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
//	Board board = Board(initial);
//	MoveGen_Init();
//	Evaluate_Init();
//	bool endGame = false;
//	while (!endGame)
//	{
//		cout << board.ToString() << endl;
//
//		if (board.GetPlayersTurn() == WHITE)
//		{
//			makeUserMove(board);
//		}
//		else
//		{
//			Move move = BestMove(board, );
//			cout << "Move: " << move.ToString() << endl;
//			board.MakeMove(move);
//		}
//	}
//}