#ifndef TYPES_H
#define TYPES_H

#include "string"

namespace ast{
    enum Type {DEFAULT_TYPE, INT, UINT, BOOL, CHAR, FLOAT, VOID, STREAM};
    enum OpType { DEFAULT_OP,
        LT, GT, LEQ, GEQ, EQ, NEQ, REL_OP_BREAK,
        AND, OR, NOT, BOOL_OP_BREAK,
        ADD, SUB, MUL, DIV, MOD
    };
}

ast::Type toType(std::string type){
    ast::Type var_type = ast::Type::DEFAULT_TYPE;
    if(type == "int") var_type = ast::Type::INT;
    if(type == "uint") var_type = ast::Type::UINT;
    if(type == "char") var_type = ast::Type::CHAR;
    if(type == "bool") var_type = ast::Type::BOOL;
    if(type == "float") var_type = ast::Type::FLOAT;
    if(type == "void") var_type = ast::Type::VOID;
    return var_type;
}

bool isCasteable(ast::Type type1, ast::Type type2) {
    return  (
                (type1!=ast::Type::BOOL and type1!=ast::Type::STREAM and type1!= ast::Type::VOID) and 
                (type2!=ast::Type::BOOL and type2!=ast::Type::STREAM and type2!= ast::Type::VOID)
            );
}

bool isCasteableOrSame(ast::Type type1, ast::Type type2) {
    return isCasteable(type1, type2) || type1 == type2;
}

bool isValidVar(ast::Type type) {
    return  type == ast::Type::INT || 
            type == ast::Type::UINT || 
            type == ast::Type::BOOL ||
            type == ast::Type::CHAR ||
            type == ast::Type::FLOAT;
}

bool isValidVarOrStream(ast::Type type) {
    return isValidVar(type) || type == ast::Type::STREAM;
}
#endif