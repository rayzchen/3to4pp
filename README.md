# 3to4++ v1.1

A virtual simulator for the [physical 3x3x3x3 design](https://hypercubing.xyz/puzzles/physical/3x3x3x3/) by Grant S.
Written with [GLFW](https://www.glfw.org/), [Dear ImGui](https://github.com/ocornut/imgui), [OpenGL](https://www.opengl.org/) and [C++](https://isocpp.org/).
Credit to [Akkei's physical 3^4 program](https://hypercubing.xyz/software/#other) for inspiration and design.

## Download

For Windows only: https://github.com/rayzchen/3to4pp/releases/latest

## Compiling

### Linux

On Linux, install a C++ compiler, `make`, the required GLFW and OpenGL dev packages.

Then run
```
$ make shared
$ cd 3to4pp/
$ ./3to4++
```
to compile and run. If `make shared` fails, try `make build`.

### Windows

On Windows, install [MinGW](https://www.mingw-w64.org/) and [Git for Windows](https://gitforwindows.org/), or [Cygwin](https://www.cygwin.com/index.html). This will setup a GNU C++ compiler and `make` system.

Next, go to https://www.glfw.org/download.html, select Windows binaries for the correct architecture. In the zip, copy all files under `lib-mingw-w64/` into this repo's `lib/` folder, and copy `include/GLFW/` into this repo's `include/` folder.

Finally run
```
> make build
> cd 3to4pp
> 3to4++.exe
```
