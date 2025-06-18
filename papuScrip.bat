@echo off
echo "Starting Bison and Flex compilation..."
bison -d bison_ast.y
echo "Bison compilation complete."
pause
flex flex.l
echo "Flex compilation complete."
pause
gcc -o compilador main.c bison_ast.tab.c lex.yy.c ast.c interpreter.c scope.c -lfl
echo "GCC compilation complete."
pause
./compilador < test_completo.txt
echo "Running Compilado.exe with Prueba1.txt..."
pause
echo "Execution of Compilado.exe above Prueba 1 complete. Now deleting temporary files..."
del lex.yy.c
del bison_ast.tab.c
del bison_ast.tab.h
del compilador
echo "Temporary files deleted."
pause