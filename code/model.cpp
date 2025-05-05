/**
 * @brief Implements the Reversi game model
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2023-2024
 */

#include "raylib.h"

#include "model.h"

// Para que memset no tire error
#include <string.h>

void initModel(GameModel &model)
{
    model.gameOver = true;

    model.playerTime[0] = 0;
    model.playerTime[1] = 0;

    memset(model.board, PIECE_EMPTY, sizeof(model.board));
}

void startModel(GameModel &model)
{
    model.gameOver = false;

    model.currentPlayer = PLAYER_BLACK;

    model.playerTime[0] = 0;
    model.playerTime[1] = 0;
    model.turnTimer = GetTime();

    memset(model.board, PIECE_EMPTY, sizeof(model.board));
    model.board[BOARD_SIZE / 2 - 1][BOARD_SIZE / 2 - 1] = PIECE_WHITE;
    model.board[BOARD_SIZE / 2 - 1][BOARD_SIZE / 2] = PIECE_BLACK;
    model.board[BOARD_SIZE / 2][BOARD_SIZE / 2] = PIECE_WHITE;
    model.board[BOARD_SIZE / 2][BOARD_SIZE / 2 - 1] = PIECE_BLACK;
}

Player getCurrentPlayer(GameModel &model)
{
    return model.currentPlayer;
}

int getScore(GameModel &model, Player player)
{
    int score = 0;

    for (int y = 0; y < BOARD_SIZE; y++)
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            if (((model.board[y][x] == PIECE_WHITE) &&
                (player == PLAYER_WHITE)) ||
                ((model.board[y][x] == PIECE_BLACK) &&
                (player == PLAYER_BLACK)))
                score++;
        }

    return score;
}

double getTimer(GameModel &model, Player player)
{
    double turnTime = 0;

    if (!model.gameOver && (player == model.currentPlayer))
        turnTime = GetTime() - model.turnTimer;

    return model.playerTime[player] + turnTime;
}

Piece getBoardPiece(GameModel &model, Square square)
{
    return model.board[square.y][square.x];
}

void setBoardPiece(GameModel &model, Square square, Piece piece)
{
    model.board[square.y][square.x] = piece;
}

bool isSquareValid(Square square)
{
    return (square.x >= 0) &&
        (square.x < BOARD_SIZE) &&
        (square.y >= 0) &&
        (square.y < BOARD_SIZE);
}

/**
 * @brief Checks if a move in a certain direction would flip any opponent pieces
 * 
 * @param model The game model
 * @param move Starting square for the check
 * @param dx X-direction delta (-1, 0, or 1)
 * @param dy Y-direction delta (-1, 0, or 1)
 * @param flippedPieces Vector to store squares that should be flipped
 * @return True if pieces would be flipped in this direction
 */
bool checkDirection(GameModel &model, Square move, int dx, int dy, std::vector<Square> &flippedPieces)
{
    // Get the current player's piece color and opponent's piece color
    Piece currentPlayerPiece = (getCurrentPlayer(model) == PLAYER_WHITE) ? PIECE_WHITE : PIECE_BLACK;
    Piece opponentPiece = (getCurrentPlayer(model) == PLAYER_WHITE) ? PIECE_BLACK : PIECE_WHITE;
    
    // Temporary vector to store potential flips
    std::vector<Square> tempFlipped;
    
    // Start checking in the specified direction
    int x = move.x + dx;
    int y = move.y + dy;
    Square checkSquare = {x, y};
    
    // First square in this direction must be opponent's piece
    if (!isSquareValid(checkSquare) || getBoardPiece(model, checkSquare) != opponentPiece)
        return false;
        
    // Store this square as potentially flipped
    tempFlipped.push_back(checkSquare);
    
    // Keep moving in the direction
    x += dx;
    y += dy;
    checkSquare = {x, y};
    
    // Continue until we find an invalid square or an empty square
    while (isSquareValid(checkSquare))
    {
        Piece piece = getBoardPiece(model, checkSquare);
        
        if (piece == PIECE_EMPTY)
            return false; // Empty square, so no flips
            
        if (piece == currentPlayerPiece)
        {
            // Found our own piece, so all pieces in between can be flipped
            flippedPieces.insert(flippedPieces.end(), tempFlipped.begin(), tempFlipped.end());
            return true;
        }
        
        // Another opponent piece, add to potential flips
        tempFlipped.push_back(checkSquare);
        
        // Move to next square in this direction
        x += dx;
        y += dy;
        checkSquare = {x, y};
    }
    
    // If we reached here, we went off the board without finding our own piece
    return false;
}

