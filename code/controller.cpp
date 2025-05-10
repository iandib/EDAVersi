/* *****************************************************************
    * FILE INFORMATION *
   ***************************************************************** */
   
/// @brief Implements the Reversi game controller
/// @author Marc S. Ressl, Ian A. Dib, Luciano S. Cordero
/// @copyright Copyright (c) 2023-2024


/* *****************************************************************
    * FILE CONFIGURATION *
   ***************************************************************** */

//* NECESSARY LIBRARIES & HEADERS

#include <algorithm>
#include <string>

#include "raylib.h"

#include "ai.h"
#include "view.h"
#include "controller.h"


/* *****************************************************************
    * UTILITY MODULES *
   ***************************************************************** */

//* NOTATION CONVERSION

/// @brief Converts board coordinates to chess notation
/// @param square Board coordinates
/// @return String in format "A1", "H8", etc.
std::string squareToChessNotation(Square square)
{
    std::string notation;
    notation += static_cast<char>('A' + square.x);  // 'A' + 0 = 'A', 'A' + 7 = 'H'
    notation += std::to_string(square.y + 1);       // 0 + 1 = 1, 7 + 1 = 8
    return notation;
}


/* *****************************************************************
    * GAME CONTROLLER IMPLEMENTATION *
   ***************************************************************** */

//* VIEW UPDATE FUNCTION

/// @brief Updates the game view based on user interactions and game state
/// @param model The game model to update
/// @return false if window should close, true otherwise
bool updateView(GameModel &model)
{
    if (WindowShouldClose())
        return false;

    if (model.gameOver)
    {
        if (IsMouseButtonPressed(0))
        {
            if (isMousePointerOverPlayBlackButton())
            {
                model.humanPlayer = PLAYER_BLACK;

                startModel(model);
            }
            else if (isMousePointerOverPlayWhiteButton())
            {
                model.humanPlayer = PLAYER_WHITE;

                startModel(model);
            }
        }
    }

    else if (model.currentPlayer == model.humanPlayer)
    {
        if (IsMouseButtonPressed(0))
        {
            Square square = getSquareOnMousePointer();

            if (isSquareValid(square))
            {
                Moves validMoves;
                getValidMoves(model, validMoves);

                for (auto move : validMoves)
                {
                    if ((square.x == move.x) && (square.y == move.y) && 
                        playMove(model, square))
                    {
                        model.lastHumanMove = squareToChessNotation(square);
                    }
                }
            }
        }
    }

    else // AI turn
    {
        Square square = getBestMove(model);

        if (playMove(model, square))
        {
            model.lastAIMove = squareToChessNotation(square);
        }
    }

    if ((IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)) 
        && IsKeyPressed(KEY_ENTER))
        ToggleFullscreen();

    drawView(model);

    return true;
}
