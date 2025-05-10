/* *****************************************************************
    * FILE INFORMATION *
   ***************************************************************** */
   
/// @brief Implements the Reversi game view
/// @author Marc S. Ressl, Ian A. Dib, Luciano S. Cordero
/// @copyright Copyright (c) 2023-2024


/* *****************************************************************
    * FILE CONFIGURATION *
   ***************************************************************** */

//* NECESSARY LIBRARIES & HEADERS

#include <cmath> // For floor function
#include <string>

#include "raylib.h"

#include "controller.h"
#include "model.h"


//* CONSTANTS

// Window configuration
#define GAME_NAME "EDAversi"
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

// Square configuration
#define SQUARE_SIZE 80
#define SQUARE_PADDING 1.5F
#define SQUARE_CONTENT_OFFSET (SQUARE_PADDING)
#define SQUARE_CONTENT_SIZE (SQUARE_SIZE - 2 * SQUARE_PADDING)

// Piece configuration
#define PIECE_CENTER (SQUARE_SIZE / 2)
#define PIECE_RADIUS (SQUARE_SIZE * 80 / 100 / 2)
#define VALID_MOVE_RADIUS (SQUARE_SIZE * 30 / 100 / 2)

// Board configuration
#define BOARD_X 40
#define BOARD_Y 40
#define BOARD_CONTENT_SIZE (BOARD_SIZE * SQUARE_SIZE)

// Border configuration
#define OUTERBORDER_X (BOARD_X - OUTERBORDER_PADDING)
#define OUTERBORDER_Y (BOARD_Y - OUTERBORDER_PADDING)
#define OUTERBORDER_PADDING 40
#define OUTERBORDER_WIDTH 10
#define OUTERBORDER_SIZE (BOARD_CONTENT_SIZE + 2 * OUTERBORDER_PADDING)

// Text configuration
#define TITLE_FONT_SIZE 72
#define SUBTITLE_FONT_SIZE 36
#define LASTMOVE_FONT_SIZE 24

// Info panel configuration
#define INFO_CENTERED_X (OUTERBORDER_SIZE + (WINDOW_WIDTH - OUTERBORDER_SIZE) / 2)

#define INFO_TITLE_Y (WINDOW_HEIGHT / 2)

#define INFO_WHITE_SCORE_Y (WINDOW_HEIGHT * 1 / 4 - SUBTITLE_FONT_SIZE / 2)
#define INFO_WHITE_TIME_Y (WINDOW_HEIGHT * 1 / 4 + SUBTITLE_FONT_SIZE / 2)
#define INFO_WHITE_LASTMOVE_Y (INFO_WHITE_SCORE_Y - 40)

#define INFO_BLACK_SCORE_Y (WINDOW_HEIGHT * 3 / 4 - SUBTITLE_FONT_SIZE / 2)
#define INFO_BLACK_TIME_Y (WINDOW_HEIGHT * 3 / 4 + SUBTITLE_FONT_SIZE / 2)
#define INFO_BLACK_LASTMOVE_Y (INFO_BLACK_TIME_Y + 40)

// Button configuration
#define INFO_BUTTON_WIDTH 280
#define INFO_BUTTON_HEIGHT 64

#define INFO_PLAYBLACK_BUTTON_X INFO_CENTERED_X
#define INFO_PLAYBLACK_BUTTON_Y (WINDOW_HEIGHT * 1 / 8)

#define INFO_PLAYWHITE_BUTTON_X INFO_CENTERED_X
#define INFO_PLAYWHITE_BUTTON_Y (WINDOW_HEIGHT * 7 / 8)


/* *****************************************************************
    * LOGIC MODULES *
   ***************************************************************** */

//* VIEW INITIALIZATION

/// @brief Initializes the game window and sets the frame rate
void initView()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, GAME_NAME);

    SetTargetFPS(60);
}

/// @brief Closes the game window
void freeView()
{
    CloseWindow();
}


//* TEXT RENDERING

