/* util.h */

#ifndef _UTIL_H_
#define _UTIL_H_

/* Procedure printToken prints a token and its lexeme to the listing file */

void printToken(TokenType, const char*);

/* Function newStmtNode creates a new statement node for syntax tree construction */

TreeNode *newStmtNode(StmtKind);

/* Function newExpNode creates a new expression node for syntax tree construction */

TreeNode *newExp(ExpKind);

TreeNode *newDeclNode(DeclKind);
TreeNode *addNode(TreeNode*, TreeNode*);
TreeNode *variableDeclarationNode(TreeNode*, TreeNode*, int);
TreeNode *arrayDeclarationNode(TreeNode*, TreeNode*, int, TreeNode*);
TreeNode *functionDeclarationNode(TreeNode*, TreeNode*, TreeNode*, TreeNode*, int);
TreeNode *variableParameterNode(TreeNode*, TreeNode*, int);
TreeNode *arrayParameterNode(TreeNode*, TreeNode*, int);
TreeNode *compoundStatementNode(TreeNode*, TreeNode*, int);
TreeNode *expressionStatementNode(TreeNode*);
TreeNode *assignExpression(TreeNode*, TreeNode*);
TreeNode *selectionStatementNode(TreeNode*, TreeNode*, TreeNode*);
TreeNode *iterationStatementNode(TreeNode*, TreeNode*);
TreeNode *returnStatementNode(TreeNode*);
TreeNode *tokenType(TokenType);
TreeNode *newIdNode(char*);
TreeNode *newConstNode(char*);


/* Function copyString allocates and make a new copy of an existing string */

char *copyString(char *);

/* procedure printTree prints a syntax tree to the listing file using indentation to indicate subtrees */

void printTree (TreeNode *);

#endif
