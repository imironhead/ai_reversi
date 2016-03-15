.PHONY: test

test :
	g++ -std=c++11 reversi.cpp state.cpp uct.cpp ./test/*.cpp
	./a.out

run :
	g++ -std=c++11 *.cpp
	./a.out
