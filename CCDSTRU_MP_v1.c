#include <stdio.h>

#define DIM 4 

typedef struct {
    int row;
    int col;
} Position;

typedef struct {
    int good;
    int go;
    int start;
    int over;
    int found;
    int val;
    int R[DIM][DIM];
    int B[DIM][DIM];
    int S[DIM][DIM];
    int T[DIM][DIM];
} GameState;

/* ---------- Utility Functions ---------- */

int inBounds(Position pos) {
    return (pos.row >= 1 && pos.row <= 3 && pos.col >= 1 && pos.col <= 3);
}

int countSet(int Set[DIM][DIM]) {
    int i, j, count = 0;
    for (i = 1; i <= 3; i++)
        for (j = 1; j <= 3; j++)
            if (Set[i][j]) count++;
    return count;
}

void updateOver(GameState *g) {
    int rCount = countSet(g->R);
    int bCount = countSet(g->B);
    int fCount = 0;
    int i, j;

    for (i = 1; i <= 3; i++)
        for (j = 1; j <= 3; j++)
            if (!g->R[i][j] && !g->B[i][j]) fCount++;

    if (fCount == 3 || g->val >= 20 || 
       (!g->start && ((rCount > 0 && bCount == 0) || (rCount == 0 && bCount > 0)))) {
        g->over = 1;
    }
}

/* ---------- System Behaviors ---------- */

void Remove(GameState *g, Position pos) {
    if (inBounds(pos)) {
        if (g->go) g->R[pos.row][pos.col] = 0;
        else g->B[pos.row][pos.col] = 0;
        
        g->S[pos.row][pos.col] = 0;
        g->T[pos.row][pos.col] = 0;
    }
}

void Replace(GameState *g, Position pos);

void Expand(GameState *g, Position pos) {
    Position u = {pos.row - 1, pos.col};
    Position d = {pos.row + 1, pos.col};
    Position k = {pos.row, pos.col - 1};
    Position r = {pos.row, pos.col + 1};

    Remove(g, pos);

    if (g->go) {
        Replace(g, u);
        Replace(g, d);
    }
    Replace(g, k);
    Replace(g, r);
}

void Replace(GameState *g, Position pos) {
    if (inBounds(pos)) {
        g->found = 0;

        if (g->go) {
            if (g->B[pos.row][pos.col]) { g->B[pos.row][pos.col] = 0; g->found = 1; }
            if (g->R[pos.row][pos.col]) g->found = 1;
            if (!g->R[pos.row][pos.col]) g->R[pos.row][pos.col] = 1;
        } else {
            if (g->R[pos.row][pos.col]) { g->R[pos.row][pos.col] = 0; g->found = 1; }
            if (g->B[pos.row][pos.col]) g->found = 1;
            if (!g->B[pos.row][pos.col]) g->B[pos.row][pos.col] = 1;
        }

        if (g->found && !g->S[pos.row][pos.col]) {
            g->S[pos.row][pos.col] = 1;
        } 
        else if (g->found && g->S[pos.row][pos.col] && !g->T[pos.row][pos.col]) {
            g->T[pos.row][pos.col] = 1;
            Expand(g, pos);
        }
    }
}

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

void NextPlayerMove(GameState *g, Position pos) {
    if (!inBounds(pos)) {
        g->good = 0;
        return;
    }

    g->good = 0;

    if (g->start) {
        if (g->go && !g->R[pos.row][pos.col] && !g->B[pos.row][pos.col]) {
            g->R[pos.row][pos.col] = 1;
            g->S[pos.row][pos.col] = 1;
            g->good = 1;
        } 
        else if (!g->go && !g->R[pos.row][pos.col] && !g->B[pos.row][pos.col]) {
            g->B[pos.row][pos.col] = 1;
            g->S[pos.row][pos.col] = 1;
            g->good = 1;
        }

        if (countSet(g->R) == 1 && countSet(g->B) == 1) g->start = 0;
    } 
    else {
        if ((g->go && g->R[pos.row][pos.col]) || (!g->go && g->B[pos.row][pos.col])) {
            Update(g, pos);
            g->good = 1;
        }
    }

    if (!g->over && g->good) {
        g->good = 0;
        g->go = !g->go;
        g->val = g->val + 1;
    }

    updateOver(g);
}

/* ---------- Main Logic ---------- */

int main() {
    GameState game;
    Position move;
    int i, j, rFin, bFin;

    game.good = 0; game.go = 1; game.start = 1; game.over = 0;
    game.found = 0; game.val = 0;

    for (i = 0; i < DIM; i++) {
        for (j = 0; j < DIM; j++) {
            game.R[i][j] = 0; game.B[i][j] = 0;
            game.S[i][j] = 0; game.T[i][j] = 0;
        }
    }

    while (game.over == 0) {
        printf("\n  1 2 3\n");
        for (i = 1; i <= 3; i++) {
            printf("%d ", i);
            for (j = 1; j <= 3; j++) {
                if (game.R[i][j]) printf("R ");
                else if (game.B[i][j]) printf("B ");
                else printf(". ");
            }
            printf("\n");
        }

        printf("\nTurn: %s | Val: %d\n", game.go ? "R" : "B", game.val);
        printf("Enter row and column (1-3): ");

        if (scanf("%d %d", &move.row, &move.col) != 2) {
            printf("Invalid input format. Please enter two numbers separated by space.\n");
            while (getchar() != '\n'); // clear invalid input
            continue;
        }

        /* Check if the cell is already occupied */
        if ((game.R[move.row][move.col] || game.B[move.row][move.col])) {
            printf("Invalid move. Cell is already occupied. Try again.\n");
            continue;
        }

        NextPlayerMove(&game, move);
    }

    rFin = countSet(game.R);
    bFin = countSet(game.B);

    printf("\n--- GAME OVER ---\n");
    if (rFin > bFin) printf("Result: R wins\n");
    else if (bFin > rFin) printf("Result: B wins\n");
    else printf("Result: draw\n");

    return 0;
}
