#include "global.h"
#include "util.h"
#include "cminus.tab.h"

TreeNode * newStmtNode(StmtKind kind){

	TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    	int i;

	for (i=0;i<MAXCHILDREN;i++)
		t->child[i] = NULL;
	t->sibling = NULL;
	t->nodekind = Stmt;
	t->kind.stmt = kind;
	t->lineno = lineno;

    return t;
}

TreeNode * newExpNode(ExpKind kind){
	
	TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;

	for (i=0;i<MAXCHILDREN;i++)
		t->child[i] = NULL;
	t->sibling = NULL;
	t->nodekind = Exp;
	t->kind.exp = kind;
	t->lineno = lineno;

    return t;
}

TreeNode * newDeclNode(DeclKind kind){
	
	TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;

	for (i=0;i<MAXCHILDREN;i++)
		t->child[i] = NULL;
	t->sibling = NULL;
	t->nodekind = Decl;
	t->kind.decl = kind;
	t->lineno = lineno;

   return t;
}

/*TreeNode * newListNode(ListKind kind){
	
	TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;

	for (i=0;i<MAXCHILDREN;i++)
		t->child[i] = NULL;
	t->sibling = NULL;
	t->nodekind = ListK;
	t->kind.list = kind;
	t->lineno = lineno;

    return t;
}*/



/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString(char * s)
{ int n;
  char * t;
  if (s==NULL) return NULL;
  n = strlen(s)+1;
  t = malloc(n);
  if (t==NULL)
    printf("Out of memory error at line %d\n",lineno);
  else strcpy(t,s);
  return t;
}


/* =========================== IMPRESSAO DA ARVORE SINTATICA ============================= */


/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{ int i;
  for (i=0;i<indentno;i++)
    printf(" ");
}

/* procedure printTree prints a syntax tree to the 
 * listing file using indentation to indicate subtrees
 */
 
void printTree( TreeNode * tree ){
    int i;
    INDENT;
    while (tree != NULL) {
        printSpaces();
        if (tree->nodekind==Stmt){
            switch (tree->kind.stmt){
                case If:
                    printf("if\n");
                    break;
                case Iter:
                    printf("while\n");
                    break;
                case Attr:
                    printf("=\n");
                    break;
                /*case Cmpd:
                    printf("Cmpd\n");
                    break;*/
                case Return:
                    printf("return\n");
                    break;
                case Ativacao:
                    printf("%s\n",tree->attr.name);
                    break;
                default:
                    printf("Unknown ExpNode kind\n");
                    break;
            }
        } else if (tree->nodekind==Exp){
            switch (tree->kind.exp){
                /*case Factor:
                    printf("Factor\n");
                    break;*/
                case Const:
                    printf("%d\n",tree->attr.val);
                    break;
                /*case Additive:
                    printf("Additive\n");
                    break;
                case Simple:
                    printf("Simple\n");
                    break;*/
                case Id:
                    printf("%s\n",tree->attr.name);
                    break;
                case Term:
                    printf("Term\n");
                    break;
                case Op:
                    //printf("Op\n");
			switch(tree->attr.op){
				case ASSIGN: printf("=");
				break;

				case MENOROUIGUAL: printf("<=");
				break;

				case LT: printf("<");
				break;

				case MAIOROUIGUAL: printf(">=");
				break;

				case MT: printf(">");
				break;

				case EQ: printf("==");
				break;

				case DIFERENT: printf("!=");
				break;

				case PLUS: printf("+");
				break;

				case MINUS: printf("-");
				break;

				case TIMES: printf("*");
				break;

				case OVER: printf("/");
				break;
			}
		    printf("\n");
                    break;
                default:
                    printf("Unknown ExpNode kind\n");
                    break;
            }
        } else if (tree->nodekind==Decl){
            switch (tree->kind.decl){
                case Var:
                    printf("int %s\n",tree->attr.name);
                    break;
                case Fun:
			switch(tree->type){
				case Void:
					printf("void ");
					break;
				case Int:
					printf("int ");
					break;
			}
                    printf("%s\n",tree->attr.name);
                    break;
                case Param:
                    printf("%s\n",tree->attr.name);
                    break;
                case Type:
                    printf("Type\n");
                    break;
                default:
                    printf("Unknown ExpNode kind\n");
                    break;
            }
        } /*else if (tree->nodekind==ListK){
            switch (tree->kind.list){
                case Params:
                    printf("Params\n");
                    break;
                case Local:
                    printf("Local\n");
                    break;
                case StmtList:
                    printf("StmtList\n");
                    break;
                case AdditiveList:
                    printf("AdditiveList\n");
                    break;
                case TermList:
                    printf("TermList\n");
                    break;
                case Args:
                    printf("Args\n");
                    break;
                default:
                    printf("Unknown ExpNode kind\n");
                    break;
            }
        } */else {
            printf("Unknown node kind\n");
        }

        for (i=0;i<MAXCHILDREN;i++)
            printTree(tree->child[i]);

        tree = tree->sibling;
    }

    UNINDENT;
}
