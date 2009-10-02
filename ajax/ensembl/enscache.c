/******************************************************************************
** @source Ensembl Cache functions
**
** @author Copyright (C) 1999 Ensembl Developers
** @author Copyright (C) 2006 Michael K. Schuster
** @modified 2009 by Alan Bleasby for incorporation into EMBOSS core
** @version $Revision: 1.2 $
** @@
**
** Bio::EnsEMBL::Utils::Cache CVS Revision: 1.2
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

/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */

#include "enscache.h"




/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */




/* @datastatic CachePNode *****************************************************
**
** Ensembl Cache Node.
**
** @alias CacheSNode
** @alias CacheONode
**
** @attr Key [void*] Key data address
** @attr Value [void*] Value data address
** @attr Bytes [ajuint] Byte size of this node including key and value data
** @attr Dirty [AjBool] Flag to mark that value data has not been written back
** @@
******************************************************************************/

typedef struct CacheSNode
{
    void *Key;
    void *Value;
    ajuint Bytes;
    AjBool Dirty;
} CacheONode;

#define CachePNode CacheONode*




/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

static CachePNode cacheNodeNew(const EnsPCache cache, void* key, void* value);

static void cacheNodeDel(const EnsPCache cache, CachePNode* Pnode);

static AjBool cacheNodeInsert(EnsPCache cache, CachePNode node);

static AjBool cacheNodeRemove(EnsPCache cache, const CachePNode node);




/* @funcstatic cacheNodeNew ***************************************************
**
** Default constructor for an Ensembl Cache Node.
**
** The size of the Cache Node will be estimated according to the Ensembl Cache
** type with the function already provided at the Cache initialisation stage.
** This fuction will also reference value data to increment an internal usage
** counter and prevent deletion of value data while in the cache.
**
** @param [r] cache [const EnsPCache] Ensembl Cache
** @param [r] key [void*] Key data address
** @param [r] value [void*] Value data address
**
** @return [CachePNode] Ensembl Cache Node or NULL
** @@
******************************************************************************/

static CachePNode cacheNodeNew(const EnsPCache cache, void* key, void* value)
{
    ajuint size = 0;
    
    ajuint *Puint = NULL;
    ajuint *Puintkey = NULL;
    
    CachePNode node = NULL;
    
    if(!cache)
	return NULL;
    
    if(!key)
	return NULL;
    
    if(!value)
	return NULL;
    
    AJNEW0(node);
    
    /* Add the size of the Ensembl Cache Node itself. */
    
    size += (ajuint) sizeof (CacheONode);
    
    if(cache->Type == ensECacheTypeNumeric)
    {
	/* Reference AJAX unsigned integer key data. */
	
	Puint = (ajuint *) key;
	
	AJNEW0(Puintkey);
	
	*Puintkey = *Puint;
	
	node->Key = (void *) Puintkey;
	
	/* Add the size of unsigned integer key data. */
	
	size += (ajuint) sizeof (ajuint);
    }
    
    if(cache->Type == ensECacheTypeAlphaNumeric)
    {
	/* Reference AJAX String key data. */
	
	node->Key = (void *) ajStrNewS((AjPStr) key);
	
	/* Add the size of AJAX String key data. */
	
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes((AjPStr) node->Key);
    }
    
    /* Reference the value data. */
    
    if(cache->Reference && value)
	node->Value = (*cache->Reference)(value);
    
    /* Calculate the size of the value data. */
    
    if(cache->Size && node->Value)
	size += (*cache->Size)(node->Value);
    
    node->Bytes = size;
    
    node->Dirty = ajFalse;
    
    return node;
}




/* @funcstatic cacheNodeDel ***************************************************
**
** Default destructor for an Ensembl Cache Node.
**
** @param [r] cache [const EnsPCache] Ensembl Cache
** @param [d] Pnode [CachePNode*] Ensembl Cache Node address
**
** @return [void]
** @@
******************************************************************************/

