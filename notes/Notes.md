
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
* [Security Implications of JIT Compilation](https://wingolog.org/archives/2011/06/21/security-implications-of-jit-compilation)
    * Mostly just echoes what we know to be true: JITs aren't inherently insecure -- bad code is insecure
