#include "../include/board.h"

#define BOT_DEPTH 5 

// 
// The bot uses a minimax algorithm to decide its best move
//
const float boardPriority[ROWS][COLS] = {
	{0.0, 0.0, 0.0, 0.1, 0.0, 0.0, 0.0},
	{0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1},
	{0.1, 0.2, 0.2, 0.3, 0.2, 0.2, 0.2},
	{0.2, 0.2, 0.3, 0.3, 0.3, 0.3, 0.3},
	{0.3, 0.4, 0.4, 0.5, 0.4, 0.4, 0.3},
	{0.6, 0.5, 0.5, 1.0, 0.5, 0.5, 0.6}
};

float staticPositionEvaluate(board gameBoard, int depth) {
	if (gameBoard.checkGameOver() != GameOverStatus::ONGOING) {
		switch (gameBoard.checkGameOver()) {
			case GameOverStatus::RED_WIN:
				return 999.9f - (BOT_DEPTH - depth);
			case GameOverStatus::YELLOW_WIN:
				return -999.9f + (BOT_DEPTH - depth);
			case GameOverStatus::TIE:
				return 0.0f;
			default:
				break;
		}
	}

	float eval = 0.0;

	for (int i = 0; i < COLS; i++) {
		for (int j = 0; j < ROWS; j++) {
			const int color = gameBoard[i][j];
			if (color == Color::RED) {
				eval += boardPriority[j][i];
			} else if (color == Color::YELLOW) {
				eval -= boardPriority[j][i];
			}
		}
	}

	return eval;
}

// evaluate returns a positive value for a red position and negative for yellow
float evaluate(board gameBoard, int depth) {
	// bot should try to minimize if playing as yellow
	bool minimizing = gameBoard.getTurn() % 2 == 1; 

	if (depth == 0 || gameBoard.checkGameOver() != GameOverStatus::ONGOING) {
		return staticPositionEvaluate(gameBoard, depth);
	}

	board boardCopy = gameBoard;

	float curMoveEval = 0.0f, bestMoveEval = minimizing ? 1000.0f : -1000.0f;

	for (int i = 1; i <= COLS; i++) {
		boardCopy = gameBoard;

		column curColumn = boardCopy[i - 1];
		if (!curColumn.isLegal()) {
			continue;
		}

		boardCopy.dropPiece(i);
	
		curMoveEval = evaluate(boardCopy, depth - 1);

		if (minimizing && curMoveEval < bestMoveEval) {
			bestMoveEval = curMoveEval;
		} else if (!minimizing && curMoveEval > bestMoveEval) {
			bestMoveEval = curMoveEval;
		}
	}

	// if (minimizing) {
	// 	for (int i = 1; i <= COLS; i++) {
	// 		boardCopy = gameBoard;

	// 		column curColumn = boardCopy[i - 1];
	// 		if (!curColumn.isLegal()) {
	// 			continue;
	// 		}

	// 		boardCopy.dropPiece(i);
	// 	
	// 		curMoveEval = evaluate(boardCopy, depth - 1);

	// 		if (curMoveEval < bestMoveEval) {
	// 			bestMoveEval = curMoveEval;
	// 		}
	// 	}
	// } else {
	// 	for (int i = 1; i <= COLS; i++) {
	// 		boardCopy = gameBoard;

	// 		column curColumn = boardCopy[i - 1];
	// 		if (!curColumn.isLegal()) {
	// 			continue;
	// 		}

	// 		boardCopy.dropPiece(i);
	// 	
	// 		curMoveEval = evaluate(boardCopy, depth - 1);

	// 		if (curMoveEval > bestMoveEval) {
	// 			bestMoveEval = curMoveEval;
	// 		}
	// 	}
	// }

	return bestMoveEval;
}

// recursive minimax function that returns bot's column choice
int getBotChoice(board gameBoard, int depth) {
	int bestMove = gameBoard.getLegalMoves()[0]; 
	bool minimizing = gameBoard.getTurn() % 2 == 1;
	float curEval = 0.0f, maxEval = minimizing ? 1000.0f : -1000.0f;

	for (int i = 1; i <= COLS; i++) {
		board boardCopy = gameBoard;

		column curColumn = boardCopy[i - 1];
		if (!curColumn.isLegal())
			continue;

		if (!boardCopy.dropPiece(i)) {
			continue;
		}

		curEval = evaluate(boardCopy, depth);

		if (minimizing && curEval < maxEval) {
			bestMove = i;
			maxEval = curEval;
		} else if (!minimizing && curEval > maxEval) {
			bestMove = i;
			maxEval = curEval;
		}
	}

	std::cout << "Best move eval: " << maxEval << std::endl;
	std::cout << "I choose column " << bestMove << std::endl;
	return bestMove;
}
