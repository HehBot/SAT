# Implementing SAT solvers

## Build

    $ make -j$(nproc)

## Run

    $ bin/main (sat|sudoku) (dpll|cdcl) <SAT file|sudoku file>

To see algorithm runs (WARNING large amount of text on stdout),

    $ bin/main.DEBUG (sat|sudoku) (dpll|cdcl) <SAT file|sudoku file>

## Acknowledgement
Tests in
 - [test/sudoku](test/sudoku) taken from [rkkautsar/sudoku-solver](https://github.com/rkkautsar/sudoku-solver/tree/main/sudoku)
 - [test/sat](test/sat) taken from [sgomber/CDCL-SAT](https://github.com/sgomber/CDCL-SAT/tree/master/test)
