#ifndef TYPECHECKINGVISITOR_H
#define TYPECHECKINGVISITOR_H

#include "AstVisitorBase.h"
#include "ast/ast.h"
#include "ast/Types.h"

#include "Exception.h"

class TypeCheckingVisitor : public AstVisitorBase {
private:
    ast::Type type = ast::Type::DEFAULT_TYPE;
    ast::Type return_type = ast::Type::DEFAULT_TYPE;
public:
    void visit(AstProg& node) {
        debugArg("");
        node.scope_block->accept(*this);
        
        type = ast::Type::DEFAULT_TYPE;
    }
    
    void visit(AstScopeBlock& node) {
        debugArg("");
        for(auto statement : node.statements)
            statement->accept(*this);
        
        type = ast::Type::DEFAULT_TYPE;
    }
    
    void visit(AstVarDeclStatement& node) {
        debugArg("");
        for(auto var_decl : node.var_decls)
            var_decl->accept(*this);
        
        type = ast::Type::DEFAULT_TYPE;
    }
    
    void visit(AstWhile& node) {
        debugArg("");
        
        node.expr->accept(*this);
        ASSERT(type == ast::Type::BOOL, "While loop", "( " + node.expr->text + " ) Condition must return boolean");
        node.scope_block->accept(*this);
        
        type = ast::Type::DEFAULT_TYPE;
    }
    
    void visit(AstFor& node) {
        debugArg("");
        node.iterator->accept(*this);
        ast::Type iterator_type = type;

        if(node.list_lit){
            node.list_lit->accept(*this);
            ASSERT(type == iterator_type, "For loop", "( " + node.text + " ) Iterator type and list type must match");
        }
        else{
            ASSERT(
                    iterator_type == ast::Type::INT or iterator_type == ast::Type::UINT, 
                    "For loop", 
                    "( " + node.iterator->text + " ) iterator must be int for range based iteration"
                );

            node.start->accept(*this);
            ASSERT(type == iterator_type, "For loop", "( " + node.start->text + " ) start index must be int");

            node.end->accept(*this);
            ASSERT(type == iterator_type, "For loop", "( " + node.end->text + " ) end index must be int");

            if(node.step) {
                node.step->accept(*this);
                ASSERT(type == iterator_type, "For loop", "( " + node.step->text + " ) step size must be int");
            } 
        }

        node.scope_block->accept(*this);
        type = ast::Type::DEFAULT_TYPE;
    }

    void visit(AstIfStatement& node) {
        debugArg("");
        
        for(auto cond : node.conditions){
            cond->accept(*this);
            ASSERT(type == ast::Type::BOOL, "If statement", "( " + cond->text + " ) Condition must be boolean");
        }

        for(auto act : node.actions)
            act->accept(*this);
        
        type = ast::Type::DEFAULT_TYPE;
    }
    
    void visit(AstReturn& node) {
        debugArg("");

        // return type is the type of the returned expr
        if(node.return_expr) node.return_expr->accept(*this);
        else type = ast::Type::VOID;

        ASSERT(isCasteableOrSame(type,this->return_type), "Return", "( " + node.text + " ) Returned expr type does not match the declared type");
    }

    void visit(AstMethodDecl& node) {
        debugArg("");

        for(auto arg : node.args)
            arg->accept(*this);
        
        for(auto arg_by_ref : node.args_by_ref)
            arg_by_ref->accept(*this);
        
        this->return_type = node.return_type;
        node.body->accept(*this);
        this->return_type = ast::Type::DEFAULT_TYPE;

        type = ast::Type::DEFAULT_TYPE;
    }
    
    void visit(AstListLit& node) {
        debugArg("");
        ast::Type member_type = ast::Type::DEFAULT_TYPE;
        
        for(auto member : node.members) {
            member->accept(*this);
            if(member_type == ast::Type::DEFAULT_TYPE) member_type = type;
            else ASSERT(member_type == type, "List literal", "( " + node.text + " ) All member types must be same");
        }

        type = member_type;
    }
    
    void visit(AstUnaryExpr& node) {
        debugArg("");
        node.expr->accept(*this);
        ASSERT(
            (node.op == ast::OpType::NOT and type==ast::Type::BOOL) or 
            (node.op == ast::OpType::SUB and (type==ast::Type::INT or type == ast::Type::FLOAT)),
            "Unary Expr",
            "( " + node.text + " ) Operator not suitable for the operand type"
        );

        type = (node.op == ast::OpType::NOT ? ast::Type::BOOL : ast::Type::INT);

        node.type = type;
    }

