#!/bin/bash
echo 'Generando archivos con bison y flex...'
bison -d bison_ast.y
flex flex.l
echo 'Compilando el proyecto con AST...'
gcc -o compilador main.c bison_ast.tab.c lex.yy.c ast.c interpreter.c scope.c -lfl

if [ $? -eq 0 ]; then
    echo 'Compilación completada!'
    if [ "$1" = "compile" ]; then
        echo 'Solo compilación solicitada. Terminando.'
    else
        echo 'Probando el compilador con declaraciones básicas...'
        echo 'num a = 5;' | ./compilador
        echo 'Test completado!'
    fi
else
    echo 'Error en la compilación!'
    exit 1
fi
