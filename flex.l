%{
#include "shared_ast.h"        
#include "bison_ast.tab.h"     
#include <stdlib.h>
#include <string.h>
%}

%%
"method"            { return METHOD; }
"return"            { return RETURN; }
"paputexto"         { return TEXT; }
"texto"             { return TEXTO; }
"get"               { return GET; }
"si"                { return SI; }
"sino"              { return SINO; }
"mientras"          { return MIENTRAS; }
"dinoint"           { return NUM_TIPO; }
"float"             { return FLOAT_TIPO; }
";"                 { return PUNTOCOMA; }

"=="                { return IGUAL; }
"!="                { return DIFERENTE; }
"<="                { return MENORIGUAL; }
">="                { return MAYORIGUAL; }
"<"                 { return MENOR; }
">"                 { return MAYOR; }

"{"                 { return LLAVE_IZQ; }
"}"                 { return LLAVE_DER; }
"["                 { return '['; }
"]"                 { return ']'; }

[0-9]+              { yylval.numero = atoi(yytext); return NUM; }
[0-9]+\.[0-9]+      { yylval.flotante = atof(yytext); return FLOAT; }
\"[^"\n]*\"         { yylval.texto = strdup(yytext); return STRING; }
[a-zA-Z_][a-zA-Z0-9_]*  { yylval.id = strdup(yytext); return ID; }

"//".*              { }
[ \t\r\n]+         { }
.                   { return yytext[0]; }
%%

int yywrap() {
    return 1;
}