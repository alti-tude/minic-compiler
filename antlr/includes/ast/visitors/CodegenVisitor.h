#ifndef CODEGENVISITOR_H
#define CODEGENVISITOR_H

#include "AstVisitorBase.h"
#include "ast/ast.h"
#include "codegen/LLVMConstructs.h"
#include "codegen/CodegenUtils.h"

#include "string"

void createIOFunction(std::string name){
    std::vector<llvm::Type*> param_types = {codegen::Type::CHAR->getPointerTo()};

    auto function_type = llvm::FunctionType::get(
        codegen::Type::INT32, 
        param_types, 
        true
    );

    auto function = llvm::Function::Create(
        function_type, 
        llvm::GlobalValue::LinkageTypes::ExternalLinkage, 
        name, 
        *codegen::get().module
    );
}

llvm::Value* indexInto(std::string name, int idx1){
    auto var = codegen::get().getVar(name);
    auto idx = llvm::ConstantInt::get(codegen::Type::INT32, idx1);
    std::vector<llvm::Value*> idxs{idx};
    return codegen::get().builder->CreateLoad(codegen::get().builder->CreateGEP(var, idxs));
}

class CodeGenVisitor : public AstVisitorBase {
private:
    llvm::Value *return_value;
    llvm::Function *return_function;
    llvm::Constant *return_constant;

    bool return_found = false;

public:
    void visit(AstProg& node) {
        createIOFunction("printf");
        createIOFunction("scanf");

        node.scope_block->accept(*this);
    }

    void visit(AstScopeBlock& node) {
        codegen::get().enterScope();

        debugArg(node.text);
        for(auto statement : node.statements){
            statement->accept(*this);
        } 

        codegen::get().exitScope();
    }

    void visit(AstVarDeclStatement& node) {
        debugArg("");
        for(auto var_decl : node.var_decls){
            var_decl->accept(*this);
        }
    }

    void visit(AstWhile& node) {debugArg("");}
    void visit(AstFor& node) {
        codegen::get().enterScope();
        node.iterator->expr = node.start;
        node.iterator->accept(*this);
        auto iterator_ptr = (llvm::Value*) codegen::get().getVar(node.iterator->name);

        auto function = codegen::get().builder->GetInsertBlock()->getParent();
        auto loop_condnBB = llvm::BasicBlock::Create(*codegen::get().context, "loop_condn", function);
        codegen::get().builder->CreateBr(loop_condnBB);
        codegen::get().builder->SetInsertPoint(loop_condnBB);

        node.end->accept(*this);
        auto end = return_value;
        llvm::Value* condn;

        auto iterator = codegen::get().builder->CreateLoad(iterator_ptr);
        if(node.iterator->type != ast::Type::FLOAT) condn = codegen::get().builder->CreateICmpSLT(iterator, end);
        else condn = codegen::get().builder->CreateFCmpOLT(iterator, end);
        
        auto loop_bodyBB = llvm::BasicBlock::Create(*codegen::get().context, "loop_body");
        auto mergeBB = llvm::BasicBlock::Create(*codegen::get().context, "merge");

        codegen::get().builder->CreateCondBr(condn, loop_bodyBB, mergeBB);
        function->getBasicBlockList().push_back(loop_bodyBB);
        codegen::get().builder->SetInsertPoint(loop_bodyBB);
        iterator = codegen::get().builder->CreateLoad(iterator_ptr);

        node.scope_block->accept(*this);
        
        llvm::Value* step;
        if(node.end->type != ast::Type::FLOAT) step = llvm::ConstantInt::get(codegen::Type::get(node.iterator->type), 1);
        else step = llvm::ConstantFP::get(codegen::Type::get(node.iterator->type), 1);

        if(node.step){
            node.step->accept(*this);
            step = return_value;
        }
        if(node.end->type != ast::Type::FLOAT) codegen::get().builder->CreateStore(codegen::get().builder->CreateAdd(iterator, step), iterator_ptr);
        else codegen::get().builder->CreateStore(codegen::get().builder->CreateFAdd(iterator, step), iterator_ptr);

        codegen::get().builder->CreateBr(loop_condnBB);

        function->getBasicBlockList().push_back(mergeBB);
        codegen::get().builder->SetInsertPoint(mergeBB);

        codegen::get().exitScope();
        debugArg("");
    }
    
