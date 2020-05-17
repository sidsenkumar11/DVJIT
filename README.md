# DVJIT
CS 263 Runtime Systems Project


## Log of weekly progress:

### Week of April 20th

* Reviewed community & research literature, added to README


### Week of April 27

* Started language specs
* Created 2 example exploits (pseudoscope, see BADLANG-drafting.md)
* More paper reading & discussion


### Week of May 4

* created src/ directory with asmjit, basic makefile
* add some debugging features to jit emitted code for ease of use
* add examples/

### Week of May 11
* refactored code and rewrote Makefile
* created scanner and parser infrastructure for badlang
* working "fizzbuzz" example

### Week of May 18
* inserted "Dictionary" type into badlang, supporting "get", "set", and iteration through "forkey"
* inserted "LeakJit" and "Jump" instructions
* wrote several test programs, including one that leaks a pointer
* made several quality-of-life improvements to the interpreter (ie the interpreter prints useful error messages instead of segfaulting)

## Relevant Literature

* [Pointer Exploitation: Pointer Inference and JIT Spraying](http://www.semantiscope.com/research/BHDC2010/BHDC-2010-Paper.pdf)
    * BlackHat 2010 presentation
    * two key concepts
        * _Pointer Inference_ makes deductions about memory addresses
        * _JIT Spraying_: allocating a lot of heap space the attacker controls
    * Focuses on ActionScript
    * Pointer Inference
        * via Integer Sieve: Dictionary keys are iterated over in hash(object) order; insert a bunch of Integer keys (which are hashed by value) and an Object key (hashed by reference) to see where in the iteration the object lands.
* [Attacking Clientside JIT Compilers](https://media.blackhat.com/bh-us-11/Rohlf/BH_US_11_RohlfIvnitskiy_Attacking_Client_Side_JIT_Compilers_WP.pdf)
    * BlackHat 2011 presentation
    * Roughly two categories of JIT
        * "Tracing" JIT - only emit native code for "hot" paths
        * "Method" JIT - emits native code for each method invocation as they occur
    * Surveyed JIT implementations commonly include:
        * memory manager (since malloc won't work for RWX mem)
        * IR / translation layer
        * assembler
    * Case study: Firefox
    * front-end: SpiderMonkey (parsing, IR)
    * Contains two JIT engines: TraceMonkey, JaegerMonkey
        * Former is a Trace JIT, latter Method JIT
        * Back-ends are named Nitro and NanoJIT, respectively
    * interesting optimization: polymorphic inline caching
        * inside a container, caches method invocations for types
    * LLVM Internals
        * LLVM IR is a "graph of C++ objects"
        * packages code into Modules
        * LLVM JIT only emits code entry -- lazy-compiles other functions as they are needed via compilation callback
    * Interesting issue: JIT emitted code can be un-protected by via system call, but it's SLOW and impractical if more performance tuning is needed
    * Selected Vulnerabilities
        * errors in emitted code, subtracting from $esp instead of from [$esp]
        * crafted predictable JIT output
    * Selected Mitigations
        * random NOP insertion
        * protect RWX memory w/ nearby padding
        * "Constant Folding" break user-supplied input into smaller 2-byte chunks so they can't fit an entire address
        * "Constant Blinding" XOR user-supplied input w/ secret key, and re-XOR during execution. Prevents predictable jit output.
* [security implications of jit compilation](https://wingolog.org/archives/2011/06/21/security-implications-of-jit-compilation)
    * mostly just echoes what we know to be true: JITs aren't inherently insecure -- bad code is insecure
