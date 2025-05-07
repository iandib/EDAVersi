/**
 * @brief Implements the Reversi game AI
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2023-2024
 */

 #include <cstdlib>
 #include <limits>
 #include <algorithm>
 
 #include "ai.h"
 #include "controller.h"
 
 /**
  * @brief Evaluates the board state for the given player
  * This is a simple evaluation function that just counts the difference
  * in pieces between the current player and the opponent.
  * 
  * @param model The game model
  * @param player The player to evaluate for
  * @return int Score for the player (positive is good for player)
  */
 int evaluateBoard(GameModel &model, Player player) {
     // For terminal states (game over)
     if (model.gameOver) {
         int blackScore = getScore(model, PLAYER_BLACK);
         int whiteScore = getScore(model, PLAYER_WHITE);
         
         // If this player won, return a high value
         if ((player == PLAYER_BLACK && blackScore > whiteScore) ||
             (player == PLAYER_WHITE && whiteScore > blackScore)) {
             return 1000;
         }
         // If this player lost, return a low value
         else if ((player == PLAYER_BLACK && blackScore < whiteScore) ||
                  (player == PLAYER_WHITE && whiteScore < blackScore)) {
             return -1000;
         }
         // If it's a tie
         else {
             return 0;
         }
     }
     
     // For non-terminal states, calculate the piece difference
     int playerScore = getScore(model, player);
     int opponentScore = getScore(model, (player == PLAYER_BLACK) ? PLAYER_WHITE : PLAYER_BLACK);
     
     return playerScore - opponentScore;
 }
 
 /**
  * @brief Creates a deep copy of the game model
  * 
  * @param source The source game model
  * @return GameModel A new copy of the game model
  */
 GameModel copyGameModel(const GameModel &source) {
     GameModel copy = source;
     return copy;
 }
 
 /**
  * @brief Implementation of the Minimax algorithm
  * Recursively explores the game tree and returns the best score
  * 
  * @param model The game model
  * @param depth The current depth in the game tree
  * @param maximizingPlayer True if the current player is maximizing, false if minimizing
  * @param originalPlayer The player for whom we're calculating the best move
  * @return int The best score available
  */
 int minimax(GameModel &model, int depth, bool maximizingPlayer, Player originalPlayer) {
     // Get the current player
     Player currentPlayer = getCurrentPlayer(model);
     
     // Base case: if we've reached a terminal state or maximum depth
     if (depth == 0 || model.gameOver) {
         return evaluateBoard(model, originalPlayer);
     }
     
     // Get valid moves for the current player
     Moves validMoves;
     getValidMoves(model, validMoves);
     
     // If no valid moves, the turn passes to the opponent
     if (validMoves.empty()) {
         // Switch player
         model.currentPlayer = (currentPlayer == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE;
         
         // Check if the opponent has valid moves
         getValidMoves(model, validMoves);
         
         // If opponent also has no valid moves, game is over
         if (validMoves.empty()) {
             model.gameOver = true;
             return evaluateBoard(model, originalPlayer);
         }
         
         // Recursive call with switched player
         return minimax(model, depth, !maximizingPlayer, originalPlayer);
     }
     
     // For maximizing player
     if (maximizingPlayer) {
         int maxEval = std::numeric_limits<int>::min();
         
         for (const Square &move : validMoves) {
             // Create a copy of the current game state
             GameModel modelCopy = copyGameModel(model);
             
             // Apply the move
             playMove(modelCopy, move);
             
             // Call updateView to keep the UI responsive
             updateView(modelCopy);
             
             // Recursive call to explore this path
             int eval = minimax(modelCopy, depth - 1, false, originalPlayer);
             
             // Update best score
             maxEval = std::max(maxEval, eval);
         }
         
         return maxEval;
     }
     // For minimizing player
     else {
         int minEval = std::numeric_limits<int>::max();
         
         for (const Square &move : validMoves) {
             // Create a copy of the current game state
             GameModel modelCopy = copyGameModel(model);
             
             // Apply the move
             playMove(modelCopy, move);
             
             // Call updateView to keep the UI responsive
             updateView(modelCopy);
             
             // Recursive call to explore this path
             int eval = minimax(modelCopy, depth - 1, true, originalPlayer);
             
             // Update best score
             minEval = std::min(minEval, eval);
         }
         
         return minEval;
     }
 }
 
 Square getBestMove(GameModel &model)
 {
     const int MAX_DEPTH = 4; // Adjust this value based on performance
     
     // Get valid moves
     Moves validMoves;
     getValidMoves(model, validMoves);
     
     // If no valid moves, return an invalid square
     if (validMoves.empty()) {
         return GAME_INVALID_SQUARE;
     }
     
     // If only one move is available, return it
     if (validMoves.size() == 1) {
         return validMoves[0];
     }
     
     // Get the current player
     Player currentPlayer = getCurrentPlayer(model);
     
     // Initialize best move and best score
     Square bestMove = validMoves[0];
     int bestScore = std::numeric_limits<int>::min();
     
     // Evaluate each move
     for (const Square &move : validMoves) {
         // Create a copy of the current game state
         GameModel modelCopy = copyGameModel(model);
         
         // Apply the move
         playMove(modelCopy, move);
         
         // Call updateView to keep the UI responsive
         updateView(modelCopy);
         
         // Calculate the score for this move using minimax
         // The next player will be minimizing (trying to minimize our score)
         int score = minimax(modelCopy, MAX_DEPTH - 1, false, currentPlayer);
         
         // Update best move if this move has a better score
         if (score > bestScore) {
             bestScore = score;
             bestMove = move;
         }
     }
     
     return bestMove;
 }

