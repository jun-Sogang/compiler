/* FILE: globals.h */

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef YYPARSER

#include "cm.tab.h"
#define ENDFILE 0
#endif

#ifndef FALSE
#define FALSE 0
#endif  

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = the number of reserved words */

#define MAXRESERVED 8
/*
typedef enum
{
  ENDFILE, ERROR, 
  IF, THEN, ELSE, END, REPEAT, UNTIL, READ, WRITE,
  ID, NUM,
  ASSIGN, EQ, LT, PLUS, MINUS, TIMES, OVER, LPAREN, RPAREN, SEMI,
  LTEQ, EQEQ, RTEQ, RT,
  RETURN,
  WHILE, INT, VOID, COMMA, COMMENT, LBRACE, RBRACE, LBRACKET, RBRACKET, COMMENT_ERROR

} TokenType;
*/

typedef int TokenType;

extern FILE* source; /* source code text file */
extern FILE* listing; /* listing output text file */
extern FILE* code; /* code text file for TM simulator */

extern int lineno; /* source line number for listing */

/* Syntax tree for parsing */

typedef enum {StmtK, ExpK, DeclK} NodeKind;
typedef enum {IfK, WhileK, RepeatK, AssignK, ReadK, WriteK, 
 CompoundStmtK, ExpressionStmtK, SelectionStmtK, IterationStmtK, ReturnStmtK
} StmtKind;
typedef enum {OpK, ConstK, IdK, ArrK, 
 LvarK, ComparisionExpK, AdditiveExpK, MultiplicativeExpK, CallK
} ExpKind;
typedef enum {VarK, VarArrK, ParamK, ParamArrK, ParamVoidK, FuncK, TypeK,
} DeclKind;

/* ExpType is used for type checking */
typedef enum {Void, Integer, Boolean,
} ExpType;

#define MAXCHILDREN 8

typedef struct treeNode
{
  struct treeNode *child[MAXCHILDREN];
  struct treeNode *sibling;
  int lineno;
  NodeKind nodekind;
  union {
    StmtKind stmt; 
    ExpKind exp;
	DeclKind decl;
  } kind;
  union {
    TokenType op;
    int val;
    char *name;
  } attr;
  int arraySize;
  ExpType type;
} TreeNode;

/*
   	Declaration

	child	0			1			2			3					4
			type		ID			parameters	compoundStatement	_num	
 
	Statement

	child	0					1				3			4			5				6
			local_declarations	statement_list	expression	statement	elseStatement	IF:
	
	Expression

	child	0		1			2			3			4			5	6
			_var	expression	lExpression	Operator	rExpression	ID	args
*/


/* Flags for tracing */
/* EchoSource = Ture cause the source program to be echoed to the listing file with line numbers during parsing */

extern int EchoSource;

/* TraceScan = TRUE cause token information to be printed to the listing file as each token is recognized by the scanner */

extern int TraceScan;

/* TraceParse = TURE causes the syntax tree to be printed to the listing file in linearized form (using indents for children) */

extern int TraceParse;

/* TraceAnalyze = TRUE causes symbol table inserts and lookups to be reported to the listing file */

extern int TraceAnalyze;

/* TraceCode = TRUE causes comments to be written to the TM code file as code is generated */

extern int TraceCode;

/* Error = TRUE prevents further passed if an error occurs */

extern int Error;

#endif
