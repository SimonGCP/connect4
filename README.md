# connect4
Terminal Connect 4 game - featuring a working Connect 4 AI! The game can be played against a friend or against the AI.

### Motivation
I started this project because I was interested in the algorithms used by computers to play adversarial board games, particulary chess. I decided to do Connect 4 because it has many of the same traits as chess (two player, adversarial, zero-sum) with a much simpler game loop and rules. This meant I could focus on implementing many of the same algorithms as a chess bot without having to worry as much about complex game logic. The bot and game logic are written in C++ to take advantage of its high performance and thread library.

### The Algorithm
The bot uses the Minimax algorithm to traverse the game tree. Alongside the basic algorithm, I also implemented the following optimizations:
- Alpha-beta pruning
- Multithreading
- Bitboards to represent game state
- Transposition table

### Running Locally
To run the project, clone the repository, then after navigating to it: (assuming you have CMake and make installed)
1. To generate build files: ```cmake -S . -B build && cd build```
2. To build the executable: ```make``` (from build directory)
3. To run the executable: ```./main.exe```
