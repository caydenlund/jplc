# The JPL Compiler

This program implements a compiler of the [JPL language specification](https://github.com/utah-cs4470-sp23/class).
It's broken up into modules, which are detailed below.

## 1: Examples

The programs in the `examples/` folder are JPL programs.
Each program contains a single function, as follows:

- `blur(pict) : pict` blurs the given image.
- `circle(float, float) : pict` draws a white circle in the middle of a black 800x600 image.
- `gradient(int, int, int, int) : {float,float,float,float}` is an instructor-provided example that creates a multi-colored gradient.
- `invert(pict) : pict` inverts colors in the given image.
- `red(int, int) : pict` creates a red image of the given size.
- `sepia(pict) : pict` converts the given image to sepia-tone.
- `subtract(float, float) : float` returns the difference of the arguments.