static void cacheNodeDel(const EnsPCache cache, CachePNode* Pnode)
{
    if(!cache)
	return;
    
    if(!Pnode)
	return;
    
    if(!*Pnode)
	return;
    
    /* Delete AJAX unsigned integer key data. */
    
    if(cache->Type == ensECacheTypeNumeric)
	AJFREE((*Pnode)->Key);
    
    /* Delete AJAX String key data. */
    
    if(cache->Type == ensECacheTypeAlphaNumeric)
	ajStrDel((AjPStr *) &((*Pnode)->Key));
    
    /* Delete the value data. */
    
    if(cache->Delete && (*Pnode)->Value)
	(*cache->Delete)(&((*Pnode)->Value));
    
    AJFREE(*Pnode);

    *Pnode = NULL;
    
    return;
}




/* @funcstatic cacheNodeInsert ************************************************
**
** Insert an Ensembl Cache Node into an Ensembl Cache.
**
** @param [u] cache [EnsPCache] Ensembl Cache
** @param [r] node [CachePNode] Ensembl Cache Node
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool cacheNodeInsert(EnsPCache cache, CachePNode node)
{
    CachePNode old = NULL;
    
    if(!cache)
	return ajFalse;
    
    if(!node)
	return ajFalse;
    
    if(cache->MaxSize && (node->Bytes > cache->MaxSize))
	return ajFalse;
    
    /* Insert the node into the AJAX List. */
    
    ajListPushAppend(cache->List, (void *) node);
    
    /* Insert the node into the AJAX Table. */
    
    ajTablePut(cache->Table, node->Key, (void *) node);
    
    /* Update the cache statistics. */
    
    cache->Bytes += node->Bytes;
    
    cache->Count++;
    
    cache->Stored++;
    
    /* If the cache is too big, remove the top node(s). */
    
    while((cache->MaxBytes && (cache->Bytes > cache->MaxBytes)) ||
	  (cache->MaxCount && (cache->Count > cache->MaxCount)))
    {
	/* Remove the top node from the AJAX List. */
	
	ajListPop(cache->List, (void **) &old);
	
	/* Remove the node also from the AJAX Table. */
	
	ajTableRemove(cache->Table, old->Key);
	
	/* Update the cache statistics. */
	
	cache->Bytes -= old->Bytes;
	
	cache->Count--;
	
	cache->Dropped++;
	
	/* Write changes of value data to disk if any. */
	
	if (cache->Write && old->Value && old->Dirty)
	    (*cache->Write)(old->Value);
	
	/* Both, key and value data are deleted via cacheNodeDel. */
	
	cacheNodeDel(cache, &old);
    }
    
    return ajTrue;
}




/* @funcstatic cacheNodeRemove ************************************************
**
** Remove an Ensembl Cache Node from an Ensembl Cache.
**
** @param [u] cache [EnsPCache] Ensembl Cache
** @param [r] node [const CachePNode] Ensembl Cache Node
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool cacheNodeRemove(EnsPCache cache, const CachePNode node)
{
    AjIList iter = NULL;
    
    CachePNode lnode = NULL;
    
    if(!cache)
	return ajFalse;
    
    if(!node)
	return ajFalse;
    
    /* Remove the node from the AJAX List. */
    
    iter = ajListIterNew(cache->List);
    
    while(!ajListIterDone(iter))
    {
	lnode = (CachePNode) ajListIterGet(iter);
	
	if(lnode == node)
	{
	    ajListIterRemove(iter);
	    
	    break;
	}
    }
    
    ajListIterDel(&iter);
    
    /* Remove the node from the AJAX Table. */
    
    ajTableRemove(cache->Table, node->Key);
    
    /* Update the cache statistics. */
    
    cache->Bytes -= node->Bytes;
    
    cache->Count--;
    
    cache->Removed++;
    
    return ajTrue;
}




