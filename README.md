# DVJIT

[![Build Status](https://travis-ci.com/sidsenkumar11/DVJIT.svg?branch=master)](https://travis-ci.com/sidsenkumar11/DVJIT)

CS 263 Runtime Systems Project

## Requirements

Everything has been tested and is known to work on Ubuntu 18.04 on an x86_64 processor.

```bash
$ sudo apt-get -y install flex bison make g++
$ sudo apt-get -y install python3-pip python3-setuptools
$ sudo -H python3 -m pip install wheel
$ sudo -H python3 -m pip install pwntools
```

## Build

```bash
$ cd badlang
$ make clean
$ make

# Run unit-tests on every badlang file in the examples directory
$ make test
```

## Execute

```bash
# Hello World
$ ./bin/badlang ../examples/hello_world.bad

# Shell Exploit
$ ./bin/badlang ../exploits/getshell/exploit.bad
```

## Weekly Progress Log


### Week of April 20th

* Reviewed community & research literature, added to README

### Week of April 27

* Started language specs
* Created 2 example exploits (pseudoscope, see BADLANG-drafting.md)
* More paper reading & discussion

### Week of May 4

* Created src/ directory with asmjit, basic makefile
* Add some debugging features to jit emitted code for ease of use
* Add examples/

### Week of May 11
* Refactored code and rewrote Makefile
* Created scanner and parser infrastructure for badlang
* Working "fizzbuzz" example

### Week of May 18
* Inserted "Dictionary" type into badlang, supporting "get", "set", and iteration through "forkey"
* Inserted "LeakJit" and "Jump" instructions
* Wrote several test programs, including one that leaks a pointer
* Made several quality-of-life improvements to the interpreter (ie the interpreter prints useful error messages instead of segfaulting)

### Week of May 25
* Made exploit script reliable by using internal AsmJit allocator instead of custom allocator.
* Began project writeup.
* Began project presentation.
