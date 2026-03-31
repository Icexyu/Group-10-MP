#include <stdio.h>

#define DIM 4   
/* We use indices 1 to 3 only. 
Index 0 is ignored. */

/* 
   A board position is represented by a row and column.
   Valid positions are only from (1,1) to (3,3).
*/
typedef struct {
    int row;
    int col;
} Position;

/*
   This struct stores the whole game state.
   The 2D arrays act like sets:
   1 means the position is part of the set
   0 means it is not
*/
typedef struct {
    int good;   /* tells if the current move is considered valid */
    int go;     /* 1 = R's turn, 0 = B's turn */
    int start;  /* 1 = starting phase, 0 = update phase */
    int over;   /* 1 = game over, 0 = game still running */
    int found;  /* helper variable used inside Replace() */
    int val;    /* move counter */

    int R[DIM][DIM];   /* set of positions owned by R */
    int B[DIM][DIM];   /* set of positions owned by B */
    int S[DIM][DIM];   /* set of visited/selected positions */
    int T[DIM][DIM];   /* set of already expanded positions */
} GameState;

/* ---------- Function Prototypes ---------- */
/* ---------- Implemented Code ------------- */
int inBounds(Position pos);
int countSet(int setArr[DIM][DIM]);
int countFree(GameState *g);
void recomputeOver(GameState *g);
void initializeGame(GameState *g);
void printBoard(GameState *g);
int isValidMove(GameState *g, Position pos);

/* ---------- Implemented Specs ------------ */
void Remove(GameState *g, Position pos);
void Replace(GameState *g, Position pos);
void Expand(GameState *g, Position pos);
void Update(GameState *g, Position pos);
void NextPlayerMove(GameState *g, Position pos);

/* ---------- Shows the Result ------------ */
void printResult(GameState *g);
void clearInputBuffer(void);

/* ---------- Utility Functions ---------- */

/* 
   Checks if a position is inside the valid board.
   Only rows and columns from 1 to 3 are allowed.
*/
int inBounds(Position pos) {
    int inside = 0;		// Is inside

    if (pos.row >= 1 
	 && pos.row <= 3 
	 && pos.col >= 1 
	 && pos.col <= 3) {
        inside = 1;		// Is not inside, therefore invalid placement
    }
    return inside;	
}

/*
   Counts how many cells are currently inside a given set.
   This is used for sets like R, B, etc.
*/
int countSet(int setArr[DIM][DIM]) {
    int i;
    int j;
    int count;

    count = 0;

    for (i = 1; i <= 3; i++) {
        for (j = 1; j <= 3; j++) {
            if (setArr[i][j] == 1) {
                count++;
            }
        }
    }

    return count;
}

/*
   Counts the number of free cells.
   A cell is free if it is not in R and not in B.
   This corresponds to F = M - (R union B).
*/
int countFree(GameState *g) {
    int i;
    int j;
    int count;

    count = 0;

    for (i = 1; i <= 3; i++) {
        for (j = 1; j <= 3; j++) {
            if (g->R[i][j] == 0 && g->B[i][j] == 0) {
                count++;
            }
        }
    }

    return count;
}

void recomputeOver(GameState *g) {	// recomputes the "over" condition based
    int rCount;						// on the formal specification
    int bCount;
    int fCount;
    rCount = countSet(g->R);
    bCount = countSet(g->B);
    fCount = countFree(g);
    g->over = 0;

    if (fCount == 3) {		// gameover if there are exactly 3 free cells left
        g->over = 1;
    }
    if (g->val >= 20) {		// gameover if val is at least 20
        g->over = 1;
    }
    if (g->start == 0) {	// gameover if no longer in the start phase AND only one 
    						// player still has pieces on the board (R or B)
        if ((rCount > 0 && bCount == 0) || (rCount == 0 && bCount > 0)) {
            g->over = 1;
        }
    }
}

void initializeGame(GameState *g) {
    int i;
    int j;
	/* Initializes the game exactly 
	according to the specification:*/
    g->good = 0;	// good = false
    g->go = 1;		// go = true
    g->start = 1;	// start = true
    g->over = 0;	// over = false
    g->found = 0;	// found = false
    g->val = 0;		// val = 0			R, B, S, T are all empty
	
    for (i = 0; i < DIM; i++) {
        for (j = 0; j < DIM; j++) {
            g->R[i][j] = 0;
            g->B[i][j] = 0;
            g->S[i][j] = 0;
            g->T[i][j] = 0;
        }
    }
}

