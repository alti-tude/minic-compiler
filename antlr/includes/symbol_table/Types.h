#ifndef SYMBOLTABLETYPES_H
#define SYMBOLTABLETYPES_H

#include "string"
#include "vector"
#include "utility"

#include "ast/Types.h"

using KeyType = std::pair<std::string, unsigned int>;

class VarType{
public:
    ast::Type type;
    std::string name;
    unsigned int scope;

    VarType()
        :type(ast::Type::DEFAULT_TYPE), name(""), scope(0){}

    VarType(ast::Type type, std::string name, unsigned int scope)
        :type(type), name(name), scope(scope){}

    VarType(std::string name, unsigned int scope)
        :type(ast::Type::DEFAULT_TYPE), name(name), scope(scope){}

    KeyType getKey(){
        return {name, scope};
    }
};

class FuncType{
public:
    ast::Type return_type;
    std::string name;
    unsigned int scope;
    std::vector<ast::Type> arg_types;
    std::vector<ast::Type> arg_by_ref_types;
    
    FuncType(){}

    FuncType(ast::Type type, std::string name)
        :return_type(type), name(name) {}

    KeyType getKey(){
        return {name, scope};
    }
};

#endif