/* @func ensCacheNew **********************************************************
**
** Default constructor for an Ensembl Cache.
**
** @param [r] type [AjEnum] Ensembl Cache type
**                          (ensECacheTypeNumeric or ensECacheTypeAlphaNumeric)
** @param [r] maxbytes [ajuint] Maximum number of bytes held in the cache
** @param [r] maxcount [ajuint] Maximum number of objects to be cached
** @param [r] maxsize [ajuint] Maximum size of an object to be cached
** @param [f] Freference [(void*)] Object-specific referencing function
** @param [f] Fdelete [(void)] Object-specific deletion function
** @param [f] Fsize [(ajuint)] Object-specific memory sizing function
** @param [f] Fread [(void*)] Object-specific reading function
** @param [f] Fwrite [(AjBool)] Object-specific writing function
** @param [r] syncron [AjBool] ajTrue: Immediately write-back value data
**                             ajFalse: Write-back value data later
** @param [r] label [const char*] Cache label for statistics output
**
** @return [EnsPCache] Ensembl Cache or NULL
** @@
** The maximum size parameter should prevent the cache from purging too many
** objects when very large objects are inserted. If not set it defaults to
** a tenth of the maximum cache size.
**
** Object-specific functions are required to reference objects held in the
** cache or delete objects once purged from the cache, as well as memory sizing
** functions and object-specific read and write back functions.
******************************************************************************/

EnsPCache ensCacheNew(AjEnum type,
                      ajuint maxbytes,
                      ajuint maxcount,
                      ajuint maxsize,
                      void* Freference(void* value),
                      void Fdelete(void** value),
                      ajuint Fsize(const void* value),
                      void* Fread(const void* key),
                      AjBool Fwrite(const void* value),
                      AjBool syncron,
                      const char *label)
{
    EnsPCache cache = NULL;
    
    /*
     ajDebug("ensCacheNew\n"
	     "  type %d\n"
	     "  maxbytes %u\n"
	     "  maxcount %u\n"
	     "  maxsize %u\n"
	     "  Freference %p\n"
	     "  Fdelete %p\n"
	     "  Fsize %p\n"
	     "  Fread %p\n"
	     "  Fwrite %p\n"
	     "  syncron '%B'\n"
	     "  label '%s'\n",
	     type,
	     maxbytes,
	     maxcount,
	     maxsize,
	     Freference,
	     Fdelete,
	     Fsize,
	     Fread,
	     Fwrite,
	     syncron,
	     label);
     */
    
    if((type < 1) || (type > 2))
	ajFatal("ensCacheNew requires a valid type.\n");
    
    if((!maxbytes) && (!maxcount))
	ajFatal("ensCacheNew requires either a "
		"maximum bytes or maximum count limit.\n");
    
    if(!maxsize)
	maxsize = maxbytes ? maxbytes / 10 + 1 : 0;
    
    if(maxbytes && (!maxsize))
	ajFatal("ensCacheNew requires a maximum size limit, "
		"when a maximum bytes limit is set.");
    
    /* TODO: Find and set a sensible value here! */
    
    /*
     ajDebug("ensCacheNew maxbytes %u, maxcount %u, maxsize %u.\n",
	     maxbytes, maxcount, maxsize);
     */
    
    if(maxbytes && (maxbytes < 1000))
	ajFatal("ensCacheNew cannot set a maximum bytes limit (%u) under "
		"1000, as each Cache Node requires %u bytes alone.",
		maxbytes, sizeof(CachePNode));
    
    /* TODO: Find and set a sensible value here! */
    
    if(maxsize && (maxsize < 3))
        ajFatal("ensCacheNew cannot set a maximum size limit (%u) under "
		"3 bytes. maximum bytes %u maximum count %u.",
                maxsize, maxbytes, maxcount);
    
    /*
    ** Pointers to functions for automatic reading of data not yet in the
    ** cache and writing of data modified in cache are not mandatory.
    ** If not specified the cache will simply lack this functionality.
    ** However, the specification of a function deleting stale cache entries
    ** and a function calculating the size of value data are required.
    */
    
    if(!(void*)Freference)
	ajFatal("ensCacheNew requires a referencing function.");
    
    if(!(void*)Fdelete)
	ajFatal("ensCacheNew requires a deletion function.");
    
    if(maxsize && (!(void*)Fsize))
	ajFatal("ensCacheNew requires a memory sizing function "
		"when a maximum size limit has been defined.");
    
    AJNEW0(cache);
    
    cache->List = ajListNew();
    
    if(type == ensECacheTypeNumeric)
	cache->Table =
	    ajTableNewFunctionLen(0, ensTableCmpUInt, ensTableHashUInt);
    
    if(type == ensECacheTypeAlphaNumeric)
	cache->Table = ajTablestrNewLen(0);
    
    cache->Label = ajStrNewC(label);
    
    cache->Reference = Freference;
    
    cache->Delete = Fdelete;
    
    cache->Size = Fsize;
    
    cache->Read = Fread;
    
    cache->Write = Fwrite;
    
    cache->Type = type;
    
    cache->Syncron = syncron;
    
    cache->MaxBytes = maxbytes;
    
    cache->MaxCount = maxcount;
    
    cache->MaxSize = maxsize;
    
    cache->Bytes = 0;
    
    cache->Count = 0;
    
    cache->Dropped = 0;
    
    cache->Removed = 0;
    
    cache->Stored = 0;
    
    cache->Hit = 0;
    
    cache->Miss = 0;
    
    return cache;
}




