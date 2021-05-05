cmake_minimum_required(VERSION 3.0.0)
project(minic VERSION 0.1.0)

include(parser/CMakeLists.txt)
include_directories(/usr/local/include/antlr4-runtime parser includes)

add_executable(minic main.cpp)
target_link_libraries(minic antlr4-runtime parser_lib)
find_program(BASH bash)

execute_process(COMMAND bash -c "llvm-config --cppflags" OUTPUT_VARIABLE LLVM_CXXFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE "\n" " " LLVM_CXXFLAGS "${LLVM_CXXFLAGS}")
string(REPLACE " " ";" LLVM_CXXFLAGS "${LLVM_CXXFLAGS}")
target_compile_options(minic PUBLIC ${LLVM_CXXFLAGS})

execute_process(COMMAND bash -c "llvm-config --ldflags --libs core --system-libs" OUTPUT_VARIABLE LLVM_LINK_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE "\n" " " LLVM_LINK_FLAGS "${LLVM_LINK_FLAGS}")
string(REPLACE " " ";" LLVM_LINK_FLAGS "${LLVM_LINK_FLAGS}")
target_link_libraries(minic ${LLVM_LINK_FLAGS})

target_compile_definitions(minic PUBLIC DEBUG) #comment me to turn OFF debugging

enable_testing()
if(BASH)
    add_test(parsing_test ${BASH} ${CMAKE_CURRENT_SOURCE_DIR}/tests/parser_test.sh)
endif(BASH)