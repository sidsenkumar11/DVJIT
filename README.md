# DVJIT
CS 263 Runtime Systems Project


## Log of weekly progress:

### Week of April 20th

* Reviewed community & research literature, added to README


## Relevant Literature

* [Pointer Exploitation: Pointer Inference and JIT Spraying](http://www.semantiscope.com/research/BHDC2010/BHDC-2010-Paper.pdf)
    * BlackHat 2010 presentation
    * two key concepts
        * _Pointer Inference_ makes deductions about memory addresses
        * _JIT Spraying_: allocating a lot of heap space the attacker controls
    * Focuses on ActionScript
    * Pointer Inference
        * via Integer Sieve: Dictionary keys are iterated over in hash(object) order; insert a bunch of Integer keys (which are hashed by value) and an Object key (hashed by reference) to see where in the iteration the object lands.
* [security implications of jit compilation](https://wingolog.org/archives/2011/06/21/security-implications-of-jit-compilation)
    * mostly just echoes what we know to be true: JITs aren't inherently insecure -- bad code is insecure
