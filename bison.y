%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shared.h" 

int yylex(void);
int yyerror(char *s);

/* globales*/
scope *currentScope = NULL;
int scopeLevel = 0;

/* metodos */
typedef struct methodDef {
    char *id;
    int param1;
    int param2;
    struct methodDef *next;
} methodDef;

methodDef *methodtab = NULL;

/* dinoint a paputextoo */
char* numToStr(int num) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d", num);
    return strdup(buffer);
}

/* float a paputexto */
char* floatToStr(double f) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.2f", f);
    return strdup(buffer);
}

/* scope manegment u know papu */
void enterScope() {
    scope *newScope = malloc(sizeof(scope));
    newScope->symbols = NULL;
    newScope->parent = currentScope;
    newScope->level = scopeLevel++;
    currentScope = newScope;
    printf("Entrando al scope nivel %d\n", currentScope->level);
}

void exitScope() {
    if (!currentScope) {
        fprintf(stderr, "Error: intentando salir de scope pero no hay scope actual\n");
        return;
    }
    
    scope *oldScope = currentScope;
    currentScope = currentScope->parent;
    scopeLevel--;
    
    /* liberar la memoria de los símbolos del scope que se cierra */
    symbol *sym = oldScope->symbols;
    while (sym) {
        symbol *next = sym->next;
        free(sym->id);        if (sym->type == TYPE_TEXT && !sym->isArray) {
            free(sym->value.text);
        } else if (sym->type == TYPE_FLOAT && !sym->isArray) {
            // Float simple no necesita liberación especial
        } else if (sym->isArray) {            if (sym->type == TYPE_NUM) {
                free(sym->value.numArray);
            } else if (sym->type == TYPE_FLOAT) {
                free(sym->value.floatArray);
            } else {
                for (int i = 0; i < sym->arraySize; i++) {
                    free(sym->value.textArray[i]);
                }
                free(sym->value.textArray);
            }
        }
        free(sym);
        sym = next;
    }
    free(oldScope);
    
    printf("Saliendo del scope, nivel actual: %d\n", currentScope ? currentScope->level : -1);
}

/* función para buscar símbolo en el scope actual y scopes padre */
symbol* lookupSymbol(char *id) {
    scope *s = currentScope;
    while (s) {
        symbol *p = s->symbols;
        while (p) {
            if (strcmp(p->id, id) == 0) {
                printf("Variable %s encontrada en scope nivel %d\n", id, s->level);
                return p;
            }
            p = p->next;
        }
        s = s->parent;
    }
    printf("Variable %s no encontrada en ningún scope\n", id);
    return NULL;
}

/* función para buscar símbolo solo en el scope actual (para declaraciones) */
symbol* lookupSymbolCurrentScope(char *id) {
    if (!currentScope) return NULL;
    
    symbol *p = currentScope->symbols;
    while (p) {
        if (strcmp(p->id, id) == 0)
            return p;
        p = p->next;
    }
    return NULL;
}

/* insertar variables */
void insertNumericSymbol(char *id, int value) {
    if (!currentScope) {
        fprintf(stderr, "Error: no hay scope activo\n");
        return;
    }
    
    symbol *s = lookupSymbolCurrentScope(id);
    if(s) {
        fprintf(stderr, "Error: variable %s ya declarada en este scope.\n", id);
        return;
    }
    
    s = malloc(sizeof(symbol));
    s->id = strdup(id);
    s->type = TYPE_NUM;
    s->isArray = 0;
    s->arraySize = 0;
    s->value.num = value;
    s->next = currentScope->symbols;
    currentScope->symbols = s;
    printf("Variable numérica %s declarada en scope nivel %d\n", id, currentScope->level);
}

void insertTextSymbol(char *id, char* value) {
    if (!currentScope) {
        fprintf(stderr, "Error: no hay scope activo\n");
        return;
    }
    
    symbol *s = lookupSymbolCurrentScope(id);
    if(s) {
        fprintf(stderr, "Error: variable %s ya declarada en este scope.\n", id);
        return;
    }
    
    s = malloc(sizeof(symbol));
    s->id = strdup(id);
    s->type = TYPE_TEXT;
    s->isArray = 0;
    s->arraySize = 0;
    s->value.text = strdup(value);
    s->next = currentScope->symbols;
    currentScope->symbols = s;
    printf("Variable de texto %s declarada en scope nivel %d\n", id, currentScope->level);
}

