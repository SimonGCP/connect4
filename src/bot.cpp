#include "../include/board.h"
#include <random>
#include <thread>

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
float evaluate(board gameBoard, float alpha, float beta, int depth) {
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
	
		curMoveEval = evaluate(boardCopy, alpha, beta, depth - 1);

		if (minimizing) {
			if (beta <= alpha) {
				break;
			}

			bestMoveEval = std::min(curMoveEval, bestMoveEval);
			beta = std::min(curMoveEval, beta);
			// bestMoveEval = curMoveEval;
		} else if (!minimizing) {
			if (beta <= alpha) {
				break;
			}

			bestMoveEval = std::max(curMoveEval, bestMoveEval);
			alpha = std::max(curMoveEval, alpha);
			// bestMoveEval = curMoveEval;
		}
	}

	return bestMoveEval;
}

// recursive minimax function that returns bot's column choice
int getBotChoice(board gameBoard, int depth) {
	int bestMove = gameBoard.getLegalMoves()[0]; 
	bool minimizing = gameBoard.getTurn() % 2 == 1;
	float curEval = 0.0f, maxEval = minimizing ? 1000.0f : -1000.0f;
    
	std::random_device rd;
	std::mt19937 gen(rd());
	
	std::uniform_int_distribution<> dis(0, 1);

	std::vector<std::thread> threads(COLS);
	std::vector<float> evals(COLS);

	for (int i = 1; i <= COLS; i++) {
		board boardCopy = gameBoard;

		column curColumn = boardCopy[i - 1];
		if (!curColumn.isLegal())
			continue;

		if (!boardCopy.dropPiece(i)) 
			continue;

		threads[i-1] = std::thread([&evals, i, boardCopy, depth]() {
			evals[i-1] = evaluate(boardCopy, -1000, 1000, depth);
		});
	}


	for (int i = 0; i < COLS; i++) {
		if (threads[i].joinable()) {
			threads[i].join();
		} else {
			continue;
		} 

		curEval = evals[i];

		// if two moves have identical evaluations
		// randomly pick between the two
		if (curEval == maxEval) {
			if(dis(gen)) {
				bestMove = i+1;
			} 
		}

		if (minimizing && curEval < maxEval) {
			bestMove = i+1;
			maxEval = curEval;
		} else if (!minimizing && curEval > maxEval) {
			bestMove = i+1;
			maxEval = curEval;
		}
	}

	std::cout << "Best move eval: " << maxEval << std::endl;
	std::cout << "I choose column " << bestMove << std::endl;
	return bestMove;
}
