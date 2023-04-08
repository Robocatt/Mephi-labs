all: main.o tasks.o
	g++ --std=c++2a -ftemplate-depth=10000000000 main.o tasks.o -o main
	./main

main.o: main.cpp
	g++ --std=c++2a -c main.cpp -o main.o

clean:
	rm -f main.o tasks.o main