void insertFloatSymbol(char *id, double value) {
    if (!currentScope) {
        fprintf(stderr, "Error: no hay scope activo\n");
        return;
    }
    
    symbol *s = lookupSymbolCurrentScope(id);
    if(s) {
        fprintf(stderr, "Error: variable %s ya declarada en este scope.\n", id);
        return;
    }
    
    s = malloc(sizeof(symbol));
    s->id = strdup(id);
    s->type = TYPE_FLOAT;
    s->isArray = 0;
    s->arraySize = 0;
    s->value.floatVal = value;
    s->next = currentScope->symbols;
    currentScope->symbols = s;
    printf("Variable float %s declarada en scope nivel %d\n", id, currentScope->level);
}

void insertNumericArraySymbol(char *id, int size) {
    if (!currentScope) {
        fprintf(stderr, "Error: no hay scope activo\n");
        return;
    }
    
    symbol *s = lookupSymbolCurrentScope(id);
    if(s) {
        fprintf(stderr, "Error: variable %s ya declarada en este scope.\n", id);
        return;
    }
    
    s = malloc(sizeof(symbol));
    s->id = strdup(id);
    s->type = TYPE_NUM;
    s->isArray = 1;
    s->arraySize = size;
    s->value.numArray = calloc(size, sizeof(int));
    s->next = currentScope->symbols;
    currentScope->symbols = s;
    printf("Array numérico %s declarado en scope nivel %d con tamaño %d\n", id, currentScope->level, size);
}

void insertTextArraySymbol(char *id, int size) {
    if (!currentScope) {
        fprintf(stderr, "Error: no hay scope activo\n");
        return;
    }
    
    symbol *s = lookupSymbolCurrentScope(id);
    if(s) {
        fprintf(stderr, "Error: variable %s ya declarada en este scope.\n", id);
        return;
    }
    
    s = malloc(sizeof(symbol));
    s->id = strdup(id);
    s->type = TYPE_TEXT;
    s->isArray = 1;
    s->arraySize = size;
    s->value.textArray = calloc(size, sizeof(char*));
    for (int i = 0; i < size; i++)
        s->value.textArray[i] = strdup("");
    s->next = currentScope->symbols;
    currentScope->symbols = s;
    printf("Array de texto %s declarado en scope nivel %d con tamaño %d\n", id, currentScope->level, size);
}

void insertFloatArraySymbol(char *id, int size) {
    if (!currentScope) {
        fprintf(stderr, "Error: no hay scope activo\n");
        return;
    }
    
    symbol *s = lookupSymbolCurrentScope(id);
    if(s) {
        fprintf(stderr, "Error: variable %s ya declarada en este scope.\n", id);
        return;
    }
    
    s = malloc(sizeof(symbol));
    s->id = strdup(id);
    s->type = TYPE_FLOAT;
    s->isArray = 1;
    s->arraySize = size;
    s->value.floatArray = calloc(size, sizeof(double));
    s->next = currentScope->symbols;
    currentScope->symbols = s;
    printf("Array de float %s declarado en scope nivel %d con tamaño %d\n", id, currentScope->level, size);
}

void insertMethod2(char *id, int t1, int t2) {
    methodDef *m = malloc(sizeof(methodDef));
    m->id = strdup(id);
    m->param1 = t1;
    m->param2 = t2;
    m->next = methodtab;
    methodtab = m;
}

methodDef* lookupMethod(char *id) {
    methodDef *m = methodtab;
    while (m) {
        if (strcmp(m->id, id) == 0)
            return m;
        m = m->next;
    }
    return NULL;
}

char* concat(const char* s1, const char* s2) {
    size_t len1 = strlen(s1), len2 = strlen(s2);
    char* result = malloc(len1 + len2 + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}
%}

%union {
    int numero;
    double flotante;
    char* texto;
    char* id;
    ParamPair* paramPair;
    ArgPair* argPair;
    ASTNode* ast_node;
}

