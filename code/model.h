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

/// @brief Initializes a game model
/// @param model The game model
void initModel(GameModel &model);

/// @brief Starts a game
/// @param model The game model
void startModel(GameModel &model);

/// @brief Returns the model's current player
/// @param model The game model
/// @return PLAYER_WHITE or PLAYER_BLACK
Player getCurrentPlayer(GameModel &model);

/// @brief Returns the model's current score
/// @param model The game model
/// @param player The player (PLAYER_WHITE or PLAYER_BLACK)
/// @return The score
int getScore(GameModel &model, Player player);

/// @brief Returns the game timer for a player
/// @param model The game model
/// @param player The player (PLAYER_WHITE or PLAYER_BLACK)
/// @return The time in seconds
double getTimer(GameModel &model, Player player);

/// @brief Return a model's piece
/// @param model The game model
/// @param square The square
/// @return The piece at the square
Piece getBoardPiece(GameModel &model, Square square);

/// @brief Sets a model's piece
/// @param model The game model
/// @param square The square
/// @param piece The piece to be set
void setBoardPiece(GameModel &model, Square square, Piece piece);

/// @brief Checks whether a square is within the board
/// @param square The square
/// @return True if valid, false otherwise
bool isSquareValid(Square square);

/// @brief Returns a list of valid moves for the current player
/// @param model The game model
/// @param validMoves A list that receives the valid moves
void getValidMoves(GameModel &model, Moves &validMoves);

/// @brief Plays a move
/// @param model The game model
/// @param move The move to play
/// @return True if move was accepted, false otherwise
bool playMove(GameModel &model, Square move);

/// @brief Converts board coordinates to chess notation
/// @param square Board coordinates
/// @return String in "A1", "H8", etc. format
std::string squareToChessNotation(Square square);


#endif // MODEL_H
