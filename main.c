#include <stdio.h>
#include "shared_ast.h"

int yyparse();

int main() {
    printf("Iniciando el compilador...\n");
    return yyparse();
}

