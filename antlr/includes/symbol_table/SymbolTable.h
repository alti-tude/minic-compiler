#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "map"
#include "stack"

#include "symbol_table/Types.h"
#include "mixins/SingletonMixin.h"
#include "Exception.h"

class SymbolTable {
protected:
    SymbolTable()
        :num_scopes(0) 
    {
        scopes.push(num_scopes);
    }
    virtual ~SymbolTable(){}

    VarType getVariable(KeyType key){
        ASSERT_NM(
            variable_table.find(key)!=variable_table.end(),
            SymbolTableVarNotFoundException
        );
        return variable_table[key];
    }
    
    FuncType getFunction(KeyType key){
        ASSERT_NM(
            function_table.find(key) != function_table.end(),
            SymbolTableFunctionNotFoundException
        );
        return function_table[key];
    }

public:
    std::map<KeyType, VarType> variable_table;
    std::map<KeyType, FuncType> function_table;
    std::stack<unsigned int> scopes;
    size_t num_scopes;

    int enterScope(){
        scopes.push(num_scopes++);
        return scopes.top();
    }

    int exitScope(){
        scopes.pop();
        return scopes.top();
    }

    void resetScopes(){
        scopes = std::stack<unsigned int>();
        num_scopes = 0;
    }

    void reset(){
        num_scopes = 0;
        scopes = std::stack<unsigned int>();
        variable_table = std::map<KeyType, VarType>();
        function_table = std::map<KeyType, FuncType>(); 
    }
    
    void addVariable(VarType& var){
        var.scope = scopes.top();
        try{
            var = getVariable(var.getKey());
            THROW(SymbolTableVarReDeclarationException);
        } catch(SymbolTableVarNotFoundException& exception){
            variable_table[var.getKey()] = var;
        }
    }

    VarType getVariable(std::string name){
        std::stack<unsigned int> tmp = scopes;
        VarType found_variable;

        while(!scopes.empty())
            try{
                found_variable = getVariable(KeyType(name, scopes.top()));
                scopes = tmp;
                return found_variable;
            } catch(SymbolTableVarNotFoundException& exception){
                scopes.pop();
            }
        
        scopes = tmp;
        THROW(SymbolTableVarNotFoundException);
    }

    FuncType& addFunction(FuncType& func){
        func.scope = scopes.top();
        try{
            func = getFunction(func.getKey());
            THROW(SymbolTableFunctionReDeclarationException);
        } catch(SymbolTableFunctionNotFoundException& exception){
            function_table[func.getKey()] = func;
        }

        return function_table[func.getKey()];
    }

    FuncType getFunction(std::string name){
        std::stack<unsigned int> tmp = scopes;
        FuncType found_func;

        while(!scopes.empty())
            try{
                found_func = getFunction(KeyType(name, scopes.top()));
                scopes = tmp;
                return found_func;
            } catch(SymbolTableFunctionNotFoundException& exception){
                scopes.pop();
            }
        
        scopes = tmp;
        THROW(SymbolTableFunctionNotFoundException);
    }
    
};


using SymTabSingleton = Singleton<SymbolTable>;

#endif
