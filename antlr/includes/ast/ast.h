#ifndef AST_H
#define AST_H

#include "iostream"
#include "vector"
#include "string"

#include "ast/Types.h"
#include "visitors/AstVisitorBase.h"

class AstNode{
public:
    std::string text;

    virtual ~AstNode(){}
    virtual void accept(AstVisitorBase& visitor) = 0;
};

class AstProg : virtual public AstNode {
private:
public:
    AstScopeBlock* scope_block;

    AstProg(AstScopeBlock* _scope_block)
        :scope_block(_scope_block) {}

    virtual void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}
};

class AstScopeBlock : virtual public AstNode{
private:
public:
    std::vector<AstStatement*> statements;

    virtual void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}
};

class AstStatement : virtual public AstNode{
public:
    virtual ~AstStatement(){}
    virtual void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}
};

class AstVarDeclStatement : public AstStatement {
private:
public:
    std::vector<AstVar*> var_decls;
    void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}
};

class AstWhile : public AstStatement {
private:
public:
    AstExpr* expr;
    AstScopeBlock* scope_block;
    
    AstWhile(AstExpr* _expr, AstScopeBlock* _scope_block)
        :expr(_expr), scope_block(_scope_block) {}

    void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}
};

class AstFor : public AstStatement {
public:
    AstVar* iterator;
    AstListLit* list_lit=nullptr;
    AstExpr *start=nullptr, *end=nullptr, *step=nullptr;
    AstScopeBlock* scope_block;

    AstFor(AstVar* iterator, AstScopeBlock* scope_block, AstListLit* list_lit)
        :iterator(iterator), scope_block(scope_block), list_lit(list_lit) {}
    
    AstFor(AstVar* iterator, AstScopeBlock* scope_block, AstExpr* start, AstExpr* end)
        :iterator(iterator), scope_block(scope_block), start(start), end(end) {}
    
    AstFor(AstVar* iterator, AstScopeBlock* scope_block, AstExpr* start, AstExpr* end, AstExpr* step)
        :iterator(iterator), scope_block(scope_block), start(start), end(end), step(step) {}
    
    void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}
};

class AstIfStatement : public AstStatement {
public:
    std::vector<AstExpr*> conditions;
    std::vector<AstScopeBlock*> actions;
    
    AstIfStatement(){}
    void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}
};

class AstBreak : public AstStatement {
public:
    AstBreak(){}
    void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}
};

class AstContinue : public AstStatement {
public:
    AstContinue(){}
    void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}
};

class AstReturn : public AstStatement {
public:
    AstListExpr* return_expr;

    AstReturn(AstListExpr* _return_expr)
        :return_expr(_return_expr) {}

    void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}
};

class AstMethodDecl : public AstStatement {
public:
    ast::Type return_type;
    std::string name;
    std::vector<AstVar*> args;
    std::vector<AstVar*> args_by_ref;
    AstScopeBlock* body;

    AstMethodDecl(){}
    AstMethodDecl(ast::Type _return_type, std::string _name, AstScopeBlock* _body)
        :return_type(_return_type), name(_name), body(_body) {}

    void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}
};

class AstListExpr : virtual public AstNode {
public:
    virtual void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}
};

class AstListLit : public AstListExpr {
public:
    ast::Type type = ast::Type::DEFAULT_TYPE;
    std::vector<AstExpr*> members;
    void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}
};

class AstExpr : public AstListExpr {
public:
    ast::Type type;

    virtual void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}
};

class AstUnaryExpr : public AstExpr {
private:
public:
    ast::OpType op;
    AstExpr* expr;

    AstUnaryExpr(ast::OpType _op, AstExpr* _expr)
        :op(_op), expr(_expr) {}

    void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}
};

class AstBinExpr : public AstExpr {
public:
    ast::OpType op;
    AstExpr* lhs;
    AstExpr* rhs;

    AstBinExpr(ast::OpType _op, AstExpr* _lhs, AstExpr* _rhs)
        :op(_op), lhs(_lhs), rhs(_rhs) {}
    
