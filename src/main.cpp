#include <iostream>
#include <string>

#include "../include/board.h"
#include "../include/bot.h"

#define BOT_DEPTH 6 

enum GameMode {
	VS_PLAYER,
	VS_BOT,
	BOT_VS_BOT
};

int curGameMode = VS_PLAYER;

int getGameMode() {
	std::cout << "Welcome to Connect 4! Select a game mode:\n";
	std::string input;
	bool selected = false;

	while (!selected) {
		try {
			std::cout << "Enter 1 for VS. Human\n";
			std::cout << "Enter 2 for VS. Bot\n";
			std::cout << "Enter 3 for BOT VS. BOT mode\n";

			std::cin >> input;
			int chosenGameMode = stoi(input);
			if (chosenGameMode != 1 && chosenGameMode != 2 && chosenGameMode != 3) {
				throw "invalid choice";
			}

			switch (chosenGameMode) {
				case 1:
					return GameMode::VS_PLAYER;
				case 2:
					return GameMode::VS_BOT;
				case 3:
					return GameMode::BOT_VS_BOT;
				default:
					throw "invalid choice";
			}
		} catch(...) {
			std::cout << "Invalid selection\n";
		}
	}
	return -1;
}


// game loop helper functions
bool gameOngoing(board &gameBoard) {
	return gameBoard.checkGameOver() == GameOverStatus::ONGOING;
}

int getUserChoice() {
	std::string input;
	bool selected = false;

	while (!selected) {
		try {
			std::cout << "Select a row (1-" << ROWS << "): ";
			std::cin >> input;

			int rowChoice = stoi(input);
			if (rowChoice <= 7 && rowChoice > 0) {
				return rowChoice;
			} else {
				throw "invalid number";
			}
		} catch (...) {
			std::cout << "Invalid selection.\n";
		}
	}

	return -1;
}

void gameLoop(board &gameBoard) {
	gameBoard.displayBoard();
	while(gameOngoing(gameBoard)) {
		const char *message = gameBoard.getTurn() % 2 ? "Yellow's turn" : "Red's turn";
		std::cout << message << std::endl;

		bool usersTurn = true;
		if (curGameMode == GameMode::BOT_VS_BOT) {
			usersTurn = false;	
		} else if (curGameMode == GameMode::VS_BOT) {
			usersTurn = gameBoard.getTurn() % 2 == 0;	
		}

		int rowChoice = 0;
		if (usersTurn) { 
			rowChoice = getUserChoice();
		} else {
			rowChoice = getBotChoice(gameBoard, BOT_DEPTH);	
		}

		if (!gameBoard.dropPiece(rowChoice)) {
			std::cout << "Invalid selection\n";
		}

		gameBoard.displayBoard();
		if (!gameOngoing(gameBoard)) {
			break;
		}
	} 

	switch(gameBoard.checkGameOver()) {
		case GameOverStatus::TIE:
			std::cout << "Tie!\n";
			break;
		case GameOverStatus::RED_WIN:
			std::cout << "Red wins!\n";
			break;
		case GameOverStatus::YELLOW_WIN:
			std::cout << "Yellow wins!\n";
			break;
		default:
			std::cout << "err";
			break;
	}
}

int main(int argc, char *argv[]) {
	curGameMode = getGameMode();
	board gameBoard = board();

	gameLoop(gameBoard);
}
