#include <stdio.h>

#define DIM 4   // Using 4 so we can index from 1 to 3 (ignore index 0)

/* Represents a position (a, b) ? M */
typedef struct {
    int row;
    int col;
} Position;

/* Represents the entire system state */
typedef struct {
    int good;   // indicates if last move is valid
    int go;     // true = R's turn, false = B's turn
    int start;  // starting phase flag
    int over;   // game over flag
    int found;  // helper flag used in Replace
    int val;    // move counter

    /* Sets represented as 2D arrays (1 = element exists) */
    int R[DIM][DIM];  // Red positions
    int B[DIM][DIM];  // Blue positions
    int S[DIM][DIM];  // visited set
    int T[DIM][DIM];  // expanded set
} GameState;

/* ---------- Utility Functions ---------- */

/* Check if position is inside M = C × C, where C = {1,2,3} */
int inBounds(Position pos) {
    return (pos.row >= 1 && pos.row <= 3 && pos.col >= 1 && pos.col <= 3);
}

/* Count number of elements in a set */
int countSet(int Set[DIM][DIM]) {
    int i, j, count = 0;

    for (i = 1; i <= 3; i++)
        for (j = 1; j <= 3; j++)
            if (Set[i][j]) count++;

    return count;
}

/* Compute |F| = free cells = positions not in R or B */
int countFree(GameState *g) {
    int i, j, count = 0;

    for (i = 1; i <= 3; i++)
        for (j = 1; j <= 3; j++)
            if (!g->R[i][j] && !g->B[i][j]) count++;

    return count;
}

/* Evaluate "over" condition based on formal specification */
void recomputeOver(GameState *g) {
    int rCount = countSet(g->R);
    int bCount = countSet(g->B);
    int fCount = countFree(g);

    /* over ? (|F| = 3 OR val = 20 OR imbalance after start) */
    if (fCount == 3 ||
        g->val >= 20 ||
        (!g->start && ((rCount > 0 && bCount == 0) || (rCount == 0 && bCount > 0))))
        g->over = 1;
    else
        g->over = 0;
}

/* Initialize system variables based on spec */
void initializeGame(GameState *g) {
    int i, j;

    g->good = 0;
    g->go = 1;      // R starts
    g->start = 1;
    g->over = 0;
    g->found = 0;
    g->val = 0;

    /* initialize all sets to empty */
    for (i = 0; i < DIM; i++)
        for (j = 0; j < DIM; j++) {
            g->R[i][j] = 0;
            g->B[i][j] = 0;
            g->S[i][j] = 0;
            g->T[i][j] = 0;
        }
}

/* Display board */
void printBoard(GameState *g) {
    int i, j;

    printf("\n  1 2 3\n");
    for (i = 1; i <= 3; i++) {
        printf("%d ", i);
        for (j = 1; j <= 3; j++) {
            if (g->R[i][j]) printf("R ");
            else if (g->B[i][j]) printf("B ");
            else printf(". ");
        }
        printf("\n");
    }
}

/* Validate move depending on phase */
int isValidMove(GameState *g, Position pos) {
    if (!inBounds(pos)) return 0;

    /* Start phase: must choose empty cell */
    if (g->start)
        return (!g->R[pos.row][pos.col] && !g->B[pos.row][pos.col]);

    /* Update phase: must choose own piece */
    if (g->go)
        return g->R[pos.row][pos.col];
    else
        return g->B[pos.row][pos.col];
}

/* ---------- System Behaviors ---------- */

/* Remove(pos): removes pos from current player's set and S, T */
void Remove(GameState *g, Position pos) {
    if (inBounds(pos)) {
        if (g->go) g->R[pos.row][pos.col] = 0;
        else g->B[pos.row][pos.col] = 0;

        g->S[pos.row][pos.col] = 0;
        g->T[pos.row][pos.col] = 0;
    }
}

/* Forward declaration */
void Replace(GameState *g, Position pos);

/* Expand(pos): applies directional expansion rules */
void Expand(GameState *g, Position pos) {
    Position u = {pos.row - 1, pos.col};
    Position d = {pos.row + 1, pos.col};
    Position k = {pos.row, pos.col - 1};
    Position r = {pos.row, pos.col + 1};

    Remove(g, pos);

    /* Follow exact spec behavior */
    if (g->go) Replace(g, u);
    if (!g->go) Replace(g, d);

    Replace(g, k);
    Replace(g, r);
}

