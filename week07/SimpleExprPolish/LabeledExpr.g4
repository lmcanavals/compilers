grammar LabeledExpr;

/* Syntax analizer */

prog: stat+ ;

stat
	: expr NEWLINE        # printExpr
	| ID '=' expr NEWLINE # assign
	| NEWLINE             # blank
	;

expr
	: expr expr '^'       # Pow
	| expr expr '*'       # Mul
	| expr expr '/'       # Div
	| expr expr '+'       # Add
	| expr expr '-'       # Sub
	| expr 'n'            # Neg
	| NUM                 # num
	| ID                  # id
	;

/* Lexical analizer */

ID     : [a-zA-Z]+ ;
NUM    : [0-9]+('.'[0-9]+)? ;
NEWLINE: '\r'? '\n' ;
WS     : [ \t]+ -> skip ;
