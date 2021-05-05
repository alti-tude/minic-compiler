#ifndef LLVMCONSTRUCTS_H
#define LLVMCONSTRUCTS_H

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

#include "memory"
#include "stack"
#include "map"

#include "mixins/SingletonMixin.h"

class LLVMConstructs {
protected:
    LLVMConstructs(){
        context = std::make_unique<llvm::LLVMContext>();
        builder = std::make_unique<llvm::IRBuilder<>>(*context);
        module = std::make_unique<llvm::Module>("minic", *context);

        scope_count = 0;
    }
    virtual ~LLVMConstructs(){}
    unsigned int scope_count;
    
    std::map<std::string, std::stack<llvm::AllocaInst*> > allocation_map;
    std::stack<std::vector<std::string> > scope_vars;

public:

    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> module;

    void enterScope(){ 
        scope_vars.push(std::vector<std::string>());
    }

    void exitScope(){ 
        for(auto var_name : scope_vars.top()){
            allocation_map[var_name].pop();

            if(allocation_map[var_name].empty())
                allocation_map.erase(var_name);
        }

        scope_vars.pop();
    }

    bool isGlobalScope(){
        return scope_vars.size() == 0;
    }

    void addVar(std::string var_name, llvm::AllocaInst* alloca_inst){
        std::cout << var_name << std::endl;
        scope_vars.top().push_back(var_name);
        allocation_map[var_name].push(alloca_inst);
    }

    llvm::AllocaInst* getVar(std::string var_name){
        if(allocation_map.find(var_name) == allocation_map.end()){
            return nullptr;
        }
        return allocation_map[var_name].top();        
    }

    llvm::AllocaInst* declVar(std::string var_name, llvm::Type* type, llvm::Value* array_size = nullptr){
        auto alloca_inst = builder->CreateAlloca(type, array_size, var_name);
        addVar(var_name, alloca_inst);
        return alloca_inst;
    }
};

using LLVMConstructsSingleton = Singleton<LLVMConstructs>;

#endif