/* @func ensCacheDel **********************************************************
**
** Default destructor for an Ensembl Cache.
**
** @param [u] Pcache [EnsPCache*] Ensembl Cache address
**
** @return [void]
** @@
** Value data in Cache Nodes that have not been synchronised are written-back.
** Cache flags are reset for value data before the value data is deleted.
** After deletion of all Cache Nodes a summary statistics is printed and the
** Ensembl Cache is destroyed.
******************************************************************************/

void ensCacheDel(EnsPCache* Pcache)
{
    double ratio = 0;
    EnsPCache pthis = NULL;
    
    CachePNode node = NULL;
    
    /*
     ajDebug("ensCacheDel\n"
	     "  Pcache %p\n",
	     Pcache);
     */
    
    if(!Pcache)
	return;
    
    if(!*Pcache)
	return;

    pthis = *Pcache;
    
    /* Remove nodes from the AJAX List. */
    
    while(ajListPop(pthis->List, (void **) &node))
    {
	/* Remove the node from the AJAX Table. */
	
	ajTableRemove(pthis->Table, node->Key);
	
	/* Update the cache statistics. */
	
	pthis->Count--;
	
	pthis->Bytes -= node->Bytes;
	
	/* Write changes of value data to disk if any. */
	
	if (pthis->Write && node->Value && node->Dirty)
	    (*pthis->Write)(node->Value);
	
	/* Both, key and value data are deleted via cacheNodeDel. */
	
	cacheNodeDel(pthis, &node);
    }
    
    if(pthis->Hit || pthis->Miss)
	ratio = (double) pthis->Hit /
	    ((double) pthis->Hit + (double) pthis->Miss);
    
    ajDebug("ensCacheDel %S "
	    "hit: %u "
	    "miss: %u "
	    "ratio: %f "
            "dropped: %u "
	    "removed: %u "
	    "stored: %u "
	    "count: %u "
	    "bytes: %u\n",
	    pthis->Label,
	    pthis->Hit,
	    pthis->Miss,
	    ratio,
	    pthis->Dropped,
	    pthis->Removed,
	    pthis->Stored,
	    pthis->Count,
	    pthis->Bytes);
    
    ajListFree(&pthis->List);
    
    ajTableFree(&pthis->Table);
    
    ajStrDel(&pthis->Label);
    
    AJFREE(pthis);

    *Pcache = NULL;
    
    return;
}




/* @func ensCacheFetch ********************************************************
**
** Fetch a value from an Ensembl Cache via a key. If the value is not already
** in the cache it will be read by the function provided at the Cache
** initialisation stage.
**
** The caller is resposible for deleting the returned object.
**
** @param [u] cache [EnsPCache] Ensembl Cache
** @param [r] key [void*] Key data address
**
** @return [void*] Value data address or NULL
** @@
******************************************************************************/