    void visit(AstIfStatement& node) {
        auto function = codegen::get().builder->GetInsertBlock()->getParent();

        auto thenBB = llvm::BasicBlock::Create(*codegen::get().context, "start_if");
        auto mergeBB = llvm::BasicBlock::Create(*codegen::get().context, "merge");

        int i = 1;
        // for( i = 1; i < node.conditions.size(); i++){
        //     auto condn = node.conditions[i-1];
        //     condn->accept(*this);
        //     auto condn_value = return_value;
        //     auto current_elseBB = llvm::BasicBlock::Create(*codegen::get().context, std::to_string(i)+"_if");

        //     codegen::get().builder->CreateCondBr(condn_value, thenBB, current_elseBB);

        //     function->getBasicBlockList().push_back(thenBB);
        //     codegen::get().builder->SetInsertPoint(thenBB);

        //     node.actions[i-1]->accept(*this);
            
        //     thenBB = codegen::get().builder->GetInsertBlock();
        //     if(!thenBB->getTerminator()) codegen::get().builder->CreateBr(mergeBB);
            
        //     thenBB = current_elseBB;
            
        // }

        auto elseBB = llvm::BasicBlock::Create(*codegen::get().context, "else");
        if(node.conditions.size() == node.actions.size())
            elseBB = mergeBB;

        auto condn = node.conditions[i-1];
        condn->accept(*this);
        auto condn_value = return_value;

        codegen::get().builder->CreateCondBr(condn_value, thenBB, elseBB);

        function->getBasicBlockList().push_back(thenBB);
        codegen::get().builder->SetInsertPoint(thenBB);

        node.actions[i-1]->accept(*this);
        
        thenBB = codegen::get().builder->GetInsertBlock();
        if(!thenBB->getTerminator()) codegen::get().builder->CreateBr(mergeBB);

        if(elseBB != mergeBB){
            function->getBasicBlockList().push_back(elseBB);
            codegen::get().builder->SetInsertPoint(elseBB);

            node.actions[i]->accept(*this);
            
            elseBB = codegen::get().builder->GetInsertBlock();
            if(!elseBB->getTerminator()) codegen::get().builder->CreateBr(mergeBB);
        }

        function->getBasicBlockList().push_back(mergeBB);
        codegen::get().builder->SetInsertPoint(mergeBB);

        debugArg("");
    }
    
    void visit(AstBreak& node) {debugArg("");}
    void visit(AstContinue& node) {debugArg("");}

    void visit(AstReturn& node) {
        if(node.return_expr == nullptr)
            codegen::get().builder->CreateRetVoid();
        else{
            node.return_expr->accept(*this);
            codegen::get().builder->CreateRet(return_value);
        }

        debugArg(node.text);
    }
    
    void visit(AstMethodDecl& node) {
        std::vector<llvm::Type*> param_types;
        codegen::get().enterScope();

        
        for(auto arg : node.args)
            param_types.push_back(codegen::Type::get(arg->type));

        auto function_type = llvm::FunctionType::get(
            codegen::Type::get(node.return_type), 
            param_types, 
            false
        );

        auto function = llvm::Function::Create(
            function_type, 
            llvm::GlobalValue::LinkageTypes::ExternalLinkage, 
            node.name, 
            *codegen::get().module
        );
        llvm::BasicBlock *bb = llvm::BasicBlock::Create(*codegen::get().context, node.name + "_entry", function);
        codegen::get().builder->SetInsertPoint(bb);
        
        auto arg_it = function->arg_begin();

        for(int i = 0; arg_it != function->arg_end(); i++, arg_it++){
            arg_it->setName(node.args[i]->name);
            node.args[i]->accept(*this);
            auto alloca_arg = codegen::get().getVar(node.args[i]->name);
            codegen::get().builder->CreateStore(static_cast<llvm::Value*>(&*arg_it), alloca_arg);
        }
        
        node.body->accept(*this);

        if(codegen::get().builder->GetInsertBlock()->getTerminator() == nullptr)
            codegen::get().builder->CreateRetVoid();

        codegen::get().exitScope();
        debugArg("");
    }
    
