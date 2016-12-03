#ifndef _UTIL_H_
#define _UTIL_H_

TreeNode * newStmtNode(StmtKind);

TreeNode * newExpNode(ExpKind);

TreeNode * newDeclNode(DeclKind);

TreeNode * newListNode(ListKind);

TreeNode * newErrNode();

/* allocates and makes a new copy of an existing string */
char * copyString( char * );

#endif
