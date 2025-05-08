/**
 * @brief Implements the Reversi game controller
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2023-2024
 */

#include <algorithm>
#include <string>

#include "raylib.h"

#include "ai.h"
#include "view.h"
#include "controller.h"

/**
 * @brief Convierte coordenadas de tablero a notación de ajedrez
 * 
 * @param square Coordenadas del tablero
 * @return Cadena en formato "A1", "H8", etc.
 */
std::string squareToChessNotation(Square square) {
    std::string notation;
    notation += static_cast<char>('A' + square.x);  // 'A' + 0 = 'A', 'A' + 7 = 'H'
    notation += std::to_string(square.y + 1);       // 0+1 = 1, 7+1 = 8
    return notation;
}

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
            // Human player
            Square square = getSquareOnMousePointer();

            if (isSquareValid(square))
            {
                Moves validMoves;
                getValidMoves(model, validMoves);

                // Play move if valid
                for (auto move : validMoves)
                {
                    if ((square.x == move.x) &&
                        (square.y == move.y)) {
                        if (playMove(model, square)) {
                            // Store the move in the model instead of printing to console
                            model.lastHumanMove = squareToChessNotation(square);
                        }
                    }
                }
            }
        }
    }
    else
    {
        // AI player
        Square square = getBestMove(model);

        if (playMove(model, square)) {
            // Store the move in the model instead of printing to console
            model.lastAIMove = squareToChessNotation(square);
        }
    }

    if ((IsKeyDown(KEY_LEFT_ALT) ||
        IsKeyDown(KEY_RIGHT_ALT)) &&
        IsKeyPressed(KEY_ENTER))
        ToggleFullscreen();

    drawView(model);

    return true;
}
