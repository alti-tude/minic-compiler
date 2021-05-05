#ifndef UTILITIES_H
#define UTILITIES_H

#include "antlr4-runtime.h"

#include "iostream"
#include "string"

#ifdef DEBUG

#define debug(a) std::cout << __PRETTY_FUNCTION__  << ": " << (a) << std::endl
#define debugArg(a) std::cout << printArgType(__PRETTY_FUNCTION__) << ": " << (a) << std::endl
#define debugIR(value) value->print(llvm::errs(), true); llvm::errs() << "\n"

std::string printArgType(std::string func_name){
    size_t spos;
    spos = func_name.find('(');
    return func_name.substr(spos);    
}

#else

#define debug(a)
#define debugArg(a) 
#define debugIR(value)

#endif

std::string getClassName(std::string full_func_name, std::string func_name){
    size_t spos, epos;
    epos = full_func_name.find(func_name)-2;
    spos = full_func_name.substr(0, epos).find_last_of(' ')+1;
    return full_func_name.substr(spos, epos-spos);    
}

std::string truncateFilename(std::string file){
    size_t spos = file.find_last_of("/")+1;
    return file.substr(spos);
}

template<typename T, typename U> 
T get(antlrcpp::Any input){
    return dynamic_cast<T>((U)(input.as<U>()));
}

#endif
