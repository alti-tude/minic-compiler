#include "antlr4-runtime.h"
#include "ExprLexer.h"
#include "ExprParser.h"


#include "ast/visitors/TypeCheckingVisitor.h"
#include "ast/visitors/DeclarationCheckingVisitor.h"
#include "ast/visitors/CodegenVisitor.h"
#include "ast/ast.h"
#include "parse_tree/BuildAstVisitor.h"
#include "symbol_table/SymbolTable.h"
#include "Utilities.h"
#include "Exception.h"

using namespace antlr4;

int main(int argc, const char* argv[]) {
    std::ifstream stream;

    std::cout << "Processing input file " << argv[1] << std::endl;
    stream.open(argv[1]);
    
    ANTLRInputStream input(stream); 

    ExprLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    ExprParser parser(&tokens);    


    ExprParser::ProgContext *ctx = parser.prog();
    BuildAstVisitor visitor = BuildAstVisitor();
    AstProg* ast = get<AstProg*, AstNode*>(ctx->accept(&visitor));

    try{
        std::cout << "declaration checking ====================\n";
        DeclarationCheckingVisitor decl_visitor = DeclarationCheckingVisitor();
        ast->accept(decl_visitor);
        std::cout << "typecheking ====================\n";
        TypeCheckingVisitor ast_visitor = TypeCheckingVisitor();
        ast->accept(ast_visitor);
        std::cout << "codegen ====================\n";
        CodeGenVisitor codegen_visitor = CodeGenVisitor();
        ast->accept(codegen_visitor);
        std::error_code err;
        std::string file = "tmp.ir";
        auto fd = new llvm::raw_fd_ostream(
                llvm::StringRef(file), 
                err
            );
        LLVMConstructsSingleton::getInstance().module->print(*fd, nullptr);
    } catch (UserException& exception){
        std::cerr << exception.what() << std::endl;
        exit(1);
    }

    return 0;
}