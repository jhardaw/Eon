#include "Search.h"
#include "Board.h"
#include "UCI.h"
#include "Perft.h"

//#define _CRTDBG_MAP_ALLOC  
//#include <stdlib.h>  
//#include <crtdbg.h>

#define MAX_BUFF_LEN 100

int main(int argc, char **argv)
{
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	UCI::UCI eon = UCI::UCI();
	eon.Start();
	
	//Perft_Test();

	return 0;
}