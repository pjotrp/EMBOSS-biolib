/******************************************************************************
** @source AJAX tree functions
** These functions create and control trees.
**
** @author Copyright (C) 2004 Peter Rice
** @@
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public
** License along with this library; if not, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
******************************************************************************/

/*Library* Tree Library *******************************************************
**
** All trees consist of an initial header followed by the body
** of the tree. The Header has three variables:-
** 1) Nodes -  an array of top level nodes with names
** 2) Count - which holds the number of named objects in the trees
** 3) Type - the tree type
**
** The body of the tree contains three variables:-
** 1) next - a pointer to the next tree node or NULL
** 2) prev - a pointer to the higher tree node or NULL
** 3) item - a void pointer to the data.
******************************************************************************/

#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "ajstr.h"
#include "ajassert.h"
#include "ajmem.h"
#include "ajtree.h"
#include "ajmess.h"

static ajint treeNewCnt     = 0;
static ajint treeDelCnt     = 0;
static ajint treeMaxNum     = 0;
static ajint treeNodeCnt    = 0;
static ajint treeIterNewCnt = 0;
static ajint treeIterDelCnt = 0;




static AjPTree treeNew(AjEnum type);
static void* treeNodeItem(const AjPTreeNode node);
static AjBool treeNodeDel(AjPTreeNode * pnode);


/* @func ajTreeNew ************************************************************
**
** Creates a new general tree.
**
** @return [AjPTree] new tree;
** @category new [AjPTree] Creates a new general tree.
** @@
******************************************************************************/

AjPTree ajTreeNew(void)
{
    return treeNew(ajETreeAny);
}




/* @func ajTreestrNew *********************************************************
**
** Creates a new string tree.
**
** @return [AjPTree] new tree;
** @category new [AjPTree] Creates a new AjPStr tree.
** @@
******************************************************************************/

AjPTree ajTreestrNew(void)
{
    return treeNew(ajETreeStr);
}




/* @funcstatic treeNew ********************************************************
**
** Creates a new tree.
**
** @param [r] type [AjEnum] Defined tree type.
** @return [AjPTree] new tree;
** @@
******************************************************************************/

static AjPTree treeNew(AjEnum type)
{
    AjPTree tree;

    AJNEW0(tree);
    tree->Type = type;

    tree->Nodes = ajListNew();

    treeNodeCnt--;			/* dummy */
    treeNewCnt++;

    return tree;
}



/* @func ajTreestrCopy ********************************************************
**
** Copy a string tree.
**
** WARNING: pointers to the data are copied, NOT the data
**          so be careful when cleaning up after copy.
**
** @param [r] thys [const AjPTree] Tree to be copied
** @return [AjPTree] New, copied, tree.
** @category new [AjPTree] Copies an AjPStr tree to a new tree.
** @@
******************************************************************************/

AjPTree ajTreestrCopy(const AjPTree thys)
{
    return ajTreeCopy(thys);
}




/* @func ajTreeCopy ***********************************************************
**
** Copy a tree.
**
** WARNING: pointers to the data are copied, NOT the data
**          so be careful when cleaning up after copy.
**
** @param [r] thys [const AjPTree] tree to be copied
** @return [AjPTree] new copied tree.
** @category new [AjPTree] Copies a tree to a new tree.
** @@
******************************************************************************/

AjPTree ajTreeCopy(const AjPTree thys)
{
    AjPTree newtree;

    if(!thys)
	return NULL;

    newtree = ajTreeNew();
    newtree->Type = thys->Type;

    treeNodeItem(NULL);			/* just so it gets used */

    return newtree;
}




/* @func ajTreestrClone *******************************************************
**
** Copy a string tree. The destination tree should be empty.
** If it is not, new entries are appended.
**
** WARNING: Makes new copies of the strings. No good general solution
**          so this is a strings-only function.
**
** @param [r] thys [const AjPTree] tree to be copied
** @param [u] newtree [AjPTree] (empty) target tree
** @return [ajint] number of nodes.
** @@
******************************************************************************/

ajint ajTreestrClone(const AjPTree thys, AjPTree newtree)
{
    ajint ret = 0;

    if(!thys)
	return 0;

    if(!newtree)
	return 0;

    return ret;
}




/* @func ajTreeFirst **********************************************************
**
** Set pointer to first node's data. Does NOT remove the first node.
**
** @param [r] thys [const AjPTree] Tree
** @param [w] x [void**] pointer to pointer to data
** @return [AjBool] ajTrue on success.
** @category cast [AjPTree] Set pointer to first node's data.
**                          Doesn't remove the node.
** @@
******************************************************************************/

AjBool ajTreeFirst(const AjPTree thys, void** x)
{
    if(!thys)
	return ajFalse;

    return ajTrue;
}




/* @func ajTreeLast ***********************************************************
**
** Set pointer to last node's data. Does NOT remove the last node.
**
** @param [r] thys [const AjPTree] Tree
** @param [w] x [void**] pointer to pointer to data
** @return [AjBool] ajTrue on success.
** @category cast [AjPTree] Set pointer to last node's data.
**                          Doesn't remove the node.
** @@
******************************************************************************/

