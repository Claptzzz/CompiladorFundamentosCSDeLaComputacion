#include "shared_ast.h"



typedef struct {
    symbolType type;
    union {
        int intValue;
        double floatValue;
        char* stringValue;
    } value;
} EvalResult;

EvalResult evaluateExpression(ASTNode* expr);
int evaluateCondition(ASTNode* condition);
void executeStatement(ASTNode* stmt);
void executeStatementList(ASTNode* stmtList);
EvalResult callFunction(char* functionName, ASTNode* arguments);



char* resultToString(EvalResult result) {
    char buffer[256];
    switch(result.type) {
        case TYPE_NUM:
            snprintf(buffer, sizeof(buffer), "%d", result.value.intValue);
            return strdup(buffer);
        case TYPE_FLOAT:
            snprintf(buffer, sizeof(buffer), "%.2f", result.value.floatValue);
            return strdup(buffer);
        case TYPE_TEXT:
            return strdup(result.value.stringValue);
        default:
            return strdup("");
    }
}



EvalResult evaluateExpression(ASTNode* expr) {
    EvalResult result = {0};
    
    if (!expr) {
        result.type = TYPE_NUM;
        result.value.intValue = 0;
        return result;
    }
    
    switch(expr->type) {
        case NODE_LITERAL:
            result.type = expr->data.literal.valueType;
            switch(result.type) {
                case TYPE_NUM:
                    result.value.intValue = expr->data.literal.value.intValue;
                    break;
                case TYPE_FLOAT:
                    result.value.floatValue = expr->data.literal.value.floatValue;
                    break;
                case TYPE_TEXT:
                    result.value.stringValue = strdup(expr->data.literal.value.stringValue);
                    break;
            }
            break;
            
        case NODE_IDENTIFIER: {
            symbol* sym = lookupSymbol(expr->data.identifier.name);
            if (!sym) {
                fprintf(stderr, "Error: Variable '%s' no declarada\n", expr->data.identifier.name);
                result.type = TYPE_NUM;
                result.value.intValue = 0;
            } else if (sym->isArray) {
                fprintf(stderr, "Error: No se puede usar array en expresión\n");
                result.type = TYPE_NUM;
                result.value.intValue = 0;
            } else {
                result.type = sym->type;
                switch(sym->type) {
                    case TYPE_NUM:
                        result.value.intValue = sym->value.num;
                        break;
                    case TYPE_FLOAT:
                        result.value.floatValue = sym->value.floatVal;
                        break;
                    case TYPE_TEXT:
                        result.value.stringValue = strdup(sym->value.text);
                        break;                }
            }
            break;
        }
        
        case NODE_FUNCTION_CALL: {
            result = callFunction(expr->data.functionCall.name, 
                                expr->data.functionCall.arguments);
            break;
        }
        
        case NODE_BINARY_OP: {
            EvalResult left = evaluateExpression(expr->data.binaryOp.left);
            EvalResult right = evaluateExpression(expr->data.binaryOp.right);
            
            
            if (expr->data.binaryOp.operator >= OP_ADD && expr->data.binaryOp.operator <= OP_DIV) {
                
                if (left.type == TYPE_FLOAT || right.type == TYPE_FLOAT) {
                    double leftVal = (left.type == TYPE_FLOAT) ? left.value.floatValue : (double)left.value.intValue;
                    double rightVal = (right.type == TYPE_FLOAT) ? right.value.floatValue : (double)right.value.intValue;
                    
                    result.type = TYPE_FLOAT;
                    switch(expr->data.binaryOp.operator) {
                        case OP_ADD: result.value.floatValue = leftVal + rightVal; break;
                        case OP_SUB: result.value.floatValue = leftVal - rightVal; break;
                        case OP_MUL: result.value.floatValue = leftVal * rightVal; break;
                        case OP_DIV: 
                            if (rightVal == 0.0) {
                                fprintf(stderr, "Error: División por cero\n");
                                result.value.floatValue = 0.0;
                            } else {
                                result.value.floatValue = leftVal / rightVal;
                            }
                            break;
                    }
                } else if (left.type == TYPE_NUM && right.type == TYPE_NUM) {
                    result.type = TYPE_NUM;
                    switch(expr->data.binaryOp.operator) {
                        case OP_ADD: result.value.intValue = left.value.intValue + right.value.intValue; break;
                        case OP_SUB: result.value.intValue = left.value.intValue - right.value.intValue; break;
                        case OP_MUL: result.value.intValue = left.value.intValue * right.value.intValue; break;
                        case OP_DIV:
                            if (right.value.intValue == 0) {
                                fprintf(stderr, "Error: División por cero\n");
                                result.value.intValue = 0;
                            } else {
                                result.value.intValue = left.value.intValue / right.value.intValue;
                            }
                            break;
                    }
                } else if (left.type == TYPE_TEXT && right.type == TYPE_TEXT && expr->data.binaryOp.operator == OP_ADD) {
                    
                    result.type = TYPE_TEXT;
                    size_t len = strlen(left.value.stringValue) + strlen(right.value.stringValue) + 1;
                    result.value.stringValue = malloc(len);
                    strcpy(result.value.stringValue, left.value.stringValue);
                    strcat(result.value.stringValue, right.value.stringValue);
                }
            }
            
            
            else {
                result.type = TYPE_NUM; 
                
                if (left.type == TYPE_FLOAT || right.type == TYPE_FLOAT) {
                    double leftVal = (left.type == TYPE_FLOAT) ? left.value.floatValue : (double)left.value.intValue;
                    double rightVal = (right.type == TYPE_FLOAT) ? right.value.floatValue : (double)right.value.intValue;
                    
                    switch(expr->data.binaryOp.operator) {
                        case OP_EQ: result.value.intValue = (leftVal == rightVal); break;
                        case OP_NE: result.value.intValue = (leftVal != rightVal); break;
                        case OP_LT: result.value.intValue = (leftVal < rightVal); break;
                        case OP_GT: result.value.intValue = (leftVal > rightVal); break;
                        case OP_LE: result.value.intValue = (leftVal <= rightVal); break;
                        case OP_GE: result.value.intValue = (leftVal >= rightVal); break;
                    }
                } else {
                    switch(expr->data.binaryOp.operator) {
                        case OP_EQ: result.value.intValue = (left.value.intValue == right.value.intValue); break;
                        case OP_NE: result.value.intValue = (left.value.intValue != right.value.intValue); break;
                        case OP_LT: result.value.intValue = (left.value.intValue < right.value.intValue); break;
                        case OP_GT: result.value.intValue = (left.value.intValue > right.value.intValue); break;
                        case OP_LE: result.value.intValue = (left.value.intValue <= right.value.intValue); break;
                        case OP_GE: result.value.intValue = (left.value.intValue >= right.value.intValue); break;
                    }
                }
            }
            
            
            if (left.type == TYPE_TEXT) free(left.value.stringValue);
            if (right.type == TYPE_TEXT) free(right.value.stringValue);
            break;
        }
        
        default:
            result.type = TYPE_NUM;
            result.value.intValue = 0;
            break;
    }
    
    return result;
}



