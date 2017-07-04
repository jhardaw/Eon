# makefile for Eon Chess Engine
# Version: 1
#
# -lm is used to link in the math library
#
# -Wall turns on all warning messages 
#
comp = g++
optimize_flags = -Wall -O2
debug_flags = -Wall -g
comp_libs = -lm -pthread

Eon: Main.o Move.o Board.o MoveGen.o Search.o Evaluate.o
	$(comp) $(debug_flags) Main.o Move.o Board.o MoveGen.o -o Eon $(comp_libs)

EonPerft: Perft.o Move.o Board.o MoveGen.o
	$(comp) $(debug_flags) Perft.o Move.o Board.o MoveGen.o -o EonPerft $(comp_libs)
	
Main.o: Main.cpp
	$(comp) $(debug_flags) -c Main.cpp
	
Perft.o: Perft.cpp
	$(comp) $(debug_flags) -c Perft.cpp

Move.o: Move.cpp Move.h
	$(comp) $(debug_flags) -c Move.cpp Move.h

Board.o: Board.cpp Board.h
	$(comp) $(debug_flags) -c Board.cpp Board.h
	
MoveGen.o: MoveGen.cpp MoveGen.h
	$(comp) $(debug_flags) -c MoveGen.cpp MoveGen.h
	
UCI.o: UCI.cpp UCI.h
	$(comp) $(debug_flags) -c UCI.cpp UCI.h
	
Search.o: Search.cpp Search.h
	$(comp) $(debug_flags) -c Search.cpp Search.h
	
Evaluate.o: Evaluate.cpp Evaluate.h
	$(comp) $(debug_flags) -c Evaluate.cpp Evaluate.h

clean :
	rm -f *.o *.gch Eon EonPerft gmon.out

profile: gmon.out
	gprof Eon gmon.out > prof_output

tarball: README.txt makefile *.h *.cpp
	tar cvzf eon.tgz README.txt makefile *.cpp *.h
