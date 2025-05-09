/* *****************************************************************
    * FILE INFORMATION *
   ***************************************************************** */
   
/// @brief Implements the Reversi game model
/// @author Marc S. Ressl
/// @copyright Copyright (c) 2023-2024


/* *****************************************************************
    * FILE CONFIGURATION *
   ***************************************************************** */

//* NECESSARY HEADERS

#include "model.h"
#include "view.h"
#include "controller.h"


/* *****************************************************************
    * SIMULATION UPDATE *
   ***************************************************************** */

int main()
{
    GameModel model;

    initModel(model);
    initView();

    while (updateView(model))
        ;

    freeView();
}