/// @brief Draws centered text
/// @param position The center position for the text
/// @param fontSize The font size
/// @param s The string to display
static void drawCenteredText(Vector2 position, int fontSize, std::string s)
{
    DrawText(s.c_str(),
             (int)position.x - MeasureText(s.c_str(), fontSize) / 2,
             (int)position.y - fontSize / 2,
             fontSize,
             BROWN);
}


/// @brief Draws a player's score with label
/// @param label The label for the score
/// @param position The center position for the score
/// @param score The score value
static void drawScore(std::string label, Vector2 position, int score)
{
    std::string s = label + std::to_string(score);

    drawCenteredText(position, SUBTITLE_FONT_SIZE, s);
}


/// @brief Draws a player's timer in MM:SS format
/// @param position The center position for the timer
/// @param time The number of seconds of the timer
static void drawTimer(Vector2 position,
                      double time)
{
    int totalSeconds = (int)time;

    int seconds = totalSeconds % 60;
    int minutes = totalSeconds / 60;

    std::string s;

    if (minutes < 10)
        s.append("0");
    s.append(std::to_string(minutes));
    s.append(":");
    if (seconds < 10)
        s.append("0");
    s.append(std::to_string(seconds));

    drawCenteredText(position, SUBTITLE_FONT_SIZE, s);
}


/// @brief Draws the last move information on the screen
/// @param position The position to draw the text
/// @param moveText The text of the last move
void drawLastMove(Vector2 position, const std::string &moveText)
{
    if (!moveText.empty())
    {
        std::string s = "Last move: " + moveText;
        drawCenteredText(position, LASTMOVE_FONT_SIZE, s);
    }
}


//* UI ELEMENTS

/// @brief Draws a button with text
/// @param position The center position of the button
/// @param label The text of the button
/// @param backgroundColor The background color of the button
static void drawButton(Vector2 position, std::string label, Color backgroundColor)
{
    DrawRectangle(position.x - INFO_BUTTON_WIDTH / 2,
                  position.y - INFO_BUTTON_HEIGHT / 2,
                  INFO_BUTTON_WIDTH, INFO_BUTTON_HEIGHT, backgroundColor);

    drawCenteredText({position.x, position.y}, SUBTITLE_FONT_SIZE, label.c_str());
}


/// @brief Indicates whether the mouse pointer is over a button
/// @param position The center position of the button
/// @return true if mouse is over the button, false otherwise
static bool isMousePointerOverButton(Vector2 position)
{
    Vector2 mousePosition = GetMousePosition();

    return ((mousePosition.x >= (position.x - INFO_BUTTON_WIDTH / 2)) &&
            (mousePosition.x < (position.x + INFO_BUTTON_WIDTH / 2)) &&
            (mousePosition.y >= (position.y - INFO_BUTTON_HEIGHT / 2)) &&
            (mousePosition.y < (position.y + INFO_BUTTON_HEIGHT / 2)));
}


//* MAIN VIEW RENDERING

