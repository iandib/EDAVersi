/* *****************************************************************
    * FILE INFORMATION *
   ***************************************************************** */
   
/// @brief Implements the Reversi game view
/// @author Marc S. Ressl, Ian A. Dib, Luciano S. Cordero
/// @copyright Copyright (c) 2023-2024


/* *****************************************************************
    * HEADER CONFIGURATION *
   ***************************************************************** */

#ifndef VIEW_H
#define VIEW_H

//* NECESSARY LIBRARIES & HEADERS

#include "model.h"
#include "raylib.h"  // For the drawLastMove() definition


//* PUBLIC FUNCTION PROTOTYPES

/// @brief Initializes a game view
void initView();

/// @brief Frees the game view
void freeView();

/// @brief Draws the game view
/// @param model The game model
void drawView(GameModel &model);

/// @brief Returns the square over the mouse pointer
/// @return The square coordinates or GAME_INVALID_SQUARE if outside the board
Square getSquareOnMousePointer();

/// @brief Indicates whether the mouse pointer is over the "Play black" button
/// @return true if mouse is over the button, false otherwise
bool isMousePointerOverPlayBlackButton();

/// @brief Indicates whether the mouse pointer is over the "Play white" button
/// @return true if mouse is over the button, false otherwise
bool isMousePointerOverPlayWhiteButton();

/// @brief Draws the last move information on the screen
/// @param position The position to draw the text
/// @param moveText The text of the last move
void drawLastMove(Vector2 position, const std::string &moveText);


#endif // VIEW_H