%type <flotante> condicion 
%type <numero> valor tipo
%type <texto> textexp instruccion_llamada_metodo
%type <ast_node> programa instrucciones instruccion exp
%type <ast_node> declaracion_num declaracion_float declaracion_texto 
%type <ast_node> decl_num decl_float decl_texto
%type <ast_node> instruccion_imprimir instruccion_sin_sino instruccion_con_sino
%type <paramPair> parametro_dos
%type <argPair> argumentos_dos

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
;

instruccion:
      instruccion_sin_sino { $$ = $1; }
    | instruccion_con_sino { $$ = $1; }
    | declaracion_num { $$ = $1; }
    | declaracion_float { $$ = $1; }
    | declaracion_texto { $$ = $1; }
    | instruccion_imprimir { $$ = $1; }
    | LLAVE_IZQ instrucciones LLAVE_DER { $$ = createBlockNode($2); }
;

bloque:
      instruccion
;

instruccion_sin_sino:
      GET ID PUNTOCOMA                    { printf("Leer variable: %s\n", $2); }
    | SI '(' condicion ')' bloque         { printf("Condición SI ejecutada\n"); }
    | MIENTRAS '(' condicion ')' bloque   { printf("Inicio del bucle MIENTRAS\n"); }
;

instruccion_con_sino:
    SI '(' condicion ')' bloque SINO bloque { printf("Condición SI/SINO ejecutada\n"); }
;

textexp:
      STRING               { $$ = strdup($1); free($1); }
    | ID {
        symbol *s = lookupSymbol($1);
        if (!s) {
            yyerror("Variable no declarada");
            $$ = strdup("");
        } else if (s->isArray) {
            yyerror("No se puede usar un array en una expresión de texto");
            $$ = strdup("");
        } else if (s->type == TYPE_TEXT) {
            $$ = strdup(s->value.text);        } else if (s->type == TYPE_NUM) {
            $$ = numToStr(s->value.num); 
        } else if (s->type == TYPE_FLOAT) {
            $$ = floatToStr(s->value.floatVal);
        } else {
            yyerror("Tipo no válido en textexp");
            $$ = strdup("");
        }
      }
    | textexp '+' textexp  { $$ = concat($1, $3); free($1); free($3); }
    | '(' textexp ')'      { $$ = $2; }
;

declaracion_num:
      NUM_TIPO decl_num PUNTOCOMA { $$ = $2; }

decl_num:
      ID '=' exp { $$ = createDeclarationNode(TYPE_NUM, $1, $3); }
    | ID { $$ = createDeclarationNode(TYPE_NUM, $1, NULL); }

declaracion_float:
      FLOAT_TIPO decl_float PUNTOCOMA { $$ = $2; }

decl_float:
      ID '=' exp { $$ = createDeclarationNode(TYPE_FLOAT, $1, $3); }
    | ID { $$ = createDeclarationNode(TYPE_FLOAT, $1, NULL); }

declaracion_texto:
      TEXT decl_texto PUNTOCOMA { $$ = $2; }

decl_texto:
      ID '=' textexp { 
        // Convertir string a nodo literal
        ASTNode* strNode = createLiteralNode(TYPE_TEXT, $3);
        $$ = createDeclarationNode(TYPE_TEXT, $1, strNode);
        free($3);
      }
    | ID { $$ = createDeclarationNode(TYPE_TEXT, $1, NULL); }
;

declaracion_array_num:
      NUM_TIPO '[' ']' lista_decl_array_num PUNTOCOMA
;

lista_decl_array_num:
      decl_array_num
    | lista_decl_array_num ',' decl_array_num
;

decl_array_num:
      ID '=' '[' NUM ']'   { insertNumericArraySymbol($1, $4); }
    | ID                   { insertNumericArraySymbol($1, 0); }
;

declaracion_array_text:
      TEXT '[' ']' lista_decl_array_text PUNTOCOMA
;

lista_decl_array_text:
      decl_array_text
    | lista_decl_array_text ',' decl_array_text
;

decl_array_text:
      ID '=' '[' NUM ']'   { insertTextArraySymbol($1, $4); }
    | ID                   { insertTextArraySymbol($1, 0); }
;

declaracion_array_float:
      FLOAT_TIPO '[' ']' lista_decl_array_float PUNTOCOMA
;

lista_decl_array_float:
      decl_array_float
    | lista_decl_array_float ',' decl_array_float
;

