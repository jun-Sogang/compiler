%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"
#include "symtab.h"

#define YYSTYPE TreeNode *
static int yylex(void);

static char * savedName; /* for use in assignments */
static int savedLineNo;  /* ditto */
static TreeNode * savedTree; /* stores syntax tree for later return */

%}

%token ELSE IF INT RETURN VOID WHILE
%token ID NUM
%token PLUS MINUS TIMES OVER
%token LT LE GT GE EQ NE
%token ASSIGN SEMI COMMA
%token LPAREN RPAREN LBRACKET RBRACKET LBRACE RBRACE
%token THEN END REPEAT UNTIL READ WRITE
%token ENDFILE ERROR

%nonassoc LOWER_ELSE
%nonassoc ELSE

%right ASSIGN
%start program

%% /* Grammar for TINY */

program	: declaration_list{ 
 				savedTree = $1;
			}
			;
declaration_list	:	declaration_list declaration {
						 $$ = addNode($1, $2);
					 	}
						|	declaration { $$ = $1; }
						;
declaration	:	variable_declaration {
					$$ = $1;
				}
				| function_declaration {
					$$ = $1;
				}
				;
variable_declaration	:	type_check _id SEMI {
							 savedName = copyString(tokenString);	
							 savedLineNo = lineno;
							 $$ = variableDeclarationNode($1, $2, savedLineNo);
							}
							| type_check _id LBRACKET _num RBRACKET SEMI {
							 	savedLineNo = lineno;
								$$ = arrayDeclarationNode($1, $2, savedLineNo, $4);
							}
							;

function_declaration	:	type_check _id LPAREN _parameters RPAREN _compoundStatement {
								savedLineNo = lineno;
								$$ = functionDeclarationNode($1, $2, $4, $6, savedLineNo);
							}
							;

type_check	:	INT {
				 	$$ = tokenType(Integer);
				}
				| VOID {
					$$ = tokenType(Void);
					}
				;

_parameters	:	parameterList {
					$$ = $1;
				}
				| VOID {
				 	savedLineNo = lineno;
					$$ = voidParameterNode(savedLineNo);
				}
				;
parameterList	:	parameterList COMMA param {
						$$ = addNode($1, $3);
					}
					| param {
						$$ = $1;
					}
					;

param	:	type_check _id {
			 	savedLineNo = lineno;
				$$ = variableParameterNode($1, $2, savedLineNo);
			}
			| type_check _id LBRACKET RBRACKET {
			 	savedLineNo = lineno;
				$$ = arrayParameterNode($1, $2, savedLineNo);
			}
			;

_compoundStatement	:	LBRACE local_declarations statement_list RBRACE {
						 	savedLineNo = lineno;
							$$ = compoundStatementNode($2, $3, savedLineNo);
						}
						;
local_declarations	:	local_declarations variable_declaration {
							$$ = addNode($1, $2);
						}
						| /* empty */ { $$ = NULL; }
						;
statement_list	:	statement_list statement {
						$$ = addNode($1, $2);
					}
					| /* empty */ { $$ = NULL; }
					;
statement	:	expressionStmt {
					$$ = $1;
				}
				| _compoundStatement {
				 	$$ = $1;
				}
				| selectionStmt {
				 	$$ = $1;
				}
				| iterationStmt {
				 	$$ = $1;
				}
				| returnStmt {
				 	$$ = $1;
				}
				;
expressionStmt	:	expression SEMI {
						$$ = expressionStatementNode($1);
					}
					| SEMI { $$ = NULL; }
					;

selectionStmt	:	IF LPAREN expression RPAREN statement {
						$$ = selectionStatementNode($3, $5, NULL);
					} %prec LOWER_ELSE 
					| IF LPAREN expression RPAREN statement ELSE statement {
						$$ = selectionStatementNode($3, $5, $7);
					}
					;

iterationStmt	:	WHILE LPAREN expression RPAREN statement {
						$$ = iterationStatementNode($3, $5);
					}
					;

returnStmt	:	RETURN SEMI {
					$$ = returnStatementNode(NULL);
				}
				| RETURN expression SEMI {
					$$ = returnStatementNode($2);
				}
				;

expression	:	_var ASSIGN expression {
					$$ = assignExpression($1, $3);
				}
				| simple_expression {
					$$ = $1;
				}
				;

simple_expression	:	additive_expression relativeOp additive_expression {
							$$ = comparisionExpressionNode($1, $2, $3);
						}
						| additive_expression {
							$$ = $1;
						}
						;
					
relativeOp	:	LT {
				 $$ = tokenType(LT);
				}
				| LE {
				 $$ = tokenType(LE);
				}
				| GT {
				 $$ = tokenType(GT);
				}
				| GE {
				 $$ = tokenType(GE);
				}
				| EQ {
				 $$ = tokenType(EQ);
				}
				| NE {
				 $$ = tokenType(NE);
				}
				;

additive_expression	:	additive_expression addop term {
							$$ = additiveExpressionNode($1, $2, $3);
						}
						| term {
						 	$$ = $1;
						}
						;

addop	:	PLUS {
				$$ = tokenType(PLUS);
			}
			| MINUS {
				$$ = tokenType(MINUS);
			}
			;

term	:	term mulop factor {
				$$ = multiplicativeExpressionNode($1, $2, $3);
			}
			| factor {
				$$ = $1;
			}
			;

mulop	:	TIMES {
				$$ = tokenType(TIMES);
			}
			| OVER {
				$$ = tokenType(OVER);
			}
			;

factor	:	LPAREN expression RPAREN {
				$$ = $2;
			}
			| _var {
				$$ = $1;
			}
			| call {
				$$ = $1;
			}
			| _num {
				$$ = $1;
			}
			;

call	:	_id LPAREN args RPAREN {
				$$ = callNode($1, $3);
			}
			;

args	:	arg_list {
				$$ = $1; 
		  	}
          	| { $$ = NULL; }
			;

arg_list	: arg_list COMMA expression { 
				$$ = addNode($1, $3); 
			}
       		| expression { 
		 		$$ = $1; 
			}
			;

_var	:	_id {
				$$ = $1;
			}
			| _id LBRACKET expression RBRACKET {
				$$ = newArrayNode($1, $3);
			}
			;

_id	:	ID {
			$$ = newIdNode(tokenString);
		}
		;
					
_num	:	NUM {
				$$ = newConstNode(tokenString);
			}
			;
%%

int yyerror(char * message) { 
	fprintf(listing,"Syntax error at line %d: %s\n",lineno,message);
	fprintf(listing,"Current token: ");
	printToken(yychar,tokenString);
	Error = TRUE;
	
	return 0;
}

/* yylex calls getToken to make Yacc/Bison output
    * compatible with ealier versions of the TINY scanner
	 */

static int yylex(void) {
	TokenType token = getToken();

	if (token == ENDFILE) return 0;
	if (token == ERROR) {
		fprintf(listing, "ERROR\n");
		Error = TRUE;
		return 0;
	}
	return token;
}

TreeNode * parse(void) { 
	yyparse();
	return savedTree;
}