AjBool ajTreeLast(const AjPTree thys, void** x)
{

    if(!thys)
	return ajFalse;
    if(!ajListLength(thys->Nodes))
	return ajFalse;

    return ajTrue;
}




/* @func ajTreeNth ************************************************************
**
** Set pointer to last node's nth data item. 0 <= n < number of elements.
**
** @param [r] thys [const AjPTree] Tree
** @param [r] n [ajint] element of the tree
** @param [w] x [void**] pointer to pointer to data
** @return [AjBool] ajTrue on success.
** @@
******************************************************************************/

AjBool ajTreeNth(const AjPTree thys, ajint n, void** x)
{
    ajint len;

    if(!thys || n<1)
	return ajFalse;

    len = ajTreeLength(thys);
    if(n>len)
	return ajFalse;

    return ajTrue;
}




/* @funcstatic treeNodeDel ****************************************************
**
** Remove a first node from the tree.
**
** @param [d] pnode  [AjPTreeNode*] Current node.
** @return [AjBool] ajTrue on success.
** @@
******************************************************************************/

static AjBool treeNodeDel(AjPTreeNode * pnode)
{
    return ajTrue;
}




/* @funcstatic treeNodeItem ***************************************************
**
** Return the data item for a tree node.
**
** @param [r] node  [const AjPTreeNode] Current node.
** @return [void*] Data item.
** @@
******************************************************************************/

static void* treeNodeItem(const AjPTreeNode node)
{
    if(!node || !node->Next)
	return NULL;

    return node->Item;
}






/* @func ajTreeLength *********************************************************
**
** get the number of nodes in the linked tree.
**
** @param [r] thys [const AjPTree] Tree
** @return [ajint] Number of nodes in tree.
** @category cast [AjPTree] get the number of nodes in a linked tree.
** @@
******************************************************************************/

ajint ajTreeLength(const AjPTree thys)
{
    if(!thys)
	return 0;

    return ajListLength(thys->Nodes);
}




/* @func ajTreestrLength ******************************************************
**
** get the number of nodes in the linked tree.
**
** @param [r] thys [const AjPTree] Tree
** @return [ajint] Number of nodes in tree.
** @category cast [AjPTree] get the number of nodes in an AjPStr linked tree.
** @@
******************************************************************************/

ajint ajTreestrLength(const AjPTree thys)
{
    return ajTreeLength(thys);
}




/* @func ajTreeFree ***********************************************************
**
** Free all nodes in the tree.
** NOTE: The data is only freed with a specified tree type.
**       For undefined data types we recommend you to
**       use ajTreeMap with a routine to free the memory.
**
** @param [d] pthis [AjPTree*] Tree
** @return [void]
** @category delete [AjPTree] Free the tree, and free the items with
**                            a simple "free".
** @@
******************************************************************************/

void ajTreeFree(AjPTree* pthis)
{
    AjPTree thys;

    if(!pthis)
	return;

    if(!*pthis)
	return;

    treeDelCnt++;

    thys = *pthis;


    return;
}




/* @func ajTreestrFree ********************************************************
**
** Free all nodes in a string tree.
** Also deletes all the strings. If these are to be preserved,
** use ajTreestrDel instead.
**
** @param [d] pthis [AjPTree*] Tree
** @return [void]
** @category delete [AjPTree] Free the tree, and free the items with ajStrDel
** @@
******************************************************************************/

void ajTreestrFree(AjPTree* pthis)
{
    AjPTree thys;

    if(!pthis)
	return;

    if(!*pthis)
	return;

    treeDelCnt++;

    thys = *pthis;
    AJFREE(*pthis);

    return;
}




/* @func ajTreeDel ************************************************************
**
** Free the tree. Do not attempt to free the nodes.
** For use where the node data has been saved elsewhere, for example
** by ajTreeToArray or where the tree is a temporary structure
** referring to permanent data.
**
** @param [d] pthis [AjPTree*] Tree
** @return [void]
** @category delete [AjPTree] Free the tree but do not try to free the nodes.
**                            Nodes should be freed first by ajTreeMap.
** @@
******************************************************************************/

void ajTreeDel(AjPTree* pthis)
{
    AjPTree tree;

    if(!pthis)
	return;
    if(!*pthis)
	return;

    treeDelCnt++;

    treeNodeDel(NULL);
    tree = *pthis;

    AJFREE(*pthis);

    return;
}




/* @func ajTreestrDel *********************************************************
**
** Free the tree. Do not attempt to free the nodes.
** For use where the node data has been saved elsewhere, for example
** by ajTreeToArray or where the tree is a temporary structure
** referring to permanent data.
**
** @param [d] pthis [AjPTree*] Tree
** @return [void]
** @category delete [AjPTree] Free the tree but do not try to free the nodes.
**                         use where nodes are still in use,
**                         e.g. in ajTreeToArray.
** @@
******************************************************************************/

void ajTreestrDel(AjPTree* pthis)
{
    ajTreeDel(pthis);

    return;
}




