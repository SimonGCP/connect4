#ifndef BOT_H
#define BOT_H

#include "./board.h"
#include "./transposition_table.h"

// given the current board, returns the best move found by the bot
int getBotChoice(board gameBoard, int depth, t_table *table);

#endif
