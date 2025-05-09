/* *****************************************************************
    * FILE INFORMATION *
   ***************************************************************** */
   
/// @brief Implements the Reversi game controller
/// @author Marc S. Ressl
/// @copyright Copyright (c) 2023-2024


/* *****************************************************************
    * HEADER CONFIGURATION *
   ***************************************************************** */

#ifndef CONTROLLER_H
#define CONTROLLER_H

//* NECESSARY LIBRARIES & HEADERS

#include "model.h"


//* PUBLIC FUNCTION PROTOTYPES

/// @brief Updates the game view
/// @param model The game model
/// @return Should the view be closed?
bool updateView(GameModel &model);


#endif // CONTROLLER_H
