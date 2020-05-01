# DRAFT - high-level design of BADLANG

## Overall Goals (and Non-Goals)

The language BADLANG seeks to provide a minimal environment for investigating JIT hacking. Some constructs are built-in to provide for (minimal) computation, whereas others are purely for making JIT hacking easier.

BADLANG does _not_ attempt to:

    1. provide a mechanism for memory management
    2. expose underlying OS functionality in any useful capacity
    3. run in a performant manner


## High-level Overview

BADLANG is an language with only three primitive types: `int`, `string`, and `dict`. A program file `example.bad` is invoked with the command `bad-lang example.bad`. A program's entry point is interpreted top-to-bottom, line-by-line. A program consists of a series of commands (detailed below). These commands operate upon 8 local variable array entries.


## Guiding Examples

The following section will provide some small examples.


### Hello, world

```
# hello-world.bad
LOADS 0 "hello, world";
PRINTS 0;
```

### Adding two numbers

```
# put the value 20 in register 0
LOADI 0 20;
# put the value 10 in register 1
LOADI 1 10;
# Add registers 0 and 1, and store the result in register 2
ADD 2 0 1;
# prints 30
PRINTI 2;
```

### Conditional Statement

```
LOADI 0 1;
LOADI 1 3;
LOADS 2 "hello";
# Tests register 0 and 1 for equality
TEST 0 1;
IFEQ;
    # this will not run
    LOADS 2 "wow";
ENDIF;
PRINTS 2;
```

### Loop Statement

```
# put the value 0 in register 0
LOADI 0 0;
# compare register 0 to itself
TEST 0 0;
WHILEEQ;
    # add 3 to register 0
    LOADI 1 3;
    ADD 0 1 0;
    # compare register 0 to 9
    LOADI 2 9;
    TEST 0 2;
ENDWHILE;
```

### [Backdoor] Force JIT compilation

```
LOADI 0 1;
BEGINJIT;
    LOADS 1 "foobar";
    PRINTS 1;
ENDJIT;
```

### [Backdoor] Jump to address

```
LOADI 0 0xDEADBEEF;
# Start executing instructions at address 0xDEADBEEF
JUMP 0;
```

# Unsorted Notes from discussion May 1

Question: where is the mmap()’d jit code relative to glibc malloc heap

Exploit #1

<jit spray>
Leak current ip
Subtract (some constant) from leak
Jump to jit spray
<shell>

Exploit #2

// dictionary is a TreeDict or something
    T
          I

Allocate a dictionary, d
Insert address inference target object AS A KEY (string?)
// binary search…
Print target addr

Our jit engine library: https://github.com/asmjit/asmjit