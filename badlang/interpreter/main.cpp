#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <vector>
#include "codegenerator.hpp"
#include "helpers.hpp"
#include "ast.hpp"
#include "asmjit/asmjit.h"

using namespace std;
using namespace asmjit;

extern void yyset_in(FILE *);
extern int yyparse();
ASTNode* astRoot;

int main(int argc, char **argv)
{
    FILE *fd;

    if (argc < 2)
    {
        cerr << "Please supply file name" << endl;
        return -1;
    }

    if (argc > 2)
    {
        cerr << "Too many arguments" << endl;
        return -1;
    }

    if (!(fd = fopen(argv[1], "r")))
    {
        cerr << "Could not open source file." << endl;
        return -1;
    }

    if (is_debug())
    {
        cout << "Reading file: " << argv[1] << endl << endl;
    }

    FileLogger logger(stdout);

    // generate AST from source file
    yyset_in(fd);
    yyparse();
    fclose(fd);

    // Create a runtime and a code holder for asmjit
    asmjit::JitRuntime *jit = new asmjit::JitRuntime();
    CodeHolder code;
    code.init(jit->codeInfo());
    if (is_debug()) code.setLogger(&logger);

    // emit code to memory
    CodeGenerator *visitor = new CodeGenerator(code);
    astRoot->accept(visitor);
    jit_ptr fn;
    Error err = jit->add(&fn, &code);
    if (err)
    {
        std::cerr << "asmjit error" << endl;
        return 1;
    }

    // execute
    if (is_debug()) cout << endl;
    fn();

    // cleanup
    jit->release(fn);
    delete jit;
    return 0;
}
