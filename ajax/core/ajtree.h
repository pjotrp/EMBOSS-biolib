#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ajtree_h
#define ajtree_h

#include "ajdefine.h"
#include "ajlist.h"

enum AjETreeType {ajETreeAny, ajETreeStr};

#define AjPTreeNode AjOTreeNode*

/* @data AjPTree **************************************************************
**
** Tree data object. Trees are simple linked nodes with back pointers.
**
** Trees can hold any data type. Special functions are available for trees
** of AjPStr values. In general, these functions are the same. Many are
** provided for ease of use to save remembering which calls need special cases.
**
** At the top level, a tree has a list of named nodes and a pointer to the
** top node in the tree.
**
** @new ajTreeNew Creates a new general tree.
** @new ajTreestrNew Creates a new AjPStr tree.
**
** @attr Nodes [AjPList] List of tree nodes, each of which is a valid subtree
** @attr Top [AjPTreeNode] Top node in the tree
** @attr Type [AjEnum] Tree type (any, string, etc.)
** @attr Data [void*] Data value
** @@
******************************************************************************/

typedef struct AjSTree {
  const struct AjSTree* Right;
  const struct AjSTree* Left;
  const struct AjSTree* Up;
  const struct AjSTree* Down;
  AjEnum Type;
  void* Data;
} AjOTree;

#define AjPTree AjOTree*

AjBool  ajTreeAddData(AjPTree thys, void* data);
AjPTree ajTreeAddNode(AjPTree thys);
AjPTree ajTreeAddSubNode(AjPTree thys);
AjPTree ajTreeCopy(const AjPTree thys);

void    ajTreeDel(AjPTree* pthis);
AjPTree ajTreeDown(AjPTree thys);
void    ajTreeExit(void);
void    ajTreeFree(AjPTree* pthis);
AjBool  ajTreeGetData(AjPTree thys, void** data);
ajint   ajTreeLength(const AjPTree thys);
void    ajTreeMap(AjPTree thys, void apply(void** x, void* cl), void* cl);
AjPTree ajTreeNew(void);
AjPTree ajTreeNext(AjPTree thys);
AjPTree ajTreePrev(AjPTree thys);
AjPTree ajTreestrCopy(const AjPTree thys);
void    ajTreestrDel(AjPTree* pthis);
void    ajTreestrFree(AjPTree* pthis);
void    ajTreestrMap(AjPTree thys, void apply(AjPStr* x, void* cl), void* cl);
AjPTree ajTreestrNew(void);
ajint   ajTreestrToArray(const AjPTree thys, AjPStr** array);
ajint   ajTreeToArray(const AjPTree thys, void*** array);
void    ajTreeTrace(const AjPTree thys);
AjPTree ajTreeUp(AjPTree thys);

void    ajTreeDummyFunction();

#endif

#ifdef __cplusplus
}
#endif
