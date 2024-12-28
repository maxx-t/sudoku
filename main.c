#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SIZE 9
#define GRID_SIZE SIZE * SIZE

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
} grid_t;

static inline void assign_cell_ptr(cell_t *c, int i) {
    c->row[i] = c->col[i] = c->block[i] = c;
}

static inline void reset_cell_ptr(cell_t *c, int i) {
    c->row[i] = c->col[i] = c->block[i] = NULL;
}

void print_grid(cell_t *grid) {
  int i, j, val;
  for (i = 0; i < SIZE; i++) {
    if (i % 3 == 0) {
      printf("+-------+-------+-------+\n");
    }
    for (j = 0; j < SIZE; j++) {
      if (j % 3 == 0) {
        printf("| ");
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
  printf("+-------+-------+-------+\n");
}

grid_t *alloc_grid(void) {
  grid_t *grid = malloc(sizeof(grid_t));

  cell_t **data = calloc(4 * GRID_SIZE, sizeof(cell_t *));

  grid->data = data;
  grid->columns = data + GRID_SIZE;
  grid->rows = data + 2 * GRID_SIZE;
  grid->blocks = data + 3 * GRID_SIZE;

  return grid;
}

void free_grid(grid_t *grid) {
  free(grid->data);
  free(grid);
}

static inline int assign_cell_value(cell_t *cells, int pos, int val, grid_t *grid) {
  cell_t *cell = cells + pos;

  if (cell->col[val - 1] || cell->row[val - 1] || cell->block[val - 1]) {
    return false;
  }

  cell->val = val;

  assign_cell_ptr(cell, val - 1);

  grid->data[pos] = cell;

  return true;
}

static inline void reset_cell_value(cell_t *cells, int pos, int val, grid_t *grid) {
  cell_t *cell = cells + pos;
  cell->val = 0;

  reset_cell_ptr(cell, val - 1);

  grid->data[pos] = NULL;
}

void init(cell_t *cells, int *array_grid, grid_t *grid) {
  int i, j, pos;
  cell_t *cell;

  for (i = 0; i < SIZE; i++) {
    for (j = 0; j < SIZE; j++) {
      pos = i * SIZE + j;
      int val = *(array_grid + pos);

      cell = cells + pos;
      cell->val = val;
      cell->row = &grid->rows[i * SIZE];
      cell->col = &grid->columns[j * SIZE];
      cell->block = &grid->blocks[((j / 3) + 3 * (i / 3)) * SIZE];

      if (val) {
        assign_cell_ptr(cell, val - 1);
      }

      grid->data[pos] = cell;
    }
  }
}

int solve(cell_t *cells, grid_t *grid, int depth, int current) {
  int i, j, success = true;

  // Looking for next empty cell
  for (i = current; i < GRID_SIZE; i++) {
    if ((cells + i)->val == 0) {
      break;
    }
  }

  if (i != GRID_SIZE) {
    j = 1;
    success = false;
    while (j < 10 && !success) {
      success = assign_cell_value(cells, i, j, grid);
      if (success) {
        success = solve(cells, grid, depth + 1, i);
        if (!success) {
          reset_cell_value(cells, i, j, grid);
        }
      }
      j++;
    }
  }

  if (depth == 0) {
    if (success) {
      printf("Solution found!\n");
    }
    else {
      printf("No solution found.\n");
    }
    print_grid(cells);
  }

  return success;
}

int main(void) {
  cell_t cells[GRID_SIZE];

  grid_t *grid = alloc_grid();

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

  init(cells, *initial_grid, grid);

  print_grid(*grid->data);

  solve(cells, grid, 0, 0);

  free_grid(grid);
  return 0;
}
