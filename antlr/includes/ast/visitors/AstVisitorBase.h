#ifndef ASTVISITORBASE_H
#define ASTVISITORBASE_H

#include "Utilities.h"

class AstNode;
class AstProg;
class AstScopeBlock;
class AstStatement;
class AstVarDeclStatement;
class AstWhile;
class AstFor;
class AstIfStatement;
class AstBreak;
class AstContinue;
class AstReturn;
class AstMethodDecl;
class AstListExpr;
class AstListLit;
class AstExpr;
class AstUnaryExpr;
class AstBinExpr;
class AstTernaryExpr;
class AstIntLit;
class AstUIntLit;
class AstCharLit;
class AstBoolLit;
class AstFloatLit;
class AstVar;
class AstMethodCall;
class AstStream;
class AstInputIoStatement;
class AstOutputIoStatement;

class AstVisitorBase{
public:
    virtual void visit(AstNode& node) {debugArg("");}
    virtual void visit(AstProg& node) {debugArg("");}
    virtual void visit(AstScopeBlock& node) {debugArg("");}
    virtual void visit(AstStatement& node) {debugArg("");}
    virtual void visit(AstVarDeclStatement& node) {debugArg("");}
    virtual void visit(AstWhile& node) {debugArg("");}
    virtual void visit(AstFor& node) {debugArg("");}
    virtual void visit(AstIfStatement& node) {debugArg("");}
    virtual void visit(AstBreak& node) {debugArg("");}
    virtual void visit(AstContinue& node) {debugArg("");}
    virtual void visit(AstReturn& node) {debugArg("");}
    virtual void visit(AstMethodDecl& node) {debugArg("");}
    virtual void visit(AstListExpr& node) {debugArg("");}
    virtual void visit(AstListLit& node) {debugArg("");}
    virtual void visit(AstExpr& node) {debugArg("");}
    virtual void visit(AstUnaryExpr& node) {debugArg("");}
    virtual void visit(AstBinExpr& node) {debugArg("");}
    virtual void visit(AstTernaryExpr& node) {debugArg("");}
    virtual void visit(AstIntLit& node) {debugArg("");}
    virtual void visit(AstUIntLit& node) {debugArg("");}
    virtual void visit(AstCharLit& node) {debugArg("");}
    virtual void visit(AstBoolLit& node) {debugArg("");}
    virtual void visit(AstFloatLit& node) {debugArg("");}
    virtual void visit(AstVar& node) {debugArg("");}
    virtual void visit(AstMethodCall& node) {debugArg("");}
    virtual void visit(AstStream& node) {debugArg("");}
    virtual void visit(AstInputIoStatement& node) {debugArg("");}
    virtual void visit(AstOutputIoStatement& node) {debugArg("");}
};

#endif
