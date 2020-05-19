%option yylineno
%pointer

%{
    #include <stdlib.h>
    #include <errno.h>
    #include <limits.h>
    #include "ast.hpp"
    #include "parser.hpp"
    void yyerror(const char *);
%}

/* Write any definitions here. You can find information on
            definitions here:
            ftp://ftp.gnu.org/old-gnu/Manuals/flex-2.5.4/html_mono/flex.html#SEC10 */

/* If you are going to use start conditions, write them here.
            You can find more information on start conditions here:
            ftp://ftp.gnu.org/old-gnu/Manuals/flex-2.5.4/html_mono/flex.html#SEC11
            You will probably want to use exclusive conditions rather
            than inclusive conditions (use the %x instead of the %s). */

%x COMMENT

%%

  /* Write your Flex token rules here */
"PRINT"           { return T_PRINT; }
"LOADI"           { return T_LOADI; }
"LOADS"           { return T_LOADS; }
"MOVE"            { return T_MOVE; }

"ADD"             { return T_ADD; }
"MUL"             { return T_MUL; }
"DIV"             { return T_DIV; }

"IFEQ"            { return T_IFEQ; }
"IFNEQ"           { return T_IFNEQ; }
"ELSE"            { return T_ELSE; }
"ENDIF"           { return T_ENDIF; }
"WHILELT"         { return T_WHILELT; }
"ENDWHILE"        { return T_ENDWHILE; }

"JUMP"            { return T_JUMP; }
"LEAKJIT"         { return T_LEAKJIT; }
"DICTINIT"        { return T_DICTINIT; }
"SETDICT"         { return T_SETDICT; }
"GETDICT"         { return T_GETDICT; }
"FORKEY"          { return T_FORKEY; }
"ENDFORKEY"       { return T_ENDFORKEY; }

  /* Match ';' and ',' exactly */
[,\;]             { return *yytext; }

  /* Match strings */
\"([^\\\"]|\\.)*\"  {
    char *stripped = strdup(yytext+1);
    stripped[strlen(stripped)-1] = '\0';
    yylval.string_ptr = new StringNode(stripped);
    return T_STRING;
  }


0|([1-9][0-9]*)   {
                    long long value = strtoll(yytext, NULL, 0);
                    if (errno != 0 || value > LONG_MAX || value < LONG_MIN)
                      yyerror("Integer out of range");

                    yylval.integer_ptr = new IntegerNode(value);
                    return T_INTEGER;
                  }


"#"               { BEGIN(COMMENT); }
<COMMENT>.        ;
<COMMENT>\n       { BEGIN(INITIAL); }

\n                ;
[[:space:]]       ;
.                 { yyerror("invalid character"); }

%%

int yywrap(void) {
  return 1;
}
