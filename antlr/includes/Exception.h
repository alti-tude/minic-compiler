#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include "exception"
#include "string"

#include "Utilities.h"

#define THROW(T) throw T(truncateFilename(__FILE__), __LINE__)
#define THROW_CUSTOM(T, ...) throw T(truncateFilename(__FILE__), __VA_ARGS__)

#define ASSERT_NM(cond, T) if(!(cond)) throw T(truncateFilename(__FILE__), __LINE__)
#define ASSERT(cond, construct_name, message) if(!(cond)) \
    throw UserException(truncateFilename(__FILE__), construct_name, message)
#define ASSERT_FUNC(cond, message) if(!(cond)) throw UserException(__FUNCTION__, message)


class BaseException : public std::exception {
private:
    std::string msg;
public:
    BaseException(){}

    BaseException(std::string msg)
        :msg(msg){}
    
    BaseException(std::string function, std::string msg)
        :msg("(" + function + "): " + msg){}

    BaseException(std::string phase, std::string function, std::string msg)
        :msg("["+ phase +"] "+"(" + function + "): " + msg){}
           
    virtual const char* what() const noexcept{
        return msg.c_str();
    }
};

class UserException : public BaseException {
public:
    UserException()
        :BaseException(){}
    
    UserException(std::string msg)
        :BaseException(msg){}
    
    UserException(std::string function, std::string msg)
        :BaseException(function, msg){}

    UserException(std::string phase, std::string function, std::string msg)
        :BaseException(phase, function, msg){}
};

class SymbolTableException : BaseException {
public:
    SymbolTableException(std::string file, int line_number)
        :BaseException(file, std::to_string(line_number), "SymbolTableException") {}
};

class SymbolTableVarNotFoundException : SymbolTableException {
public:
    SymbolTableVarNotFoundException(std::string file, int line_number)
        :SymbolTableException(file, line_number) {}
};

class SymbolTableFunctionNotFoundException : SymbolTableException {
public:
    SymbolTableFunctionNotFoundException(std::string file, int line_number)
        :SymbolTableException(file, line_number) {}
};

class SymbolTableVarReDeclarationException : SymbolTableException {
public:
    SymbolTableVarReDeclarationException(std::string file, int line_number)
        :SymbolTableException(file, line_number) {}
};


class SymbolTableFunctionReDeclarationException : SymbolTableException {
public:
    SymbolTableFunctionReDeclarationException(std::string file, int line_number)
        :SymbolTableException(file, line_number) {}
};
#endif