#ifndef BUILDASTVISITOR_H
#define BUILDASTVISITOR_H

#include "iostream"
#include "vector"
#include "string"

#include "ExprBaseVisitor.h"
#include "Utilities.h"
#include "ast/ast.h"

template<typename T, typename U>
static T addText(T node, U ctx){
    node->text = ctx->getText();
    return node;
}

class BuildAstVisitor : public ExprVisitor{
    antlrcpp::Any visitProg(ExprParser::ProgContext *ctx){
        debug("");
        AstScopeBlock* scope_block = get<AstScopeBlock*, AstNode*>(visit(ctx->scope_block()));
        return (AstNode*) addText(new AstProg(scope_block), ctx);
    }

    antlrcpp::Any visitScope_block(ExprParser::Scope_blockContext *ctx){
        debug("");
        AstScopeBlock* scope_block = new AstScopeBlock();
        for(auto statement_ctx : ctx->statement()){
            AstStatement* statement = get<AstStatement*, AstNode*>(visit(statement_ctx));
            scope_block->statements.push_back(statement);
        }
        return (AstNode*) addText(scope_block, ctx);
    }

    antlrcpp::Any visitVar_decl(ExprParser::Var_declContext *ctx) {
        debug("");
        AstVarDeclStatement* var_decl_stat = new AstVarDeclStatement();
        std::string type = ctx->TYPE()->getText();
        ast::Type var_type = toType(type);

        for(auto var_combined:ctx->var_combined()){
            AstVar* var = get<AstVar*, AstNode*>(visit(var_combined));
            var->type = var_type;
            var_decl_stat->var_decls.push_back(var);
        }

        return (AstNode*) addText(var_decl_stat, ctx);
    }

    antlrcpp::Any visitVarDeclStatement(ExprParser::VarDeclStatementContext *ctx){
        debug("");
        return (AstNode*) visit(ctx->var_decl());
    }

    antlrcpp::Any visitVar_combined(ExprParser::Var_combinedContext *ctx){
        debug("");
        if(ctx->var_init()) 
            return visit(ctx->var_init());
        return (AstNode*) visit(ctx->var());
    }

    antlrcpp::Any visitScalarVarInit(ExprParser::ScalarVarInitContext *ctx) {
        debug("");
        std::string name = ctx->NAME()->getText();
        AstExpr* value = get<AstExpr*, AstNode*>(visit(ctx->expr()));

        return (AstNode*) addText(new AstVar(ast::Type::DEFAULT_TYPE, name, value), ctx);
    }

    antlrcpp::Any visitOneDVarInit(ExprParser::OneDVarInitContext *ctx) {
        debug("");
        std::string name = ctx->NAME()->getText();
        AstExpr* rows = get<AstExpr*, AstNode*>(visit(ctx->expr(0)));
        AstExpr* value = get<AstExpr*, AstNode*>(visit(ctx->expr(1)));

        return (AstNode*) addText(new AstVar(ast::Type::DEFAULT_TYPE, name, value, rows), ctx);
    }

    antlrcpp::Any visitTwoDVarInit(ExprParser::TwoDVarInitContext *ctx) {
        debug("");
        std::string name = ctx->NAME()->getText();
        AstExpr* rows = get<AstExpr*, AstNode*>(visit(ctx->expr(0)));
        AstExpr* cols = get<AstExpr*, AstNode*>(visit(ctx->expr(1)));
        AstExpr* value = get<AstExpr*, AstNode*>(visit(ctx->expr(2)));
        
        return (AstNode*) addText(new AstVar(ast::Type::DEFAULT_TYPE, name, value, rows, cols), ctx);
    }

    antlrcpp::Any visitUnaryExpr(ExprParser::UnaryExprContext *ctx) {
        debug("");
        ast::OpType op = ast::OpType::DEFAULT_OP;
        if(ctx->op->getText() == "-") op = ast::OpType::SUB;
        if(ctx->op->getText() == "not") op = ast::OpType::NOT;
        AstExpr* expr = get<AstExpr*, AstNode*>(visit(ctx->expr()));

        return (AstNode*) addText(new AstUnaryExpr(op, expr), ctx);
    }

    antlrcpp::Any visitIntLiteralExpr(ExprParser::IntLiteralExprContext *ctx) {
        int value = atoi(ctx->INT_LITERAL()->getText().c_str());
        debug("");
        return (AstNode*) addText(new AstIntLit(value), ctx);
    }

    antlrcpp::Any visitIoStatement(ExprParser::IoStatementContext *ctx) {
        debug(""); 
        return visit(ctx->io());
    }
    
