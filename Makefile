sudoku:
	gcc main.c -Wpedantic -o sudoku

sudoku-verbose-mode:
	gcc main.c -Wpedantic -DVERBOSE -o sudoku

sudoku-diagonal-mode:
	gcc main.c -Wpedantic -DDIAGONALS -o sudoku
