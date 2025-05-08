/**
 * @brief Implements the Reversi game AI
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2023-2024
 */

#include <cstdlib>
#include <limits>
#include <algorithm>

#include "ai.h"
#include "controller.h"

// Configuración del algoritmo minimax
const int MAX_DEPTH = 5;          // Profundidad máxima para podar
const int MAX_NODES = 10000;      // Máximo número de nodos a evaluar
int nodesEvaluated = 0;           // Contador de nodos evaluados

// Valores para la función de evaluación
const int CORNER_VALUE = 25;      // Valor de una esquina
const int EDGE_VALUE = 5;         // Valor de una casilla en el borde
const int MOBILITY_WEIGHT = 2;    // Peso de la movilidad (número de movimientos posibles)
const int PIECE_VALUE = 1;        // Valor de una ficha normal

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

/**
 * @brief Verifica si una posición es una esquina
 * @param x Coordenada X
 * @param y Coordenada Y
 * @return true si es una esquina, false en caso contrario
 */
bool isCorner(int x, int y) {
    return (x == 0 && y == 0) ||
        (x == 0 && y == BOARD_SIZE - 1) ||
        (x == BOARD_SIZE - 1 && y == 0) ||
        (x == BOARD_SIZE - 1 && y == BOARD_SIZE - 1);
}

/**
 * @brief Verifica si una posición está en el borde
 * @param x Coordenada X
 * @param y Coordenada Y
 * @return true si está en el borde, false en caso contrario
 */
bool isEdge(int x, int y) {
    return x == 0 || y == 0 || x == BOARD_SIZE - 1 || y == BOARD_SIZE - 1;
}

/**
 * @brief Función de evaluación para el tablero
 * @param model Modelo del juego
 * @param depth Profundidad actual en el árbol de búsqueda
 * @return Valor del tablero desde la perspectiva del jugador actual
 */
int evaluateBoard(GameModel& model, int depth) {
    nodesEvaluated++;
    
    // Obtener el jugador actual y el oponente
    Player currentPlayer = getCurrentPlayer(model);
    Player opponent = (currentPlayer == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE;
    
    // Si se alcanzó el límite de nodos, detener la evaluación
    if (nodesEvaluated >= MAX_NODES) {
        return 0;
    }
    
    // Contar fichas para cada jugador
    int currentPlayerScore = getScore(model, currentPlayer);
    int opponentScore = getScore(model, opponent);
    
    // Si es fin de juego, retornar un valor alto o bajo
    if (model.gameOver) {
        if (currentPlayerScore > opponentScore)
            return 10000 - depth; // Victoria: valor alto, favoreciendo victorias rápidas
        else if (currentPlayerScore < opponentScore)
            return -10000 + depth; // Derrota: valor bajo, favoreciendo derrotas lentas
        else
            return 0; // Empate
    }
    
    // Evaluar movilidad (cantidad de movimientos posibles)
    Moves currentPlayerMoves;
    getValidMoves(model, currentPlayerMoves);
    int mobility = currentPlayerMoves.size() * MOBILITY_WEIGHT;
    
    // Cambiar al oponente para evaluar su movilidad
    GameModel tempModel = model;
    tempModel.currentPlayer = opponent;
    Moves opponentMoves;
    getValidMoves(tempModel, opponentMoves);
    int opponentMobility = opponentMoves.size() * MOBILITY_WEIGHT;
    
    // Evaluar posesión de esquinas y bordes
    int cornerValue = 0;
    int edgeValue = 0;
    
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            Square square = {x, y};
            Piece piece = getBoardPiece(model, square);
            
            if (piece == PIECE_EMPTY)
                continue;
                
            if (isCorner(x, y)) {
                if ((piece == PIECE_WHITE && currentPlayer == PLAYER_WHITE) ||
                    (piece == PIECE_BLACK && currentPlayer == PLAYER_BLACK))
                    cornerValue += CORNER_VALUE;
                else
                    cornerValue -= CORNER_VALUE;
            }
            else if (isEdge(x, y)) {
                if ((piece == PIECE_WHITE && currentPlayer == PLAYER_WHITE) ||
                    (piece == PIECE_BLACK && currentPlayer == PLAYER_BLACK))
                    edgeValue += EDGE_VALUE;
                else
                    edgeValue -= EDGE_VALUE;
            }
        }
    }
    
    // Diferencia ponderada de fichas
    int pieceDiff = (currentPlayerScore - opponentScore) * PIECE_VALUE;
    
    // Valor total combinando todos los factores
    return pieceDiff + cornerValue + edgeValue + (mobility - opponentMobility);
}

