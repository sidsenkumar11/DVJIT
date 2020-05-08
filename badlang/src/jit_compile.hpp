#include <string>
#include <iostream>

#include <asmjit/asmjit.h>

typedef int (*jit_ptr)(std::string);


typedef struct {
    jit_ptr ptr;
    asmjit::JitRuntime *runtime;
} jit_result;

jit_result *jit_compile(std::stringstream &program);

void jit_release(jit_result *jit);
