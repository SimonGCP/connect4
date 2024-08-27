#pragma once

//
//	DEFINITIONS OF BOARD COMPONENTS - pieces, columns & entire board
//

#include <iostream>
#include <vector>

#define BOT_DEPTH 18 

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

		uint64_t redBitboard;
		uint64_t yellowBitboard;
		uint8_t heights[COLS] = {0, 7, 14, 21, 28, 35, 42};

		board() {
			turn = 0;
			redBitboard = 0;
			yellowBitboard = 0;
		}

		int getTurn() {
			return turn;
		}

		column operator[](int i) {
			return columns[i];
		}

		void displayBoard() {
			for (int i = 5; i >= 0; i--) {
				uint64_t idx = 1UL << i;
				std::cout << "🟦";
				for (int j = 0; j < COLS; j++) {
					if (redBitboard & idx) {
						std::cout << "🔴";
					} else if (yellowBitboard & idx) {
						std::cout << "🟡";
					} else {
						std::cout << "⚪";
					}

					idx <<= 7;
				}
				std::cout << "🟦\n";
			}

			// next line is broken on nvim, don't touch it :)
			std::cout << "🟦1️⃣2️⃣3️⃣4️⃣5️⃣6️⃣7️⃣🟦";
			std::cout << std::endl;
		}

		std::vector<int> getLegalMoves() {
			const uint64_t top = 0b1000000100000010000001000000100000010000001000000UL;
			std::vector<int> legalMoves;

			for (int i = 0; i < COLS; i++) {
				if (!(1L << heights[i] & top))
					legalMoves.push_back(i);
			}

			return legalMoves;
		}

		bool dropPiece(int col) {
			if (col < 1 || col > 7) {
				return false;
			}
			col--;

			// check if legal
			if (heights[col] >= col * 7 + ROWS) return false;

			int curColor = turn & 1 ? Color::YELLOW : Color::RED;

			uint64_t idx = 1UL << heights[col];
			if (curColor == Color::RED) {
				redBitboard |= idx;	
			} else {
				yellowBitboard |= idx;
			}
			heights[col]++;

			if(columns[col].addPiece(curColor)) {
				turn++;
				return true;
			} else {
				return false;
			}

		}

		// thanks to 
		// https://github.com/denkspuren/BitboardC4/blob/master/BitboardDesign.md
		bool isWin(uint64_t bitboard) {
			int directions[4] = {1, 7, 6, 8};
			uint64_t bb;
			for(int direction : directions) {
				bb = bitboard & (bitboard >> direction);
				if ((bb & (bb >> (2 * direction))) != 0) return true;
			}
			return false;
		}

		GameOverStatus checkGameOver() {
			if (isWin(redBitboard)) return GameOverStatus::RED_WIN;
			if (isWin(yellowBitboard)) return GameOverStatus::YELLOW_WIN;

			if (turn >= COLS * ROWS) {
				return GameOverStatus::TIE;
			}

			return GameOverStatus::ONGOING;
		}
};
