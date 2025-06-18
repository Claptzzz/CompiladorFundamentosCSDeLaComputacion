#include "shared_ast.h"

//creacion de los papunodos
ASTNode* createProgramNode(ASTNode* statements) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_PROGRAM;
    node->data.block.statements = statements;
    node->next = NULL;
    return node;
}

ASTNode* createLiteralNode(symbolType type, void* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_LITERAL;
    node->data.literal.valueType = type;
    
    switch(type) {
        case TYPE_NUM:
            node->data.literal.value.intValue = *(int*)value;
            break;
        case TYPE_FLOAT:
            node->data.literal.value.floatValue = *(double*)value;
            break;
        case TYPE_TEXT:
            node->data.literal.value.stringValue = strdup((char*)value);
            break;
    }
    node->next = NULL;
    return node;
}

ASTNode* createIdentifierNode(char* name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_IDENTIFIER;
    node->data.identifier.name = strdup(name);
    node->next = NULL;
    return node;
}

ASTNode* createDeclarationNode(symbolType type, char* name, ASTNode* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_DECLARATION;
    node->data.declaration.varType = type;
    node->data.declaration.varName = strdup(name);
    node->data.declaration.initialValue = value;
    node->next = NULL;
    return node;
}

ASTNode* createBinaryOpNode(OperatorType op, ASTNode* left, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_BINARY_OP;
    node->data.binaryOp.operator = op;
    node->data.binaryOp.left = left;
    node->data.binaryOp.right = right;
    node->next = NULL;
    return node;
}

ASTNode* createBlockNode(ASTNode* statements) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_BLOCK;
    node->data.block.statements = statements;
    node->next = NULL;
    return node;
}

ASTNode* createIfNode(ASTNode* condition, ASTNode* thenBlock, ASTNode* elseBlock) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = elseBlock ? NODE_IF_ELSE : NODE_IF;
    node->data.ifStatement.condition = condition;
    node->data.ifStatement.thenBlock = thenBlock;
    node->data.ifStatement.elseBlock = elseBlock;
    node->next = NULL;
    return node;
}

ASTNode* createWhileNode(ASTNode* condition, ASTNode* body) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_WHILE;
    node->data.whileStatement.condition = condition;
    node->data.whileStatement.body = body;
    node->next = NULL;
    return node;
}

ASTNode* createPrintNode(ASTNode* expression) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_PRINT;
    node->data.print.expression = expression;
    node->next = NULL;
    return node;
}

ASTNode* createInputNode(char* varName) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_INPUT;
    node->data.input.varName = strdup(varName);
    node->next = NULL;
    return node;
}

ASTNode* createFunctionDefNode(char* name, ASTNode* parameters, ASTNode* body) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_FUNCTION_DEF;
    node->data.functionDef.name = strdup(name);
    node->data.functionDef.parameters = parameters;
    node->data.functionDef.body = body;
    node->next = NULL;
    return node;
}

ASTNode* createFunctionCallNode(char* name, ASTNode* arguments) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_FUNCTION_CALL;
    node->data.functionCall.name = strdup(name);
    node->data.functionCall.arguments = arguments;
    node->next = NULL;
    return node;
}

ASTNode* createParameterNode(symbolType type, char* name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_PARAMETER;
    node->data.parameter.paramType = type;
    node->data.parameter.paramName = strdup(name);
    node->next = NULL;
    return node;
}

ASTNode* createReturnNode(ASTNode* expression) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_RETURN;
    node->data.returnStatement.expression = expression;
    node->next = NULL;
    return node;
}

ASTNode* createAssignmentNode(char* varName, ASTNode* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_ASSIGNMENT;
    node->data.assignment.varName = strdup(varName);
    node->data.assignment.value = value;
    node->next = NULL;
    return node;
}

//delete memory
void freeAST(ASTNode* node) {
    if (!node) return;
    
    switch(node->type) {
        case NODE_LITERAL:
            if (node->data.literal.valueType == TYPE_TEXT) {
                free(node->data.literal.value.stringValue);
            }
            break;
            
        case NODE_IDENTIFIER:
            free(node->data.identifier.name);
            break;
            
        case NODE_DECLARATION:
            free(node->data.declaration.varName);
            freeAST(node->data.declaration.initialValue);
            break;
            
        case NODE_BINARY_OP:
            freeAST(node->data.binaryOp.left);
            freeAST(node->data.binaryOp.right);
            break;
            
        case NODE_BLOCK:
        case NODE_PROGRAM:
            freeAST(node->data.block.statements);
            break;
            
        case NODE_IF:
        case NODE_IF_ELSE:
            freeAST(node->data.ifStatement.condition);
            freeAST(node->data.ifStatement.thenBlock);
            freeAST(node->data.ifStatement.elseBlock);
            break;
            
        case NODE_WHILE:
            freeAST(node->data.whileStatement.condition);
            freeAST(node->data.whileStatement.body);
            break;
            
        case NODE_PRINT:
            freeAST(node->data.print.expression);
            break;
            
        case NODE_INPUT:
            free(node->data.input.varName);
            break;
            
        case NODE_FUNCTION_DEF:
            free(node->data.functionDef.name);
            freeAST(node->data.functionDef.parameters);
            freeAST(node->data.functionDef.body);
            break;
            
        case NODE_FUNCTION_CALL:
            free(node->data.functionCall.name);
            freeAST(node->data.functionCall.arguments);
            break;
            
        case NODE_PARAMETER:
            free(node->data.parameter.paramName);
            break;
            
        case NODE_RETURN:
            freeAST(node->data.returnStatement.expression);
            break;
    }

    freeAST(node->next);
    free(node);
}

