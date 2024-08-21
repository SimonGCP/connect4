#pragma once

//
//	DEFINITIONS OF BOARD COMPONENTS - pieces, columns & entire board
//

#include <iostream>
#include <vector>

#define COLS 7
#define ROWS 6

enum Color {
	EMPTY,
	RED,
	YELLOW
};

// one column of the board
class column {
	public:
		int pieceCount;
		int pieces[ROWS];

		column() {
			pieceCount = 0;
			for (int i = 0; i < ROWS; i++) {
				pieces[i] = Color::EMPTY;
			}
		}

		int getPieceCount() {
			return pieceCount;
		}

		int operator[](int index) {
			return pieces[index];
		}

		bool addPiece(int newPiece) {
			if (pieceCount >= ROWS) {
				return false;
			}

			pieces[ROWS - 1 - pieceCount] = newPiece;
			pieceCount++;
			return true;
		}

		bool isLegal() {
			return pieceCount <= ROWS;
		}
};

enum GameOverStatus {
	RED_WIN,
	YELLOW_WIN,
	TIE,
	ONGOING
};

class board {
	public:
		column columns[7];
		int turn;

		board() {
			turn = 0;
		}

		int getTurn() {
			return turn;
		}

		column operator[](int i) {
			return columns[i];
		}

		void displayBoard() {
			for (int i = 0; i < ROWS; i++) {
				std::cout << "🟦";
				for (int j = 0; j < COLS; j++) {
					switch (columns[j][i]) {
						case Color::RED:
							std::cout << "🔴";
							break;
						case Color::YELLOW:
							std::cout << "🟡";
							break;
						case Color::EMPTY:
							std::cout << "⚪";
							break;
						default:
							std::cout << columns[i][j];
							std::cout << "";
					}
				}
				std::cout << "🟦";
				std::cout << std::endl;
			}
			// next line is broken on nvim, don't touch it :)
			std::cout << "🟦1️⃣2️⃣3️⃣4️⃣5️⃣6️⃣7️⃣🟦";
			std::cout << std::endl;
		}

		std::vector<int> getLegalMoves() {
			std::vector<int> legalMoves;

			for (int i = 0; i < COLS; i++) {
				if (columns[i].isLegal()) {
					legalMoves.push_back(i + 1);		
				}
			}

			return legalMoves;
		}

		bool dropPiece(int col) {
			if (col < 1 || col > 7) {
				return false;
			}

			int index = col - 1;
			if (!columns[index].isLegal()) {
				return false;
			}

			if(columns[index].addPiece(turn % 2 ? Color::YELLOW : Color::RED)) {
				turn++;
				return true;
			} else {
				return false;
			}
		}

		GameOverStatus checkGameOver() {
			int streak = 0;
			int curColor = Color::EMPTY,  prevColor = Color::EMPTY;

			// check vertical
			for (auto curColumn : columns) {
				streak = 0;
				prevColor = Color::EMPTY;
				for (int i = 0; i < ROWS; i++) {
					curColor = curColumn[i];

					if (curColor == prevColor && curColor != Color::EMPTY) {
						streak++;

						if (streak == 4) {
							return curColor == Color::RED ?
								GameOverStatus::RED_WIN :
								GameOverStatus::YELLOW_WIN;
						}
						continue;
					}

					streak = curColor != Color::EMPTY ? 1 : 0;
					prevColor = curColor;
				}
			}

			// check horizontal
			for (int i = 0; i < ROWS; i++) {
				streak = 0;
				prevColor = Color::EMPTY;
				for (int j = 0; j < COLS; j++) {
					curColor = columns[j][i];
					if (curColor == prevColor && curColor != Color::EMPTY) {
						streak++;

						if (streak == 4) {
							return curColor == Color::RED ?
								GameOverStatus::RED_WIN :
								GameOverStatus::YELLOW_WIN;
						}
						continue;
					}

					streak = curColor != Color::EMPTY ? 1 : 0;
					prevColor = curColor;
				}
			}

			// check diagonal
			for (int i = 0; i < COLS; i++) {
				for (int j = 0; j < ROWS; j++) {
					if (columns[i][j] != Color::EMPTY) {
						curColor = columns[i][j];
						// check up and to left
						for (int k = 1; k <= 3; k++) {
							if (i - k < 0 || j - k < 0) {
								break;
							}
							if (columns[i-k][j-k] != curColor) {
								break;
							}

							if (k == 3) {
								return curColor == Color::RED?
									GameOverStatus::RED_WIN :
									GameOverStatus::YELLOW_WIN;
							}
						}

						// check up and to right
						for (int k = 1; k <= 3; k++) {
							if (i + k >= COLS || j - k < 0) {
								break;
							}
							if (columns[i+k][j-k] != curColor) {
								break;
							}

							if (k == 3) {
								return curColor == Color::RED?
									GameOverStatus::RED_WIN :
									GameOverStatus::YELLOW_WIN;
							}
						}
					}
				}
			}

			if (turn >= COLS * ROWS) {
				return GameOverStatus::TIE;
			}

			return GameOverStatus::ONGOING;
		}
};
