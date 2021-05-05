set(ANTLR4 "java -jar /usr/local/lib/antlr-4.8-complete.jar")
add_custom_command(OUTPUT 
    ${CMAKE_CURRENT_LIST_DIR}/ExprBaseVisitor.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/ExprLexer.cpp  
    ${CMAKE_CURRENT_LIST_DIR}/ExprParser.cpp  
    ${CMAKE_CURRENT_LIST_DIR}/ExprVisitor.cpp
    COMMAND bash -c "${ANTLR4} -Dlanguage=Cpp -no-listener -visitor -o ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_SOURCE_DIR}/Expr.g4"
)
add_custom_target(
    generate_parser 
    DEPENDS 
    ${CMAKE_CURRENT_LIST_DIR}/ExprBaseVisitor.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/ExprLexer.cpp  
    ${CMAKE_CURRENT_LIST_DIR}/ExprParser.cpp  
    ${CMAKE_CURRENT_LIST_DIR}/ExprVisitor.cpp
)

add_library(parser_lib STATIC 
    ${CMAKE_CURRENT_LIST_DIR}/ExprBaseVisitor.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/ExprLexer.cpp  
    ${CMAKE_CURRENT_LIST_DIR}/ExprParser.cpp  
    ${CMAKE_CURRENT_LIST_DIR}/ExprVisitor.cpp
)
add_dependencies(parser_lib generate_parser)