/* Replace(pos): handles transfer between R and B and triggers expansion */
void Replace(GameState *g, Position pos) {
    if (inBounds(pos)) {
        g->found = 0;

        /* If R's turn */
        if (g->go) {
            if (g->B[pos.row][pos.col]) {
                g->B[pos.row][pos.col] = 0;
                g->found = 1;
            }
            if (g->R[pos.row][pos.col]) g->found = 1;
            if (!g->R[pos.row][pos.col]) g->R[pos.row][pos.col] = 1;
        }
        /* If B's turn */
        else {
            if (g->R[pos.row][pos.col]) {
                g->R[pos.row][pos.col] = 0;
                g->found = 1;
            }
            if (g->B[pos.row][pos.col]) g->found = 1;
            if (!g->B[pos.row][pos.col]) g->B[pos.row][pos.col] = 1;
        }

        /* Manage S and T sets based on found */
        if (g->found && !g->S[pos.row][pos.col]) {
            g->S[pos.row][pos.col] = 1;
            g->found = 0;
        }

        if (g->found && g->S[pos.row][pos.col] && !g->T[pos.row][pos.col]) {
            g->T[pos.row][pos.col] = 1;
            Expand(g, pos);
        }
    }
}

/* Update(pos): handles revisiting and expansion logic */
void Update(GameState *g, Position pos) {
    g->good = 0;

    if (inBounds(pos)) {
        if (!g->S[pos.row][pos.col]) {
            g->S[pos.row][pos.col] = 1;
            g->good = 1;
        }

        if (!g->good && g->S[pos.row][pos.col] && !g->T[pos.row][pos.col]) {
            g->T[pos.row][pos.col] = 1;
            Expand(g, pos);
        }
    }
}

/* NextPlayerMove: main transition function */
void NextPlayerMove(GameState *g, Position pos) {
    recomputeOver(g);
    g->good = 0;

    if (!g->over && inBounds(pos)) {

        /* Starting phase */
        if (g->start && g->go) {
            g->R[pos.row][pos.col] = 1;
            g->S[pos.row][pos.col] = 1;
            g->good = 1;
        }

        if (g->start && !g->go) {
            g->B[pos.row][pos.col] = 1;
            g->S[pos.row][pos.col] = 1;
            g->good = 1;
        }

        /* Update phase */
        if (!g->start &&
            ((g->go && g->R[pos.row][pos.col]) ||
             (!g->go && g->B[pos.row][pos.col]))) {
            Update(g, pos);
            g->good = 1;
        }

        /* End start phase after first moves */
        if (g->start && countSet(g->R) == 1 && countSet(g->B) == 1)
            g->start = 0;

        recomputeOver(g);

        /* Switch turn */
        if (!g->over && g->good) {
            g->good = 0;
            g->go = !g->go;
            g->val++;
        }

        recomputeOver(g);
    }
}

/* ---------- Main ---------- */

int main(void) {
    GameState game;
    Position move;
    int scanOK, validMove;
    int rFin, bFin;
    int done = 0;

    initializeGame(&game);

    while (!done) {
        recomputeOver(&game);

        if (!game.over) {
            printBoard(&game);

            printf("\nTurn: %s | Val: %d\n", game.go ? "R" : "B", game.val);

            if (game.start)
                printf("Starting phase: choose empty cell\n");
            else
                printf("Update phase: choose your own piece\n");

            printf("Enter row and column: ");
            scanOK = scanf("%d %d", &move.row, &move.col);

            if (scanOK != 2) {
                printf("Invalid input.\n");
                while (getchar() != '\n');
            } else {
                validMove = isValidMove(&game, move);

                if (!validMove)
                    printf("Invalid move.\n");
                else
                    NextPlayerMove(&game, move);
            }
        } else {
            done = 1;
        }
    }

    rFin = countSet(game.R);
    bFin = countSet(game.B);

    printBoard(&game);
    printf("\n--- GAME OVER ---\n");

    if (rFin > bFin) printf("Result: R wins\n");
    else if (bFin > rFin) printf("Result: B wins\n");
    else printf("Result: draw\n");

    return 0;
}