int evaluateCondition(ASTNode* condition) {
    EvalResult result = evaluateExpression(condition);
    int condValue = 0;
    
    switch(result.type) {
        case TYPE_NUM:
            condValue = (result.value.intValue != 0);
            break;
        case TYPE_FLOAT:
            condValue = (result.value.floatValue != 0.0);
            break;
        case TYPE_TEXT:
            condValue = (strlen(result.value.stringValue) > 0);
            free(result.value.stringValue);
            break;
    }
    
    return condValue;
}



void executeStatement(ASTNode* stmt) {
    if (!stmt) return;
    
    switch(stmt->type) {
        case NODE_DECLARATION: {
            EvalResult value = evaluateExpression(stmt->data.declaration.initialValue);
            
            switch(stmt->data.declaration.varType) {
                case TYPE_NUM:
                    if (value.type == TYPE_FLOAT) {
                        insertNumericSymbol(stmt->data.declaration.varName, (int)value.value.floatValue);
                    } else {
                        insertNumericSymbol(stmt->data.declaration.varName, value.value.intValue);
                    }
                    break;
                case TYPE_FLOAT:
                    if (value.type == TYPE_NUM) {
                        insertFloatSymbol(stmt->data.declaration.varName, (double)value.value.intValue);
                    } else {
                        insertFloatSymbol(stmt->data.declaration.varName, value.value.floatValue);
                    }
                    break;
                case TYPE_TEXT:
                    if (value.type == TYPE_TEXT) {
                        insertTextSymbol(stmt->data.declaration.varName, value.value.stringValue);
                        free(value.value.stringValue);
                    } else {
                        char* strVal = resultToString(value);
                        insertTextSymbol(stmt->data.declaration.varName, strVal);
                        free(strVal);
                    }                    break;
            }
            break;
        }
        
        case NODE_ASSIGNMENT: {
            EvalResult value = evaluateExpression(stmt->data.assignment.value);
            symbol* sym = lookupSymbol(stmt->data.assignment.varName);
            
            if (!sym) {
                printf("Error: Variable '%s' no definida\n", stmt->data.assignment.varName);
                return;
            }
              switch(sym->type) {
                case TYPE_NUM:
                    if (value.type == TYPE_FLOAT) {
                        sym->value.num = (int)value.value.floatValue;
                    } else {
                        sym->value.num = value.value.intValue;
                    }
                    break;
                case TYPE_FLOAT:
                    if (value.type == TYPE_NUM) {
                        sym->value.floatVal = (double)value.value.intValue;
                    } else {
                        sym->value.floatVal = value.value.floatValue;
                    }
                    break;
                case TYPE_TEXT:
                    if (sym->value.text) {
                        free(sym->value.text);
                    }
                    if (value.type == TYPE_TEXT) {
                        sym->value.text = strdup(value.value.stringValue);
                        free(value.value.stringValue);
                    } else {
                        char* strVal = resultToString(value);
                        sym->value.text = strdup(strVal);
                        free(strVal);
                    }
                    break;
            }
            break;
        }
        
        case NODE_BLOCK:
            enterScope();
            executeStatementList(stmt->data.block.statements);
            exitScope();
            break;
            
        case NODE_IF: {
            if (evaluateCondition(stmt->data.ifStatement.condition)) {
                executeStatement(stmt->data.ifStatement.thenBlock);
            }
            break;
        }
          case NODE_IF_ELSE: {
            if (evaluateCondition(stmt->data.ifStatement.condition)) {
                executeStatement(stmt->data.ifStatement.thenBlock);
            } else {
                executeStatement(stmt->data.ifStatement.elseBlock);
            }
            break;
        }
        
        case NODE_WHILE: {
            while (evaluateCondition(stmt->data.whileStatement.condition)) {
                executeStatement(stmt->data.whileStatement.body);
            }
            break;
        }
        
        case NODE_PRINT: {
            EvalResult result = evaluateExpression(stmt->data.print.expression);
            char* output = resultToString(result);
            printf("Imprimir: %s\n", output);
            free(output);
            if (result.type == TYPE_TEXT) free(result.value.stringValue);
            break;
        }
          case NODE_INPUT: {
            printf("Leer variable: %s\n", stmt->data.input.varName);
            break;
        }
        
        case NODE_FUNCTION_DEF: {
            insertFunction(stmt->data.functionDef.name, 
                          stmt->data.functionDef.parameters, 
                          stmt->data.functionDef.body);
            printf("Función '%s' definida\n", stmt->data.functionDef.name);
            break;
        }
        
        case NODE_FUNCTION_CALL: {
            EvalResult result = callFunction(stmt->data.functionCall.name, 
                                           stmt->data.functionCall.arguments);
            break;
        }
        
        default:
            break;
    }
}