    void visit(AstBinExpr& node) {
        debugArg("");

        ast::Type lhs_type, rhs_type;
        
        node.lhs->accept(*this);
        lhs_type = type;

        node.rhs->accept(*this);
        rhs_type = type;

        ASSERT(
            (node.op < ast::OpType::REL_OP_BREAK and lhs_type==rhs_type) or
            (node.op < ast::OpType::BOOL_OP_BREAK and node.op > ast::OpType::REL_OP_BREAK and lhs_type == ast::Type::BOOL and rhs_type == ast::Type::BOOL) or
            (node.op == ast::OpType::MOD and lhs_type == rhs_type and (lhs_type == ast::Type::INT or lhs_type == ast::Type::UINT)) or
            (node.op > ast::OpType::BOOL_OP_BREAK and lhs_type == rhs_type), 
            //no other checks since char + int is allowed
            "Binary Expression",
            "( " + node.text + " ) Operator and Operands have incompatible type"
        );

        if(node.op < ast::OpType::BOOL_OP_BREAK)
            type = ast::Type::BOOL;
        else type = lhs_type;
        node.type = type;
    }
    
    void visit(AstTernaryExpr& node) {
        debugArg("");
        node.first->accept(*this);
        ASSERT(type == ast::Type::BOOL, "Ternary Expr", "( " + node.first->text + " ) Condition must be boolean");

        node.second->accept(*this);
        ast::Type second_type = type;

        node.third->accept(*this);
        ast::Type third_type = type;

        ASSERT(second_type == third_type, "Ternary Expr", "( " + node.text + " ) both alternatives must be of the same type");
        type = second_type;

        node.type = type;
    }
    
    void visit(AstVar& node) {
        debugArg("");
        
        if(node.expr) {
            node.expr->accept(*this);
            ASSERT(type == node.type or isCasteable(type, node.type), "Var assignment", "( " + node.text + " ) Assigned value type and variable type are incompatible");
        }

        if(node.rows) {
            node.rows->accept(*this);
            ASSERT(type == ast::Type::INT or type == ast::Type::UINT, "Array", "( " + node.text + " ) Row index must be int or uint");
        }

        if(node.cols) {
            node.cols->accept(*this);
            ASSERT(type == ast::Type::INT or type == ast::Type::UINT, "Array", "( " + node.text + " ) Col index must be int or uint");
        }

        type = node.type;
    }
    
    void visit(AstMethodCall& node) {
        debugArg("");
        int i = 0;
        ASSERT(node.args.size() == node.args_types.size(), "Method Call", "( " + node.text + " ) # of passed args must be same as # of declared args");
        ASSERT(node.args_by_ref.size() == node.args_by_ref_types.size(), "Method Call", "( " + node.text + " ) # of passed args must be same as # of declared args");

        for(auto arg : node.args){
            arg->accept(*this);
            ASSERT(
                isCasteableOrSame(type, node.args_types[i]),
                "Method Call",
                "( " + node.text + " ) argument type does not match method decl"
            );
            i++;
        }

        i = 0;
        for(auto arg_by_ref : node.args_by_ref){
            arg_by_ref->accept(*this);
            ASSERT(
                isCasteableOrSame(type, node.args_by_ref_types[i]),
                "Method Call",
                "( " + node.text + " ) argument type does not match method decl"
            );
            i++;
        }
        
        type = node.return_type;
    }
    
    void visit(AstStream& node) {
        debugArg("");
        if(node.cur_pos) {
            node.cur_pos->accept(*this);
            ASSERT(
                isCasteable(type, ast::Type::INT),
                "Stream decl",
                "( " + node.text + " ) cur_pos must be casteable to integer"
            );
        }
        if(node.size) {
            node.size->accept(*this);
            ASSERT(
                isCasteable(type, ast::Type::INT),
                "Stream decl",
                "( " + node.text + " ) size must be casteable to integer"
            );
        }
        type = ast::Type::STREAM;
    }
    
    void visit(AstInputIoStatement& node) {
        debugArg("");
        node.stream->accept(*this);
        node.var->accept(*this);

        type = ast::Type::DEFAULT_TYPE;
    }
    
    void visit(AstOutputIoStatement& node) {
        debugArg("");
        node.stream->accept(*this);
        node.list_expr->accept(*this);
        
        type = ast::Type::DEFAULT_TYPE;
    }
    
    void visit(AstIntLit& node) {
        debugArg("");
        type = ast::Type::INT;

        node.type = type;
    }
    
    void visit(AstUIntLit& node) {
        debugArg("");
        type = ast::Type::UINT;

        node.type = type;
    }
    
    void visit(AstCharLit& node) {
        debugArg("");
        type = ast::Type::CHAR;

        node.type = type;
    }
    
    void visit(AstBoolLit& node) {
        debugArg("");
        type = ast::Type::BOOL;

        node.type = type;
    }

    void visit(AstFloatLit& node) {
        debugArg("");
        type = ast::Type::FLOAT;

        node.type = type;
    }
};

#endif