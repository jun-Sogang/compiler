/****************************************************/
/* File: analyze.c                                  */
/* Semantic analyzer implementation                 */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "analyze.h"

/* counter for variable memory locations */
static int func_location = 0;
static int global_location = 0;
static int local_location = -4;
static int param_location = 0;
static int global_check = 1;
static int isTypeCheck = 0;

/* Procedure traverse is a generic recursive 
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc 
 * in postorder to tree pointed to by t
 */

static void traverse(TreeNode *t, void (* preProc) (TreeNode *), void(* postProc) (TreeNode *), int isFirst) {
	if (t != NULL) {
		int i ;
		switch (t->nodekind) {
		 	case StmtK:
				switch (t->kind.stmt) {
					case CompoundStmtK:
					 	/*	
							child[0] -> local declarations
							child[1] -> statement list
							*/
					 	if (isFirst == 1 && isTypeCheck == 0){
							st_createHashTable();
						}

					 	traverse(t->child[0], preProc, postProc, 0);

						if(t->child[1] != NULL && t->child[1]->kind.stmt == 6){
						 	scopeUp();
							traverse(t->child[1], preProc, postProc, 1);
							scopeDown();
						}
						else
							traverse(t->child[1], preProc, postProc, 0);
						break;
					case SelectionStmtK:
						/*
						   child[3] -> expression
						   child[4] -> statement
						   child[5] -> elseStatement
						   */
						traverse(t->child[3], preProc, postProc, 0);
						scopeUp();
						if (isTypeCheck == 0)
							st_createHashTable();
						traverse(t->child[4], preProc, postProc, 0);
						if (t->child[5] != NULL) {
						 	if (isTypeCheck != 0)
							 	st_createHashTable();
							traverse(t->child[5], preProc, postProc, 0);
						}
						
						scopeDown();
						break;
					case IterationStmtK:
						traverse(t->child[3], preProc, postProc, 0);
						scopeUp();
						if (isTypeCheck == 0)
							st_createHashTable();
						traverse(t->child[4], preProc, postProc, 0);
						
						scopeDown();
						break;
					case ExpressionStmtK:
						traverse(t->child[2], preProc, postProc, 0);
						break;
					case ReturnStmtK:
						traverse(t->child[4], preProc, postProc, 0);
						break;
					defualt:
						break;
				}
				break;
			case ExpK:
				switch (t->kind.exp) {
					case IdK:
					 	printf("[DEBUG] IdK name: %s, lineno: %d\n", t->attr.name, t->lineno);
					 	lineno_insert(t->attr.name, t->lineno);
						break;
					case ArrK:
						traverse(t->child[5], preProc, postProc, 0);
						traverse(t->child[1], preProc, postProc, 0);
						break;
					case LvarK:
						traverse(t->child[0], preProc, postProc, 0);
						traverse(t->child[1], preProc, postProc, 0);
						break;
					case ComparisionExpK:
					case AdditiveExpK:
					case MultiplicativeExpK:
						traverse(t->child[3], preProc, postProc, 0);
						break;
					case CallK:
						traverse(t->child[5], preProc, postProc, 0);
						if (t->child[6] != NULL)
							traverse(t->child[6], preProc, postProc, 0);
						break;
				}
				break;
			case DeclK:
				switch (t->kind.decl) {
					case FuncK:
					 	/*
						   child[0] -> type
						   child[2] -> parameters
						   child[3] -> compoundStatement
						   */

						global_check = 0;
						local_location = -4;
					 	preProc(t);
					 	scopeUp();
				
						if (isTypeCheck == 0)
							st_createHashTable();

						int param_len = param_length(t->child[2]);
						param_location += param_len*4;
						traverse(t->child[2], preProc, postProc, 0);
						traverse(t->child[3], preProc, postProc, 0);
						scopeDown();
						global_check = 1;
						param_location = 0;
						break;
					case VarK:
					case VarArrK:
						preProc(t);
						break;
					case ParamK:
					case ParamArrK:
						preProc(t);
						break;
					case TypeK:
						switch (t->type) {
							case LT:
						 	case LE:
						 	case GT:
						 	case GE:
						 	case EQ:
							case NE:
							case PLUS:
							case MINUS:
							case TIMES:
							case OVER:
						  		traverse(t->child[2], preProc, postProc, 0);
						  		traverse(t->child[4], preProc, postProc, 0);
								break;
							case ASSIGN:
								traverse(t->child[0], preProc, postProc, 0);
								traverse(t->child[1], preProc, postProc, 0);
								break;
						}
					default:
						break;
				}
				break;
			default:
				break;
		}

		postProc(t);
		traverse(t->sibling, preProc, postProc, 0);
	}
}
static void insertNode(TreeNode *t) {
	switch (t->kind.decl) {
		case VarK:

		 	if (st_lookup(t->child[1]->attr.name) == -1){ 
		  		if(global_check == 0){
				 	local_location -= 4;
					st_insert(t->child[1]->attr.name, t->child[1]->lineno, local_location, "int", 0, "Var");
				}
				else{
				 	global_location += 4;
					st_insert(t->child[1]->attr.name, t->child[1]->lineno, global_location, "int", 0, "Var");
				}
		 	}
			else {
				printf("ERROR in line %d : declaration of %s duplicated first declared at line %d\n", t->child[1]->lineno, t->child[1]->attr.name, st_lookup(t->child[1]->attr.name));
				exit(1);
			}

			break;
		case VarArrK:
		 	if (st_lookup(t->child[1]->attr.name) == -1) {
			 	int temp_arraySize = t->child[4]->attr.val;
			 	if(global_check == 0){
					local_location -= temp_arraySize*4;
			 		st_insert(t->child[1]->attr.name, t->child[1]->lineno, local_location, "array", temp_arraySize, "Var");
				}
				else{
				 	global_location += temp_arraySize*4;
			 		st_insert(t->child[1]->attr.name, t->child[1]->lineno, global_location, "array", temp_arraySize, "Var");
				}
			}		
			else {
				printf("ERROR in line %d : declaration of %s duplicated first declared at line %d\n", t->child[1]->lineno, t->child[1]->attr.name, st_lookup(t->child[1]->attr.name));
				exit(1);
			}
			break;
		case ParamK:
		 	if (st_lookup(t->child[1]->attr.name) == -1) {
			 	st_insert(t->child[1]->attr.name, t->child[1]->lineno, param_location, "int", 0, "Par");
			 	param_location -= 4;
			}		
			else {
				printf("ERROR in line %d : declaration of %s duplicated first declared at line %d\n", t->child[1]->lineno, t->child[1]->attr.name, st_lookup(t->child[1]->attr.name));
				exit(1);
			}

			break;
		case ParamArrK:
		 	if (st_lookup(t->child[1]->attr.name) == -1) {
			 	st_insert(t->child[1]->attr.name, t->child[1]->lineno, param_location, "array", 0, "Par");
			 	param_location -= 4;
			}
			else {
				printf("ERROR in line %d : declaration of %s duplicated first declared at line %d\n", t->child[1]->lineno, t->child[1]->attr.name, st_lookup(t->child[1]->attr.name));
				exit(1);
			}

			break;
		case FuncK:
		 	if (st_function_lookup(t->child[1]->attr.name) == -1) 
			 	st_insert(t->child[1]->attr.name, t->child[1]->lineno, func_location++, t->child[0]->type == 0 ? "void" : "int", 0, "Func");
			else {
			 printf("ERROR in line %d : declaration of %s duplicated first declared at line %d\n", t->child[1]->lineno, t->child[1]->attr.name, st_function_lookup(t->child[1]->attr.name));
				exit(1);
			}

			break;
		defualt:
			break;
	}
}
/*
static void traverse( TreeNode * t,
  void (* preProc) (TreeNode *),
  void (* postProc) (TreeNode *) )
{ if (t != NULL)
 { preProc(t);
  { int i;
   for (i=0; i < MAXCHILDREN; i++)
	traverse(t->child[i],preProc,postProc);
  }
  postProc(t);
  traverse(t->sibling,preProc,postProc);
 }
}
*/
/* nullProc is a do-nothing procedure to 
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(TreeNode * t)
{ if (t==NULL) return;
 else return;
}

/* Procedure insertNode inserts 
 * identifiers stored in t into 
 * the symbol table 
 */
