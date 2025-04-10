# tipc-passes
A set of simple LLVM passes for analyzing LLVM bitcode generated by [`tipc`](https://github.com/matthewbdwyer/tipc/)

## TIP Language and Compiler

TIP is a "Tiny Imperative Programming" language developed by Anders Moeller and Michael I. Schwartzbach for the [Static Program Analysis](https://cs.au.dk/~amoeller/spa/) lecture notes that they developed for graduate instruction at Aarhus University.

Accompanying those notes is a [Scala implementation](https://github.com/cs-au-dk/TIP/) that provides a number of static analysis implementations and interpreter-based evaluators.

This project processes the output of `tipc` [a compiler from TIP programs to LLVM bitcode](https://github.com/matthewbdwyer/tipc), but it might work on LLVM IR generated by other compilers.

## The Passes
There are four passes in this project:

  1. `funvisitpass` : the simplest imaginable `FunctionPass` 
  2. `printinstpass` : a function pass that identifies and prints a subset of instructions that are relevant for TIP programs
  3. `instcountpass` : a function pass that prints the number instructions for each function
  4. `userspass` : an extension of `printinstpass` that prints the users of an instruction  
  5. `intervalrangepass` : a skeletal interval range analysis pass

The first three are intended just to give a basic idea of how to write little passes so that you can experiment with getting up the LLVM pass writing learning curve.

The third serves as a basis for getting into pass writing a bit more deeply.
It is an intentionally partial implementation of an interval range analysis for integer expressions (which are encoded as bitcode variables).
This serves to illustrate:

  1. how SSA can be exploited to simplify the management of analysis state;
  2. how data flow analyses can exploit the use-def relationships that are explicit in SSA; and
  3. how abstract domains and associated transfer functions can be encoded to support an analysis.

The pass is not intended to reflect the architecture, coding style, or efficiency one finds in LLVM analysis passes.
Rather the intention is to very clearly express how the analysis functions as relates to the above issues.

## Dependencies
These passes were developed and tested on Mac OS X and Ubuntu 20.04 LTS using LLVM 14;  older versions of LLVM can be used with some slight modifications.  To install the necessary packages on ubuntu run:

`apt install libllvm-14-ocaml-dev libllvm14 llvm-14 llvm-14-dev llvm-14-doc llvm-14-examples llvm-14-runtime llvm-14-tools libclang-common-14-dev`

To install them on a mac run:

`brew install llvm@14`

Once you install you may need to set up your search path so that the LLVM tools are visible.  Note that as long as `llvm-config` is visible then the following build process will find the include files and libraries.

## Building the passes

This project uses CMake to manage the build process.  There are a set of `CMakeLists.txt` files that orchestrate the build process.  

You need to create a `build` directory within which you will build the passes.  To get started you should create that, empty, build directory if it doesn't exist.  Follow these steps:
  1. `mkdir build`
  2. `cd build`
  3. `cmake ..`
  4. `make`

During development you need only run steps 1 and 2 a single time, unless you modify the CmakeLists.txt file.  Just run make in the build directory to rebuild after making changes to your tool source.  If you run into trouble with `cmake` then you can delete the file `build/CMakeCache.txt` and rerun the `cmake` command.

This will create a set a subdirectory for each pass in the `build` directory.

### A Note to Apple Silicon Users
If you are building the passes on Apple silicon hardware it is possible that
[CMake does not properly identify][1] the target architecture as `arm64`. This
will result in your passes being compiled for `x86_64`. Those build artifacts
will not be linkable with the `arm` flavored `llvm` libraries Homebrew
installed. To [force `CMake`][2] into configuring the build properly, tweak
your build steps slightly.

```bash
mkdir build
cd build
cmake -DCMAKE_APPLE_SILICON_PROCESSOR=arm64 ..
make
```

## Running a pass
These passes all emit output on the standard error stream.

To run a pass you use LLVM's `opt` tool.  You run a pass using the following command:

`opt -enable-new-pm=0 -load <dylibpath>/passfile.suffix --passname < file.bc >/dev/null`

where `passfile.suffix` is the name of the shared object (dynamic) library for the pass and `passname` is the name used to register the pass (see the declaration of the form `static RegisterPass<...> X(...)` in the source of the pass, or just look below).  On a Mac the `suffix` is `dylib` and on linux it is `so`.

`<dylibpath>` depends on which pass you are running.  In this project they are built in `~/tipc-passes/build/src/PN/passfile.suffix` where `PN` is the name of the pass directory, e.g., `funvisitpass`

Since `opt` writes the transformed bitcode file to output you need to either pipe the result to `/dev/null`, as above, or use the `-o <filename>` option to redirect it to a file.

Note that these projects use the legacy pass manager, so we explicitly disable the new pass manager with the `-enable-new-pm=0` option.

There are five passes in this project:
  1. `funvisitpass` : the simplest imaginable `FunctionPass`; `passname` is `fvpass` 
  2. `printinstpass` : a function pass that identifies a subset of instructions that are relevant for TIP programs; `passname` is `pipass`
  3. `instcountpass` : a function pass that prints the number instructions for each function; `passname` is `icpass`
  4. `userspass` : an extension of `printinstpass` that prints the users of an instruction; `passname` is `userspass`
  5. `intervalrangepass` : a skeletal interval range analysis pass that depends on the program being converted to SSA form, so you need to add the `-mem2reg` option to the `opt` command line; `passname` is `irpass`


## Limitations
These are all intra-procedural `FunctionPass`es and, as such, are limited in their precision.  There are many other limitations as well, e.g., 
  1. only a limited set of instructions are supported 
  2. only a limited set of binary opcodes is supported
  3. the interval computations are unsound with respect to computer arithmetic
  4. the interval computations for some operators are very imprecise
  5. there is no widening implemented for intervals

These are intentional as the passes are provided just as a context for learning about LLVM, i.e., by students.  If you want to understand what real LLVM passes look like, then have a look at LLVM source code.

## Tests

The directory `src/intervalrangepass/test` contains a set of tests `interval*.tip` which can be run using the script `runirpass.sh`.  This script requires that you have installed the [tipc compiler](https://github.com/matthewbdwyer/tipc) in your home directory (i.e., `~`).  

The script takes the base name of the TIP file as input and outputs a file, `interval*.irpass`, that record the results of running the pass.  You can compare the output of your pass to the expected output in `interval*.expected`.

The `tipc` compiler is run with the `-do` option which disables optimizations.  Removing that flag may produce bitcode operations that are not supported by the interval analysis, in which case it will raise an LLVM unreachable exception, e.g., "Unsupported BinaryOperator".

The source files are annotated with expected results, but it can be tricky to compare the source level view of the analysis results with the view in terms of the bitcode file.  This is due in part to the lowering of the program to bitcode, which makes temporary computations explicit and thereby tracked by the analysis, and by the application of optimizations associated with the `-mem2reg` pass.  

There is a debug flag that you can add to the command line, `-intervalrange-debug`, which prints information like the updating of interval values, adding user instructions to the worklist, etc.  For example, on a Mac you can run:

`opt -load ~/tipc-passes/build/src/intervalrangepass/irpass.dylib -mem2reg -irpass -intervalrange-debug >/dev/null <interval1.tip.bc`

Note that due to the limitations stated above these expected results are not computed correctly.  Also take care that if you have a loop in your example, like `interval5.tip`, the `irpass` may diverge.  You can fix this by implementing widening.

## Documentation

Perhaps the most relevant documentation for this project is the tutorial [Writing an LLVM Pass](http://llvm.org/docs/WritingAnLLVMPass.html).

There is lots of great general advice about using LLVM available:
  * https://www.cs.cornell.edu/~asampson/blog/llvm.html
  * the [LLVM Programmer's Manual](http://llvm.org/docs/ProgrammersManual.html) is a key resource
  * someone once told me to just use a search engine to find the LLVM APIs and its a standard use case for me, e.g., I don't remember where the docs are I just search for `llvm irbuilder`



[1]: https://gitlab.kitware.com/cmake/cmake/-/issues/20989
[2]: https://cmake.org/cmake/help/latest/variable/CMAKE_APPLE_SILICON_PROCESSOR.html#variable:CMAKE_APPLE_SILICON_PROCESSOR
