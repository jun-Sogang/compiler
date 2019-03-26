/* FILE: analyze.h */

#ifndef _ANALYZE_H_
#define _ANALYZE_H_

/* Function buildSymtab constructs the symbol table by preorder traversal of the syntax tree */

void buildSymtab(TreeNode *);

/* Procedure typeCheck perform type checking by a postorder syntax tree traversal */

void typeCheck(TreeNode *);

#endif
