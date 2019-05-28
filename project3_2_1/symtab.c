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
/* The record in the bucket lists for
 * each variable, including name, 
 * assigned memory location, and
 * the list of line numbers in which
 * it appears in the source code
 */
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
int scopeCheck() {
	return currentScopeNum;
}

void scopeDown() {
 if (currentScopeNum == 0) ;
 else currentScopeNum -= 1;
}

void st_createHashTable() {
 HashList new = (HashList)malloc(sizeof(struct HashRec));
 //printf("HashTable created!!!\n");
 curTable->next = new;
 new->before = curTable;
 new->next = NULL;
 new->scopeNum = currentScopeNum;
 //printf("scopeNum: %d\n", new->scopeNum);

 curTable = curTable->next;

}

void lineno_insert(char *name, int lineno) {
	int curScopeNum = currentScopeNum;

	HashList now = curTable;
	while (now != NULL) {
		if (curScopeNum == now->scopeNum) {
//		 	printf("222\n");
			int h = hash(name);
			if (lineno_lookup(name, now) != -1) {
				BucketList l = now->hashTable[h];
				LineList t = l->lines;
				
				while (t->next != NULL) t = t->next;
				if (t->lineno == lineno) break;
				t->next = (LineList) malloc(sizeof(struct LineListRec));
				t->next->lineno = lineno;
				t->next->next = NULL;
				return ;
			}
			else curScopeNum--;
		}
//		printSymTabHead(stdout,now);
		now = now->before;
		//if(now != NULL)
		//	printf("lineno_scopenum: %d\n", now->scopeNum);
	}
	if (now == NULL) {
	 printf("hellossss\n");
	 	printf("ERROR in line %d : %s not exist\n", lineno, name);
		exit(1);
		// handling exception
	}

}

int lineno_lookup (char * name, HashList now){ 
 	int h = hash(name);
 	BucketList l =  now->hashTable[h];
/*
	iif(!strcmp(name, "parB")){
	 	printf("-------------parB-------------\n");
		printSymTabHead(stdout,now);
	}
	if(!strcmp(name, "parB") && l != NULL)
	 	printf("--------------------parB name: %s\n", l->name);
 */	
	while ((l != NULL) && (strcmp(name,l->name) != 0)){
  		l = l->next;
	}
 	if (l == NULL) return -1;
 	else return l->lines->lineno;
}

void st_insert( char * name, int lineno, int loc, char *type, int arraySize, char *VPF ){ 
	int h = hash(name);
	
	BucketList l;
	if (currentScopeNum == 0) {
		l = head->hashTable[h];
	}
	else {
		l = curTable->hashTable[h];
	}
	
	while ((l != NULL) && (strcmp(name,l->name) != 0))
		l = l->next;
	if (l == NULL) /* variable not yet in table */
	{ l = (BucketList) malloc(sizeof(struct BucketListRec));
	 l->name = name;
	 l->lines = (LineList) malloc(sizeof(struct LineListRec));
	 l->lines->lineno = lineno;
	 l->memloc = loc;
	 
	 strncpy(l->type, type, strlen(type) + 1);
	 l->arraySize = arraySize;
	 strncpy(l->VPF, VPF, strlen(VPF) + 1);

	 l->lines->next = NULL;
	 if(currentScopeNum == 0){
	   l->next = head->hashTable[h];
	   head->hashTable[h] = l;
	 }
	 else{
		l->next = curTable->hashTable[h];
		curTable->hashTable[h] = l;
	 }
	}
	else /* found in table, so just add line number */
	{ 
	 printf("ERROR in symtab.c\n");
	}
} /* st_insert */

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
int st_lookup ( char * name )
{ int h = hash(name);
 BucketList l =  curTable->hashTable[h];
  printf("st_lookup name: %s, scopeNum: %d\n", name, curTable->scopeNum);
 while ((l != NULL) && (strcmp(name,l->name) != 0)){
  l = l->next;
 }
 if (l == NULL) return -1;
 else return l->lines->lineno;
}
int st_function_lookup (char *name) {
	int h = hash(name);
	BucketList l = head->hashTable[h];

	while((l != NULL) && (strcmp(name, l->name) !=0)) {
		l = l->next;
	}
	if (l == NULL) return -1;
	else return l->lines->lineno;
}
char * st_functionType_lookup (char *name) {
	int h = hash(name);
	BucketList l = head->hashTable[h];

	while((l != NULL) && (strcmp(name, l->name) !=0)) {
		l = l->next;
	}
	return l->type;
}

