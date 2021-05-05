#ifndef CODEGENUTILS_H
#define CODEGENUTILS_H

#include "LLVMConstructs.h"
#include "ast/Types.h"

namespace codegen{
    LLVMConstructs& get(){
        return LLVMConstructsSingleton::getInstance();
    }

    namespace Type{
        llvm::Type* BOOL = llvm::Type::getInt1Ty(*codegen::get().context);
        llvm::Type* CHAR = llvm::Type::getInt8Ty(*codegen::get().context);
        llvm::Type* INT16 = llvm::Type::getInt16Ty(*codegen::get().context);
        llvm::Type* INT32 = llvm::Type::getInt32Ty(*codegen::get().context);
        llvm::Type* FLOAT = llvm::Type::getFloatTy(*codegen::get().context);
        llvm::Type* VOID = llvm::Type::getVoidTy(*codegen::get().context);

        llvm::Type* get(ast::Type type){
            if(type == ast::Type::BOOL) return BOOL;
            if(type == ast::Type::CHAR) return CHAR;
            if(type == ast::Type::INT) return INT32;
            if(type == ast::Type::UINT) return INT32;
            if(type == ast::Type::FLOAT) return FLOAT;
            return VOID;
        }
    };
}


#endif