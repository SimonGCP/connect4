#pragma once
#include <random>
#include <unordered_map>
#include <ctime>

enum evaluation_type {
	EXACT,
	UPPER_BOUND,
	LOWER_BOUND
};

struct positionInfo {
	uint64_t position;
	float eval;
	int type;
	int depth;
};

class random_number {
	private:
	std::mt19937_64 engine;
  std::uniform_int_distribution<uint64_t> dist;

	public:
	random_number() : engine(time(NULL)), dist() { return; }

	uint64_t getRand() {
		return dist(engine);
	}
};

class t_table {
	uint64_t randTable[ROWS*COLS][2]; // 2 for number of players
	random_number rand;
	std::unordered_map<uint64_t, positionInfo> table;

	public:
	t_table() {
		for (int i = 0; i < ROWS*COLS; i++) {
			randTable[i][0] = rand.getRand();
			randTable[i][1] = rand.getRand();
		}
	}

	uint64_t computeZobristHash(uint64_t bitboards[2], unsigned int player) {
		if (player > 1) return 0;

		uint64_t zobHash = 0;
		uint64_t curBitboard = bitboards[player];

		for (int i = 0; i < ROWS*COLS; i++) {
			if ((curBitboard >> i) & 1ULL) {
				zobHash ^= randTable[i][player];
			}
		}

		return zobHash;
	}

	/*
	 * put a position into the randTable
	 * @param bitboards - the red and yellow bitboards, sequentially
	 **/
	void put
		(uint64_t bitboards[2], float eval, unsigned int player, int type, int depth) {
		if (player > 1) return;

		// generate a zobrist hash of the current position
		uint64_t zobHash = computeZobristHash(bitboards, player);

		auto res = table.find(zobHash);
		if (res != table.end()) {
			const positionInfo foundInfo = res->second;

			if (foundInfo.depth > depth) return;
		}

		positionInfo info {
			bitboards[player],
			eval,
			type,
			depth
		};
		
		table[zobHash] = info;
	}

	/*
	 * retrieve the evaluation of a position if it exists in the randTable
	 * @param bitboards - the red and yellow bitboards, sequentially
	 * @return - a float that is the evaluation of the position if it is found
	 **/
	positionInfo* get(uint64_t bitboards[2], unsigned int player) {
		uint64_t zobHash = computeZobristHash(bitboards, player);
		auto res = table.find(zobHash);

		if (res == table.end()) {
			return NULL;
		}

		return &res->second;
	}
};
