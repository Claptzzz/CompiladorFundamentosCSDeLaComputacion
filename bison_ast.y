%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shared_ast.h" 

int yylex(void);
int yyerror(char *s);
%}

%union {
    int numero;
    double flotante;
    char* texto;
    char* id;
    ASTNode* ast_node;
}

%type <ast_node> programa instrucciones instruccion exp
%type <ast_node> declaracion_num declaracion_float declaracion_texto 
%type <ast_node> instruccion_imprimir instruccion_sin_sino instruccion_con_sino
%type <ast_node> definicion_funcion parametros parametro argumentos
%type <numero> valor
%type <ast_node> textexp

%token IGUAL DIFERENTE MENOR MAYOR MENORIGUAL MAYORIGUAL
%token GET SI SINO MIENTRAS NUM_TIPO FLOAT_TIPO
%token METHOD RETURN TEXT TEXTO
%token LLAVE_IZQ LLAVE_DER
%token PUNTOCOMA
%token <numero> NUM
%token <flotante> FLOAT
%token <texto> STRING
%token <id> ID

%start programa
%expect 1

%left IGUAL DIFERENTE
%left MENOR MAYOR MENORIGUAL MAYORIGUAL  
%left '+' '-'
%left '*' '/'

%%

programa: instrucciones { 
    $$ = createProgramNode($1); 
    printf("=== AST CONSTRUIDO EXITOSAMENTE ===\n");
    interpretAST($$);
    freeAST($$);
}

instrucciones:
      instruccion { $$ = $1; }
    | instrucciones instruccion { 
        if ($1) {
            ASTNode* current = $1;
            while (current->next) current = current->next;
            current->next = $2;
            $$ = $1;
        } else {
            $$ = $2;
        }
      }

instruccion:
      declaracion_num { $$ = $1; }
    | declaracion_float { $$ = $1; }
    | declaracion_texto { $$ = $1; }
    | instruccion_imprimir { $$ = $1; }
    | instruccion_sin_sino { $$ = $1; }
    | instruccion_con_sino { $$ = $1; }
    | definicion_funcion { $$ = $1; }
    | LLAVE_IZQ instrucciones LLAVE_DER { $$ = createBlockNode($2); }
    | ID '=' exp PUNTOCOMA { $$ = createAssignmentNode($1, $3); }

instruccion_sin_sino:
      GET ID PUNTOCOMA { $$ = createInputNode($2); }
    | SI '(' exp ')' instruccion { $$ = createIfNode($3, $5, NULL); }
    | MIENTRAS '(' exp ')' instruccion { $$ = createWhileNode($3, $5); }
    | ID '(' argumentos ')' PUNTOCOMA { $$ = createFunctionCallNode($1, $3); }
    | ID '(' ')' PUNTOCOMA { $$ = createFunctionCallNode($1, NULL); }

instruccion_con_sino:
    SI '(' exp ')' instruccion SINO instruccion { $$ = createIfNode($3, $5, $7); }

declaracion_num:
      NUM_TIPO ID '=' exp PUNTOCOMA { $$ = createDeclarationNode(TYPE_NUM, $2, $4); }
    | NUM_TIPO ID PUNTOCOMA { $$ = createDeclarationNode(TYPE_NUM, $2, NULL); }

declaracion_float:
      FLOAT_TIPO ID '=' exp PUNTOCOMA { $$ = createDeclarationNode(TYPE_FLOAT, $2, $4); }
    | FLOAT_TIPO ID PUNTOCOMA { $$ = createDeclarationNode(TYPE_FLOAT, $2, NULL); }

declaracion_texto:
      TEXT ID '=' textexp PUNTOCOMA { 
        ASTNode* strNode = createLiteralNode(TYPE_TEXT, $4);
        $$ = createDeclarationNode(TYPE_TEXT, $2, strNode);
        free($4);
      }
    | TEXT ID PUNTOCOMA { $$ = createDeclarationNode(TYPE_TEXT, $2, NULL); }

instruccion_imprimir:
      TEXTO textexp PUNTOCOMA { 
        $$ = createPrintNode($2);
      }

textexp:
      STRING               { 
        ASTNode* strNode = createLiteralNode(TYPE_TEXT, $1);
        free($1);
        $$ = strNode;
      }
    | ID {
        $$ = createIdentifierNode($1);
        free($1);
      }    | textexp '+' textexp  { 
        $$ = createBinaryOpNode(OP_ADD, $1, $3);
      }
    | '(' textexp ')'      { $$ = $2; }

exp:
      valor                { 
        int* val = malloc(sizeof(int));
        *val = $1;
        $$ = createLiteralNode(TYPE_NUM, val);
        free(val);
      }
    | FLOAT                { 
        double* val = malloc(sizeof(double));
        *val = $1;
        $$ = createLiteralNode(TYPE_FLOAT, val);
        free(val);
      }
    | ID                   { $$ = createIdentifierNode($1); }
    | ID '(' argumentos ')' { $$ = createFunctionCallNode($1, $3); }
    | ID '(' ')'           { $$ = createFunctionCallNode($1, NULL); }
    | exp '+' exp          { $$ = createBinaryOpNode(OP_ADD, $1, $3); }
    | exp '-' exp          { $$ = createBinaryOpNode(OP_SUB, $1, $3); }
    | exp '*' exp          { $$ = createBinaryOpNode(OP_MUL, $1, $3); }
    | exp '/' exp          { $$ = createBinaryOpNode(OP_DIV, $1, $3); }
    | exp MENOR exp        { $$ = createBinaryOpNode(OP_LT, $1, $3); }
    | exp IGUAL exp        { $$ = createBinaryOpNode(OP_EQ, $1, $3); }
    | exp MAYOR exp        { $$ = createBinaryOpNode(OP_GT, $1, $3); }
    | exp DIFERENTE exp    { $$ = createBinaryOpNode(OP_NE, $1, $3); }
    | exp MENORIGUAL exp   { $$ = createBinaryOpNode(OP_LE, $1, $3); }
    | exp MAYORIGUAL exp   { $$ = createBinaryOpNode(OP_GE, $1, $3); }
    | '(' exp ')'          { $$ = $2; }

valor:
      NUM                  { $$ = $1; }

definicion_funcion:
    METHOD ID '(' parametros ')' LLAVE_IZQ instrucciones LLAVE_DER { 
        ASTNode* body = createBlockNode($7);
        $$ = createFunctionDefNode($2, $4, body);
    }
    | METHOD ID '(' ')' LLAVE_IZQ instrucciones LLAVE_DER { 
        ASTNode* body = createBlockNode($6);
        $$ = createFunctionDefNode($2, NULL, body);
    }

parametros:
      parametro { $$ = $1; }
    | parametros ',' parametro { 
        if ($1) {
            ASTNode* current = $1;
            while (current->next) current = current->next;
            current->next = $3;
            $$ = $1;
        } else {
            $$ = $3;
        }
      }

parametro:
      NUM_TIPO ID { $$ = createParameterNode(TYPE_NUM, $2); }
    | FLOAT_TIPO ID { $$ = createParameterNode(TYPE_FLOAT, $2); }
    | TEXT ID { $$ = createParameterNode(TYPE_TEXT, $2); }

argumentos:
      exp { $$ = $1; }
    | argumentos ',' exp { 
        if ($1) {
            ASTNode* current = $1;
            while (current->next) current = current->next;
            current->next = $3;
            $$ = $1;
        } else {
            $$ = $3;
        }
      }

%%

int yyerror(char *s) {
    fprintf(stderr, "Error de sintaxis: %s\n", s);
    return 1;
}
