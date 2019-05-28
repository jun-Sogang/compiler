/****************************************************/
/* File: symtab.h                                   */
/* Symbol table interface for the TINY compiler     */
/* (allows only one symbol table)                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _SYMTAB_H_
#define _SYMTAB_H_

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */

typedef struct LineListRec
{ int lineno;
 struct LineListRec * next;
} * LineList;

typedef struct BucketListRec
{ char * name;
 LineList lines;
 int memloc ; /* memory location for variable */
 char type[10];
 int arraySize;
 char VPF[10];
 struct BucketListRec * next;
} * BucketList;


void scopeUp();
void scopeDown();
int scopeCheck();
void st_createHashTable();

void st_insert( char * name, int lineno, int loc, char *type, int arraySize, char *VPF);

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
int st_lookup ( char * name );
int st_function_lookup (char *name);
char * st_functionType_lookup (char *name);

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing);

#endif
