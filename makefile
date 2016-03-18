.PHONY: test reversi

test :
	g++ -std=c++11 ./test/*.cpp ./reversi/*.cpp ./uct/*.cpp
	./a.out

reversi :
	g++ -std=c++11 ./game/reversi_game.cpp ./reversi/*.cpp ./uct/*.cpp
	./a.out

reversi_demo:
	g++ -std=c++11 ./game/reversi_demo.cpp ./reversi/*.cpp ./uct/*.cpp
	./a.out
