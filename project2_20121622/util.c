/* util.c */

#include "globals.h"
#include "util.h"

/* Procedure printToken prints a token and its lexeme to the listing file */


void printToken(TokenType token, const char* tokenString) {


 fprintf(listing,"\t%d\t\t", lineno);
  switch (token) {
    case IF: fprintf(listing, "IF\t\tif\n"); break;
    case THEN:
    case ELSE: fprintf(listing, "ELSE\t\telse\n"); break;
    case END:
    case REPEAT:
    case UNTIL:
    case READ:
    case WRITE:
      fprintf(listing, "reserved word: %s\n", tokenString);
      break;
    //case ASSIGN: fprintf(listing, ":=\n"); break;
	
	case INT: fprintf(listing, "INT\t\tint\n"); break;
	case VOID: fprintf(listing, "VOID\t\tvoid\n"); break;
 	case WHILE: fprintf(listing, "WHILE\t\twhile\n"); break;

    case LT: fprintf(listing, "<\t\t<\n"); break;
    case EQ: fprintf(listing, "=\t\t=\n"); break;
	
	//case LTEQ: fprintf(listing, "<=\t\t<=\n"); break;
	//case RTEQ: fprintf(listing, ">=\t\t>=\n"); break;
	//case EQEQ: fprintf(listing, "==\t\t==\n"); break;
	//case RT: fprintf(listing, ">\t\t>\n"); break;
    
	case LPAREN: fprintf(listing, "(\t\t(\n"); break;
    case RPAREN: fprintf(listing, ")\t\t)\n"); break;
	case LBRACE: fprintf(listing, "{\t\t{\n"); break;
	case RBRACE: fprintf(listing, "}\t\t}\n"); break;
	case LBRACKET: fprintf(listing, "[\t\t[\n"); break;
	case RBRACKET: fprintf(listing, "]\t\t]\n"); break;
    case SEMI: fprintf(listing, ";\t\t;\n"); break;
    
	case PLUS: fprintf(listing, "+\t\t+\n"); break;
    case MINUS: fprintf(listing, "-\t\t-\n"); break;
    case TIMES: fprintf(listing, "*\t\t*\n"); break;
    case OVER: fprintf(listing, "/\t\t/\n"); break;
	
	case COMMA: fprintf(listing, ",\t\t,\n"); break;
	case RETURN: fprintf(listing, "RETURN\t\treturn\n"); break;
    case ENDFILE: fprintf(listing, "EOF\n"); break;
    case NUM:
      fprintf(listing, "NUM\t\t%s\n", tokenString);
      break;
    case ID:
      fprintf(listing, "ID\t\t%s\n", tokenString);
      break;
    case ERROR:
      fprintf(listing, "ERROR\t\t%s\n", tokenString);
      break;
    default:
      fprintf(listing, "Unknown token %d", token);
  }
}

/* Function newStmtNode creates a new statement node for syntax tree construction */

TreeNode *newStmtNode(StmtKind kind) {
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;

  if (t == NULL) {
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  }
  else {
    for (i = 0; i < MAXCHILDREN; ++i) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->lineno = lineno;
  }
  
  return t;
}

/* Function newExpNode creates a new expression node for syntax tree construction */

TreeNode *newExpNode(ExpKind kind) {
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;

  if (t == NULL) {
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  }
  else {
    for (i = 0; i < MAXCHILDREN; ++i) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->lineno = lineno;
    t->type = Void;
  }

  return t;
}

/* Declare part */

TreeNode *newDeclNode(DeclKind kind) {
	TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
	int i;

	if (t == NULL) fprintf(listing, "Out of memory error at line %d\n", lineno);
	else {
		for (i = 0; i < MAXCHILDREN; ++i) t->child[i] = NULL;
		t->sibling = NULL;
		t->nodekind = DeclK;
		t->kind.decl = kind;
		t->lineno = lineno;
	}
	return t;
}

TreeNode *addNode(TreeNode *now, TreeNode *input) {
	if (now != NULL) {
		TreeNode *copy = now;

		while (copy->sibling != NULL) copy = copy->sibling;
		copy->sibling = input;
	} else {
		now = input;
	}

	return now;
}
// Declaration
TreeNode *variableDeclarationNode(TreeNode *typeNode, TreeNode *ID, int savedLineNo) {
	TreeNode *declNode = newDeclNode(VarK);
	
	declNode->child[0] = typeNode;
	declNode->child[1] = ID;
	declNode->lineno = savedLineNo;

	return declNode;
}

