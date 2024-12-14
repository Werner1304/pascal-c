# PL/0 Interpreter in C

## Sources

+ [Wikipedia PL/0](https://en.wikipedia.org/wiki/PL/0)

## Requirements

*Only tested on Windows with Clang so far*

+ python
+ clang

## How To Build

```sh

# to build
python ./build.py build

# build and run
python ./build.py build run

```

## Debugging with LLDB

On Windows:
+ set environment variable `LLDB_USE_NATIVE_PDB_READER = 1`

