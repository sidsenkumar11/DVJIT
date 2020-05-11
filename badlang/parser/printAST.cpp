#include <iostream>
#include <string>
#include "ast.hpp"

extern void yyset_in(FILE *);
extern int yyparse();

using namespace std;

ASTNode* astRoot;

int printAST(char *fname) {

    FILE *fd;

    if (!(fd = fopen(fname, "r")))
    {
        cerr << "Could not open source file." << endl;
        return -1;
    }

    // generate AST from file
    yyset_in(fd);
    yyparse();
    fclose(fd);

    // visit AST
    Visitor *visitor = new Print();
    astRoot->accept(visitor);

    return 0;
}
