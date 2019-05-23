/****************************************************/
/* File: symtab.c                                   */
/* Symbol table implementation for the TINY compiler*/
/* (allows only one symbol table)                   */
/* Symbol table is implemented as a chained         */
/* hash table                                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

/* SIZE is the size of the hash table */
#define SIZE 211

/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4

/* the hash function */
static int hash ( char * key )
{ int temp = 0;
 int i = 0;
 while (key[i] != '\0')
 { temp = ((temp << SHIFT) + key[i]) % SIZE;
  ++i;
 }
 return temp;
}

/* the list of line numbers of the source 
 * code in which a variable is referenced
 */
typedef struct LineListRec
{ int lineno;
 struct LineListRec * next;
} * LineList;

/* The record in the bucket lists for
 * each variable, including name, 
 * assigned memory location, and
 * the list of line numbers in which
 * it appears in the source code
 */
typedef struct BucketListRec
{ char * name;
 LineList lines;
 int memloc ; /* memory location for variable */
 struct BucketListRec * next;
} * BucketList;

/* the hash table */
static BucketList hashTable[SIZE];

typedef struct HashRec {
 BucketList hashTable[SIZE];
 int scopeNum;
 struct HashRec *next;
 struct HashRec *before;
} * HashList;

int currentScopeNum = 0;

HashList head;
HashList curTable;

void init() {
 head = (HashList)malloc(sizeof(struct HashRec));
 head->scopeNum = 0;
 head->next = NULL;
 head->before = NULL;
 curTable = head;
}


/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */

void scopeUp() {
 currentScopeNum += 1;
}

void scopeDown() {
 if (currentScopeNum == 0) ;
 else currentScopeNum -= 1;
}

void st_createHashTable() {
 HashList new = (HashList)malloc(sizeof(struct HashRec));

 curTable->next = new;
 new->before = curTable;
 new->next = NULL;
 new->scopeNum = currentScopeNum;
 printf("scopeNum: %d\n", new->scopeNum);

 curTable = curTable->next;

}

void st_zero_insert( char * name, int lineno, int loc )
{ int h = hash(name);
 if (currentScopeNum == 0) {
  BucketList l =  head->hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
   l = l->next;
  if (l == NULL) /* variable not yet in table */
  { l = (BucketList) malloc(sizeof(struct BucketListRec));
   l->name = name;
   l->lines = (LineList) malloc(sizeof(struct LineListRec));
   l->lines->lineno = lineno;
   l->memloc = loc;
   l->lines->next = NULL;
   l->next = head->hashTable[h];
   head->hashTable[h] = l; }
  else /* found in table, so just add line number */
  { LineList t = l->lines;
   while (t->next != NULL) t = t->next;
   t->next = (LineList) malloc(sizeof(struct LineListRec));
   t->next->lineno = lineno;
   t->next->next = NULL;
  }}
}

void st_insert( char * name, int lineno, int loc )
{ int h = hash(name);
 BucketList l =  curTable->hashTable[h];
 while ((l != NULL) && (strcmp(name,l->name) != 0))
  l = l->next;
 if (l == NULL) /* variable not yet in table */
 { l = (BucketList) malloc(sizeof(struct BucketListRec));
  l->name = name;
  l->lines = (LineList) malloc(sizeof(struct LineListRec));
  l->lines->lineno = lineno;
  l->memloc = loc;
  l->lines->next = NULL;
  l->next = curTable->hashTable[h];
  curTable->hashTable[h] = l; }
 else /* found in table, so just add line number */
 { LineList t = l->lines;
  while (t->next != NULL) t = t->next;
  t->next = (LineList) malloc(sizeof(struct LineListRec));
  t->next->lineno = lineno;
  t->next->next = NULL;
 }
} /* st_insert */

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
int st_lookup ( char * name )
{ int h = hash(name);
 BucketList l =  curTable->hashTable[h];
 while ((l != NULL) && (strcmp(name,l->name) != 0))
  l = l->next;
 if (l == NULL) return -1;
 else return l->memloc;
}

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing)
{ int i;

 fprintf(listing,"Variable Name  Location   ScopeNum  Line Numbers\n");
 fprintf(listing,"-------------  --------   --------  ------------\n");
 HashList temp = head;
 while (temp != NULL) {
  for (i=0;i<SIZE;++i)
  { if (temp->hashTable[i] != NULL)
   { BucketList l = temp->hashTable[i];
	while (l != NULL)
	{ LineList t = l->lines;
	 fprintf(listing,"%-14s ",l->name);
	 fprintf(listing,"%-8d  ",l->memloc);
	 fprintf(listing,"%-8d  ",temp->scopeNum);
	 while (t != NULL)
	 { fprintf(listing,"%4d ",t->lineno);
	  t = t->next;
	 }
	 fprintf(listing,"\n");
	 l = l->next;
	}
   }
  }
  temp = temp->next;
 }
} /* printSymTab */
