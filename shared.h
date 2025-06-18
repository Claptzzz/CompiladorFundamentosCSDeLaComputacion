#ifndef SHARED_H
#define SHARED_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { TYPE_NUM, TYPE_TEXT, TYPE_FLOAT } symbolType;

typedef struct symbol {
    char *id;
    symbolType type;
    int isArray;
    int arraySize;
    union {
        int num;
        char *text;
        double floatVal;
        int *numArray;
        char **textArray;
        double *floatArray;
    } value;
    struct symbol *next;
} symbol;

typedef struct scope {
    symbol *symbols;
    struct scope *parent;
    int level;
} scope;

typedef enum {
    NODE_PROGRAM,
    NODE_INSTRUCTION_LIST,
    NODE_DECLARATION,
    NODE_ASSIGNMENT,
    NODE_EXPRESSION,
    NODE_LITERAL,
    NODE_IDENTIFIER,
    NODE_BINARY_OP,
    NODE_BLOCK,
    NODE_IF,
    NODE_IF_ELSE,
    NODE_WHILE,
    NODE_PRINT,
    NODE_INPUT
} NodeType;

typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_EQ,
    OP_NE,
    OP_LT,
    OP_GT,
    OP_LE,
    OP_GE
} OperatorType;

typedef struct ASTNode {
    NodeType type;
    
    union {
        struct {
            symbolType valueType;
            union {
                int intValue;
                double floatValue;
                char* stringValue;
            } value;
        } literal;

        struct {
            char* name;
        } identifier;

        struct {
            symbolType varType;
            char* varName;
            struct ASTNode* initialValue;
        } declaration;

        struct {
            char* varName;
            struct ASTNode* value;
        } assignment;

        struct {
            OperatorType operator;
            struct ASTNode* left;
            struct ASTNode* right;
        } binaryOp;

        struct {
            struct ASTNode* statements;
        } block;

        struct {
            struct ASTNode* condition;
            struct ASTNode* thenBlock;
            struct ASTNode* elseBlock;
        } ifStatement;

        struct {
            struct ASTNode* condition;
            struct ASTNode* body;
        } whileStatement;

        struct {
            struct ASTNode* expression;
        } print;

        struct {
            char* varName;
        } input;
        
    } data;
    
    struct ASTNode* next;
} ASTNode;

ASTNode* createProgramNode(ASTNode* statements);
ASTNode* createLiteralNode(symbolType type, void* value);
ASTNode* createIdentifierNode(char* name);
ASTNode* createDeclarationNode(symbolType type, char* name, ASTNode* value);
ASTNode* createBinaryOpNode(OperatorType op, ASTNode* left, ASTNode* right);
ASTNode* createBlockNode(ASTNode* statements);
ASTNode* createIfNode(ASTNode* condition, ASTNode* thenBlock, ASTNode* elseBlock);
ASTNode* createWhileNode(ASTNode* condition, ASTNode* body);
ASTNode* createPrintNode(ASTNode* expression);
ASTNode* createInputNode(char* varName);

void interpretAST(ASTNode* root);

void freeAST(ASTNode* node);
void printAST(ASTNode* node, int indent);

typedef enum { TYPE_NUM, TYPE_TEXT, TYPE_FLOAT } Type;

typedef struct ParamPair {
    int type1;
    int type2;
} ParamPair;

typedef struct ArgPair {
    int argType1;
    int argType2;
    char* argText;
    int argNum;
} ArgPair;

typedef struct symbol {
    char *id;
    Type type;
    int isArray;
    int arraySize;    union {
        int num;
        char* text;
        double floatVal;
        int* numArray;
        char** textArray;
        double* floatArray;
    } value;
    struct symbol *next;
} symbol;

typedef struct scope {
    symbol *symbols;
    struct scope *parent;
    int level;
} scope;

extern scope *currentScope;
extern int scopeLevel;

void enterScope(void);
void exitScope(void);
symbol* lookupSymbol(char *id);
symbol* lookupSymbolCurrentScope(char *id);
void insertNumericSymbol(char *id, int value);
void insertTextSymbol(char *id, char* value);
void insertFloatSymbol(char *id, double value);
void insertNumericArraySymbol(char *id, int size);
void insertTextArraySymbol(char *id, int size);
void insertFloatArraySymbol(char *id, int size);

#endif