BucketList st_bucket_lookup(char *name) {
	int curScopeNum = currentScopeNum;

	HashList now = curTable;

	while (now != NULL) {
		if (curScopeNum == now->scopeNum) {
			int h = hash(name);

			if (lineno_lookup(name, now) != -1) {
				BucketList l = now->hashTable[h];
				return l;
			}
			else curScopeNum--;
		}
		now = now->before;
	}

}

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing)
{ int i;
 fprintf(listing,"Name  Scope  Loc  V/P/F  Array?  ArrSize  Type  Line Numbers\n");
 fprintf(listing,"------------------------------------------------------------\n");
 HashList temp = head;
 while (temp != NULL) {
  for (i=0;i<SIZE;++i)
  { if (temp->hashTable[i] != NULL)
   { BucketList l = temp->hashTable[i];
	while (l != NULL)
	{ LineList t = l->lines;
	 fprintf(listing,"%-4s ",l->name);
	 fprintf(listing,"%-5d  ",temp->scopeNum);
	 fprintf(listing,"%-3d  ",l->memloc);
	 fprintf(listing,"%-5s  ",l->VPF);

	 if(!strcmp(l->type, "array")){
	 	fprintf(listing,"%-5s  ", "Array" );
	 	fprintf(listing,"%-7d  ", l->arraySize);
	 }
	 else{
	 	fprintf(listing,"%-5s  ", "No" );
	 	fprintf(listing,"%-7s  ", "-");
	 }
	 fprintf(listing,"%-4s  ",l->type);
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
  printf("\n\n");
 }
} /* printSymTab */

void printSymTabHead(FILE * listing, HashList now)
{ int i;

 fprintf(listing,"Name  Scope  Loc  V/P/F  Array?  ArrSize  Type  Line Numbers\n");
 fprintf(listing,"------------------------------------------------------------\n");
 HashList temp = now;
  for (i=0;i<SIZE;++i)
  { if (temp->hashTable[i] != NULL)
   { BucketList l = temp->hashTable[i];
	while (l != NULL)
	{ LineList t = l->lines;
	 fprintf(listing,"%-4s ",l->name);
	 fprintf(listing,"%-5d  ",temp->scopeNum);
	 fprintf(listing,"%-3d  ",l->memloc);
	 fprintf(listing,"%-5s  ",l->VPF);

	 if(!strcmp(l->type, "array")){
	 	fprintf(listing,"%-5s  ", "Array" );
	 	fprintf(listing,"%-7d  ", l->arraySize);
	 }
	 else{
	 	fprintf(listing,"%-5s  ", "No" );
	 	fprintf(listing,"%-7s  ", "-");
	 }
	 fprintf(listing,"%-4s  ",l->type);
	 while (t != NULL)
	 { fprintf(listing,"%4d ",t->lineno);
	  t = t->next;
	 }
	 fprintf(listing,"\n");
	 l = l->next;
	}
   }
  }
} /* printSymTab */

void isMainLast()
{
 HashList temp = head;
 int maxLoc = -1;
 int mainLoc = -1;

  for (int i=0;i<SIZE;++i)
  { if (temp->hashTable[i] != NULL)
   { BucketList l = temp->hashTable[i];
	while (l != NULL)
	{ 

	 if(!strcmp(l->VPF, "Func") && maxLoc < l->memloc)
	  maxLoc = l->memloc;
	 if(!strcmp(l->VPF, "Func") && !strcmp(l->name, "main"))
	  mainLoc = l->memloc;

	  l = l->next;
	}
   }
  }

  if(mainLoc != maxLoc){
   printf("main function should lie in the last\n");
   exit(1);
  }
}


