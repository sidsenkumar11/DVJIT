%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <iostream>

    #include "ast.hpp"

    #define YYDEBUG 1
    #define YYINITDEPTH 10000
    int yylex(void);
    void yyerror(const char *);

    extern ASTNode* astRoot;
%}

%error-verbose

%token T_PRINTS
%token T_LOADI
%token T_LOADS
%token T_ADD
%token T_MUL
%token T_DIV
%token T_TEST
%token T_IFEQ
%token T_IFNEQ
%token T_ELSE
%token T_ENDIF
%token T_WHILELT
%token T_ENDWHILE
%token T_INTEGER
%token T_STRING

%token T_JUMP
%token T_LEAKJIT
%token T_DICTINIT
%token T_SETDICT
%token T_GETDICT
%token T_FORKEY
%token T_ENDFORKEY

/* Specify types for all nonterminals and necessary terminals here */

%type <program_ptr> Start
%type <statement_list_ptr> Statements /* Block */
%type <statement_ptr> Statement
%type <ifelse_ptr> IfElse
%type <while_ptr> WhileLoop
%type <forkey_ptr> ForKey

%type <loadi_ptr> Loadi
%type <loads_ptr> Loads
%type <add_ptr> Add
%type <mul_ptr> Mul
%type <div_ptr> Div
%type <test_ptr> Test
%type <prints_ptr> Prints
%type <jump_ptr> Jump
%type <leakjit_ptr> LeakJit
%type <dictinit_ptr> DictInit
%type <setdict_ptr> SetDict
%type <getdict_ptr> GetDict

/* Types for literals */
%type <integer_ptr> T_INTEGER
%type <string_ptr> T_STRING

/* Add this rule if you want to enforce non-empty blocks.

  Block           : Statement Statements { $$ = $2; $$->push_front($1); }
                  ;

*/
%%

/* Write your Bison grammar specification here */

Start           : Statements { $$ = new ProgramNode($1); astRoot = $$; }
                ;

Statements      : Statement Statements { $$ = $2; $$->push_front($1); }
                | %empty               { $$ = new std::list<StatementNode*>(); }
                ;

Statement       : Prints ';'         { $$ = $1; }
                | Loadi ';'          { $$ = $1; }
                | Loads ';'          { $$ = $1; }
                | Add ';'            { $$ = $1; }
                | Mul ';'            { $$ = $1; }
                | Div ';'            { $$ = $1; }
                | Test ';'           { $$ = $1; }
                | Jump ';'           { $$ = $1; }
                | LeakJit ';'        { $$ = $1; }
                | DictInit ';'       { $$ = $1; }
                | GetDict ';'        { $$ = $1; }
                | SetDict ';'        { $$ = $1; }
                | IfElse             { $$ = $1; }
                | WhileLoop          { $$ = $1; }
                | ForKey             { $$ = $1; }
                ;

Loadi           : T_LOADI T_INTEGER T_INTEGER { $$ = new LoadiNode($2, $3); }
                ;

Loads           : T_LOADS T_INTEGER T_STRING { $$ = new LoadsNode($2, $3); }
                ;

Add             : T_ADD T_INTEGER T_INTEGER T_INTEGER { $$ = new AddNode($2, $3, $4); }
                ;

Mul             : T_MUL T_INTEGER T_INTEGER T_INTEGER { $$ = new MulNode($2, $3, $4); }
                ;

Div             : T_DIV T_INTEGER T_INTEGER T_INTEGER { $$ = new DivNode($2, $3, $4); }
                ;

Test            : T_TEST T_INTEGER T_INTEGER { $$ = new TestNode($2, $3); }
                ;

Prints          : T_PRINTS T_INTEGER { $$ = new PrintsNode($2); }
                ;

Jump            : T_JUMP T_INTEGER { $$ = new JumpNode($2); }
                ;

LeakJit         : T_LEAKJIT T_INTEGER { $$ = new LeakJitNode($2); }
                ;

DictInit        : T_DICTINIT T_INTEGER { $$ = new DictInitNode($2); }
                ;

GetDict         : T_GETDICT T_INTEGER T_INTEGER T_INTEGER { $$ = new GetDictNode($2, $3, $4); }
                ;

SetDict         : T_SETDICT T_INTEGER T_INTEGER T_INTEGER { $$ = new SetDictNode($2, $3, $4); }
                ;

IfElse          : T_IFEQ Statements T_ENDIF { $$ = new IfElseNode($2, new std::list<StatementNode*>(), new IntegerNode(0)); }
                | T_IFEQ Statements T_ELSE Statements T_ENDIF { $$ = new IfElseNode($2, $4, new IntegerNode(0)); }
                | T_IFNEQ Statements T_ENDIF { $$ = new IfElseNode($2, new std::list<StatementNode*>(), new IntegerNode(1)); }
                | T_IFNEQ Statements T_ELSE Statements T_ENDIF { $$ = new IfElseNode($2, $4, new IntegerNode(1)); }
                ;

WhileLoop       : T_WHILELT Statements T_ENDWHILE { $$ = new WhileNode($2); }
                ;

ForKey          : T_FORKEY T_INTEGER T_INTEGER Statements T_ENDFORKEY { $$ = new ForKeyNode($2, $3, $4); }
                ;

%%

extern int yylineno;

void yyerror(const char *s) {
  fprintf(stderr, "%s at line %d\n", s, yylineno);
  exit(0);
}
