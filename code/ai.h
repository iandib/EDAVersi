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

/// @brief Returns the best move for a certain position
/// @param model The game model
/// @return The best move
Square getBestMove(GameModel &model);


#endif // AI_H
