#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ajtree_h
#define ajtree_h

#include "ajdefine.h"
#include "ajlist.h"

enum AjETreeType {ajETreeAny, ajETreeStr};

/* @data AjPTreeNode **********************************************************
**
** Substructure of AjPTree
**
** @attr Next [struct AjSTreeNode*] next item
** @attr Prev [struct AjSTreeNode*] previous item
** @attr Item [void*] data value
** @@
******************************************************************************/

typedef struct AjSTreeNode {
	struct AjSTreeNode* Next;
	struct AjSTreeNode* Up;
	void *Item;
} AjOTreeNode;

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
** @attr nodes [AjPList] List of top level nodes
** @attr Type [AjEnum] Tree type (any, string, etc.)
** @@
******************************************************************************/

typedef struct AjSTree {
  AjPList Nodes;
  AjPTreeNode Top;
  AjEnum Type;
} AjOTree;

#define AjPTree AjOTree*

AjPTree ajTreestrCopy(const AjPTree thys);
AjPTree ajTreeCopy(const AjPTree thys);
ajint ajTreeLength(const AjPTree thys);
void ajTreeDummyFunction();

#endif

#ifdef __cplusplus
}
#endif
