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

/// @brief Initializes the game window and sets the frame rate
void initView();

/// @brief Closes the game window
void freeView();

/// @brief Draws the last move information on the screen
/// @param position The position to draw the text
/// @param moveText The text of the last move
void drawLastMove(Vector2 position, const std::string &moveText);

/// @brief Draws the complete game view
/// @param model The game model containing the game state
void drawView(GameModel &model);

/// @brief Gets the square under the mouse pointer
/// @return The square coordinates or GAME_INVALID_SQUARE if outside the board
Square getSquareOnMousePointer();

/// @brief Checks if the mouse is over the "Play Black" button
/// @return true if mouse is over the button, false otherwise
bool isMousePointerOverPlayBlackButton();

/// @brief Checks if the mouse is over the "Play White" button
/// @return true if mouse is over the button, false otherwise
bool isMousePointerOverPlayWhiteButton();


#endif // VIEW_H