void executeStatementList(ASTNode* stmtList) {
    ASTNode* current = stmtList;
    while (current) {
        executeStatement(current);
        current = current->next;
    }
}



EvalResult callFunction(char* functionName, ASTNode* arguments) {
    EvalResult result = {0};
    result.type = TYPE_NUM;
    result.value.intValue = 0;
    
    function* func = lookupFunction(functionName);
    if (!func) {
        fprintf(stderr, "Error: función '%s' no encontrada\n", functionName);
        return result;
    }
    
    printf("Llamando función: %s\n", functionName);

    enterScope();

    ASTNode* param = func->parameters;
    ASTNode* arg = arguments;
    
    while (param && arg) {
        if (param->type == NODE_PARAMETER) {
            EvalResult argValue = evaluateExpression(arg);
            
            switch(param->data.parameter.paramType) {
                case TYPE_NUM:
                    if (argValue.type == TYPE_FLOAT) {
                        insertNumericSymbol(param->data.parameter.paramName, (int)argValue.value.floatValue);
                    } else {
                        insertNumericSymbol(param->data.parameter.paramName, argValue.value.intValue);
                    }
                    break;
                case TYPE_FLOAT:
                    if (argValue.type == TYPE_NUM) {
                        insertFloatSymbol(param->data.parameter.paramName, (double)argValue.value.intValue);
                    } else {
                        insertFloatSymbol(param->data.parameter.paramName, argValue.value.floatValue);
                    }
                    break;
                case TYPE_TEXT:
                    if (argValue.type == TYPE_TEXT) {
                        insertTextSymbol(param->data.parameter.paramName, argValue.value.stringValue);
                        free(argValue.value.stringValue);
                    } else {
                        char* strVal = resultToString(argValue);
                        insertTextSymbol(param->data.parameter.paramName, strVal);
                        free(strVal);
                    }
                    break;
            }
        }
        param = param->next;
        arg = arg->next;
    }

    executeStatement(func->body);

    exitScope();
    
    return result;
}


//impresiones al analizar un ejemplo de txt, es la estructura general
void interpretAST(ASTNode* root) {
    if (!root) return;
    
    printf("=== INICIANDO INTERPRETACIÓN DEL AST ===\n");
    
    
    printf("=== ESTRUCTURA DEL AST ===\n");
    printAST(root, 0);
    printf("=== EJECUTANDO PROGRAMA ===\n");
    
    
    enterScope();
    
    
    if (root->type == NODE_PROGRAM) {
        executeStatementList(root->data.block.statements);
    } else {
        executeStatement(root);
    }
    
    
    exitScope();
    
    printf("=== FIN DE LA INTERPRETACIÓN ===\n");
}