void printBoard(GameState *g) {	// Prints the board
    int i;
    int j;

    printf("\n  1 2 3\n");
    for (i = 1; i <= 3; i++) {
        printf("%d ", i);
        for (j = 1; j <= 3; j++) {
            if (g->R[i][j] == 1) {
                printf("R ");	// R = position owned by Red player
            } else if (g->B[i][j] == 1) {
                printf("B ");	// B = position owned by Blue player
            } else {
                printf(". ");	// ". " = empty cell
            }
        }
        printf("\n");
    }
}

/*
   Validates a player's chosen move.

   During the start phase:
   - the player must choose an empty cell

   During the update phase:
   - R must choose one of R's cells
   - B must choose one of B's cells
*/
int isValidMove(GameState *g, Position pos) {
    int valid = 0;

    if (inBounds(pos) == 1) {
        if (g->start == 1) {
            if (g->R[pos.row][pos.col] == 0 && g->B[pos.row][pos.col] == 0) {
                valid = 1;
            }
        } else {
            if (g->go == 1) {
                if (g->R[pos.row][pos.col] == 1) {
                    valid = 1;
                }
            } else {
                if (g->B[pos.row][pos.col] == 1) {
                    valid = 1;
                }
            }
        }
    }

    return valid;
}

/* ---------- System Behaviors ---------- */

/*
   Remove(pos)
   If it is R's turn, remove pos from R.
   If it is B's turn, remove pos from B.
   In both cases, also remove pos from S and T.
*/
void Remove(GameState *g, Position pos) {
    if (inBounds(pos) == 1) {
        if (g->go == 1) {
            g->R[pos.row][pos.col] = 0;
        } else {
            g->B[pos.row][pos.col] = 0;
        }

        g->S[pos.row][pos.col] = 0;
        g->T[pos.row][pos.col] = 0;
    }
}

/*
   Expand(pos)

   Let:
   u = up
   d = down
   k = left
   r = right

   Then:
   - remove the current position
   - if go is true, replace the cell above
   - if go is false, replace the cell below
   - always replace left and right
*/
void Expand(GameState *g, Position pos) {
    Position u;
    Position d;
    Position k;
    Position r;

    u.row = pos.row - 1;
    u.col = pos.col;

    d.row = pos.row + 1;
    d.col = pos.col;

    k.row = pos.row;
    k.col = pos.col - 1;

    r.row = pos.row;
    r.col = pos.col + 1;

    Remove(g, pos);

    if (g->go == 1) {
        Replace(g, u);
    }

    if (g->go == 0) {
        Replace(g, d);
    }

    Replace(g, k);
    Replace(g, r);
}

/*
   Replace(pos)

   This follows the symbolic rules more closely.

   For R's turn:
   - if pos is in B, remove it from B and set found = true
   - if pos is already in R, found = true
   - if pos is not in R, add it to R

   For B's turn:
   - symmetric behavior using R and B

   After that:
   - if found is true and pos is not in S, add pos to S
   - if found is true and pos is already in S but not in T,
     add pos to T and call Expand(pos)
*/
void Replace(GameState *g, Position pos) {
    if (inBounds(pos) == 1) {
        g->found = 0;

        if (g->go == 1) {
            if (g->B[pos.row][pos.col] == 1) {
                g->B[pos.row][pos.col] = 0;
                g->found = 1;
            }
            if (g->R[pos.row][pos.col] == 1) {
                g->found = 1;
            }
            if (g->R[pos.row][pos.col] == 0) {
                g->R[pos.row][pos.col] = 1;
            }
        } else {
            if (g->R[pos.row][pos.col] == 1) {
                g->R[pos.row][pos.col] = 0;
                g->found = 1;
            }
            if (g->B[pos.row][pos.col] == 1) {
                g->found = 1;
            }
            if (g->B[pos.row][pos.col] == 0) {
                g->B[pos.row][pos.col] = 1;
            }
        }
        if (g->found == 1 && g->S[pos.row][pos.col] == 0) {
            g->S[pos.row][pos.col] = 1;
            g->found = 0;
        }
        if (g->found == 1 && g->S[pos.row][pos.col] == 1 && g->T[pos.row][pos.col] == 0) {
            g->T[pos.row][pos.col] = 1;
            Expand(g, pos);
        }
    }
}

