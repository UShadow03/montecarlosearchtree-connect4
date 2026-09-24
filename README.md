# montecarlosearchtree-connect4
This project is a command-line Connect Four implementation, where the player can choose to run a Monte Carlo Tree Search algorithm to get a recommended move before taking their turn.

## Build

Requires a C++11 (or later) compiler. Type:

"g++ -std=c++17 -O2 -o connect4 main.cpp GameState.cpp MCTS.cpp"

You may need to adjust source file names/paths if yours differ.

## Run

Interactive mode, type:

"./connect4"

A 2 player game will be opened. You'll be prompted each turn to optionally run an MCTS search (`Y`/`N`), and
if you do, how many simulations to run.


### Command-line options

--simulations N | --sim N: Use a fixed simulation count for every MCTS-assisted move, skipping the interactive prompt.

--difficulty LEVEL: Preset simulation counts: `easy` (100), `medium` (2000), `hard` (10000). Overrides --simulations if both are given. 

--first-player N: Which player moves first, `1` or `2` (default `1`). 

-h | --help: Print usage and exit.

Examples:

./connect4 --difficulty hard --first-player 2
./connect4 --sim 5000

## How it works

- The board is a 6×7 grid ('GameState::board'); columns are chosen 1–7 and
  pieces fall to the lowest open row ('droppingShape').
- MCTS::findBestMove(state, simulations, player) runs the requested number
  of simulations from the current position and returns the move with the
  most visits, along with per-column visit/win/win-rate stats.
- Each simulation is the standard MCTS loop: **select** (UCT descent down
  the existing tree), **expand** (add one new child for an unexplored
  move — preferring an immediate win or an immediate block over
  arbitrary column order), **simulate** (a random rollout to a terminal
  state, itself preferring an immediate win or block at each ply), and
  **backpropagate** (update visit/win counts back to the root, crediting
  wins to whichever player actually moved into each node).
- If `select()` lands on an already-decided position (a win or a draw),
  the result is backpropagated directly instead of expanding further —
  this avoids wasting simulation budget and prevents extra moves played
  past a finished game from corrupting the recorded winner.

## Known limitations

- The rollout and expansion tactics only look one ply ahead (immediate
  win / immediate block); deeper forced sequences (e.g. a two-move trap)
  are found only through the tree search accumulating enough visits, not
  through an explicit heuristic.
- No transposition table — identical positions reached via different move
  orders are stored as separate tree nodes.