void* ensCacheFetch(EnsPCache cache, void *key)
{
    void *value = NULL;
    
    AjIList iter = NULL;
    
    CachePNode lnode = NULL;
    CachePNode tnode = NULL;
    
    if(!cache)
	return NULL;
    
    if(!key)
	return NULL;
    
    tnode = (CachePNode) ajTableFetch(cache->Table, key);
    
    if(tnode)
    {
	cache->Hit++;
	
	/* Move the node to the end of the list. */
	
	iter = ajListIterNew(cache->List);
	
	while(!ajListIterDone(iter))
	{
	    lnode = (CachePNode) ajListIterGet(iter);
	    
	    if(lnode == tnode)
	    {
		ajListIterRemove(iter);
		
		ajListPushAppend(cache->List, (void *) lnode);
		
		break;
	    }
	}
	
	ajListIterDel(&iter);
	
	/*
	** Reference the object when returned by the cache so that external
	** code has to delete it irrespectively whether it was red from the
	** cache or instantiated by the cache->Read function.
	*/
	
	if(cache->Reference && tnode->Value)
	    value = (*cache->Reference)(tnode->Value);
    }
    else
    {
	cache->Miss++;
	
	if(cache->Read)
	{
	    value = (*cache->Read)(key);
	    
	    if(value)
	    {
		tnode = cacheNodeNew(cache, key, value);
		
		if(! cacheNodeInsert(cache, tnode))
		    cacheNodeDel(cache, &tnode);
	    }
	}
    }
    
    return value;
}




/* @func ensCacheStore ********************************************************
**
** Insert value data into an Ensembl Cache under key data.
**
** @param [u] cache [EnsPCache] Ensembl Cache
** @param [r] key [void*] Key data address
** @param [r] value [void**] Value data address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensCacheStore(EnsPCache cache, void* key, void** value)
{
    CachePNode node = NULL;
    
    if(!cache)
	return ajFalse;
    
    if(!key)
	return ajFalse;
    
    if(!value)
	return ajFalse;
    
    /* Is a node already cached under this key? */
    
    node = (CachePNode) ajTableFetch(cache->Table, key);
    
    if(node)
    {
	/*
	** Delete the Object passed in and increase the reference counter
	** of the cached Object before assigning it.
	*/
	
	(*cache->Delete)(value);
	
	*value = (*cache->Reference)(node->Value);
    }
    else
    {
	node = cacheNodeNew(cache, key, *value);
	
	if(cacheNodeInsert(cache, node))
	{
	    if(cache->Syncron)
	    {
		if(cache->Write && node->Value)
		    (*cache->Write)(node->Value);
		
		node->Dirty = ajFalse;
	    }
	    else
		node->Dirty = ajTrue;
	}
	else
	{
	    if(cache->Write && node->Value)
		(*cache->Write)(node->Value);
	    
	    cacheNodeDel(cache, &node);
	}
    }
    
    return ajTrue;
}




/* @func ensCacheRemove *******************************************************
**
** Remove value data from an Ensembl Cache via key data.
**
** @param [u] cache [EnsPCache] Ensembl Cache
** @param [r] key [const void*] Key data
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensCacheRemove(EnsPCache cache, const void* key)
{
    CachePNode node = NULL;
    
    if(!cache)
	return ajFalse;
    
    if(!key)
	return ajFalse;
    
    node = (CachePNode) ajTableFetch(cache->Table, key);
    
    if(node)
    {
	cacheNodeRemove(cache, node);
	
	/* Both, key and value data are deleted via cacheNodeDel. */
	
	cacheNodeDel(cache, &node);
    }
    
    return ajTrue;
}




/* @func ensCacheSyncronise ***************************************************
**
** Synchronise an Ensembl Cache by writing-back all value data that have not
** been written before.
**
** @param [u] cache [EnsPCache] Ensembl Cache
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensCacheSyncronise(EnsPCache cache)
{
    AjIList iter = NULL;
    
    CachePNode node = NULL;
    
    if(!cache)
	return ajFalse;
    
    iter = ajListIterNew(cache->List);
    
    while(!ajListIterDone(iter))
    {
	node = (CachePNode) ajListIterGet(iter);
	
	if(cache->Write && node->Value && node->Dirty)
	{
	    (*cache->Write)(node->Value);
	    
	    node->Dirty = ajFalse;
	}
    }
    
    ajListIterDel(&iter);
    
    return ajTrue;
}




/* @func ensCacheTrace ********************************************************
**
** Writes debug messages to trace the contents of a cache.
**
** @param [r] cache [const EnsPCache] Ensembl Cache
**
** @return [void]
** @@
******************************************************************************/

void ensCacheTrace(const EnsPCache cache)
{
    if(!cache)
	return;
    
    ajDebug("Ensembl Cache trace: "
	    "table length: %u "
	    "list length: %u\n",
	    ajTableGetLength(cache->Table),
	    ajListGetLength(cache->List));
    
    return;
}