    void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}
};

class AstTernaryExpr : public AstExpr {
public:
    AstExpr* first;
    AstExpr* second;
    AstExpr* third;

    AstTernaryExpr(AstExpr* _first, AstExpr* _second, AstExpr* _third)
        :first(_first), second(_second), third(_third) {}
        
    void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}
};

class AstIntLit : public AstExpr {
public:
    int value;
    AstIntLit(int _value)
        :value(_value) {}

    void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}
};

class AstUIntLit : public AstExpr {
public:
    unsigned int value;
    AstUIntLit(unsigned int _value)
        :value(_value) {}

    void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}
};

class AstCharLit : public AstExpr{
public:
    char value;
    AstCharLit(char _value)
        :value(_value) {}
    
    void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}
};

class AstBoolLit : public AstExpr {
public:
    bool value;

    AstBoolLit(bool _value)
        :value(_value){}

    void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}
};

class AstFloatLit : public AstExpr {
public:
    float value;
    AstFloatLit(float _value)
        :value(_value) {}

    void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}
};

class AstVar : public AstExpr, public AstStatement {
public:
    ast::Type type;
    std::string name;
    AstExpr* expr;
    AstExpr* rows;
    AstExpr* cols;
    bool is_array;

    AstVar(ast::Type _type, std::string _name) 
        :type(_type), name(_name), expr(nullptr), rows(new AstUIntLit(1)), cols(new AstUIntLit(1)), is_array(false) {}

    AstVar(ast::Type _type, std::string _name, AstExpr* _expr) 
        :type(_type), name(_name), expr(_expr), rows(new AstUIntLit(1)), cols(new AstUIntLit(1)), is_array(false) {}
    
    AstVar(ast::Type _type, std::string _name, AstExpr* _expr, AstExpr* _rows) 
        :type(_type), name(_name), expr(_expr), rows(_rows), cols(new AstUIntLit(1)), is_array(true) {}

    AstVar(ast::Type _type, std::string _name, AstExpr* _expr, AstExpr* _rows, AstExpr* _cols) 
        :type(_type), name(_name), expr(_expr), rows(_rows), cols(_cols), is_array(true) {}
    
    void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}

};

class AstMethodCall : public AstExpr, public AstStatement {
public:
    ast::Type return_type = ast::Type::DEFAULT_TYPE;
    std::string name;
    std::vector<AstListExpr*> args;
    std::vector<AstVar*> args_by_ref;
    std::vector<ast::Type> args_types;
    std::vector<ast::Type> args_by_ref_types;
    
    AstMethodCall(std::string _name)
        :name(_name) {}

    void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}
};

class AstStream : public AstVar {
public:    
    std::string file_name;
    AstCharLit* attr; //r for read, w for write etc
    AstVar* size;
    AstVar* cur_pos;

    AstStream(std::string _name, std::string _file_name, AstCharLit* _attr, AstVar* _size, AstVar* _cur_pos)
        :AstVar(ast::Type::STREAM, _name), file_name(_file_name), attr(_attr), size(_size), cur_pos(_cur_pos) {}
    
    AstStream(std::string _name)
        :AstVar(ast::Type::STREAM, _name), file_name(""), attr(nullptr), size(nullptr), cur_pos(nullptr) {}

    void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}
};

class AstInputIoStatement : public AstStatement {
public:
    AstStream* stream;
    AstVar* var;  

    AstInputIoStatement(AstStream* _stream, AstVar* _var)
        :stream(_stream), var(_var){}

    virtual void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}
};

class AstOutputIoStatement : public AstStatement {
public:
    AstStream* stream;
    AstListExpr* list_expr;

    AstOutputIoStatement(AstStream* _stream, AstListExpr* _list_expr)
        :stream(_stream), list_expr(_list_expr) {}

    virtual void accept(AstVisitorBase& visitor) override {visitor.visit(*this);}
};


#endif
