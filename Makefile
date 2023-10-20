CC = g++


run: shell379
	echo "======================="
	./shell379

shell379: shell379.o jobs.o manipulatestring.o
	$(CC) -o shell379 shell379.o jobs.o manipulatestring.o

shell379.o: main.cpp
	$(CC) -c main.cpp -o shell379.o

jobs.o: jobs.cpp jobs.h manipulatestring.o
	$(CC) -c jobs.cpp -o jobs.o

manipulatestring.o: manipulatestring.cpp manipulatestring.h
	$(CC) -c manipulatestring.cpp -o manipulatestring.o

tester: tester.cpp
	$(CC) tester.cpp -o tester
	./tester