TreeNode *arrayDeclarationNode(TreeNode *typeNode, TreeNode *ID, int savedLineNo, TreeNode *_num) {
	TreeNode *declNode = newDeclNode(VarArrK);	

	declNode->child[0] = typeNode;
	declNode->child[1] = ID;
	declNode->lineno = savedLineNo;
	declNode->child[4] = _num;

	return declNode;
}

TreeNode *functionDeclarationNode(TreeNode *typeNode, TreeNode *ID, TreeNode *parameters, TreeNode *compoundStatement, int savedLineNo) {
	TreeNode *declNode = newDeclNode(FuncK);

	declNode->child[0] = typeNode;
	declNode->child[1] = ID;
	declNode->child[2] = parameters;
	declNode->child[3] = compoundStatement;
	declNode->lineno = savedLineNo;
	
	return declNode;
}

// Parameter

TreeNode *variableParameterNode(TreeNode *typeNode, TreeNode *ID, int savedLineNo) {
	TreeNode *declNode = newDeclNode(ParamK);

	declNode->child[0] = typeNode;
	declNode->child[1] = ID;
	declNode->lineno = savedLineNo;

	return declNode;
}

TreeNode *arrayParameterNode(TreeNode *typeNode, TreeNode *ID, int savedLineNo) {
	TreeNode *declNode = newDeclNode(ParamArrK);

	declNode->child[0] = typeNode;
	declNode->child[1] = ID;
	declNode->lineno = savedLineNo;

	return declNode;
}

TreeNode *voidParameterNode(int savedLineNo) {
	TreeNode *declNode = newDeclNode(ParamVoidK);
	declNode->lineno = savedLineNo;

	return declNode;
}
// Compound Statement

TreeNode *compoundStatementNode(TreeNode *local_declarations, TreeNode *statement_list, int savedLineNo) {
	TreeNode *stmtNode = newStmtNode(CompoundStmtK);

	stmtNode->child[0] = local_declarations;
	stmtNode->child[1] = statement_list;
	stmtNode->lineno = savedLineNo;
	
	return stmtNode;
}

// Expression Statement

TreeNode *expressionStatementNode(TreeNode *expressionNode) {
	TreeNode *stmtNode = newStmtNode(ExpressionStmtK);

	stmtNode->child[2] = expressionNode;

	return stmtNode;

}

TreeNode *assignExpression(TreeNode *_var, TreeNode *expression) {
	TreeNode *expNode = newExpNode(LvarK);

	expNode->child[0] = _var;
	expNode->child[1] = expression;

	return expNode;
}

// Selection Statement

TreeNode *selectionStatementNode(TreeNode *expression, TreeNode *statement, TreeNode *elseStatement) {
	TreeNode *stmtNode = newStmtNode(SelectionStmtK);
	
	stmtNode->child[3] = expression;
	stmtNode->child[4] = statement;
	stmtNode->child[5] = elseStatement;
	stmtNode->child[6] = newStmtNode(IfK);

	return stmtNode;
}

// Iteration Statement

TreeNode *iterationStatementNode(TreeNode *expression, TreeNode *statement) {

	TreeNode *stmtNode = newStmtNode(IterationStmtK);

	stmtNode->child[3] = expression;
	stmtNode->child[4] = statement;
	stmtNode->child[6] = newStmtNode(WhileK);

	return stmtNode;
}

// Return Statement 

TreeNode *returnStatementNode(TreeNode *statement) {

	TreeNode *stmtNode = newStmtNode(ReturnStmtK);

	stmtNode->child[4] = statement;

	return stmtNode;
}

// Comparision Expression

TreeNode *comparisionExpressionNode(TreeNode *lExpression, TreeNode *op, TreeNode *rExpression) {
	TreeNode *expNode = newExpNode(ComparisionExpK);

	//expNode->child[2] = lExpression;
	expNode->child[3] = op;
	expNode->child[3]->child[2] = lExpression;
	expNode->child[3]->child[4] = rExpression;
	//expNode->child[4] = rExpression;
	
	return expNode;
}

// Additive Expression

TreeNode *additiveExpressionNode(TreeNode *lExpression, TreeNode *op, TreeNode *rExpression) {
	TreeNode *expNode = newExpNode(AdditiveExpK);

	expNode->child[3] = op;
	expNode->child[3]->child[2] = lExpression;
	expNode->child[3]->child[4] = rExpression;
	
	return expNode;
}

