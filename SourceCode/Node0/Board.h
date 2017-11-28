#pragma once
#include <string>
#include <tuple>

class Board
{
private:
	int boardArray[9] = { 0 };
public:
	const std::string instrucations = " Tic Tac Tor is a secure "
		"two-player game, which every player needs to put in his"
		"turn his sign in a hole, counted from 1 - 9; like this: \n ";
	int Board::checkWin();
	int update(int hole, int player);
	std::string updateHandler(int updateRet);
	std::string getStringBoard();
	Board();
	~Board();
};