/* @func ajTreeMap ************************************************************
**
** For each node in the tree call function apply.
**
** @param [u] thys [AjPTree] Tree.
** @param [f] apply [void function] Function to call for each tree item.
** @param [u] cl [void*] Standard, usually NULL.
** @return [void]
** @category modify [AjPTree] Call a function for each node in a tree.
** @@
******************************************************************************/

void ajTreeMap(AjPTree thys, void apply(void** x, void* cl), void* cl)
{

    assert(apply);

    return;
}




/* @func ajTreestrMap *********************************************************
**
** For each node in the tree call function apply,
** with the address of the string and a client pointer.
**
** @param [u] thys [AjPTree] Tree.
** @param [f] apply [void function] Function to call for each tree item.
** @param [u] cl [void*] Standard, usually NULL.
** @return [void]
** @category modify [AjPTree] Call a function for each node in a tree.
** @@
******************************************************************************/

void ajTreestrMap(AjPTree thys, void apply(AjPStr* x, void* cl), void* cl)
{

    assert(apply);


    return;
}




/* @func ajTreeToArray ********************************************************
**
** Create an array of the pointers to the data.
**
** @param [r] thys [const AjPTree] Tree
** @param [w] array [void***] Array of pointers to tree items.
** @return [ajint] Size of array of pointers.
** @category cast [AjPTree] Create an array of the pointers to the data.
** @@
******************************************************************************/

ajint ajTreeToArray(const AjPTree thys, void*** array)
{
    ajint i;
    ajint n;
    AjPTreeNode rest = NULL;
 
    n = ajListLength(thys->Nodes);

    if(!n)
    {
	*array = NULL;
	return 0;
    }

    if (*array)
	AJFREE(*array);

    *array = AJALLOC((n+1)*sizeof(array));
    for(i = 0; i < n; i++)
    {
	(*array)[i] = rest->Item;
	rest = rest->Next;
    }
    (*array)[n] = 0;

    return n;
}




/* @func ajTreestrToArray *****************************************************
**
** create an array of the pointers to the data.
**
** @param [r] thys [const AjPTree] Tree
** @param [w] array [AjPStr**] Array of Strings.
**
** @return [ajint] Size of array of pointers.
** @category cast [AjPTree] Create an array of the pointers to the data.
** @@
******************************************************************************/

ajint ajTreestrToArray(const AjPTree thys, AjPStr** array)
{
    ajint i;
    ajint n;
    AjPTreeNode rest = NULL;

    n = ajListLength(thys->Nodes);

    if(!n)
    {
	*array = NULL;
	return 0;
    }

    *array = AJALLOC((n+1)*sizeof(array));

    for(i = 0; i < n; i++)
    {
	(*array)[i] = (AjPStr) rest->Item;
	rest = rest->Next;
    }
    (*array)[n] = 0;

    return n;
}




/* @func ajTreestrToArrayApp **************************************************
**
** append to an array of the pointers to the data.
**
** @param [r] thys [const AjPTree] Tree
** @param [w] array [AjPStr**] Array of Strings.
**
** @return [ajint] Size of array of pointers.
**
** @@
******************************************************************************/

ajint ajTreestrToArrayApp(const AjPTree thys, AjPStr** array)
{
    ajint i;
    ajint n;
    ajint j;
    AjPTreeNode rest = NULL;

    if (*array)
    {
	for (j=0; array[j]; j++)
	    continue;
    }
    else
	j = 0;

    n = ajListLength(thys->Nodes) + j;

    if(!n)
    {
	*array = NULL;
	return 0;
    }

    AJCRESIZE(*array, (n+1));

    for(i = j; i < n; i++)
    {
	(*array)[i] = (AjPStr) rest->Item;
	rest = rest->Next;
    }
    (*array)[n] = 0;

    return n;
}




/* @func ajTreeDummyFunction **************************************************
**
** Dummy function to catch all unused functions defined in ajtree
**
** @return [void]
******************************************************************************/

void ajTreeDummyFunction()
{

    return;
}




/* @func ajTreeGarbageCollect *************************************************
**
** Garbage collect a tree
**
** @param [u] tree [AjPTree] Tree.
** @param [f] destruct [void* function] Wrapper function for item destructor
** @param [f] compar [AjBool* function] Function to test whether to delete
** @return [void]
** @@
******************************************************************************/

void ajTreeGarbageCollect(AjPTree tree, void (*destruct)(const void **),
			  AjBool (*compar)(const void *))
{
    return;
}






/* @func ajTreeExit ***********************************************************
**
** Prints a summary of tree usage with debug calls
**
** @return [void]
** @@
******************************************************************************/

void ajTreeExit(void)
{
    ajDebug("Tree usage : %d opened, %d closed, %d maxsize %d nodes\n",
	     treeNewCnt, treeDelCnt, treeMaxNum, treeNodeCnt);
    ajDebug("Tree iterator usage : %d opened, %d closed, %d maxsize\n",
	     treeIterNewCnt, treeIterDelCnt);

    return;
}