decl_array_float:
      ID '=' '[' NUM ']'   { insertFloatArraySymbol($1, $4); }
    | ID                   { insertFloatArraySymbol($1, 0); }
;

instruccion_imprimir:
      TEXTO textexp PUNTOCOMA { printf("Imprimir: %s\n", $2); free($2); }
;

declaracion_metodo:
      METHOD ID '(' parametro_dos ')' { enterScope(); } bloque { exitScope(); }
         {
           insertMethod2($2, $4->type1, $4->type2);
           printf("Método definido: %s con parámetros de tipo %s y %s\n", $2,
              ($4->type1==TYPE_TEXT?"text":"num"),
              ($4->type2==TYPE_TEXT?"text":"num"));
           free($4);
         }
;

parametro_dos:
      tipo ID ',' tipo ID
         {
           ParamPair *p = malloc(sizeof(ParamPair));
           p->type1 = $1;
           p->type2 = $4;
           $$ = p;
         }
;

instruccion_llamada_metodo:
      ID '(' argumentos_dos ')' PUNTOCOMA
         {
           methodDef *m = lookupMethod($1);
           if (!m) {
               yyerror("Método no definido");
               $$ = strdup("");
           } else if (m->param1 != $3->argType1 || m->param2 != $3->argType2) {
               yyerror("Tipo de argumento incorrecto");
               $$ = strdup("");
           } else if (strcmp($1, "saludar") == 0) {
               char *numStr = numToStr($3->argNum);
               char *tmp = concat("Hola ", $3->argText);
               char buffer[256];
               snprintf(buffer, sizeof(buffer), "%s, tienes %s", tmp, numStr);
               printf("Resultado de la llamada a método %s: %s\n", $1, buffer);
               $$ = strdup(buffer);
               free(numStr); free(tmp);
           } else {
               yyerror("Método no implementado");
               $$ = strdup("");
           }
           free($3->argText);
           free($3);
         }
;

argumentos_dos:
      textexp ',' exp
         {
           ArgPair *p = malloc(sizeof(ArgPair));
           p->argType1 = TYPE_TEXT;
           p->argType2 = TYPE_NUM;
           p->argText = $1;
           p->argNum = $3;
           $$ = p;
         }
;

tipo:
      TEXT       { $$ = TYPE_TEXT; }
    | NUM_TIPO   { $$ = TYPE_NUM;  }
    | FLOAT_TIPO { $$ = TYPE_FLOAT; }
;

condicion:
      exp MENOR exp             { $$ = ($1 < $3) ? 1.0 : 0.0; }
    | exp IGUAL exp             { $$ = ($1 == $3) ? 1.0 : 0.0; }
    | exp MAYOR exp             { $$ = ($1 > $3) ? 1.0 : 0.0; }
    | exp DIFERENTE exp         { $$ = ($1 != $3) ? 1.0 : 0.0; }
    | exp MENORIGUAL exp        { $$ = ($1 <= $3) ? 1.0 : 0.0; }
    | exp MAYORIGUAL exp        { $$ = ($1 >= $3) ? 1.0 : 0.0; }
;

exp:
      valor                { $$ = (double)$1; }
    | FLOAT                { $$ = $1; }
    | ID {
           symbol *s = lookupSymbol($1);
           if (!s || s->isArray) {
               yyerror("Variable no declarada o es un array");
               $$ = 0.0;
           } else if (s->type == TYPE_NUM) {
               $$ = (double)s->value.num;
           } else if (s->type == TYPE_FLOAT) {
               $$ = s->value.floatVal;
           } else {
               yyerror("Variable no es numérica");
               $$ = 0.0;
           }
         }
    | exp '+' exp          { $$ = $1 + $3; }
    | exp '-' exp          { $$ = $1 - $3; }
    | exp '*' exp          { $$ = $1 * $3; }
    | exp '/' exp          {
           if ($3 == 0.0) {
               yyerror("División por cero");
               $$ = 0.0;
           } else $$ = $1 / $3;
      }
    | '(' exp ')'          { $$ = $2; }
;

valor:
      NUM                  { $$ = $1; }
;

%%

int yyerror(char *s) {
    fprintf(stderr, "Error de sintaxis: %s\n", s);
    return 1;
}

/* gracias bison por tanto, espero no volver a utilizarte */