    antlrcpp::Any visitStreamDeclStatement(ExprParser::StreamDeclStatementContext *ctx) {
        debug(""); 
        return visit(ctx->stream_decl());
    }

    antlrcpp::Any visitVarInitStatement(ExprParser::VarInitStatementContext *ctx) {
        debug(""); 
        return visit(ctx->var_init());
    }

    antlrcpp::Any visitCapturingMethodDeclStatement(ExprParser::CapturingMethodDeclStatementContext *ctx) {
        debug(""); 
        return visit(ctx->capturing_method_decl());
    }
    
    virtual antlrcpp::Any visitIfStatement(ExprParser::IfStatementContext *ctx) {
        debug(""); 
        AstIfStatement* if_node = new AstIfStatement();

        for(auto if_stat : ctx->if_statement()){
            if_node->conditions.push_back(get<AstExpr*, AstNode*>(visit(if_stat->expr())));
            if_node->actions.push_back(get<AstScopeBlock*, AstNode*>(visit(if_stat->scope_block())));
        }
        if(ctx->scope_block())
            if_node->actions.push_back(get<AstScopeBlock*, AstNode*>(visit(ctx->scope_block())));

        return (AstNode*) addText(if_node, ctx);
    }

    virtual antlrcpp::Any visitWhileStatement(ExprParser::WhileStatementContext *ctx) {
        debug(""); 
        AstExpr* expr = get<AstExpr*, AstNode*>(visit(ctx->expr()));
        AstScopeBlock* scope_block = get<AstScopeBlock*, AstNode*>(visit(ctx->scope_block()));
        return (AstNode*) addText(new AstWhile(expr, scope_block), ctx);
    }
    
    virtual antlrcpp::Any visitForStatement(ExprParser::ForStatementContext *ctx) {
        debug(""); 
        ast::Type var_type = ast::Type::DEFAULT_TYPE;
        if(ctx->TYPE()){
            std::string type = ctx->TYPE()->getText();
            var_type = toType(type);
        }
        AstVar* iterator = new AstVar(var_type, ctx->NAME()->getText());
        AstScopeBlock* body = get<AstScopeBlock*, AstNode*>(visit(ctx->scope_block()));

        if(ctx->list_literal()){
            AstListLit* list_lit = get<AstListLit*, AstNode*>(visit(ctx->list_literal()));
            return (AstNode*) addText(new AstFor(iterator, body, list_lit), ctx);
        }

        AstExpr *start=nullptr, *end=nullptr, *step=nullptr;
        start = get<AstExpr*, AstNode*>(visit(ctx->expr(0)));
        end = get<AstExpr*, AstNode*>(visit(ctx->expr(1)));
        if(ctx->expr().size()==3)
            step = get<AstExpr*, AstNode*>(visit(ctx->expr(2)));
        
        return (AstNode*) addText(new AstFor(iterator, body, start, end, step), ctx);
    }
    
    virtual antlrcpp::Any visitBreak(ExprParser::BreakContext *ctx) {
        debug(""); 
        return (AstNode*) addText(new AstBreak(), ctx);
    }

    virtual antlrcpp::Any visitContinue(ExprParser::ContinueContext *ctx) {
        debug(""); 
        return (AstNode*) addText(new AstContinue(), ctx);
    }
    
    virtual antlrcpp::Any visitReturn(ExprParser::ReturnContext *ctx) {
        debug(""); 
        AstListExpr* return_expr = nullptr;
        if(ctx->list_expr()) 
            return_expr = get<AstListExpr*, AstNode*>(visit(ctx->list_expr()));

        return (AstNode*) addText(new AstReturn(return_expr), ctx);
    }
    
    antlrcpp::Any visitCapturingMethodCallStatement(ExprParser::CapturingMethodCallStatementContext *ctx) {
        debug(""); 
        return visit(ctx->capturing_method_call());
    }
    
    virtual antlrcpp::Any visitVar(ExprParser::VarContext *ctx) {
        debug(""); 
        AstVar* var_node;
        AstExpr *rows = nullptr, *cols=nullptr;

        switch(ctx->expr().size()){
            case 0:
                var_node = new AstVar(ast::Type::DEFAULT_TYPE, ctx->NAME()->getText());
                break;
            
            case 1:
                rows = get<AstExpr*, AstNode*>(visit(ctx->expr(0)));
                var_node = new AstVar(ast::Type::DEFAULT_TYPE, ctx->NAME()->getText(), nullptr, rows);
                break;
            
            case 2:
                rows = get<AstExpr*, AstNode*>(visit(ctx->expr(0)));
                cols = get<AstExpr*, AstNode*>(visit(ctx->expr(1)));
                var_node = new AstVar(ast::Type::DEFAULT_TYPE, ctx->NAME()->getText(), nullptr, rows, cols);
                break;
            
            default:
                var_node = nullptr;
                break;
        }

        return (AstNode*) addText(var_node, ctx);
    }
    
