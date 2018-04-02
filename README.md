# cplot

## Building
`make`

## Using

If neither `--expression` or `--file` are specified, `cplot` will read and plot
points from standard input. Points are expected in the format `x y`, with any
amount of whitespace in between `x` and `y`. Expressions are functions of `x`.
Several basic functions are also available for use in expressions, namely `sin`,
`cos`, `tan`, `arcsin`, `arccos`, and `ln`.   

`cplot` is highly configurable. The number of ticks an each axis can be
specified using `--x-ticks` and `--y-ticks`. The ranges can be specified using
`--x-min`, `--x-max`, `--y-min`, and `--y-max`. The colors of the axes, x-tick
labels, y-tick labels, and marks can also be configured through command line
options. For the full summary of the available command-line options, see
`./cplot --help`.


## Examples

Plotting `sin(x)`:
`./cplot --expression "sin(x)" --x-min=-6.28 --x-max=6.28 --y-min=-1 --y-max=1`

Plotting parabolic functions:
`./cplot --expression "x^2 + 2*x + 1`

Plotting points from a file:
`./cplot --file my-data.dat --x-min=-1000 --x-max=1000 --y-min=-40 --y-max=40
--rows=70`

Plotting points from another program:
`point_generator | ./cplot`
