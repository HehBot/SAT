# Implementing SAT solvers

## Build

    $ make -j$(nproc)

## Run

SAT solver

    $ bin/sat dpll <SAT file>
    $ bin/sat cdcl <SAT file> [SAT file for learned clauses]

Sudoku solver

    $ bin/sudoku dpll <sudoku file>
    $ bin/sudoku cdcl <sudoku file> [SAT file for learned clauses]

To see algorithm runs, compile with `-DDEBUG` flag (WARNING large amount of text on stdout).

## Acknowledgement
Tests in
 - [test/sudoku](test/sudoku) taken from [rkkautsar/sudoku-solver](https://github.com/rkkautsar/sudoku-solver/tree/main/sudoku)
 - [test/sat](test/sat) taken from [sgomber/CDCL-SAT](https://github.com/sgomber/CDCL-SAT/tree/master/test)