    virtual antlrcpp::Any visitList_literal(ExprParser::List_literalContext *ctx) {
        debug(""); 
        AstListLit* list_lit = new AstListLit();
        for(auto expr : ctx->expr()){
            AstExpr* member = get<AstExpr*, AstNode*>(visit(expr));
            list_lit->members.push_back(member);
        }
        return (AstNode*) addText(list_lit, ctx);
    }
    
    virtual antlrcpp::Any visitList_expr(ExprParser::List_exprContext *ctx) {
        debug(""); 
        if(ctx->list_literal())
            return visit(ctx->list_literal());
        return visit(ctx->expr());
    }
    
    virtual antlrcpp::Any visitCapturing_method_decl(ExprParser::Capturing_method_declContext *ctx) {
        debug(""); 
        ast::Type return_type = ast::Type::VOID;
        if(ctx->TYPE()) return_type = toType(ctx->TYPE()->getText());
        std::string name = ctx->NAME()->getText();
        AstScopeBlock* body = get<AstScopeBlock*, AstNode*>(visit(ctx->scope_block()));
        AstMethodDecl* method_decl = new AstMethodDecl(return_type, name, body);

        for(auto arg : ctx->method_arg()){
            method_decl->args.push_back(get<AstVar*, AstNode*>(visit(arg)));
        }
        for(auto arg : ctx->captured_arg()){
            method_decl->args_by_ref.push_back(get<AstVar*, AstNode*>(visit(arg)));
        } 

        return (AstNode*) addText(method_decl, ctx);
    }
    
    antlrcpp::Any visitMethod_arg(ExprParser::Method_argContext *ctx) {
        debug(""); 
        ast::Type var_type = toType(ctx->TYPE()->getText());
        AstVar* var = get<AstVar*, AstNode*>(visit(ctx->var()));
        var->type = var_type;
        return (AstNode*) addText(var, ctx);
    }

    antlrcpp::Any visitCaptured_arg(ExprParser::Captured_argContext *ctx) {
        debug(""); 
        ast::Type var_type;
        if(ctx->TYPE()) var_type = toType(ctx->TYPE()->getText());
        else var_type = ast::Type::STREAM;
        AstVar* var = get<AstVar*, AstNode*>(visit(ctx->var()));
        var->type = var_type;
        return (AstNode*) addText(var, ctx);
    }
    
    antlrcpp::Any visitCapturing_method_call(ExprParser::Capturing_method_callContext *ctx) {
        debug(""); 
        std::string name = ctx->NAME()->getText();
        AstMethodCall* method_call = new AstMethodCall(name);
        for(auto expr : ctx->method_call_args()->list_expr()){
            method_call->args.push_back(get<AstListExpr*, AstNode*>(visit(expr)));
        }
        if(ctx->capturing_method_call_args()){
            for(auto var : ctx->capturing_method_call_args()->var()){
               method_call->args_by_ref.push_back(get<AstVar*, AstNode*>(visit(var)));
            }
        }

        return (AstNode*) addText(method_call, ctx);
    }
    
    antlrcpp::Any visitMethod_call_args(ExprParser::Method_call_argsContext *ctx) {
        debug(""); return nullptr;
    }
    
    antlrcpp::Any visitCapturing_method_call_args(ExprParser::Capturing_method_call_argsContext *ctx) {
        debug(""); 
        return nullptr;
    }
    
    antlrcpp::Any visitIf_statement(ExprParser::If_statementContext *ctx) {
        debug(""); return nullptr;
    }
    
    antlrcpp::Any visitStream_decl(ExprParser::Stream_declContext *ctx) {
        std::string name = ctx->NAME(0)->getText();
        std::string file_name = ctx->STRING()->getText();
        AstCharLit* char_lit = new AstCharLit(ctx->CHAR_LITERAL()->getText()[0]);
        AstVar *size = new AstVar(ast::Type::DEFAULT_TYPE, ctx->NAME(1)->getText());
        AstVar *cur_pos = new AstVar(ast::Type::DEFAULT_TYPE, ctx->NAME(2)->getText());
        debug("");

        return (AstNode*) addText(new AstStream(name, file_name, char_lit, size, cur_pos), ctx);
    }
    
