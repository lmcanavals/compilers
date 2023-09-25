grammar LabeledExpr;

/* Syntax analizer */

prog: stat+ ;

stat
	: expr NEWLINE          # printExpr
	| ID '=' expr NEWLINE   # assign
	| NEWLINE               # blank
	;

expr
	: expr ('*'|'/') expr   # MulDiv
	| expr ('+'|'-') expr   # AddSub
	| INT                   # int
	| ID                    # id
	| '(' expr ')'          # parens
	;

/* Lexical analizer */

MUL: '*' ;
DIV: '/' ;
ADD: '+' ;
SUB: '-' ;

ID     : [a-zA-Z]+ ;
INT    : [0-9]+ ;
NEWLINE: '\r'? '\n' ;
WS     : [ \t]+ -> skip ;
