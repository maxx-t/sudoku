#include <stdio.h>
#include <stdlib.h>

//#define VERBOSE // defined if gcc main.c -o sudoku -DVERBOSE
//#define DIAGONALS // defined if gcc main.c -o sudoku -DDIAGONALS

#ifdef VERBOSE
#include <memory.h>
#endif

#define SIZE 9
#define TOTAL SIZE * SIZE
#define TRUE 1
#define FALSE 0

#define ASSIGN_VALUE(s, pos, val) s->row[pos] = s->col[pos] = s->block[pos] = val;


typedef struct cell_t {
  int val;
  struct cell_t **row;
  struct cell_t **col;
  struct cell_t **block;
} cell_t;

typedef struct grid_t {
  cell_t **columns;
  cell_t **rows;
  cell_t **blocks;
  cell_t **data;

#ifdef DIAGONALS
  cell_t **diagonal1;
  cell_t **diagonal2;
#endif

} grid_t;

void printGrid(cell_t *grid) {
  int i, j, val;
  for (i = 0; i < SIZE; i++) {
    if (i % 3 == 0) {
      printf("+------+------+------+\n");
    }
    for (j = 0; j < SIZE; j++) {
      if (j % 3 == 0) {
        printf("|");
      }
      val = (grid + (i * SIZE + j))->val;
      if (val == 0) {
        printf("  ");
      }
      else {
        printf("%d ", val);
      }
    }
    printf("|\n");
  }
  printf("+------+------+------+\n");
}

grid_t *allocGrid() {
  grid_t *grid = malloc(sizeof(grid_t));

  cell_t **data = calloc(4 * TOTAL, sizeof(cell_t *));

  grid->data = data;
  grid->columns = data + TOTAL;
  grid->rows = data + 2 * TOTAL;
  grid->blocks = data + 3 * TOTAL;

#ifdef DIAGONALS
  grid->diagonal1 = calloc(SIZE, sizeof * grid->diagonal1);
  grid->diagonal2 = calloc(SIZE, sizeof * grid->diagonal2);
#endif

  return grid;
}

void freeGrid(grid_t *grid) {
  free(grid->data);
  free(grid);
}

#ifdef VERBOSE
void showLine(int n) {
  char r[n];
  memset(&r, '.', (size_t) n);
  r[n] = '\0';
  printf("%.2d%s", n, r);
}
#endif

int affect(cell_t *cells, int pos, int val, grid_t *grid) {
  cell_t *cell = cells + pos;

  if (cell->col[val - 1] || cell->row[val - 1] || cell->block[val - 1]) {
    return FALSE;
  }

#ifdef DIAGONALS
  int x = pos % SIZE;
  int y = pos / SIZE;
  if (x == y) {
    if (grid->diagonal1[val - 1]) {
      return FALSE;
    }
    else {
      grid->diagonal1[val - 1] = cell;
    }
  }
  if (y + x == SIZE - 1) {
    if (grid->diagonal2[val - 1]) {
      if (x == y) {
        grid->diagonal1[val - 1] = NULL;
      }
      return FALSE;
    }
    else {
      grid->diagonal2[val - 1] = cell;
    }
  }
#endif

#ifdef VERBOSE
  showLine(pos);
  printf(" +%d\n", val);
#endif

  cell->val = val;

  ASSIGN_VALUE(cell, val - 1, cell);

  grid->data[pos] = cell;

  return TRUE;
}

void discard(cell_t *cells, int pos, int val, grid_t *grid) {
  cell_t *cell = cells + pos;
  cell->val = 0;

  ASSIGN_VALUE(cell, val - 1, NULL);

#ifdef DIAGONALS
  int x = pos % SIZE;
  int y = pos / SIZE;
  if (x == y) {
    grid->diagonal1[val - 1] = NULL;
  }
  if (y + x == SIZE - 1) {
    grid->diagonal2[val - 1] = NULL;
  }
#endif

#ifdef VERBOSE
  showLine(pos);
  printf(" -%d\n", val);
#endif

  grid->data[pos] = NULL;
}

void init(cell_t *cells, int *arrayGrid, grid_t *grid) {
  int i, j, pos;
  cell_t *cell;

  for (i = 0; i < SIZE; i++) {
    for (j = 0; j < SIZE; j++) {
      pos = i * SIZE + j;
      int val = *(arrayGrid + pos);

      cell = cells + pos;
      cell->val = val;
      cell->row = &grid->rows[i * SIZE];
      cell->col = &grid->columns[j * SIZE];
      cell->block = &grid->blocks[((j / 3) + 3 * (i / 3)) * SIZE];

      if (cell->val) {
        ASSIGN_VALUE(cell, cell->val - 1, cell);
      }

      grid->data[pos] = cell;
    }
  }
}

int solve(cell_t *cells, grid_t *grid, int depth, int current) {
  int i, j, success = TRUE;

  // Looking for next empty cell
  for (i = current; i < TOTAL; i++) {
    if ((cells + i)->val == 0) {
      break;
    }
  }
  if (i != TOTAL) {
    j = 1;
    success = FALSE;
    while (j < 10 && !success) {
      success = affect(cells, i, j, grid);
      if (success) {
        success = solve(cells, grid, depth + 1, i);
        if (!success) {
          discard(cells, i, j, grid);
        }
      }
      j++;
    }
  }

  if (depth == 0) {
    if (success) {
      printf("Solution found !\n");
    }
    else {
      printf("No solution found .\n");
    }
    printGrid(cells);
  }

  return success;
}

int main() {
  cell_t cells[TOTAL];

  grid_t *grid = allocGrid();

  int initial_grid[SIZE][SIZE] = {
    { 0, 9, 5, 0, 7, 0, 0, 4, 0 },
    { 3, 0, 0, 0, 4, 6, 0, 0, 2 },
    { 6, 0, 0, 5, 0, 2, 0, 0, 0 },
    { 0, 0, 7, 0, 0, 0, 8, 1, 0 },
    { 9, 1, 0, 0, 0, 0, 0, 2, 6 },
    { 0, 3, 6, 0, 0, 0, 9, 0, 0 },
    { 0, 0, 0, 6, 0, 8, 0, 0, 1 },
    { 7, 0, 0, 2, 1, 0, 0, 0, 5 },
    { 0, 8, 0, 0, 5, 0, 2, 6, 0 }
  };

  // int initial_grid[SIZE][SIZE] = {
  //   { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  //   { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  //   { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  //   { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  //   { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  //   { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  //   { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  //   { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  //   { 0, 0, 0, 0, 0, 0, 0, 0, 0 }
  // };

  init(cells, *initial_grid, grid);

  printGrid(*grid->data);

  solve(cells, grid, 0, 0);

  freeGrid(grid);
  return 0;
}
