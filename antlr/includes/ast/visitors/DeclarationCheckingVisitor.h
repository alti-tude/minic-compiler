#ifndef DECLARATIONCHECKINGVISITOR_H
#define DECLARATIONCHECKINGVISITOR_H

#include "AstVisitorBase.h"
#include "ast/ast.h"
#include "ast/Types.h"
#include "symbol_table/SymbolTable.h"
#include "symbol_table/Types.h"

#include "Exception.h"

static void updateVariableNode(AstVar& node) {
    VarType var_decl;
    if(node.type == ast::Type::DEFAULT_TYPE){ //match with decl
        try{
            var_decl = SymTabSingleton::getInstance().getVariable(node.name);
        } catch(SymbolTableVarNotFoundException& exception){
            ASSERT(
                node.type == ast::Type::STREAM, 
                "Var usage", 
                "Variable (" + node.name + ") not declared before usage"
            );
        }
    } 
    else{ //variable decl
        try{
            var_decl = VarType(node.type, node.name, 0);
            SymTabSingleton::getInstance().addVariable(var_decl);
        } catch(SymbolTableVarReDeclarationException& exception) {
            ASSERT(
                node.type == ast::Type::STREAM, 
                "Var Decl", 
                "Variable (" + node.name + ") redeclaration"
            );
        }
    }

    node.type = var_decl.type;
    node.name = var_decl.name;
}

class DeclarationCheckingVisitor : public AstVisitorBase {
private:
    bool loop_context;
    bool function_context;
    bool return_found;
public:
    virtual void visit(AstProg& node) {
        debugArg("");
        node.scope_block->accept(*this);
    }
    virtual void visit(AstScopeBlock& node) {
        debugArg("");
        SymTabSingleton::getInstance().enterScope();
        
        for(auto statement : node.statements)
            statement->accept(*this);
        
        SymTabSingleton::getInstance().exitScope();
    }
    virtual void visit(AstVarDeclStatement& node) {
        debugArg("");
        for(auto var_decl : node.var_decls)
            var_decl->accept(*this);
    }
    virtual void visit(AstWhile& node) {
        debugArg("");
        SymTabSingleton::getInstance().enterScope();
        
        node.expr->accept(*this);

        loop_context = true;
        node.scope_block->accept(*this);
        loop_context = false;

        SymTabSingleton::getInstance().exitScope();
    }
    virtual void visit(AstFor& node) {
        debugArg("");
        SymTabSingleton::getInstance().enterScope();

        node.iterator->accept(*this);
        if(node.list_lit) node.list_lit->accept(*this);
        else{
            node.start->accept(*this);
            node.end->accept(*this);
            if(node.step) node.step->accept(*this);
        }

        loop_context = true;
        node.scope_block->accept(*this);
        loop_context = false;
        
        SymTabSingleton::getInstance().exitScope();
    }
    
    virtual void visit(AstIfStatement& node) {
        debugArg("");
        for(auto cond : node.conditions)
            cond->accept(*this);
        for(auto act : node.actions)
            act->accept(*this);
    }
    
    virtual void visit(AstBreak& node) {
        debugArg("");
        ASSERT(loop_context, "Break", "Break statement outside loop");
    }
    virtual void visit(AstContinue& node) {
        debugArg("");
        ASSERT(loop_context, "Continue", "Continue statement outside loop");
    }
    virtual void visit(AstReturn& node) {
        debugArg("");
        ASSERT(function_context, "Return", "Return outside function");
        if(node.return_expr)
            node.return_expr->accept(*this);
        return_found = true;
    }
    virtual void visit(AstMethodDecl& node) {
        debugArg("");

        FuncType func = FuncType(node.return_type, node.name);
        try{
            FuncType& func_ref = SymTabSingleton::getInstance().addFunction(func);
            
            SymTabSingleton::getInstance().enterScope();
            for(auto arg : node.args){
                arg->accept(*this);
                func_ref.arg_types.push_back(arg->type);
            }
            for(auto arg_by_ref : node.args_by_ref){
                arg_by_ref->accept(*this);
                func_ref.arg_by_ref_types.push_back(arg_by_ref->type);
            }

            function_context = true;
            return_found = false;

            node.body->accept(*this);
            
            ASSERT(return_found or node.return_type==ast::Type::VOID, "Method Decl", "No return statement");
            function_context = false;

            SymTabSingleton::getInstance().exitScope();

        } catch(SymbolTableFunctionReDeclarationException& exception) {
            THROW_CUSTOM(
                UserException,
                "Function Decl",
                "( " + node.name + " ) function redeclaration"
            );
        }

    }
    virtual void visit(AstListLit& node) {
        debugArg("");
        for(auto member : node.members) 
            member->accept(*this);
    }
    virtual void visit(AstUnaryExpr& node) {
        debugArg("");
        node.expr->accept(*this);
    }
    virtual void visit(AstBinExpr& node) {
        debugArg("");
        node.lhs->accept(*this);
        node.rhs->accept(*this);
    }
    virtual void visit(AstTernaryExpr& node) {
        debugArg("");
        node.first->accept(*this);
        node.second->accept(*this);
        node.third->accept(*this);
    }
    virtual void visit(AstVar& node) {
        debugArg("");
        if(node.expr) node.expr->accept(*this);
        if(node.rows) node.rows->accept(*this);
        if(node.cols) node.cols->accept(*this);

        updateVariableNode(node);
    }
    virtual void visit(AstMethodCall& node) {
        debugArg("");

        FuncType func;
        try{
            func = SymTabSingleton::getInstance().getFunction(node.name);
        } catch(SymbolTableFunctionNotFoundException& exception){
            THROW_CUSTOM(
                UserException,
                "Function Call",
                "( " + node.text + " ) Function not declared before use"
            );
        }
        
        node.return_type = func.return_type;
        node.args_types = func.arg_types;
        node.args_by_ref_types = func.arg_by_ref_types;
        
        for(auto arg : node.args)
            arg->accept(*this);
        for(auto arg_by_ref : node.args_by_ref)
            arg_by_ref->accept(*this);
    }
    virtual void visit(AstStream& node) {
        debugArg("");
        if(node.cur_pos) node.cur_pos->accept(*this);
        if(node.size) node.size->accept(*this);
        updateVariableNode(node);
    }
    virtual void visit(AstInputIoStatement& node) {
        debugArg("");
        node.stream->accept(*this);
        node.var->accept(*this);
    }
    virtual void visit(AstOutputIoStatement& node) {
        debugArg("");
        node.stream->accept(*this);
        node.list_expr->accept(*this);
    }
};

#endif