// Mulltiplicative Expression

TreeNode *multiplicativeExpressionNode(TreeNode *lExpression, TreeNode *op, TreeNode *rExpression) {
	TreeNode *expNode = newExpNode(MultiplicativeExpK);

	//expNode->child[2] = lExpression;
	expNode->child[3] = op;
	expNode->child[3]->child[2] = lExpression;
	expNode->child[3]->child[4] = rExpression;
	//expNode->child[4] = rExpression;
	
	return expNode;
}

// Call Expression

TreeNode *callNode(TreeNode *_id, TreeNode *args) {
	TreeNode *expNode = newExpNode(CallK);

	expNode->child[5] = _id;
	expNode->child[6] = args;

	return expNode;
}

// Type

TreeNode *tokenType(TokenType type) {
	TreeNode *declNode = newDeclNode(TypeK);

	declNode->type = type;

	return declNode;
}

TreeNode *newArrayNode(TreeNode *_id, TreeNode *expression) {
	TreeNode *new = newExpNode(ArrK);

	new->child[5] = _id;
	new->child[1] = expression;

	return new;
}

TreeNode *newIdNode(char *tokenString) {
	TreeNode *new = newExpNode(IdK);
	
	new->attr.name = copyString(tokenString);

	return new;
}

TreeNode *newConstNode(char *num) {
	TreeNode *new = newExpNode(ConstK);

	new->attr.val = atoi(num);

	return new;
}

char *copyString (char *s) {
  int n;
  char *t;

  if (s == NULL) return NULL;

  n = strlen(s) + 1;
  t = malloc(n);

  if (t == NULL) fprintf(listing, "Out of memory error at line %d\n", lineno);
  else strcpy(t, s);

  return t;
}

/* Variable indentno is used by printTree to store current number of spaces to indent */

static indentno = 0;

/* macros to increase / decrease indentation */

#define INDENT indentno += 2
#define UNINDENT indentno -= 2

/* printSpaces indents by printing spaces */

static void printSpaces (void) {
  int i;

  for (i = 0; i < indentno; ++i) fprintf(listing, " ");
}

/* procedure printTree prints a syntax tree to the listing file using indentation to indicate subtrees */