    void visit(AstListLit& node) {debugArg("");}

    void visit(AstUnaryExpr& node) {
        node.expr->accept(*this);
        llvm::Value *operand = return_value;

        if(node.op == ast::OpType::NOT) 
            return_value = LLVMConstructsSingleton::getInstance().builder->CreateNot(operand, "unary_not");
        if(node.op == ast::OpType::SUB)
            if(node.type != ast::Type::FLOAT) return_value = LLVMConstructsSingleton::getInstance().builder->CreateNeg(operand, "unary_minus_i");
            else return_value = LLVMConstructsSingleton::getInstance().builder->CreateFNeg(operand, "unary_minus_f");
        
        debugArg("");
        debugIR(return_value);
    }
    
    void visit(AstBinExpr& node) {
        node.lhs->accept(*this);
        llvm::Value *lhs = return_value;

        node.rhs->accept(*this);
        llvm::Value *rhs = return_value;

        if(node.lhs->type != ast::Type::FLOAT){
            bool is_signed = (node.lhs->type == ast::Type::INT);
            switch (node.op)
            {
                #pragma region relation ops
                case ast::OpType::EQ :
                    return_value = LLVMConstructsSingleton::getInstance().builder->CreateICmpEQ(lhs, rhs, "rel_eq");
                    break;
                
                case ast::OpType::NEQ :
                    return_value = LLVMConstructsSingleton::getInstance().builder->CreateICmpNE(lhs, rhs, "rel_neq");
                    break;

                case ast::OpType::GEQ :
                    if(!is_signed) return_value = LLVMConstructsSingleton::getInstance().builder->CreateICmpUGE(lhs, rhs, "rel_geq_u");
                    else return_value = LLVMConstructsSingleton::getInstance().builder->CreateICmpSGE(lhs, rhs, "rel_geq_s");
                    break;
                
                case ast::OpType::GT :
                    if(!is_signed) return_value = LLVMConstructsSingleton::getInstance().builder->CreateICmpUGT(lhs, rhs, "rel_gt_u");
                    else return_value = LLVMConstructsSingleton::getInstance().builder->CreateICmpSGT(lhs, rhs, "rel_gt_s");
                    break;

                case ast::OpType::LEQ :
                    if(!is_signed) return_value = LLVMConstructsSingleton::getInstance().builder->CreateICmpULE(lhs, rhs, "rel_leq_u");
                    else return_value = LLVMConstructsSingleton::getInstance().builder->CreateICmpSLE(lhs, rhs, "rel_leq_s");
                    break;

                case ast::OpType::LT :
                    if(!is_signed) return_value = LLVMConstructsSingleton::getInstance().builder->CreateICmpULT(lhs, rhs, "rel_lt_u");
                    else return_value = LLVMConstructsSingleton::getInstance().builder->CreateICmpSLT(lhs, rhs, "rel_lt_s");
                    break;
                
                #pragma endregion

                #pragma region arithmetic ops
                case ast::OpType::ADD :
                    return_value = LLVMConstructsSingleton::getInstance().builder->CreateAdd(lhs, rhs, "add_i");
                    break;

                case ast::OpType::SUB :
                    return_value = LLVMConstructsSingleton::getInstance().builder->CreateSub(lhs, rhs, "sub_i");
                    break;

                case ast::OpType::MUL :
                    return_value = LLVMConstructsSingleton::getInstance().builder->CreateMul(lhs, rhs, "mul_i");
                    break;

                case ast::OpType::DIV :
                    if(!is_signed) return_value = LLVMConstructsSingleton::getInstance().builder->CreateUDiv(lhs, rhs, "div_u");
                    else return_value = LLVMConstructsSingleton::getInstance().builder->CreateSDiv(lhs, rhs, "div_s");
                    break;

                case ast::OpType::MOD :
                    if(!is_signed) return_value = codegen::get().builder->CreateURem(lhs, rhs, "u_rem");
                    else return_value = codegen::get().builder->CreateSRem(lhs, rhs, "s_rem");
                    break;

                #pragma endregion

                #pragma region bool ops
                case ast::OpType::AND :
                    return_value = LLVMConstructsSingleton::getInstance().builder->CreateAnd(lhs, rhs, "and");
                    break;
                
                case ast::OpType::OR :
                    return_value = LLVMConstructsSingleton::getInstance().builder->CreateOr(lhs, rhs, "or");
                    break;
                
                #pragma endregion

                default:
                    break;
            }
        }
        else {
            switch (node.op)
            {
                #pragma region relation ops
                case ast::OpType::EQ :
                    return_value = LLVMConstructsSingleton::getInstance().builder->CreateFCmpOEQ(lhs, rhs, "rel_eq_f");
                    break;
                
                case ast::OpType::NEQ :
                    return_value = LLVMConstructsSingleton::getInstance().builder->CreateFCmpONE(lhs, rhs, "rel_neq_f");
                    break;

                case ast::OpType::GEQ :
                    return_value = LLVMConstructsSingleton::getInstance().builder->CreateFCmpOGE(lhs, rhs, "rel_geq_f");
                    break;
                
                case ast::OpType::GT :
                    return_value = LLVMConstructsSingleton::getInstance().builder->CreateFCmpOGT(lhs, rhs, "rel_gt_f");
                    break;

                case ast::OpType::LEQ :
                    return_value = LLVMConstructsSingleton::getInstance().builder->CreateFCmpOLE(lhs, rhs, "rel_leq_f");
                    break;

                case ast::OpType::LT :
                    return_value = LLVMConstructsSingleton::getInstance().builder->CreateFCmpOLT(lhs, rhs, "rel_lt_f");
                    break;
                
                #pragma endregion

                #pragma region arithmetic ops
                case ast::OpType::ADD :
                    return_value = LLVMConstructsSingleton::getInstance().builder->CreateFAdd(lhs, rhs, "add_f");
                    break;

                case ast::OpType::SUB :
                    return_value = LLVMConstructsSingleton::getInstance().builder->CreateFSub(lhs, rhs, "sub_f");
                    break;

                case ast::OpType::MUL :
                    return_value = LLVMConstructsSingleton::getInstance().builder->CreateFMul(lhs, rhs, "mul_f");
                    break;

                case ast::OpType::DIV :
                    return_value = LLVMConstructsSingleton::getInstance().builder->CreateFDiv(lhs, rhs, "div_f");
                    break;

                #pragma endregion

                default:
                    break;
            }
        }

        debugArg(node.text);
        debugIR(return_value);
    }
    