/// @brief Draws the complete game view
/// @param model The game model containing the game state
void drawView(GameModel &model)
{
    BeginDrawing();

    ClearBackground(BEIGE);

    // Draw outer border
    DrawRectangle(OUTERBORDER_X, OUTERBORDER_Y, OUTERBORDER_SIZE,
                  OUTERBORDER_SIZE, BLACK);

    // Get valid moves for current player
    Moves validMoves;
    if (!model.gameOver && model.currentPlayer == model.humanPlayer) 
    {
        getValidMoves(model, validMoves);
    }

    // Draw the board grid and pieces
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            Square square = {x, y};

            Vector2 position = {BOARD_X + (float)square.x * SQUARE_SIZE,
                                BOARD_Y + (float)square.y * SQUARE_SIZE};

            // Draw square background
            DrawRectangleRounded({position.x + SQUARE_CONTENT_OFFSET,
                                  position.y + SQUARE_CONTENT_OFFSET,
                                  SQUARE_CONTENT_SIZE, SQUARE_CONTENT_SIZE},
                                  0.2F, 6, DARKGREEN);

            // Draw piece if present
            Piece piece = getBoardPiece(model, square);

            if (piece != PIECE_EMPTY)
            {
                DrawCircle((int)position.x + PIECE_CENTER,
                           (int)position.y + PIECE_CENTER,
                           PIECE_RADIUS, (piece == PIECE_WHITE) ? WHITE : BLACK);
            }
            
            // Draw a gray circle to indicate valid move
            if (!model.gameOver && model.currentPlayer == model.humanPlayer) 
            {
                for (auto move : validMoves) 
                {
                    if (move.x == x && move.y == y) 
                    {
                        DrawCircle((int)position.x + PIECE_CENTER,
                                  (int)position.y + PIECE_CENTER,
                                  VALID_MOVE_RADIUS, GRAY);
                        break;
                    }
                }
            }
        }
    }

    // Draw game title
    drawCenteredText({INFO_CENTERED_X, INFO_TITLE_Y}, TITLE_FONT_SIZE, GAME_NAME);

    // Draw Black player info
    drawScore("Black score: ", {INFO_CENTERED_X, INFO_WHITE_SCORE_Y},
              getScore(model, PLAYER_BLACK));

    drawTimer({INFO_CENTERED_X, INFO_WHITE_TIME_Y},
              getTimer(model, PLAYER_BLACK));
    
    // Draw White player info
    drawScore("White score: ", {INFO_CENTERED_X, INFO_BLACK_SCORE_Y},
              getScore(model, PLAYER_WHITE));

    drawTimer({INFO_CENTERED_X, INFO_BLACK_TIME_Y},
              getTimer(model, PLAYER_WHITE));

    // Draw last move info above score based on human player color
    if((model.humanPlayer == PLAYER_BLACK) && !model.gameOver)
    {
        drawLastMove({INFO_CENTERED_X, INFO_WHITE_LASTMOVE_Y}, model.lastHumanMove);
        drawLastMove({INFO_CENTERED_X, INFO_BLACK_LASTMOVE_Y}, model.lastAIMove);
    }

    else if((model.humanPlayer == PLAYER_WHITE) && !model.gameOver)
    {
        drawLastMove({INFO_CENTERED_X, INFO_BLACK_LASTMOVE_Y}, model.lastHumanMove);
        drawLastMove({INFO_CENTERED_X, INFO_WHITE_LASTMOVE_Y}, model.lastAIMove);
    }
    
    // Draw game control buttons if game is over
    if (model.gameOver)
    {
        drawButton({INFO_PLAYBLACK_BUTTON_X, INFO_PLAYBLACK_BUTTON_Y},
                   "Play black", BLACK);

        drawButton({INFO_PLAYWHITE_BUTTON_X, INFO_PLAYWHITE_BUTTON_Y},
                   "Play white", WHITE);
    }

    EndDrawing();
}


//* MOUSE INTERACTION

/// @brief Gets the square under the mouse pointer
/// @return The square coordinates or GAME_INVALID_SQUARE if outside the board
Square getSquareOnMousePointer()
{
    Vector2 mousePosition = GetMousePosition();
    Square square = {(int)floor((mousePosition.x - BOARD_X) / SQUARE_SIZE),
                     (int)floor((mousePosition.y - BOARD_Y) / SQUARE_SIZE)};

    if (isSquareValid(square))
    {
        return square;
    }
    else
    {
        return GAME_INVALID_SQUARE;
    }
}

/// @brief Checks if the mouse is over the "Play Black" button
/// @return true if mouse is over the button, false otherwise
bool isMousePointerOverPlayBlackButton()
{
    return isMousePointerOverButton({INFO_PLAYBLACK_BUTTON_X,
                                     INFO_PLAYBLACK_BUTTON_Y});
}

/// @brief Checks if the mouse is over the "Play White" button
/// @return true if mouse is over the button, false otherwise
bool isMousePointerOverPlayWhiteButton()
{
    return isMousePointerOverButton({INFO_PLAYWHITE_BUTTON_X,
                                     INFO_PLAYWHITE_BUTTON_Y});
}