void printTree (TreeNode *tree) {
  int i ;
  INDENT;
  while (tree != NULL) {
    printSpaces();
    if (tree->nodekind == StmtK) {
      switch (tree->kind.stmt) {
        case IfK:
          fprintf(listing, "If\n");
          break;
		case WhileK:
		  fprintf(listing, "While\n");
		  break;
        case RepeatK:
          fprintf(listing, "Repeat\n");
          break;
        case AssignK:
          fprintf(listing, "Assign to: %s\n", tree->attr.name);
          break;
        case ReadK:
          fprintf(listing, "READ: %s\n", tree->attr.name);
          break;
        case WriteK:
          fprintf(listing, "Write\n");
          break;
        case CompoundStmtK:
          fprintf(listing, "Compound statement\n");
		  printTree(tree->child[0]);
		  printTree(tree->child[1]);
          break;
        case ExpressionStmtK:
          fprintf(listing, "Expression statement\n");
		  printTree(tree->child[2]);
          break;
        case SelectionStmtK:
          fprintf(listing, "Selection statement\n");
		  printTree(tree->child[6]);
		  printTree(tree->child[3]);
		  printTree(tree->child[4]);
		  printTree(tree->child[5]);
          break;
        case IterationStmtK:
          fprintf(listing, "Iteration statement\n");
		  printTree(tree->child[6]);
		  printTree(tree->child[3]);
		  printTree(tree->child[4]);
          break;
        case ReturnStmtK:
          fprintf(listing, "Return\n");
		  printTree(tree->child[4]);
          break;
        default:
          fprintf(listing, "Unknown ExpNode kind\n");
          break;
      }
    }
    else if (tree->nodekind == ExpK) {
      switch (tree->kind.exp) {
        case OpK:
          fprintf(listing, "Op: ");
          printToken(tree->attr.op, "\0");
          break;
        case ConstK:
          fprintf(listing, "Const: %d\n", tree->attr.val);
          break;
        case IdK:
          fprintf(listing, "ID: %s\n", tree->attr.name);
          break;
		case ArrK:
		  fprintf(listing, "Array\n");
		  printTree(tree->child[5]);
		  printTree(tree->child[1]);
		  break;
		case LvarK:
		  fprintf(listing, "OP : =\n");
		  printTree(tree->child[0]);
		  printTree(tree->child[1]);
		  break;
		case ComparisionExpK:
		  fprintf(listing, "ComparisionK\n");
		  printTree(tree->child[3]);
		  break;
		case AdditiveExpK:
		  fprintf(listing, "AdditiveExpK : \n");
		  printTree(tree->child[3]);
		  break;
		case MultiplicativeExpK:
		  fprintf(listing, "MultiplicativeExpK\n");
		  printTree(tree->child[3]);
		  break;
		case CallK:
          fprintf(listing, "Call Procedure: %s\n", tree->child[5]->attr.name);
		  if (tree->child[6] != NULL)
		  	printTree(tree->child[6]);
		  break;
        default:
          fprintf(listing, "Unknown ExpNode kind\n");
          break;
      }
    }
	else if (tree->nodekind == DeclK) {
		switch (tree->kind.decl) {
			case VarK:
          	 fprintf(listing, "VarK\n");
			 printTree(tree->child[1]);
			 printTree(tree->child[0]);
			 break;
			case VarArrK:
          	 fprintf(listing, "VarArrK\n");
			 printTree(tree->child[1]);
			 printTree(tree->child[0]);
			 printTree(tree->child[4]);
			 break;
			case ParamK:
          	 fprintf(listing, "Parameter : %s\n", tree->child[1]->attr.val);
			 printTree(tree->child[0]);
			 break;
			case ParamArrK:
          	 fprintf(listing, "Parameter Array : %s\n", tree->child[1]->attr.val);
			 printTree(tree->child[0]);
			 break;
			case ParamVoidK:
          	 fprintf(listing, "Parameter : (null)\n");
			 break;
			case FuncK:
          	 fprintf(listing, "Function : %s\n", tree->child[1]->attr.val);
			 printTree(tree->child[0]);
			 printTree(tree->child[2]);
			 printTree(tree->child[3]);
			 break;
			case TypeK:
			 switch (tree->type) {
			 	case Integer:
          	 	 fprintf(listing, "Type = Int\n");
				 break;
				case Void:
          	 	 fprintf(listing, "Type = Void\n");
				 break;
				case LT:
          	 	 fprintf(listing, "Op : <\n");
				 printTree(tree->child[2]);
				 printTree(tree->child[4]);
				 break;
				case LE:
          	 	 fprintf(listing, "Op : <=\n");
				 printTree(tree->child[2]);
				 printTree(tree->child[4]);
				 break;
				case GT:
          	 	 fprintf(listing, "Op : >\n");
				 printTree(tree->child[2]);
				 printTree(tree->child[4]);
				 break;
				case GE:
				 fprintf(listing, "Op : >=\n");
				 printTree(tree->child[2]);
				 printTree(tree->child[4]);
				 break;
				case EQ:
				 fprintf(listing, "Op : ==\n");
				 printTree(tree->child[2]);
				 printTree(tree->child[4]);
				 break;
				case NE:
				 fprintf(listing, "Op : !=\n");
				 printTree(tree->child[2]);
				 printTree(tree->child[4]);
				 break;
				case PLUS:
				 fprintf(listing, "Op : +\n");
				 printTree(tree->child[2]);
				 printTree(tree->child[4]);
				 break;
				case MINUS:
				 fprintf(listing, "Op : -\n");
				 printTree(tree->child[2]);
				 printTree(tree->child[4]);
				 break;
				case TIMES:
				 fprintf(listing, "Op : *\n");
				 printTree(tree->child[2]);
				 printTree(tree->child[4]);
				 break;
				case OVER:
				 fprintf(listing, "Op : /\n");
				 printTree(tree->child[2]);
				 printTree(tree->child[4]);
				 break;
				case ASSIGN:
				 fprintf(listing, "Op : =\n");
				 printTree(tree->child[0]);
				 printTree(tree->child[1]);
				defualt:
				 fprintf(listing, "Unknown Type\n");
				 break;
			 }
			 break;
			default:
			 fprintf(listing, "Unknown DeclNode kind\n");
			 break;
		}
	}
    else fprintf(listing, "Unknown node kind\n");
    /*for (i = 0; i < MAXCHILDREN; ++i) {
      printTree(tree->child[i]);
    }*/
    tree = tree->sibling;
  }
  UNINDENT;
}

