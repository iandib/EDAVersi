/**
 * @brief Implements the Reversi game AI
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2023-2024
 */

 #include <cstdlib>

 #include "ai.h"
 #include "controller.h"
 
 
 unsigned int FlippedPiecesInDirection(GameModel& model, Square move, int dx, int dy) { //Cuenta cuantas fichas se pueden voltear en cada direccion con un movimiento
 
     int flipped = 0;
 
     Piece PlayerPiece = (getCurrentPlayer(model) == PLAYER_WHITE) ? PIECE_WHITE : PIECE_BLACK;
     Piece AiPiece = (getCurrentPlayer(model) == PLAYER_WHITE) ? PIECE_BLACK : PIECE_WHITE;
 
     int x = move.x + dx;
     int y = move.y + dy;
     Square checkSquare = { x, y };
 
     // La primera casilla en esta dirección debe tener una ficha del oponente
     if (!isSquareValid(checkSquare) || getBoardPiece(model, checkSquare) != AiPiece)
         return 0;
 
     flipped++;
 
     // Continuar moviéndose en esta dirección
     x += dx;
     y += dy;
     checkSquare = { x, y };
 
     // Continuar hasta que se encuentre una casilla inválida o vacía
     while (isSquareValid(checkSquare)) {
         Piece piece = getBoardPiece(model, checkSquare);
 
         if (piece == PIECE_EMPTY)
             return 0; // Casilla vacía, no se voltean fichas
 
         if (piece == PlayerPiece)
             return flipped; // Se encontró una ficha propia, se devuelven las volteadas
 
         // Otra ficha del oponente, incrementar el contador
         flipped++;
 
         // Mover a la siguiente casilla en esta dirección
         x += dx;
         y += dy;
         checkSquare = { x, y };
     }
 
     // Si se llegó aquí, se salió del tablero sin encontrar una ficha propia
     return 0;
 }
 
 int countTotalFlippedPieces(GameModel& model, Square move) // Cuenta cuantas fichas se pueden voltear con un movimiento
 {
     int totalFlipped = 0;
 
     // Recorre todas las direcciones posibles
     for (int dy = -1; dy <= 1; dy++)
     {
         for (int dx = -1; dx <= 1; dx++)
         {
             if (dx == 0 && dy == 0)
                 continue;
 
             totalFlipped += FlippedPiecesInDirection(model, move, dx, dy);
         }
     }
 
     return totalFlipped;
 }
 
 Square getBestMove(GameModel& model)
 {
     Moves validMoves;
     getValidMoves(model, validMoves);
 
     if (validMoves.size() == 0)
         return GAME_INVALID_SQUARE;
 
     // 1. Primera prioridad: jugar en las esquinas si es posible
     for (const Square& move : validMoves)
     {
         if ((move.x == 0 && move.y == 0) ||
             (move.x == 0 && move.y == BOARD_SIZE - 1) ||
             (move.x == BOARD_SIZE - 1 && move.y == 0) ||
             (move.x == BOARD_SIZE - 1 && move.y == BOARD_SIZE - 1))
         {
             return move;
         }
     }
 
     // 2. Segunda prioridad: bloquear al oponente para que no pueda jugar en las esquinas
 
     // Crear una copia del modelo para simular movimientos
     GameModel simulatedModel = model;
     Player currentPlayer = getCurrentPlayer(model);
     Player opponentPlayer = (currentPlayer == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE;
 
     // Lista para guardar los movimientos que bloquean el acceso a las esquinas
     Moves blockingMoves;
 
     for (const Square& move : validMoves)
     {
         // Simular este movimiento
         GameModel tempModel = simulatedModel;
         bool moveSuccess = playMove(tempModel, move);
 
         if (!moveSuccess)
             continue;
 
         // Verificar si después de este movimiento el oponente puede alcanzar alguna esquina
         bool opponentCanReachCorner = false;
 
         // Asegurarse de que el jugador actual sea el oponente después del movimiento
         if (getCurrentPlayer(tempModel) == opponentPlayer)
         {
             Moves opponentMoves;
             getValidMoves(tempModel, opponentMoves);
 
             for (const Square& opponentMove : opponentMoves)
             {
                 if ((opponentMove.x == 0 && opponentMove.y == 0) ||
                     (opponentMove.x == 0 && opponentMove.y == BOARD_SIZE - 1) ||
                     (opponentMove.x == BOARD_SIZE - 1 && opponentMove.y == 0) ||
                     (opponentMove.x == BOARD_SIZE - 1 && opponentMove.y == BOARD_SIZE - 1))
                 {
                     opponentCanReachCorner = true;
                     break;
                 }
             }
 
             // Si este movimiento previene que el oponente alcance una esquina
             if (!opponentCanReachCorner)
             {
                 blockingMoves.push_back(move);
             }
         }
     }
 
     // Si se encontraron movimientos que bloquean el acceso a las esquinas, elegir el que voltee más fichas
     if (!blockingMoves.empty())
     {
         Square bestBlockingMove = blockingMoves[0];
         int maxFlipped = countTotalFlippedPieces(model, bestBlockingMove);
 
         for (size_t i = 1; i < blockingMoves.size(); i++)
         {
             int flipped = countTotalFlippedPieces(model, blockingMoves[i]);
             if (flipped > maxFlipped)
             {
                 maxFlipped = flipped;
                 bestBlockingMove = blockingMoves[i];
             }
         }
 
         return bestBlockingMove;
     }
 
     // 3. Tercera prioridad: elegir el movimiento que voltee más fichas
     Square bestMove = validMoves[0];
     int maxFlipped = countTotalFlippedPieces(model, bestMove);
 
     for (size_t i = 1; i < validMoves.size(); i++)
     {
         int flippedCount = countTotalFlippedPieces(model, validMoves[i]);
         if (flippedCount > maxFlipped)
         {
             maxFlipped = flippedCount;
             bestMove = validMoves[i];
         }
     }
 
     return bestMove;
 }