    void visit(AstTernaryExpr& node) {debugArg("");}

    #pragma region LITERALS

    void visit(AstIntLit& node) {
        return_constant = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*LLVMConstructsSingleton::getInstance().context), node.value, true);
        return_value = return_constant;

        debugArg("");
        debugIR(return_value);
    }

    void visit(AstUIntLit& node) {
        return_constant = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*LLVMConstructsSingleton::getInstance().context), node.value, false);
        return_value = return_constant;

        debugArg("");
        debugIR(return_value);
    }

    void visit(AstCharLit& node) {
        return_constant = llvm::ConstantInt::get(llvm::Type::getInt8Ty(*LLVMConstructsSingleton::getInstance().context), node.value, false);
        return_value = return_constant;

        debugArg(node.value);
        debugIR(return_value);
    }

    void visit(AstBoolLit& node) {
        return_constant = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*LLVMConstructsSingleton::getInstance().context), node.value, false);
        return_value = return_constant;

        debugArg("");
        debugIR(return_value);
    }

    void visit(AstFloatLit& node) {
        return_constant = llvm::ConstantFP::get(llvm::Type::getFloatTy(*LLVMConstructsSingleton::getInstance().context), node.value);    
        return_value = return_constant;

        debugArg("");
        debugIR(return_value);
    }

    #pragma endregion

    void visit(AstVar& node) {
        auto variable = (llvm::Value*) codegen::get().getVar(node.name);

        bool is_global = false;
        if(variable == nullptr){
            variable = codegen::get().module->getGlobalVariable(node.name);
            if(variable) is_global = true;
        }

        if(variable == nullptr || node.expr){
            if(variable == nullptr){ //declaration
                if(node.is_array){
                    if(codegen::get().isGlobalScope()){
                        size_t size = ((AstIntLit*) node.rows)->value;
                        auto array_type = llvm::ArrayType::get(codegen::Type::get(node.type), size);

                        auto global_array = new llvm::GlobalVariable(
                            *codegen::get().module, 
                            array_type,
                            false, 
                            llvm::GlobalValue::ExternalLinkage,
                            NULL,
                            node.name
                        );
                        
                        auto zero = llvm::ConstantInt::get(codegen::Type::get(node.type), 0);
                        if(node.type == ast::FLOAT) zero = llvm::ConstantFP::get(codegen::Type::get(node.type), 0);

                        std::vector<llvm::Constant*> zeros(size, zero);
                        auto init = llvm::ConstantArray::get(array_type, zeros);
                        global_array->setInitializer(init);

                        variable = global_array;
                        is_global = true;
                    }
                    else{
                        node.rows->accept(*this);
                        variable = codegen::get().declVar(node.name, codegen::Type::get(node.type), return_value);
                    }
                }
                else{
                    if(codegen::get().isGlobalScope()){
                        auto type = codegen::Type::get(node.type);

                        auto global = new llvm::GlobalVariable(
                            *codegen::get().module, 
                            type,
                            false, 
                            llvm::GlobalValue::ExternalLinkage,
                            NULL,
                            node.name
                        );
                        
                        auto init = ((node.type != ast::Type::FLOAT) ? llvm::ConstantInt::get(type, 0) : llvm::ConstantFP::get(type, 0));
                        global->setInitializer(init);

                        variable = global;
                        is_global = true;
                    }
                    else{
                        variable = codegen::get().declVar(
                            node.name, codegen::Type::get(node.type), nullptr
                        );
                    }
                }
            }
            if(node.expr){ //assignment
                if(node.is_array) {
                    std::vector<llvm::Value*> idxs;
                    if(is_global) idxs.push_back(0);
                    node.rows->accept(*this);
                    idxs.push_back(return_value);
                    variable = codegen::get().builder->CreateInBoundsGEP(variable, idxs);
                }
                node.expr->accept(*this);
                codegen::get().builder->CreateStore(return_value, variable);
            }
        }
        else{
            if(node.is_array) {
                std::vector<llvm::Value*> idxs;
                if(is_global) idxs.push_back(0);
                node.rows->accept(*this);
                idxs.push_back(return_value);
                variable = codegen::get().builder->CreateInBoundsGEP(variable, idxs);
            }

            return_value = codegen::get().builder->CreateLoad(variable);
        }
    }

    void visit(AstMethodCall& node) {
        auto function = codegen::get().module->getFunction(node.name);
        std::vector<llvm::Value*> function_args;
        for(auto arg : node.args){
            arg->accept(*this);
            function_args.push_back(return_value);
        }

        return_value = codegen::get().builder->CreateCall(function, function_args);
        debugArg(node.text);
    }
    void visit(AstStream& node) {debugArg("");}
    
    void visit(AstInputIoStatement& node) {
        debugArg("");
    
    }
    void visit(AstOutputIoStatement& node) {

        if(node.stream->name == "STDOUT"){
            auto function = codegen::get().module->getFunction("printf");
            node.list_expr->accept(*this);
            std::vector<llvm::Value*> args;

            if( return_value->getType() != codegen::Type::CHAR && 
                return_value->getType() != codegen::Type::FLOAT){
                args.push_back(codegen::get().builder->CreateGlobalStringPtr("%d"));
                args.push_back(return_value);
            }
            if(return_value->getType() == codegen::Type::CHAR){
                args.push_back(codegen::get().builder->CreateGlobalStringPtr("%c"));
                args.push_back(return_value);
            }
            if(return_value->getType() == codegen::Type::FLOAT){
                return_value = codegen::get().builder->CreateFPExt(return_value, codegen::get().builder->getDoubleTy());
                args.push_back(codegen::get().builder->CreateGlobalStringPtr("%f"));
                args.push_back(return_value);
            }

            codegen::get().builder->CreateCall(function, args);
        }

        debugArg(node.text);

    }
};

#endif