void getValidMoves(GameModel &model, Moves &validMoves)
{
    // Clear any existing moves
    validMoves.clear();
    
    // Get current player piece
    Piece currentPlayerPiece = (getCurrentPlayer(model) == PLAYER_WHITE) ? PIECE_WHITE : PIECE_BLACK;
    
    // Check all squares on the board
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            Square move = {x, y};
            
            // Skip squares that already have pieces
            if (getBoardPiece(model, move) != PIECE_EMPTY)
                continue;
                
            // Check all 8 directions
            std::vector<Square> flippedPieces;
            bool isValid = false;
            
            // Loop through all directions: (-1,-1), (-1,0), (-1,1), (0,-1), (0,1), (1,-1), (1,0), (1,1)
            for (int dy = -1; dy <= 1; dy++)
            {
                for (int dx = -1; dx <= 1; dx++)
                {
                    // Skip the center (dx=0, dy=0)
                    if (dx == 0 && dy == 0)
                        continue;
                        
                    // If any direction allows flips, it's a valid move
                    if (checkDirection(model, move, dx, dy, flippedPieces))
                        isValid = true;
                }
            }
            
            // If valid moves were found in any direction, add this move to valid moves
            if (isValid)
                validMoves.push_back(move);
        }
    }
}

bool playMove(GameModel &model, Square move)
{
    // Set game piece
    Piece piece =
        (getCurrentPlayer(model) == PLAYER_WHITE)
            ? PIECE_WHITE
            : PIECE_BLACK;

    // Find all pieces that should be flipped by this move
    std::vector<Square> flippedPieces;
    bool isValid = false;
    
    // Check all 8 directions
    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            // Skip the center (dx=0, dy=0)
            if (dx == 0 && dy == 0)
                continue;
                
            // If any direction allows flips, collect the pieces to flip
            if (checkDirection(model, move, dx, dy, flippedPieces))
                isValid = true;
        }
    }
    
    // If no pieces would be flipped, this is not a valid move
    if (!isValid)
        return false;
    
    // Set the current player's piece on the move square
    setBoardPiece(model, move, piece);
    
    // Flip all the opponent's pieces that were outflanked
    for (const Square &square : flippedPieces)
    {
        setBoardPiece(model, square, piece);
    }

    // Update timer
    double currentTime = GetTime();
    model.playerTime[model.currentPlayer] += currentTime - model.turnTimer;
    model.turnTimer = currentTime;

    // Swap player
    model.currentPlayer =
        (model.currentPlayer == PLAYER_WHITE)
            ? PLAYER_BLACK
            : PLAYER_WHITE;

    // Game over?
    Moves validMoves;
    getValidMoves(model, validMoves);

    if (validMoves.size() == 0)
    {
        // Check if the other player can make a move
        model.currentPlayer = (model.currentPlayer == PLAYER_WHITE)
            ? PLAYER_BLACK
            : PLAYER_WHITE;
        
        getValidMoves(model, validMoves);
        
        if (validMoves.size() == 0)
        {
            // Neither player can make a move, game is over
            model.gameOver = true;
        }
        else
        {
            // Current player has no moves but opponent does, so skip turn
            // The currentPlayer is already set correctly
        }
    }

    return true;
}