grammar Expr;

prog: scope_block EOF;

scope_block: '{' statement* '}';

statement: io ';'                                                                               #ioStatement
         | var_decl ';'                                                                         #varDeclStatement
         | var_init ';'                                                                         #varInitStatement
         | method_decl                                                                          #methodDeclStatement
         | capturing_method_decl                                                                #capturingMethodDeclStatement
         | if_statement ('else' if_statement)* ('else' scope_block)?                            #ifStatement
         | 'while' '(' expr ')' scope_block                                                     #whileStatement
         | 'for' '(' (TYPE)? NAME ':' (list_literal|expr',' expr(','expr)?) ')' scope_block     #forStatement
         | 'break' ';'                                                                          #break
         | 'continue' ';'                                                                       #continue
         | 'return' (expr)? ';'                                                                 #return
         | method_call ';'                                                                      #methodCallStatement
         | capturing_method_call ';'                                                            #capturingMethodCallStatement
         ;

var_decl: TYPE ((var_init|var)',')*(var_init|var);
var: NAME | NAME'['expr']' | NAME'['expr']''['expr']';
var_init: NAME '=' expr | NAME'['expr']' '=' (list_literal|expr) | NAME'[' expr ']' '[' expr ']' '=' (list_literal|expr);

list_literal: '['(((expr | list_literal)',')*(expr | list_literal))?']';

method_decl: (TYPE|'void') NAME '(' ((method_arg',')*(method_arg))? ')' scope_block;
capturing_method_decl: (TYPE|'void') NAME '(' ((method_arg',')*(method_arg))? ')' '[' (captured_arg',')*(captured_arg) ']' scope_block;
method_arg: TYPE var;
captured_arg: (TYPE|'file') var;

method_call: NAME'(' method_call_args ')';
capturing_method_call: NAME'(' method_call_args ')' '['capturing_method_call_args']';
method_call_args: ((expr',')*(expr))?;
capturing_method_call_args: (var',')*var;

if_statement: 'if' '(' expr ')' scope_block;

stream: STANDARD_STREAMS
      | 'file' NAME '(' STRING ',' (CHAR_LITERAL|NAME) ')' '[' NAME ',' NAME']'
      | NAME;
io: stream '>>' (var_decl | var) | stream '<<' (expr | list_literal) | stream;

expr: '-' expr
    | 'not' expr
    | expr ('/' | '*' | '%') expr
    | expr ('+' | '-') expr
    | expr ('>='| '<=' | '>' | '<') expr
    | expr ('==' | '!=' ) expr
    | expr BOOL_OP expr
    | '(' expr ')'
    | CHAR_LITERAL | BOOL_LITERAL | INT_LITERAL
    | var | method_call | capturing_method_call
    | expr '?' expr ':' expr
    ;

/*Tokens*/
TYPE: 'int' | 'uint' | 'char' | 'bool';
BOOL_OP: 'or' | 'and';
NAME  : [a-zA-Z][a-zA-Z0-9_]*;
CHAR_LITERAL : '\'' ('\\'. | ~('\'')) '\'';
BOOL_LITERAL : 'True' | 'False';
INT_LITERAL : [0-9]+;
STANDARD_STREAMS: 'STDIN' | 'STDOUT';
STRING: '"' (~('"'))* '"';
COMMENT : '//' ~[\r\n]*->skip;
NS : [ \t\r\n]+ -> skip; 