//imprime el ast
void printAST(ASTNode* node, int indent) {
    if (!node) return;
    
    for (int i = 0; i < indent; i++) printf("  ");
    
    switch(node->type) {
        case NODE_PROGRAM:
            printf("PROGRAM\n");
            printAST(node->data.block.statements, indent + 1);
            break;
            
        case NODE_LITERAL:
            printf("LITERAL: ");
            switch(node->data.literal.valueType) {
                case TYPE_NUM:
                    printf("%d\n", node->data.literal.value.intValue);
                    break;
                case TYPE_FLOAT:
                    printf("%.2f\n", node->data.literal.value.floatValue);
                    break;
                case TYPE_TEXT:
                    printf("\"%s\"\n", node->data.literal.value.stringValue);
                    break;
            }
            break;
            
        case NODE_IDENTIFIER:
            printf("ID: %s\n", node->data.identifier.name);
            break;
              case NODE_DECLARATION:
            printf("DECLARATION: %s %s\n", 
                   node->data.declaration.varType == TYPE_NUM ? "num" :
                   node->data.declaration.varType == TYPE_FLOAT ? "float" : "text",
                   node->data.declaration.varName);
            if (node->data.declaration.initialValue) {
                printAST(node->data.declaration.initialValue, indent + 1);
            }
            break;
            
        case NODE_ASSIGNMENT:
            printf("ASSIGNMENT: %s\n", node->data.assignment.varName);
            if (node->data.assignment.value) {
                printAST(node->data.assignment.value, indent + 1);
            }
            break;
            
        case NODE_BINARY_OP:
            printf("BINARY_OP: ");
            switch(node->data.binaryOp.operator) {
                case OP_ADD: printf("+\n"); break;
                case OP_SUB: printf("-\n"); break;
                case OP_MUL: printf("*\n"); break;
                case OP_DIV: printf("/\n"); break;
                case OP_EQ: printf("==\n"); break;
                case OP_NE: printf("!=\n"); break;
                case OP_LT: printf("<\n"); break;
                case OP_GT: printf(">\n"); break;
                case OP_LE: printf("<=\n"); break;
                case OP_GE: printf(">=\n"); break;
            }
            printAST(node->data.binaryOp.left, indent + 1);
            printAST(node->data.binaryOp.right, indent + 1);
            break;
            
        case NODE_BLOCK:
            printf("BLOCK\n");
            printAST(node->data.block.statements, indent + 1);
            break;
            
        case NODE_IF:
            printf("IF\n");
            printAST(node->data.ifStatement.condition, indent + 1);
            printAST(node->data.ifStatement.thenBlock, indent + 1);
            break;
              case NODE_IF_ELSE:
            printf("IF_ELSE\n");
            printAST(node->data.ifStatement.condition, indent + 1);
            printAST(node->data.ifStatement.thenBlock, indent + 1);
            printAST(node->data.ifStatement.elseBlock, indent + 1);
            break;
            
        case NODE_WHILE:
            printf("WHILE\n");
            printAST(node->data.whileStatement.condition, indent + 1);
            printAST(node->data.whileStatement.body, indent + 1);
            break;
            
        case NODE_PRINT:
            printf("PRINT\n");
            printAST(node->data.print.expression, indent + 1);
            break;
              case NODE_FUNCTION_DEF:
            printf("FUNCTION_DEF: %s\n", node->data.functionDef.name);
            printf("%*sPARAMETERS:\n", indent + 2, "");
            printAST(node->data.functionDef.parameters, indent + 2);
            printf("%*sBODY:\n", indent + 2, "");
            printAST(node->data.functionDef.body, indent + 2);
            break;
            
        case NODE_FUNCTION_CALL:
            printf("FUNCTION_CALL: %s\n", node->data.functionCall.name);
            if (node->data.functionCall.arguments) {
                printf("%*sARGUMENTS:\n", indent + 2, "");
                printAST(node->data.functionCall.arguments, indent + 2);
            }
            break;
            
        case NODE_PARAMETER:
            printf("PARAMETER: %s (type: %d)\n", node->data.parameter.paramName, node->data.parameter.paramType);
            break;
            
        case NODE_RETURN:
            printf("RETURN\n");
            if (node->data.returnStatement.expression) {
                printAST(node->data.returnStatement.expression, indent + 1);
            }
            break;
            
        default:
            printf("UNKNOWN NODE\n");
            break;
    }
    
    // imprime los nodos siguientes
    printAST(node->next, indent);
}
