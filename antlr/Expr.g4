grammar Expr;

prog: (scope_block)? EOF;

scope_block: '{' statement* '}';

statement: io ';'                                                                               #ioStatement
         | stream_decl ';'                                                                      #streamDeclStatement
         | var_decl ';'                                                                         #varDeclStatement
         | var_init ';'                                                                         #varInitStatement
         | capturing_method_decl                                                                #capturingMethodDeclStatement
         | if_statement ('else' if_statement)* ('else' scope_block)?                            #ifStatement
         | 'while' '(' expr ')' scope_block                                                     #whileStatement
         | 'for' '(' (TYPE)? NAME ':' (list_literal|expr',' expr(','expr)?) ')' scope_block     #forStatement
         | 'break' ';'                                                                          #break
         | 'continue' ';'                                                                       #continue
         | 'return' (list_expr)? ';'                                                            #return
         | capturing_method_call ';'                                                            #capturingMethodCallStatement
         ;

var_decl: TYPE (var_combined',')*var_combined;
var_combined: var_init      
            | var           
            ;
var: NAME | NAME'['expr']' | NAME'['expr']''['expr']';
var_init: NAME '=' expr                             #scalarVarInit
        | NAME'['expr']' '=' expr                   #oneDVarInit
        | NAME'[' expr ']' '[' expr ']' '=' expr    #twoDVarInit
        ;   

list_literal: '['((expr',')*expr)?']';
list_expr: list_literal
         | expr
         ;

capturing_method_decl: (TYPE|'void') NAME '(' ((method_arg',')*(method_arg))? ')' ('[' (captured_arg',')*(captured_arg) ']')? scope_block;
method_arg: TYPE var;
captured_arg: (TYPE|'file') var;

capturing_method_call: NAME'(' method_call_args ')' ('['capturing_method_call_args']')?;
method_call_args: ((list_expr',')*(list_expr))?;
capturing_method_call_args: (var',')*var;

if_statement: 'if' '(' expr ')' scope_block;

stream_decl: 'file' NAME '(' STRING ',' CHAR_LITERAL ')' '[' NAME ',' NAME']';
stream: stream_decl | NAME;
io: stream '>>' (var_decl | var)    #inputIo
  | stream '<<' list_expr           #outputIo
  ;

expr: op=('-' | 'not') expr                                      #unaryExpr
    | expr op=('/' | '*' | '%') expr                             #binExpr
    | expr op=('+' | '-') expr                                   #binExpr
    | expr op=('>='| '<=' | '>' | '<') expr                      #binExpr
    | expr op=('==' | '!=' ) expr                                #binExpr
    | expr op=('or' | 'and') expr                                #binExpr
    | '(' expr ')'                                               #paranthesisExpr
    | CHAR_LITERAL                                               #charLiteralExpr
    | BOOL_LITERAL                                               #boolLiteralExpr
    | INT_LITERAL                                                #intLiteralExpr
    | FLOAT_LITERAL                                              #floatLiteralExpr
    | var                                                        #variableExpr
    | capturing_method_call                                      #capturingMethodCallExpr
    | expr '?' expr ':' expr                                     #ternaryExpr
    ;

/*Tokens*/
TYPE: 'int' | 'uint' | 'char' | 'bool' | 'float';
CHAR_LITERAL : '\'' ('\\'. | ~('\'')) '\'';
BOOL_LITERAL : 'True' | 'False';
INT_LITERAL : [0-9]+;
FLOAT_LITERAL : [0-9]+'.'[0-9]+;
NAME  : [a-zA-Z][a-zA-Z0-9_]*;
STRING: '"' (~('"'))* '"';
COMMENT : '//' ~[\r\n]*->skip;
NS : [ \t\r\n]+ -> skip; 