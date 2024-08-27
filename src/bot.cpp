#include "../include/board.h"
#include "../include/transposition_table.h"
#include <random>
#include <thread>
#include <mutex>

std::mutex tableLock;

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
			const uint64_t idx = (1ULL << (COLS * i)) << j;

			if (gameBoard.redBitboard & idx) {
				eval += boardPriority[j][i];
			} else if (gameBoard.yellowBitboard & idx) {
				eval -= boardPriority[j][i];
			}
		}
	}

	return eval;
}

// evaluate returns a positive value for a red position and negative for yellow
float evaluate(board gameBoard, float alpha, float beta, int depth, t_table *table) {
	// bot should try to minimize if playing as yellow
	bool minimizing = gameBoard.getTurn() % 2 == 1; 
	int type = evaluation_type::UPPER_BOUND;
	uint64_t bitboards[2] = {gameBoard.redBitboard, gameBoard.yellowBitboard};

	if (depth == 0 || gameBoard.checkGameOver() != GameOverStatus::ONGOING) {
		const float eval = staticPositionEvaluate(gameBoard, depth);

		tableLock.lock();
		table->put(bitboards, eval, !minimizing, evaluation_type::EXACT, 0);
		tableLock.unlock();

		return eval;
	}

	// lookup position in t-table
	tableLock.lock();
	positionInfo *tableRes = table->get(bitboards, !minimizing);
	tableLock.unlock();

	// if an entry is found in the t-table
	if (tableRes != NULL && tableRes->depth >= depth) {
		int retType = tableRes->type;
		float retEval = tableRes->eval;
		
		switch (retType) {
			case evaluation_type::EXACT:
				return retEval;
			case evaluation_type::UPPER_BOUND:
				if (retEval < alpha)
					return retEval;
				break;
			case evaluation_type::LOWER_BOUND:
				if (retEval >= beta)
					return retEval;
				break;
		}
	}

	board boardCopy = gameBoard;

	float curMoveEval = 0.0f, bestMoveEval = minimizing ? 1000.0f : -1000.0f;

	for (int i : gameBoard.getLegalMoves()) {
		boardCopy = gameBoard;

		column curColumn = boardCopy[i - 1];
		if (!curColumn.isLegal()) {
			continue;
		}

		boardCopy.dropPiece(i);
	
		curMoveEval = evaluate(boardCopy, alpha, beta, depth - 1, table);

		if (minimizing) {
			bestMoveEval = std::min(curMoveEval, bestMoveEval);
			beta = std::min(curMoveEval, beta);

			if (beta <= alpha) {
				break;
			}
		} else if (!minimizing) {
			bestMoveEval = std::max(curMoveEval, bestMoveEval);
			alpha = std::max(curMoveEval, alpha);

			if (beta <= alpha) {
				break;
			}
		}
	}

	bitboards[0] = gameBoard.redBitboard;
	bitboards[1] = gameBoard.yellowBitboard;

	type = evaluation_type::EXACT;
	if (bestMoveEval > alpha) type = evaluation_type::LOWER_BOUND;
	if (bestMoveEval < beta) type = evaluation_type::UPPER_BOUND;

	tableLock.lock();
	table->put(bitboards, bestMoveEval, !minimizing, type, depth);
	tableLock.unlock();

	return bestMoveEval;
}

// recursive minimax function that returns bot's column choice
int getBotChoice(board gameBoard, int depth, t_table *table) {
	std::vector<int> legalMoves = gameBoard.getLegalMoves();
	int bestMove = legalMoves[0]; 
	bool minimizing = gameBoard.getTurn() % 2 == 1;
	float curEval = 0.0f, maxEval = minimizing ? 1000.0f : -1000.0f;
    
	std::vector<std::thread> threads(COLS);
	std::vector<float> evals(COLS);

	for (int i = 0; i < COLS; i++) {
		board boardCopy = gameBoard;

		if (!boardCopy.dropPiece(i+1)) continue;

		threads[i] = std::thread([&evals, i, boardCopy, depth, table]() {
			evals[i] = evaluate(boardCopy, -1000, 1000, depth, table);
		});	
	}

	for (int i = 0; i < COLS; i++) {
		if (threads[i].joinable()) {
			threads[i].join();
		} else {
			continue;
		} 
		
		curEval = evals[i];

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
