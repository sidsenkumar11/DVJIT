# DRAFT - high-level design of BADLANG

## Overall Goals (and Non-Goals)

The language BADLANG seeks to provide a minimal environment for investigating JIT hacking. Some constructs are built-in to provide for (minimal) computation, whereas others are purely for making JIT hacking easier.

BADLANG does _not_ attempt to:

    1. provide a mechanism for memory management
    2. expose underlying OS functionality in any useful capacity
    3. run in a performant manner


## High-level Overview

BADLANG is an language with only three primitive types: `int`, `string`, and `dict`. A program file `example.bad` is invoked with the command `badlang example.bad`. A program's entry point is interpreted top-to-bottom, line-by-line. A program consists of a series of commands (detailed below). These commands operate on 128 virtual registers, which can be thought of as a local variable array.


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
# print the result, 30
PRINT 2;
```

### Conditional Statement

```
LOADI 0 1;
LOADI 1 3;
LOADS 2 "hello";
# Tests register 0 and 1 for equality
IFEQ 0 1;
    # this will not run
    LOADS 2 "wow";
ENDIF;
PRINT 2;
```

### Loop Statement

```
# r0 = 0; r1 = 10;
LOADI 0 0;
LOADI 1 10;

# while (r0 < r1)
WHILELT 0 1;

    PRINT 0;

    # r0 += 1
    LOADI 2 1;
    ADD 0 0 2;

ENDWHILE;
```

### Dictionary Access

Please view `examples/squares.bad` for an in-depth overview of the different Dictionary operations.

### [Backdoor] Jump to address

```
LOADI 0 0xDEADBEEF;
# Start executing instructions at address 0xDEADBEEF
JUMPABS 0;
```

### [Backdoor] Leak an address

```
# Leakjit leaks the address of the next instruction
# This code continuously loops on JUMPABS 0;
LEAKJIT 0;
JUMPABS 0;
```
