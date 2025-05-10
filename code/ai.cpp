/* *****************************************************************
    * FILE INFORMATION *
   ***************************************************************** */
   
/// @brief Implements the Reversi game AI using minimax algorithm
/// @author Marc S. Ressl, Ian A. Dib, Luciano S. Cordero
/// @copyright Copyright (c) 2023-2024
/// @cite https://ceur-ws.org/Vol-1107/paper2.pdf
/// @cite https://medium.com/@jackychoi26/how-to-write-an-othello-ai-with-alpha-beta-search-58131ffe67eb
/// @cite https://samharrison00.medium.com/building-an-ai-to-play-my-favourite-board-game-othello-57f5aab1d6cf


/* *****************************************************************
    * FILE CONFIGURATION *
   ***************************************************************** */

//* NECESSARY LIBRARIES & HEADERS

#include <cstdlib>
#include <limits>
#include <algorithm>

#include "ai.h"
#include "controller.h"


//* MINIMAX CONFIGURATION

const int MAX_DEPTH = 12;         // Maximum depth for pruning
const int MAX_NODES = 100000;     // Maximum number of nodes to evaluate
int nodesEvaluated = 0;           // Counter for evaluated nodes


//* EVALUATION PARAMETERS

const int CORNER_VALUE = 25;      // Value of a corner square
const int EDGE_VALUE = 5;         // Value of an edge square
const int MOBILITY_WEIGHT = 2;    // Weight for mobility (number of possible moves)
const int PIECE_VALUE = 1;        // Value of a regular piece


/* *****************************************************************
    * HELPER MODULES *
   ***************************************************************** */

//* POSITION EVALUATION

/// @brief Counts how many pieces can be flipped in each direction with a move
/// @param model The game model
/// @param move The square where the piece would be placed
/// @param dx X-direction delta (-1, 0, 1)
/// @param dy Y-direction delta (-1, 0, 1)
/// @return Number of pieces that would be flipped in this direction
unsigned int countFlipped(GameModel& model, Square move, int dx, int dy)
{
    int flipped = 0;

    Piece playerPiece = (getCurrentPlayer(model) == PLAYER_WHITE) ? PIECE_WHITE : PIECE_BLACK;
    Piece aiPiece = (getCurrentPlayer(model) == PLAYER_WHITE) ? PIECE_BLACK : PIECE_WHITE;

    int x = move.x + dx;
    int y = move.y + dy;
    Square checkSquare = { x, y };

    // The first square in this direction must contain an opponent piece
    if (!isSquareValid(checkSquare) || getBoardPiece(model, checkSquare) != aiPiece)
        return 0;

    flipped++;

    // Start moving in specified direction
    x += dx;
    y += dy;
    checkSquare = { x, y };

    // Continue until an invalid or empty square is found
    while (isSquareValid(checkSquare))
    {
        Piece piece = getBoardPiece(model, checkSquare);

        // Empty square, no pieces flipped
        if (piece == PIECE_EMPTY)
            return 0;

        // Found own piece, return count of flipped pieces
        if (piece == playerPiece)
            return flipped;

        // Found another opponent piece
        flipped++;

        // Continue moving in specified direction
        x += dx;
        y += dy;
        checkSquare = { x, y };
    }

    // Went off the board without finding our own piece
    return 0;
}


/// @brief Counts total number of pieces that would be flipped with a move
/// @param model The game model
/// @param move The square where the piece would be placed
/// @return Total number of pieces that would be flipped
int countTotalFlipped(GameModel& model, Square move)
{
    int totalFlipped = 0;

    // Check all possible directions
    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            if (dx == 0 && dy == 0)
                continue;

            totalFlipped += countFlipped(model, move, dx, dy);
        }
    }

    return totalFlipped;
}


/// @brief Checks if a position is a corner
/// @param x X coordinate
/// @param y Y coordinate
/// @return true if it's a corner, false otherwise
bool isCorner(int x, int y)
{
    return (x == 0 && y == 0) ||
        (x == 0 && y == BOARD_SIZE - 1) ||
        (x == BOARD_SIZE - 1 && y == 0) ||
        (x == BOARD_SIZE - 1 && y == BOARD_SIZE - 1);
}


/// @brief Checks if a position is on the edge
/// @param x X coordinate
/// @param y Y coordinate
/// @return true if it's on the edge, false otherwise
bool isEdge(int x, int y)
{
    return x == 0 || y == 0 || x == BOARD_SIZE - 1 || y == BOARD_SIZE - 1;
}


/* *****************************************************************
    * MINIMAX ALGORITHM *
   ***************************************************************** */

//* EVALUATION FUNCTION

