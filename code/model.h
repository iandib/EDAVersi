/* *****************************************************************
    * FILE INFORMATION *
   ***************************************************************** */
   
/// @brief Implements the Reversi game model
/// @author Marc S. Ressl, Ian A. Dib, Luciano S. Cordero
/// @copyright Copyright (c) 2023-2024


/* *****************************************************************
    * HEADER CONFIGURATION *
   ***************************************************************** */

#ifndef MODEL_H
#define MODEL_H

//* NECESSARY LIBRARIES & HEADERS

#include <cstdint>
#include <vector>
#include <string>


//* CONSTANTS

#define BOARD_SIZE 8

#define GAME_INVALID_SQUARE \
    {                       \
        -1, -1              \
    }


//* DATA TYPES & STRUCTURES

/// @brief Player identifiers
enum Player
{
    PLAYER_BLACK,
    PLAYER_WHITE,
};

/// @brief Piece types on the board
enum Piece
{
    PIECE_EMPTY,
    PIECE_BLACK,
    PIECE_WHITE,
};

/// @brief Coordinates of a square on the board
struct Square
{
    int x;
    int y;
};

/// @brief Main game model structure
struct GameModel
{
    bool gameOver;

    Player currentPlayer;

    double playerTime[2];
    double turnTimer;

    Piece board[BOARD_SIZE][BOARD_SIZE];

    Player humanPlayer;
    
    // Store the last moves
    std::string lastHumanMove;
    std::string lastAIMove;
};

/// @brief Collection of possible moves
typedef std::vector<Square> Moves;


//* PUBLIC FUNCTION PROTOTYPES

/// @brief Initializes the game model
/// @param model The game model to initialize
void initModel(GameModel &model);

/// @brief Starts a new game
/// @param model The game model to start
void startModel(GameModel &model);

/// @brief Gets the current player
/// @param model The game model
/// @return PLAYER_WHITE or PLAYER_BLACK
Player getCurrentPlayer(GameModel &model);

/// @brief Gets the score for a player
/// @param model The game model
/// @param player The player (PLAYER_WHITE or PLAYER_BLACK)
/// @return The score
int getScore(GameModel &model, Player player);

/// @brief Gets the timer for a player
/// @param model The game model
/// @param player The player (PLAYER_WHITE or PLAYER_BLACK)
/// @return The elapsed time in seconds
double getTimer(GameModel &model, Player player);

/// @brief Gets the piece at a specific board position
/// @param model The game model
/// @param square The board coordinates
/// @return The piece at that position
Piece getBoardPiece(GameModel &model, Square square);

/// @brief Sets a piece at a specific board position
/// @param model The game model
/// @param square The board coordinates
/// @param piece The piece to set
void setBoardPiece(GameModel &model, Square square, Piece piece);

/// @brief Checks whether a square is within the board
/// @param square The square
/// @return True if valid, false otherwise
bool isSquareValid(Square square);

/// @brief Gets all valid moves for the current player
/// @param model The game model
/// @param validMoves Vector to store the valid moves
void getValidMoves(GameModel &model, Moves &validMoves);

/// @brief Plays a move at the specified position
/// @param model The game model
/// @param move The position to play at
/// @return True if the move was valid and executedm false otherwise
bool playMove(GameModel &model, Square move);


#endif // MODEL_H
