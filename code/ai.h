/* *****************************************************************
    * FILE INFORMATION *
   ***************************************************************** */
   
/// @brief Implements the Reversi game AI
/// @author Marc S. Ressl
/// @copyright Copyright (c) 2023-2024


/* *****************************************************************
    * HEADER CONFIGURATION *
   ***************************************************************** */

#ifndef AI_H
#define AI_H

//* NECESSARY LIBRARIES & HEADERS

#include "model.h"


//* PUBLIC FUNCTION PROTOTYPES

/// @brief Gets the best move for the current player using minimax algorithm
/// @param model Game model
/// @return The best square to play
Square getBestMove(GameModel &model);


#endif // AI_H
