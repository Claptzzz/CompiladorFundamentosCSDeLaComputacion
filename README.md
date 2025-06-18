# Compilador y Analizador Sintactico VeaPapu

**Profesor**: Jose Luis Veas.

**Integrantes**: -Nicolás Rojas.
                 -Manuel Jerez.

## Requisitos Previos
- **Docker Desktop** instalado y ejecutándose

## Descripción

En este laboratorio hemos buscado implementar de manera funcional un compilador, utilizando herramientas tales como Flex (para el análisis léxico), Bison (para el análisis sintáctico), Docker para poder implementar estas dependencias en un entorno externo linux, y C para la generacion de codigo.

## Implementación

Hemos decidido utilizar un scope estatico/léxico para manejar de manera correcta las variables de los bloques anidados.

Este lenguaje soporta variables de tipo:
* `dinoint` ---> int
* `paputexto` ---> String
* `float` ---> float

### Clases utilizadas y su proposito:

**main.c**: Es el archivo inicial que inicial el compilador y llama a yyparse() la cual es generada por bison anteriormente.

**shared.h**: Archivo que contiene las definiciones compartidas que definen estructura de datos fundamentales.

**flex.l**: Es el analizador léxico que define los tokens y patrones a reconocer, se encarga de convertir el codigo fuente en tokens para bison.

**bison.y**: Analizador sintáctico encargado de definir la gramática del lenguaje, este se encraga de multiples tareas tales como la declaracion de las variables, operaciones aritmeticas, controladores (si, sino, mientras), el manejo y uso del score, y los metodos.

**ast.c**: Se encarga de las funciones y la creacion del arbol AST junto con shared_ast.h e interpreter.c

**docker-compose.yml**: Se encarga de configurar el docker (trabaja con Dockerfile y los .sh), donde define tres servicios:
  * compilador: contenedor principal
  * build-compilador: para compilación
  * dev: entorno de desarrollo

## Instrucciones para compilar

### **Con Docker**
**IMPORTANTE TENER EL DOCKER ABIERTO**

**Ejecutar contenedor temporal:**
---
* docker-compose build
* docker-compose run --rm compilador bash

**Dentro del contenedor:**
---
* bison -d bison_ast.y
* flex flex.l
* gcc -o compilador main.c bison_ast.tab.c lex.yy.c ast.c interpreter.c scope.c -lfl

**Llamar al test:**
---
* ./compilador < test_completo.txt


### Sin Docker

**Requisitos**: `flex`, `bison`, `gcc` instalados en el sistema.

**En la terminal**
---
* bison -d bison_ast.y
* flex flex.l
* gcc -o compilador main.c bison_ast.tab.c lex.yy.c ast.c interpreter.c scope.c -lfl

**Llamar al test:**
---
* ./compilador < test_completo.txt


## Test Principal: `test_scope.txt`
* Contiene **TODAS** las funcionalidades del compilador:
* Tipos básicos: `dinoint`, `float`, `paputexto`
* Operaciones aritméticas: `+`, `-`, `*`, `/`
* Operaciones mixtas: `dinoint + float`
* Comparaciones: `>`, `==`, `!=`, `<`, `>=`, `<=`
* Condicionales: `si (condicion) { } sino { }`
* Scope por bloques: `{ }` anidados hasta 3 niveles
* Shadowing: ocultamiento de variables
* Input/Output: `texto "mensaje"`
* Concatenación: `"texto" + variable`

**Incluye:**
---
* .bat para ejecutar y borrar los archivos creados por bison y flex.
* .sh para las dependencias del docker y su correcto funcionamiento.