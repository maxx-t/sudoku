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


typedef struct _cell {
  int val;
  struct _cell **row;
  struct _cell **col;
  struct _cell **block;
} Cell;

typedef struct _grid {
  Cell **columns;
  Cell **rows;
  Cell **blocks;
  Cell **data;

#ifdef DIAGONALS
  Cell **diagonal1;
  Cell **diagonal2;
#endif

} Grid;

void printGrid(Cell *grid) {
  int i, j, val;
  for (i = 0; i < SIZE; i++) {
    if (i % 3 == 0) printf("+------+------+------+\n");
    for (j = 0; j < SIZE; j++) {
      if (j % 3 == 0) printf("|");
      val = (grid + (i * SIZE + j))->val;
      if (val == 0) printf("  ");
      else printf("%d ", val);
    }
    printf("|\n");
  }
  printf("+------+------+------+\n");
}

Grid *newGrid() {
  Grid *grid = malloc(sizeof(Grid));

  Cell **data = calloc(4 * TOTAL, sizeof(Cell *));

  grid->data = data;
  grid->columns = data + TOTAL;
  grid->rows = data + 2 * TOTAL;
  grid->blocks = data + 3 * TOTAL;

#ifdef DIAGONALS
  grid->diagonal1 = calloc(SIZE, sizeof * grid->diagonal1 );
  grid->diagonal2 = calloc(SIZE, sizeof * grid->diagonal2 );
#endif

  return grid;
}

void freeGrid(Grid *grid) {
  free(grid->data);
  free(grid);
}

#ifdef VERBOSE

static void showLine(int n) {
  char r[n];
  memset(&r, '.', (size_t) n);
  r[n] = '\0';
  printf("%.2d%s", n, r);
}

#endif

int affect(Cell *cells, int pos, int val, Grid *grid) {
  Cell *cell = cells + pos;

  if (cell->col[val - 1] || cell->row[val - 1] || cell->block[val - 1]) return FALSE;

#ifdef DIAGONALS
  int x = pos % SIZE;
  int y = pos / SIZE;
  if( x == y ) {
    if ( grid->diagonal1[val - 1] ) return FALSE;
    else grid->diagonal1[val - 1] = cell;
  }
  if( (y+x) == (SIZE-1) ) {
    if ( grid->diagonal2[val - 1] ) {
      if( x == y ) grid->diagonal1[val - 1] = NULL;//
      return FALSE;
    }
    else grid->diagonal2[val - 1] = cell;
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

void discard(Cell *cells, int pos, int val, Grid *grid) {
  Cell *cell = cells + pos;
  cell->val = 0;

  ASSIGN_VALUE(cell, val - 1, NULL);

#ifdef DIAGONALS
  int x = pos % SIZE;
  int y = pos / SIZE;
  if( x == y ) grid->diagonal1[val - 1] = NULL;//
  if( (y+x) == (SIZE-1) ) grid->diagonal2[val - 1] = NULL;
#endif

#ifdef VERBOSE
  showLine(pos);
  printf(" -%d\n", val);
#endif

  grid->data[pos] = NULL;
}

void init(Cell *cells, int *arrayGrid, Grid *grid) {
  int i, j, pos;
  Cell *cell;

  for (i = 0; i < SIZE; i++) {
    for (j = 0; j < SIZE; j++) {
      pos = i * SIZE + j;
      int val = *(arrayGrid + pos);

      cell = (cells + pos);
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

int solve(Cell *cells, Grid *grid, int depth, int current) {
  int i, j, success = TRUE;

  //looking for next empty cell
  for (i = current; i < TOTAL; i++) if ((cells + i)->val == 0) break;
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
    if (success) printf("Solution found !\n"), printGrid(cells);
    else printf("No solution found .\n"), printGrid(cells);
  }

  return success;
}

int main() {
  Cell cells[TOTAL];

  Grid *grid = newGrid();

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