/**
 * @brief Algoritmo minimax con poda por profundidad y nodos
 * @param model Modelo del juego
 * @param depth Profundidad actual
 * @param alpha Valor alpha para poda alpha-beta (bonus)
 * @param beta Valor beta para poda alpha-beta (bonus)
 * @param maximizingPlayer Si es el jugador que maximiza
 * @return Valor de la mejor jugada
 */
int minimax(GameModel& model, int depth, int alpha, int beta, bool maximizingPlayer) {
    // Condiciones de terminación
    if (depth == 0 || model.gameOver || nodesEvaluated >= MAX_NODES) {
        return evaluateBoard(model, depth);
    }
    
    Moves validMoves;
    getValidMoves(model, validMoves);
    
    // Si no hay movimientos válidos, pasar turno
    if (validMoves.size() == 0) {
        // Cambiar de jugador
        GameModel tempModel = model;
        tempModel.currentPlayer = (tempModel.currentPlayer == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE;
        
        // Verificar si el otro jugador puede mover
        Moves opponentMoves;
        getValidMoves(tempModel, opponentMoves);
        
        // Si tampoco puede mover, juego terminado
        if (opponentMoves.size() == 0) {
            tempModel.gameOver = true;
            return evaluateBoard(tempModel, depth);
        }
        
        // Continuar con el otro jugador
        return minimax(tempModel, depth - 1, alpha, beta, !maximizingPlayer);
    }
    
    if (maximizingPlayer) {
        int maxEval = std::numeric_limits<int>::min();
        
        for (const Square& move : validMoves) {
            // Hacer copia del modelo y jugar el movimiento
            GameModel tempModel = model;
            playMove(tempModel, move);
            
            // Evaluar recursivamente
            int eval = minimax(tempModel, depth - 1, alpha, beta, false);
            maxEval = std::max(maxEval, eval);
            
            // Poda alpha-beta (bonus)
            alpha = std::max(alpha, eval);
            if (beta <= alpha)
                break;
        }
        
        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();
        
        for (const Square& move : validMoves) {
            // Hacer copia del modelo y jugar el movimiento
            GameModel tempModel = model;
            playMove(tempModel, move);
            
            // Evaluar recursivamente
            int eval = minimax(tempModel, depth - 1, alpha, beta, true);
            minEval = std::min(minEval, eval);
            
            // Poda alpha-beta (bonus)
            beta = std::min(beta, eval);
            if (beta <= alpha)
                break;
        }
        
        return minEval;
    }
}

Square getBestMove(GameModel& model)
{
    // Reiniciar contador de nodos
    nodesEvaluated = 0;
    
    Moves validMoves;
    getValidMoves(model, validMoves);
    
    if (validMoves.size() == 0)
        return GAME_INVALID_SQUARE;
    
    // 1. Primera prioridad: jugar en las esquinas si es posible (mantiene la estrategia original)
    for (const Square& move : validMoves)
    {
        if (isCorner(move.x, move.y))
        {
            return move;
        }
    }
    
    // 2. Usar el algoritmo minimax para evaluar el mejor movimiento
    Square bestMove = validMoves[0];
    int bestValue = std::numeric_limits<int>::min();
    
    for (const Square& move : validMoves)
    {
        // Hacer copia del modelo y jugar el movimiento
        GameModel tempModel = model;
        playMove(tempModel, move);
        
        // Evaluar el movimiento con minimax
        int moveValue = minimax(tempModel, MAX_DEPTH - 1, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), false);
        
        // Actualizar el mejor movimiento si es necesario
        if (moveValue > bestValue)
        {
            bestValue = moveValue;
            bestMove = move;
        }
    }
    
    // 3. Si el mejor movimiento por minimax no es una esquina, considerar las otras prioridades (mantiene la estrategia original)
    if (!isCorner(bestMove.x, bestMove.y))
    {
        // Segunda prioridad: bloquear al oponente para que no pueda jugar en las esquinas
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
                    if (isCorner(opponentMove.x, opponentMove.y))
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
    }
    
    return bestMove;
}