    antlrcpp::Any visitStream(ExprParser::StreamContext *ctx) {
        debug("");
        if(ctx->stream_decl()) return visit(ctx->stream_decl());
        return (AstNode*) addText(new AstStream(ctx->NAME()->getText()), ctx);
    }
    
    antlrcpp::Any visitInputIo(ExprParser::InputIoContext *ctx) {
        debug(""); 
        AstStream* stream = get<AstStream*, AstNode*>(visit(ctx->stream()));
        
        AstVar* var;
        if(ctx->var_decl()) 
            var = get<AstVarDeclStatement*, AstNode*>(visit(ctx->var_decl()))->var_decls[0];
        else var = get<AstVar*, AstNode*>(visit(ctx->var()));
        
        return (AstNode*) addText(new AstInputIoStatement(stream, var), ctx);
    }

    antlrcpp::Any visitOutputIo(ExprParser::OutputIoContext *ctx) {
        debug(""); 
        AstStream* stream = get<AstStream*, AstNode*>(visit(ctx->stream()));
        AstListExpr* list_expr = get<AstListExpr*, AstNode*>(visit(ctx->list_expr()));
        return (AstNode*) addText(new AstOutputIoStatement(stream, list_expr), ctx);
    }

    virtual antlrcpp::Any visitParanthesisExpr(ExprParser::ParanthesisExprContext *ctx) {
        debug(""); 
        return visit(ctx->expr());
    }

    virtual antlrcpp::Any visitVariableExpr(ExprParser::VariableExprContext *ctx) {
        debug(""); 
        return visit(ctx->var());
    }

    virtual antlrcpp::Any visitBinExpr(ExprParser::BinExprContext *ctx) {
        ast::OpType op = ast::OpType::DEFAULT_OP;
        if(ctx->op->getText() == "+") op = ast::OpType::ADD;
        if(ctx->op->getText() == "-") op = ast::OpType::SUB;
        if(ctx->op->getText() == "*") op = ast::OpType::MUL;
        if(ctx->op->getText() == "/") op = ast::OpType::DIV;
        if(ctx->op->getText() == "%") op = ast::OpType::MOD;

        if(ctx->op->getText() == "==") op = ast::OpType::EQ;
        if(ctx->op->getText() == "!=") op = ast::OpType::NEQ;
        if(ctx->op->getText() == ">=") op = ast::OpType::GEQ;
        if(ctx->op->getText() == "<=") op = ast::OpType::LEQ;
        if(ctx->op->getText() == ">") op = ast::OpType::GT;
        if(ctx->op->getText() == "<") op = ast::OpType::LT;

        if(ctx->op->getText() == "and") op = ast::OpType::AND;
        if(ctx->op->getText() == "or") op = ast::OpType::OR;

        AstExpr* lhs = get<AstExpr*, AstNode*>(visit(ctx->expr(0)));
        AstExpr* rhs = get<AstExpr*, AstNode*>(visit(ctx->expr(1)));
        return (AstNode*) addText(new AstBinExpr(op, lhs, rhs), ctx);
    }
    
    virtual antlrcpp::Any visitTernaryExpr(ExprParser::TernaryExprContext *ctx) {
        debug(""); 
        AstExpr* first = get<AstExpr*, AstNode*>(visit(ctx->expr(0)));
        AstExpr* second = get<AstExpr*, AstNode*>(visit(ctx->expr(1)));
        AstExpr* third = get<AstExpr*, AstNode*>(visit(ctx->expr(2)));

        return (AstNode*) addText(new AstTernaryExpr(first, second, third), ctx);
    }

    virtual antlrcpp::Any visitCapturingMethodCallExpr(ExprParser::CapturingMethodCallExprContext *ctx) {
        debug(""); 
        return visit(ctx->capturing_method_call());
    }
    
    virtual antlrcpp::Any visitCharLiteralExpr(ExprParser::CharLiteralExprContext *ctx) {
        return (AstNode*) addText(new AstCharLit(ctx->CHAR_LITERAL()->getText()[1]), ctx);
    }

    virtual antlrcpp::Any visitBoolLiteralExpr(ExprParser::BoolLiteralExprContext *ctx) {
        debug(""); 
        if(ctx->BOOL_LITERAL()->getText() == "True") return (AstNode*) new AstBoolLit(true);
        return (AstNode*) addText(new AstBoolLit(false), ctx);
    }

    virtual antlrcpp::Any visitFloatLiteralExpr(ExprParser::FloatLiteralExprContext *ctx){
        debug("");
        return (AstNode*) addText(new AstFloatLit(std::stof(ctx->FLOAT_LITERAL()->getText())), ctx);
    };
};


#endif
