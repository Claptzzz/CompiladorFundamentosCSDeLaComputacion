#include "shared_ast.h"

scope *currentScope = NULL;
int scopeLevel = 0;

void enterScope() {
    scope *newScope = malloc(sizeof(scope));
    newScope->symbols = NULL;
    newScope->functions = NULL;
    newScope->functions = NULL;
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
    
    
    symbol *sym = oldScope->symbols;
    while (sym) {
        symbol *next = sym->next;
        free(sym->id);
        if (sym->type == TYPE_TEXT && !sym->isArray) {
            free(sym->value.text);
        } else if (sym->type == TYPE_FLOAT && !sym->isArray) {
            
        } else if (sym->isArray) {
            if (sym->type == TYPE_NUM) {
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

    function *func = oldScope->functions;
    while (func) {
        function *nextFunc = func->next;
        free(func->name);
        free(func);
        func = nextFunc;
    }
    
    free(oldScope);
    
    printf("Saliendo del scope, nivel actual: %d\n", currentScope ? currentScope->level : -1);
}

// busca un símbolo en el scope actual y en los scopes padres
// si lo encuentra, devuelve un puntero al símbolo, si no entonces devuelce null
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

// inserta un símbolo en el scope actual
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

// inserta un símbolo de texto en el scope actual
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
    printf("Array numérico %s[%d] declarado en scope nivel %d\n", id, size, currentScope->level);
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
    for (int i = 0; i < size; i++) {
        s->value.textArray[i] = strdup("");
    }
    s->next = currentScope->symbols;
    currentScope->symbols = s;
    printf("Array de texto %s[%d] declarado en scope nivel %d\n", id, size, currentScope->level);
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
    printf("Array float %s[%d] declarado en scope nivel %d\n", id, size, currentScope->level);
}

void insertFunction(char *name, struct ASTNode* parameters, struct ASTNode* body) {
    function *func = malloc(sizeof(function));
    func->name = strdup(name);
    func->parameters = parameters;
    func->body = body;
    func->next = currentScope->functions;
    currentScope->functions = func;
    printf("Función '%s' registrada en scope nivel %d\n", name, currentScope->level);
}

function* lookupFunction(char *name) {
    scope *scope = currentScope;
    while (scope) {
        function *func = scope->functions;
        while (func) {
            if (strcmp(func->name, name) == 0) {
                return func;
            }
            func = func->next;
        }
        scope = scope->parent;
    }
    return NULL;
}

char* numToStr(int num) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d", num);
    return strdup(buffer);
}

char* floatToStr(double f) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.2f", f);
    return strdup(buffer);
}

char* concat(const char* s1, const char* s2) {
    size_t len1 = strlen(s1), len2 = strlen(s2);
    char* result = malloc(len1 + len2 + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}