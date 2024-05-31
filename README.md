# 3to4++

A virtual simulator for the [physical 3x3x3x3 design](https://hypercubing.xyz/puzzles/physical/3x3x3x3/) by Grant S.
Written with [GLFW](https://www.glfw.org/), [FreeType](https://freetype.org/), [OpenGL](https://www.opengl.org/) and [C++](https://isocpp.org/).
Credit to [Akkei's physical 3^4 program](https://hypercubing.xyz/software/#other) for inspiration and design.

## Compiling

On Linux, install a C++ compiler, `make`, the required GLFW, freetype and OpenGL dev packages.

Then run
```
$ make shared
$ cd 3to4pp/
$ ./3to4++
```
to compile and run. If `make shared` fails, try `make build`.

On Windows, install [MinGW](https://www.mingw-w64.org/) or [Cygwin](https://www.cygwin.com/index.html) for a GNU C++ compiler.

Next, go to https://www.glfw.org/download.html, select Windows binaries for the correct architecture. In the zip, copy all files under `lib-mingw-w64/` into this repo's `lib/` folder, and copy `include/GLFW/` into this repo's `include/` folder.

Then go to https://freetype.org/download.html and build using `./configure` and `make`. Copy the header files and generated `libfreetype.a` file into `include/` and `lib/` respectively.

Finally run
```
> make build
> cd 3to4pp
> 3to4++.exe
```
