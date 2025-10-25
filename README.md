# Overview
This is a small interpreter-simulator for TOY ISA 14 architecture (made for studying purposes). DSL (that is actually just a micro-assembler for now) is written in Ruby for the ease of use.
# Build
```console
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++
cmake --build build
```
# Usage
Go to dsl directory, write some code in TOY ISA 14 assembly, assembly it with:
```console
ruby dsl_megawrapper.rb AssemblyFileName OutputFileName
```
Then, move Output File in root directory of the project and run:
```console
./Sim OutputFileName (Command line arguments for your program)
```
If you want to run example program (Fibonacci sequence calculation), you should run:
```console
./Sim OutputFileName N
```
(N is the number of element from sequence that is to be calculated)
That's it.
# Adjustments
For easier implementation of micro-assembler some adjustments were made in its syntax:
base(offset) -> base.(offset)
LabelName: -> label LabelName

Also since this ISA doesn't provide any methods for making any register non-zero, I had to hard code some needed numbers in memory. Data
section starts on address 0, so it is quite easy to use. Initial memory looks like this: {1, -1, CLI argument you passed}.