/* static void insertNode( TreeNode * t)
{
  switch (t->nodekind)
 { case StmtK:
  switch (t->kind.stmt)
  { case AssignK:
   case ReadK:
	if (st_lookup(t->attr.name) == -1)
	 // not yet in table, so treat as new definition 
	 st_insert(t->attr.name,t->lineno,location++);
	else
	 // already in table, so ignore location, 
	 //	add line number of use only  
	 st_insert(t->attr.name,t->lineno,0);
	break;
   default:
	break;
  }
  break;
  case ExpK:
  switch (t->kind.exp)
  { case IdK:
   if (st_lookup(t->attr.name) == -1)
	// not yet in table, so treat as new definition 
	st_insert(t->attr.name,t->lineno,location++);
   else
	// already in table, so ignore location, 
	   //add line number of use only  
	st_insert(t->attr.name,t->lineno,0);
   break;
   default:
   break;
  }
  break;
  default:
  break;
 }
}*/

/* Function buildSymtab constructs the symbol 
 * table by preorder traversal of the syntax tree
 */


static void typeError(TreeNode * t, char * message)
{ fprintf(listing,"Type error at line %d: %s\n",t->lineno,message);
 Error = TRUE;
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
 static void checkNode(TreeNode * t)
{ 
 switch (t->nodekind) {
		 	case StmtK:
				switch (t->kind.stmt) {
					case CompoundStmtK:
						break;
					case SelectionStmtK:
						break;
					case IterationStmtK:
						break;
					case ExpressionStmtK:
						break;
					case ReturnStmtK:
						break;
					defualt:
						break;
				}
				break;
			case ExpK:
				switch (t->kind.exp) {
					case IdK:
					 /*
					 	{	BucketList l = st_bucket_lookup(t->attr.name);
						printf("name : %s\n", t->attr.name);
						 if (strcmp(l->type, "int")) {
						  printf("type : %s\n{", l->type);
						  printf("array type can not be used like int\n");
						  printf("lineno: %d\n", t->lineno);
						  exit(1);
						 }
						}*/
						break;
					case ArrK:

						if(t->child[1]->child[5] != NULL){
						 if (strcmp(st_functionType_lookup(t->child[1]->child[5]->attr.name), "int")) {
						  printf("%s\n", st_functionType_lookup(t->child[1]->child[5]->attr.name));
						  printf("index type must be int\n");
						  exit(1);
						 }
						}
						else {
						 	printf("name: %s\n", t->child[5]->attr.name);
							BucketList l = st_bucket_lookup(t->child[5]->attr.name);
							
							if(l == NULL){
							 printf("NULL\n");
							 exit(1);
							}
							
							if (strcmp(l->type, "array")) {
							  printf("int type can not be used like array\n");
							  exit(1);
							}
						}
						break;
					case LvarK:
						break;
					case ComparisionExpK:
					case AdditiveExpK:
					case MultiplicativeExpK:
						break;
					case CallK:
						{
						 BucketList l = st_bucket_lookup(t->child[5]->attr.name);

						 if(strcmp(l->VPF, "Func")) {
						  printf("%s is not a function\n", t->child[5]->attr.name);
						  exit(1);
						 }
						 break;
						}
				}
				break;
			case DeclK:
				switch (t->kind.decl) {
					case FuncK:
					 	{
						 //printf("[DEBUG] attr name: %s\n", t->child[1]->attr.name);
						 BucketList l = st_bucket_lookup(t->child[1]->attr.name);
						 printf("[DEBUG] function type: %s, name: %s\n", l->type, l->name);
							
						 // main function type check
						 if(!strcmp(l->name, "main") && strcmp(l->type, "void")){
							printf("main function type must be void\n");
							exit(1);
						 }

						 // main function parameter check
						 
						
						 if(!strcmp(l->name, "main") && t->child[2]->kind.decl != 4) {
						  	printf("[parameter] %d\n", t->child[2]->kind.decl);
							printf("main function should not have parameters\n");
							exit(1);
						 }
						 // if return statement exist
						 if(t->child[3]->child[1] != NULL){
						     printf("[DEBUG] hello my freefefefefef\n");
						 	 TreeNode *temp = t->child[3]->child[1];
						 	 while(temp->sibling != NULL) temp = temp->sibling;

							 // if function type is void and return statement exist
							 if(!strcmp(l->type, "void") && temp->kind.stmt == 10){
							  printf("void function can not have return statement\n");
							  exit(1);
							 }

							 if(!strcmp(l->type, "int") && temp->child[4] == NULL){
								printf("int function return statement must have integer variable\n");
								exit(1);
							 }

							 //printf("[DEBUG] in FuncK, return statement name %s\n", temp->child[4]->attr.name);
						//	 printf("[DEBUG] in FuncK, exp type: %d\n", temp->child[4]->type);
						 	 printf("hihih&*(&*( %d\n", temp->kind.stmt);
						 }
						 else{

						  	if(strcmp(l->type, "void")){
								
							 	printf("int function must have return statement\n");
								exit(1);
							}


						 }

						}
						break;
					case VarK:
					case VarArrK:
						if(t->child[0]->type == 0){
						 	printf("variable type can not be void\n");
							exit(1);
						}
						
						break;
					case ParamK:
					case ParamArrK:
						if(t->child[0]->type == 0){
						 	printf("parameter type can not be void\n");
							exit(1);
						}
						break;
					case TypeK:
						switch (t->type) {
							case LT:
						 	case LE:
						 	case GT:
						 	case GE:
						 	case EQ:
							case NE:
							case PLUS:
							case MINUS:
							case TIMES:
							case OVER:
								break;
							case ASSIGN:
								break;
						}
					default:
						break;
				}
				break;
			default:
				break;
		}


 
 }

int param_length(TreeNode * t)
{
 	int len = 0;
//	printf("[DEBUG} parameter: %s\n", t->attr.name);
	while(t != NULL){
		len += 1;
		t = t->sibling;
	}
	return len;
}

/* Procedure typeCheck performs type checking 
 * by a postorder syntax tree traversal
 */
void buildSymtab(TreeNode * syntaxTree)
{ traverse(syntaxTree,insertNode,checkNode, 0);
 if (TraceAnalyze)
 { fprintf(listing,"\nSymbol table:\n\n");
  printSymTab(listing);
 }

}
void typeCheck(TreeNode * syntaxTree)
{
 isTypeCheck = 1;
 traverse(syntaxTree,nullProc,checkNode, 0);

 if (TraceAnalyze)
 { fprintf(listing,"\nSymbol table:\n\n");
  printSymTab(listing);
 }
}
