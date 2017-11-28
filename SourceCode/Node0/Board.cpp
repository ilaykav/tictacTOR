#include "Board.h"


int Board::checkWin()
{
	if (boardArray[0] == boardArray[4] && boardArray[0] == boardArray[8] && boardArray[0] != 0) { return boardArray[0]; }
	if (boardArray[2] == boardArray[4] && boardArray[2] == boardArray[6] && boardArray[2] != 0) { return boardArray[2]; }
	for (int i = 0; i < sizeof(boardArray) / sizeof(boardArray[0]); i += 3) {
		if ((boardArray[i] == boardArray[i + 1] && boardArray[i + 2] == boardArray[i]) && boardArray[i] != 0)
			return boardArray[i];
	}
	for (int i = 0; i < 3; i += 1) {
		if ((boardArray[i] == boardArray[i + 3] && (boardArray[i] == boardArray[i + 6])) && boardArray[i] != 0)
			return boardArray[i];
	}
	for (int i = 0; i < 9; i += 1) {
		if (boardArray[i] == 0) {
			return 0; // there is a place empty 
		}
	}
	return 3; // board is full
}

std::string Board::updateHandler(int updateRet)
{
	switch (updateRet) {
		case(-2) :
			return "Please select differrent, hole out of bounds. ";
		case (-1):
				return "Please select an empty hole. ";
		case (1):
				return "Congratulations. You won. ";
		case (2):
				return "Your opponent won. ";
		case(0):
				return "updated succefully, please wait for your opponent turn.";
		case(3) :
			return "it's a tie!";

	}
	std::string ret = "weird, returned with ";
	ret.append(std::to_string(updateRet));
	return ret;
}

int Board::update(int hole, int player)
{
	hole--;
	if (0 > hole || hole > 8) {
		return -2;
	}
	
	if (boardArray[hole] != 0)
		return -1;

	boardArray[hole] = player;

	int winner = this->checkWin();
	if (winner != 0)
		return winner;
	system("cls");
	return 0;
}


std::string Board::getStringBoard()
{
	std::string map[2] = { "X", "O" };
	std::string board = "\n ";
	for (int i = 0; i < sizeof(boardArray) / sizeof(boardArray[0]); i++) {
		board.append(" ");
		if (boardArray[i] == 1 || boardArray[i] == 2)
			board.append(map[boardArray[i] - 1]);
		else
			board.append(std::to_string(i + 1));
		board.append(" ");
		if ((i + 1) % 3 == 0)
			board.append(" \n ");
	}
	return board;
}


Board::Board()
{
}


Board::~Board()
{
}
