# EDAversi

## Integrantes del grupo y contribución al trabajo de cada integrante

* Ian Dib:
En model.cpp, completó getValidMoves() para recorrer el tablero, verificar cada casilla vacía y determinar si es un movimiento válido llamando a checkDirection() para cada una de las ocho direcciones posibles. Asimismo, completó playMove() para que, al colocar la ficha del jugador, se volteen las piezas capturadas.
En ai.cpp, terminó la función getBestMove(), que determina el mejor movimiento, implementando una llamada a la función minimax() en lugar de elegir el movimiento que voltea la mayor cantidad de fichas. En esta función, estableció poda por profundidad controlada con MAX_DEPTH, poda por cantidad de nodos con MAX_NODES y poda alfa-beta.
Como adicional, modificó la estructura GameModel en model.h agregando los campos lastHumanMove y lastAIMove, implementó la función squareToChessNotation() en controller.cpp, creó drawLastMove() y modificó updateView() en view.cpp para mostrar el último movimiento del jugador y de la IA en notación de ajedrez. Además, modificó drawView() para dibujar también círculos grises sobre cada casilla en la que el jugador puede ubicar una ficha.

* Luciano Cordero:
Diseñó en model.cpp la función auxiliar checkDirection() para registrar las fichas potencialmente volteables. En ai.cpp agregó las funciones countFlipped() y countTotalFlipped() que calculan cuántas fichas se voltearían con un movimiento dado, isCorner() e isEdge() que identifican posiciones estratégicas en el tablero y evaluateBoard() que asigna un valor al estado actual del tablero.
A partir de estas nuevas funciones, planteó una primera versión de getBestMove() que establecía las prioridades 1. ocupar las esquinas, 2. bloquear que el jugador ocupe las esquinas (prioridad que luego se descartó) y 3. seleccionar el movimiento inmediato que voltee la mayor cantidad de fichas (prioridad que luego fue reemplazada por el algoritmo minimax). Buscó identificar los parámetros para la poda alfa-beta.

## Parte 1: Generación de movimientos válidos y algoritmo de jugada

Gracias a la implementación en drawView() de los círculos grises que marcan las posiciones permitidas, se agilizó la validación de los movimientos cliqueando las casillas que checkDirection() identifica como inválidas. En todos los casos el algoritmo rechazó la colocación de ficha, comprobando que getValidMoves() y playMove() únicamente permiten jugadas legales. Al simular partidas completas (hasta que ninguno de los jugadores tiene movimientos válidos), se verificó que playMove() actualizara correctamente el turno y disparara la detección del fin de partida.

Adicionalmente, en las pruebas finales se enfrentó nuestra IA contra la versión online de Reversi en https://playpager.com/juego-reversi/. Gracias a la función drawLastMove(), que muestra cada movimiento en notación de ajedrez, se facilitó replicar jugada por jugada en paralelo. Esto confirmó que getBestMove() nunca selecciona una casilla inválida, garantizando consistencia en ambas implementaciones.

## Parte 2: Implementación del motor de IA

La implementación de la función minimax() es recursiva. En cada nodo se invoca a getValidMoves() para obtener las jugadas válidas y a playMove() para simularlas, creando los nodos hijos, y se llama a minimax() intercalando los roles de maximizingPlayer (IA) y minimizingPlayer (oponente).

La exploración del árbol de juego se realiza por profundidad: cada rama se expande completamente hasta alcanzar una condición terminal (profundidad cero o fin de partida) antes de retroceder y evaluar las alternativas pendientes.

Cuando se alcanza una condición terminal, la función minimax() llama a evaluateBoard() para evaluar el nodo hoja. Esto se logra asignando pesos predefinidos a factores estratégicos (diferencia de piezas, posesión de esquinas y bordes, movilidad) y devolviendo un único valor numérico que representa la calidad de la posición.

Para cada nodo interno, si el turno corresponde a la IA, minimax() selecciona el máximo entre los valores devueltos por los hijos; si es el oponente, toma el mínimo. Esta garantiza que la IA busque su propio mejor resultado mientras presupone el peor escenario impuesto por el rival.

La función getBestMove() evalúa los nodos raíz, es decir, itera sobre todos los movimientos válidos del estado inicial, llama a minimax() para cada uno y retiene la acción con mayor puntuación.

## Parte 3: Poda del árbol

En el paper "An artificial intelligence agent playing Reversi" (https://ceur-ws.org/Vol-1107/paper2.pdf) se indica que la complejidad temporal del algoritmo minimax sin poda es O(b^d), donde b es el factor de ramificación —número medio de movimientos válidos por turno— y d la profundidad de búsqueda. Además, como indica el artículo "Building an AI to Play My Favourite Board Game: Othello" (https://samharrison00.medium.com/building-an-ai-to-play-my-favourite-board-game-othello-57f5aab1d6cf), se estima que el árbol de juego contiene hasta 10^54 nodos. Resulta computacionalmente inviable que el algoritmo recorra todas las posibles secuencias de movimientos, que es lo que la función minimax() sin poda intenta hacer.

Para resolver el problema, se implementaron tres mecanismos de poda: 1. poda por profundidad, limitando la recursión a MAX_DEPTH niveles; 2. poda por cantidad de nodos, deteniendo la búsqueda tras evaluar MAX_NODES posiciones; y 3. poda alfa-beta, definiendo alpha como el mejor valor (mayor) que el maximizador (IA) puede garantizar y beta como mejor valor (menor) que el minimizador (oponente) puede garantizar, ambos hasta el momento en la exploración actual.

## Bonus points

- Se modificó drawView() para marcar las casilla válidas con círculos grises, facilitando la identificación de jugadas válidas. Además, drawLastMove() resalta el último movimiento de humano y IA en notación de ajedrez —gracias a squareToChessNotation()— lo que permite comparar IAs con más facilidad.

- Se enfrentó la versión actual del programa —con Minimax y poda alfa‑beta— contra la primera implementación y contra la IA de máxima dificultad en playpager.com. En la mayoría de pruebas la versión optimizada ganó con un margen significativo. Paralelamente, se exploraron distintos umbrales de corte en la poda por profundidad y por número de nodos, buscando encontrar el mejor compromiso entre velocidad de respuesta y calidad de la decisión.

- Se intentó aplicar la técnica de poda alfa‑beta como complemento a las podas anteriores, lo que permite reducir drásticamente el número de nodos explorados sin afectar la precisión de las jugadas. La implementación de la poda alfa-beta sigue las recomendaciones del artículo “How to write an Othello AI with alpha-beta search” (https://medium.com/@jackychoi26/how-to-write-an-othello-ai-with-alpha-beta-search-58131ffe67eb).