/// @brief Evaluation function for the board state
/// @param model Game model
/// @param depth Current depth in the search tree
/// @return Board value from the current player's perspective
int evaluateBoard(GameModel& model, int depth)
{
    nodesEvaluated++;
    
    // Get current player and opponent
    Player currentPlayer = getCurrentPlayer(model);
    Player opponent = (currentPlayer == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE;
    
    // If node limit reached, stop evaluation
    if (nodesEvaluated >= MAX_NODES)
    {
        return 0;
    }
    
    // Count pieces for each player
    int currentPlayerScore = getScore(model, currentPlayer);
    int opponentScore = getScore(model, opponent);
    
    // If game is over, return a high or low value
    if (model.gameOver)
    {
        // Victory: high value (favores quick wins)
        if (currentPlayerScore > opponentScore)
            return 10000 - depth;
        
        // Defeat: low value (favores slow losses)
        else if (currentPlayerScore < opponentScore)
            return -10000 + depth;

        // Draw
        else
            return 0; 
    }
    
    // Evaluate mobility (number of possible moves)
    Moves currentPlayerMoves;
    getValidMoves(model, currentPlayerMoves);
    int mobility = currentPlayerMoves.size() * MOBILITY_WEIGHT;
    
    // Switch to opponent to evaluate their mobility
    GameModel tempModel = model;
    tempModel.currentPlayer = opponent;
    Moves opponentMoves;
    getValidMoves(tempModel, opponentMoves);
    int opponentMobility = opponentMoves.size() * MOBILITY_WEIGHT;
    
    // Evaluate possession of corners and edges
    int cornerValue = 0;
    int edgeValue = 0;
    
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            Square square = {x, y};
            Piece piece = getBoardPiece(model, square);
            
            if (piece == PIECE_EMPTY)
                continue;
                
            if (isCorner(x, y))
            {
                if ((piece == PIECE_WHITE && currentPlayer == PLAYER_WHITE) ||
                    (piece == PIECE_BLACK && currentPlayer == PLAYER_BLACK))
                    cornerValue += CORNER_VALUE;

                else
                    cornerValue -= CORNER_VALUE;
            }

            else if (isEdge(x, y))
            {
                if ((piece == PIECE_WHITE && currentPlayer == PLAYER_WHITE) ||
                    (piece == PIECE_BLACK && currentPlayer == PLAYER_BLACK))
                    edgeValue += EDGE_VALUE;

                else
                    edgeValue -= EDGE_VALUE;
            }
        }
    }
    
    // Weighted piece difference
    int pieceDiff = (currentPlayerScore - opponentScore) * PIECE_VALUE;
    
    // Total value combining all factors
    return pieceDiff + cornerValue + edgeValue + (mobility - opponentMobility);
}


//* MINIMAX SEARCH ALGORITHM

/// @brief Minimax algorithm with depth pruning and node limitation
/// @param model Game model
/// @param depth Current depth
/// @param alpha Best score the maximizing player (AI) can guarantee so far
/// @param beta Best score the minimizing player (opponent) can guarantee so far
/// @param maximizingPlayer Whether current player is maximizing
/// @return Value of the best move
int minimax(GameModel& model, int depth, int alpha, int beta, bool maximizingPlayer)
{
    // Termination conditions (leaf nodes evaluation)
    if (depth == 0 || model.gameOver || nodesEvaluated >= MAX_NODES)
    {
        return evaluateBoard(model, depth);
    }
    
    Moves validMoves;
    getValidMoves(model, validMoves);
    
    // If no valid moves, pass turn
    if (validMoves.size() == 0)
    {
        // Switch player
        GameModel tempModel = model;
        tempModel.currentPlayer = (tempModel.currentPlayer == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE;
        
        // Check if other player can move
        Moves opponentMoves;
        getValidMoves(tempModel, opponentMoves);
        
        // If they also can't move, game over
        if (opponentMoves.size() == 0)
        {
            tempModel.gameOver = true;
            return evaluateBoard(tempModel, depth);
        }
        
        // Continue with other player
        return minimax(tempModel, depth - 1, alpha, beta, !maximizingPlayer);
    }
    
    // Selects the maximum value returned by the children nodes (inner nodes evaluation)
    if (maximizingPlayer)
    {
        int maxEval = std::numeric_limits<int>::min();
        
        for (const Square& move : validMoves)
        {
            // Make a copy of the model and play the move
            GameModel tempModel = model;
            playMove(tempModel, move);
            
            // Recursive evaluation
            int eval = minimax(tempModel, depth - 1, alpha, beta, false);
            maxEval = std::max(maxEval, eval);
            
            // Alpha-beta pruning
            alpha = std::max(alpha, eval);
            if (beta <= alpha)
                break;
        }
        
        return maxEval;
    }

    // Selects the minimum value returned by the children nodes (inner nodes evaluation)
    else
    {
        int minEval = std::numeric_limits<int>::max();
        
        for (const Square& move : validMoves)
        {
            // Make a copy of the model and play the move
            GameModel tempModel = model;
            playMove(tempModel, move);
            
            // Recursive evaluation
            int eval = minimax(tempModel, depth - 1, alpha, beta, true);
            minEval = std::min(minEval, eval);
            
            // Alpha-beta pruning
            beta = std::min(beta, eval);
            if (beta <= alpha)
                break;
        }
        
        return minEval;
    }
}


/* *****************************************************************
    * AI EVALUATION *
   ***************************************************************** */

/// @brief Gets the best move for the current player using minimax algorithm
/// @param model Game model
/// @return The best square to play
Square getBestMove(GameModel& model)
{
    // Reset node counter
    nodesEvaluated = 0;
    
    Moves validMoves;
    getValidMoves(model, validMoves);
    
    if (validMoves.size() == 0)
        return GAME_INVALID_SQUARE;
    
    // Play in corners if possible
    for (const Square& move : validMoves)
    {
        if (isCorner(move.x, move.y))
        {
            return move;
        }
    }
    
    // Use minimax algorithm to evaluate the best move (root nodes evaluation)
    Square bestMove = validMoves[0];
    int bestValue = std::numeric_limits<int>::min();
    
    for (const Square& move : validMoves)
    {
        // Make a copy of the model and play the move
        GameModel tempModel = model;
        playMove(tempModel, move);
        
        // Evaluate the move with minimax
        int moveValue = minimax(tempModel, MAX_DEPTH - 1, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), false);
        
        // Update best move if necessary
        if (moveValue > bestValue)
        {
            bestValue = moveValue;
            bestMove = move;
        }
    }
    
    return bestMove;
}
