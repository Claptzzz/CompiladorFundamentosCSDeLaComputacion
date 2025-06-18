#ifndef SHARED_H
#define SHARED_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { TYPE_NUM, TYPE_TEXT, TYPE_FLOAT } symbolType;

typedef struct function {
    char *name;
    struct ASTNode* parameters;
    struct ASTNode* body;
    struct function *next;
} function;

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
    function *functions;
    struct scope *parent;
    int level;
} scope;

typedef enum {
    NODE_PROGRAM,
    NODE_DECLARATION,
    NODE_ASSIGNMENT,
    NODE_LITERAL,
    NODE_IDENTIFIER,
    NODE_BINARY_OP,
    NODE_BLOCK,
    NODE_IF,
    NODE_IF_ELSE,
    NODE_WHILE,
    NODE_PRINT,
    NODE_INPUT,
    NODE_FUNCTION_DEF,
    NODE_FUNCTION_CALL,
    NODE_PARAMETER,
    NODE_RETURN
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

        struct {
            char* name;
            struct ASTNode* parameters;
            struct ASTNode* body;
        } functionDef;

        struct {
            char* name;
            struct ASTNode* arguments;
        } functionCall;

        struct {
            symbolType paramType;
            char* paramName;
        } parameter;

        struct {
            struct ASTNode* expression;
        } returnStatement;
        
    } data;
    
    struct ASTNode* next;
} ASTNode;
// funciones del ast
ASTNode* createProgramNode(ASTNode* statements);
ASTNode* createLiteralNode(symbolType type, void* value);
ASTNode* createIdentifierNode(char* name);
ASTNode* createDeclarationNode(symbolType type, char* name, ASTNode* value);
ASTNode* createAssignmentNode(char* varName, ASTNode* value);
ASTNode* createBinaryOpNode(OperatorType op, ASTNode* left, ASTNode* right);
ASTNode* createBlockNode(ASTNode* statements);
ASTNode* createIfNode(ASTNode* condition, ASTNode* thenBlock, ASTNode* elseBlock);
ASTNode* createWhileNode(ASTNode* condition, ASTNode* body);
ASTNode* createPrintNode(ASTNode* expression);
ASTNode* createInputNode(char* varName);
ASTNode* createFunctionDefNode(char* name, ASTNode* parameters, ASTNode* body);
ASTNode* createFunctionCallNode(char* name, ASTNode* arguments);
ASTNode* createParameterNode(symbolType type, char* name);
ASTNode* createReturnNode(ASTNode* expression);

void interpretAST(ASTNode* root);
void freeAST(ASTNode* node);
void printAST(ASTNode* node, int indent);

extern scope *currentScope;
extern int scopeLevel;

void enterScope();
void exitScope();
symbol* lookupSymbol(char *id);
symbol* lookupSymbolCurrentScope(char *id);
void insertNumericSymbol(char *id, int value);
void insertTextSymbol(char *id, char* value);
void insertFloatSymbol(char *id, double value);
void insertNumericArraySymbol(char *id, int size);
void insertTextArraySymbol(char *id, int size);
void insertFloatArraySymbol(char *id, int size);
void insertFunction(char *name, struct ASTNode* parameters, struct ASTNode* body);
function* lookupFunction(char *name);
// funciones de utilidades para transformar las skibidiVariables
char* numToStr(int num);
char* floatToStr(double f);
char* concat(const char* s1, const char* s2);

#endif