/*
   Update(pos)

   Start by setting good = false.
   - if pos is not yet in S, add it to S and toggle good
   - if good is still false and pos is already in S but not in T,
     add it to T and expand from there
*/
void Update(GameState *g, Position pos) {
    g->good = 0;

    if (inBounds(pos) == 1) {
        if (g->S[pos.row][pos.col] == 0) {
            g->S[pos.row][pos.col] = 1;
            g->good = !g->good;
        }

        if (g->good == 0 && g->S[pos.row][pos.col] == 1 && g->T[pos.row][pos.col] == 0) {
            g->T[pos.row][pos.col] = 1;
            Expand(g, pos);
        }
    }
}

/*
   NextPlayerMove(pos)

   This is the main move-processing function.

   Start phase:
   - if R's turn, place R on an empty cell
   - if B's turn, place B on an empty cell
   - in both cases, also add the position to S

   Update phase:
   - the player may only choose one of their own positions
   - then Update(pos) is called

   After both players have exactly one starting piece:
   - start becomes false

   If the move is good and the game is not over:
   - switch turns
   - increase val by 1
*/
void NextPlayerMove(GameState *g, Position pos) {
    g->good = 0;
    recomputeOver(g);

    if (g->over == 0 && inBounds(pos) == 1) {
        if (g->start == 1 && g->go == 1) {
            if (g->R[pos.row][pos.col] == 0 && g->B[pos.row][pos.col] == 0) {
                g->R[pos.row][pos.col] = 1;
                g->S[pos.row][pos.col] = 1;
                g->good = 1;
            }
        }
        if (g->start == 1 && g->go == 0) {
            if (g->R[pos.row][pos.col] == 0 && g->B[pos.row][pos.col] == 0) {
                g->B[pos.row][pos.col] = 1;
                g->S[pos.row][pos.col] = 1;
                g->good = 1;
            }
        }
        if (g->over == 0 && g->start == 0) {
            if ((g->go == 1 && g->R[pos.row][pos.col] == 1) ||
                (g->go == 0 && g->B[pos.row][pos.col] == 1)) {
                Update(g, pos);
                g->good = 1;
            }
        }
        if (g->start == 1) {
            if (countSet(g->R) == 1 && countSet(g->B) == 1) {
                g->start = 0;
            }
        }
        recomputeOver(g);
        if (g->over == 0 && g->good == 1) {
            g->good = !g->good;
            g->go = !g->go;
            g->val = g->val + 1;
        }
        recomputeOver(g);
    }
}

/*
   Prints the final result once the game is over.
   The winner is determined by comparing the sizes of R and B.
*/
void printResult(GameState *g) {
    int rFin;
    int bFin;

    rFin = countSet(g->R);
    bFin = countSet(g->B);

    printf("\n--- GAME OVER ---\n");

    if (rFin > bFin) {
        printf("Result: R wins\n");
    } else if (bFin > rFin) {
        printf("Result: B wins\n");
    } else {
        printf("Result: draw\n");
    }
}

/*
   Clears the remaining characters in the input buffer.
   This helps when the user enters invalid input.
*/
void clearInputBuffer(void) {
    int ch;
    int done = 0;

    while (done == 0) {
        ch = getchar();

        if (ch == '\n' || ch == EOF) {
            done = 1;
        }
    }
}

/* ---------- Main Program ---------- */

int main(void) {
    GameState game;
    Position move;
    int scanOK;
    int validMove;
    int done;

    initializeGame(&game);
    done = 0;

    while (done == 0) {
        recomputeOver(&game);

        if (game.over == 0) {
            printBoard(&game);

            printf("\nTurn: %s | val: %d\n", game.go == 1 ? "R" : "B", game.val);

            if (game.start == 1) {
                printf("Starting phase: choose an empty cell.\n");
            } else {
                printf("Update phase: choose one of your own pieces.\n");
            }

            printf("Enter row and column: ");
            scanOK = scanf("%d %d", &move.row, &move.col);

            if (scanOK != 2) {
                printf("Invalid input.\n");
                clearInputBuffer();
            } else {
                validMove = isValidMove(&game, move);

                if (validMove == 1) {
                    NextPlayerMove(&game, move);
                } else {
                    printf("Invalid move.\n");
                }
            }
        } else {
            done = 1;
        }
    }

    printBoard(&game);
    printResult(&game);

    return 0;
}
