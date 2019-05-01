/* File: tiny.l   */

%{
#include "globals.h"
#include "util.h"
#include "scan.h"
/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN + 1];
%}

digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
identifier  {letter}+
newline     \n
whitespace  [ \t]+

%%

"if"            {return IF;}
"then"          {return THEN;}
"else"          {return ELSE;}
"end"           {return END;}
"repeat"        {return REPEAT;}
"until"         {return UNTIL;}
"read"          {return READ;}
"write"         {return WRITE;}

"int"			{return INT;}
"void"		{return VOID;}
"while"			{return WHILE;}
"return"		{return RETURN;}

":="            {return ASSIGN;}
"=="			{return EQEQ;}
"="             {return EQ;}
"<="			{return LTEQ;}
"<"             {return LT;}
">="			{return RTEQ;}
">"				{return RT;}

"+"             {return PLUS;}
"-"             {return MINUS;}
"*"             {return TIMES;}
"/"             {return OVER;}

"{"				{return LBRACE;}
"}"				{return RBRACE;}
"["				{return LBRACKET;}
"]"				{return RBRACKET;}
"("             {return LPAREN;}
")"             {return RPAREN;}
";"             {return SEMI;}
","				{return COMMA;}

{number}        {return NUM;}
{identifier}    {return ID;}
{newline}       {lineno++;}
{whitespace}    {/* skip whitespace */}

"/*"			{
 					char c;
					int starNum = 0;
					while ((c = input()) != EOF) {
						switch (c) {
							case '*':
							 starNum += 1;
							 break;
							case '/':
							 if (starNum == 1)
							  return COMMENT;
							 break;
							case '\n':
							 lineno += 1;
							 starNum = 0;
							 break;

							default:
							 starNum = 0;
						}
					}
					return COMMENT_ERROR;
 				}

.               {return ERROR;}

%%

TokenType getToken(void)
{ static int firstTime = TRUE;
  TokenType currentToken;
  if (firstTime)
  { firstTime = FALSE;
    lineno++;
    yyin = source;
    yyout = listing;
  }
  currentToken = yylex();
  strncpy(tokenString,yytext,MAXTOKENLEN);
  if (TraceScan) {
//    fprintf(listing,"\t%d: ",lineno);
    printToken(currentToken,tokenString);
  }
  return currentToken;
}

