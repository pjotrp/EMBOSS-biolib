/* @source embindex.c
**
** B+ Tree Indexing plus Disc Cache.
** Copyright (c) 2003 Alan Bleasby
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
******************************************************************************/
#include "emboss.h"

#define BTENTRYFILE     ".ent"

static AjPFile    btreeCreateFile(AjPStr idirectory, AjPStr dbname,
				  char *add);
static EmbPBtpage embBtreeCacheLocate(EmbPBtcache cache, ajlong page);
static EmbPBtpage embBtreeCacheLruUnlink(EmbPBtcache cache);
static void       embBtreeCacheUnlink(EmbPBtcache cache, EmbPBtpage cpage);
static void       embBtreeCacheDestage(EmbPBtcache cache, EmbPBtpage cpage);
static EmbPBtpage embBtreePageNew(EmbPBtcache cache);
static void       embBtreeCacheFetch(EmbPBtcache cache, EmbPBtpage cpage,
				     ajlong pageno);
static void       embBtreeCacheMruAdd(EmbPBtcache cache, EmbPBtpage cpage);
static EmbPBtpage embBtreeCacheControl(EmbPBtcache cache, ajlong pageno,
				       AjBool isread);
static EmbPBtpage embBtreeFindINode(EmbPBtcache cache, EmbPBtpage page,
				    char *item);


static EmbPBtpage embBtreePageFromKey(EmbPBtcache cache, unsigned char *buf,
				      char *item);
static ajint      embBtreeNumInBucket(EmbPBtcache cache, ajlong pageno);
static EmbPBucket embBtreeReadBucket(EmbPBtcache cache, ajlong pageno);
static void       embBtreeWriteBucket(EmbPBtcache cache, EmbPBucket bucket,
				      ajlong pageno);
static void       embBtreeAddToBucket(EmbPBtcache cache, ajlong pageno,
				      EmbPBtId id);
static void 	  embBtreeBucketDel(EmbPBucket *thys);
static AjBool     embBtreeReorderBuckets(EmbPBtcache cache, EmbPBtpage page);
static void       embBtreeGetKeys(EmbPBtcache cache, unsigned char *buf,
				  AjPStr **keys, ajlong **ptrs);
static ajint      embBtIdCompare(const void *a, const void *b);
static EmbPBucket embBtreeBucketNew(ajint n);
static void       embBtreeWriteNode(EmbPBtcache cache, EmbPBtpage page,
				    AjPStr *keys, ajlong *ptrs, ajint nkeys);
static AjBool     embBtreeNodeIsFull(EmbPBtcache cache, EmbPBtpage page);
static void       embBtreeInsertNonFull(EmbPBtcache cache, EmbPBtpage page,
					AjPStr key, ajlong less,
					ajlong greater);
static void       embBtreeSplitRoot(EmbPBtcache cache);
static void       embBtreeInsertKey(EmbPBtcache cache, EmbPBtpage page,
				    AjPStr key, ajlong less, ajlong greater);
static EmbPBtpage embBtreeSplitLeaf(EmbPBtcache cache, EmbPBtpage spage);


static ajlong     embBtreeFindBalance(EmbPBtcache cache, ajlong thisNode,
				      ajlong leftNode, ajlong rightNode,
				      ajlong lAnchor, ajlong rAnchor,
				      EmbPBtId id);
static AjBool     embBtreeRemoveEntry(EmbPBtcache cache,ajlong pageno,
				      EmbPBtId id);
static void       embBtreeAdjustBuckets(EmbPBtcache cache, EmbPBtpage leaf);
static ajlong     embBtreeCollapseRoot(EmbPBtcache cache, ajlong pageno);
static ajlong     embBtreeRebalance(EmbPBtcache cache, ajlong thisNode,
				    ajlong leftNode, ajlong rightNode,
				    ajlong lAnchor, ajlong rAnchor);
static ajlong     embBtreeShift(EmbPBtcache cache, ajlong thisNode,
				ajlong balanceNode, ajlong anchorNode);
static ajlong     embBtreeMerge(EmbPBtcache cache, ajlong thisNode,
				ajlong mergeNode, ajlong anchorNode);

static void       embBtreeFindMin(EmbPBtcache cache, ajlong pageno, char *key);
static ajlong     embBtreeInsertShift(EmbPBtcache cache, EmbPBtpage tpage,
				      char *key);
static void       embBtreeKeyShift(EmbPBtcache cache, EmbPBtpage tpage);


static EmbPBtpage embBtreeTraverseLeaves(EmbPBtcache cache, EmbPBtpage thys);

static EmbPBtpage embBtreeFindINodeW(EmbPBtcache cache, EmbPBtpage page,
				     char *item);
static EmbPBtpage embBtreePageFromKeyW(EmbPBtcache cache, unsigned char *buf,
				       char *key);
static void       embBtreeReadLeaf(EmbPBtcache cache, EmbPBtpage page,
				   AjPList list);


/* @func embBtreeCacheNewC **************************************************
**
** Open a b+tree index file and initialise a cache object
**
** @param [r] file [const char *] name of file
** @param [r] mode [const char *] opening mode
** @param [r] pagesize [ajint] pagesize
** @param [r] order [ajint] Tree order
** @param [r] fill [ajint] Number of entries per bucket
** @param [r] level [ajint] level of tree
** @param [r] cachesize [ajint] size of cache
**
** @return [EmbPBtcache] initialised disc block cache structure
** @@
******************************************************************************/
EmbPBtcache embBtreeCacheNewC(const char *file, const char *mode,
			      ajint pagesize, ajint order, ajint fill,
			      ajint level, ajint cachesize)
{
    FILE *fp;
    EmbPBtcache cache = NULL;
#if defined (HAVE64)
    struct stat64 buf;
#else
    struct stat buf;
#endif
    ajlong filelen = 0L;

    fp = fopen(file,mode);
    if(!fp)
	return NULL;


#if defined (HAVE64)
    if(!stat64(file, &buf))
#else
    if(!stat(file, &buf))
#endif
	filelen = buf.st_size;

    AJNEW0(cache);

    cache->fp = fp;
    cache->listLength = 0;
    cache->lru = NULL;
    cache->mru = NULL;

    cache->replace = ajStrNew();

    if(pagesize>0)
	cache->pagesize = pagesize;
    else
	cache->pagesize = BT_PAGESIZE;

    cache->totsize   = 0L;
    cache->level     = level;

    cache->order      = order;
    cache->nperbucket = fill;
    cache->totsize    = filelen;
    cache->cachesize  = cachesize;
    
    return cache;
}




/* @funcstatic embBtreePageNew ***********************************************
**
** Construct a cache page object
**
** @param [r] cache [EmbPBtcache] cache
**
** @return [EmbPBtpage] initialised disc block cache structure
** @@
******************************************************************************/
static EmbPBtpage embBtreePageNew(EmbPBtcache cache)
{
    EmbPBtpage thys = NULL;

/*    ajDebug("In embBtreePageNew\n"); */

    AJNEW0(thys);
    AJCNEW0(thys->buf,cache->pagesize);
    thys->next = NULL;
    thys->prev = NULL;
    
    ++cache->listLength;

    
    return thys;
}




/* @funcstatic embBtreeBucketNew *********************************************
**
** Construct a bucket object
**
** @param [r] n [ajint] Number of IDs
**
** @return [EmbPBucket] initialised disc block cache structure
** @@
******************************************************************************/

static EmbPBucket embBtreeBucketNew(ajint n)
{
    EmbPBucket bucket = NULL;
    ajint i;

/*    ajDebug("In embBtreeBucketNew\n"); */
    
    AJNEW0(bucket);

    if(n)
    {
	AJCNEW0(bucket->Ids,n);
	AJCNEW0(bucket->keylen,n);
    }
    
    for(i=0;i<n;++i)
	bucket->Ids[i] = embBtreeIdNew();

    bucket->NodeType = BT_BUCKET;
    bucket->Nentries = n;
    bucket->Overflow = 0L;
    
    return bucket;
}




/* @funcstatic embBtreeCacheLocate *******************************************
**
** Search for a page in the cache
**
** @param [r] cache [EmbPBtcache] cache structure
** @param [r] page [ajlong] page number to locate
**
** @return [EmbPBtpage]	pointer to page or NULL if not found
** @@
******************************************************************************/

static EmbPBtpage embBtreeCacheLocate(EmbPBtcache cache, ajlong page)
{
    EmbPBtpage cpage = NULL;

/*    ajDebug("In embBtreeCacheLocate\n");*/
    
    for(cpage = cache->mru; cpage; cpage = cpage->prev)
	if(cpage->pageno == page)
	    return cpage;

    return NULL;
}




/* @funcstatic embBtreeCacheUnlink *******************************************
**
** Remove links to a cache page and return the address of the page
**
** @param [w] cache [EmbPBtcache] cache structure
** @param [r] cpage [EmbPBtpage] cache page
**
** @return [void]
** @@
******************************************************************************/

static void embBtreeCacheUnlink(EmbPBtcache cache, EmbPBtpage cpage)
{
/*    ajDebug("In embBtreeCacheUnlink\n"); */

    if(cache->mru == cpage)
    {
	cache->mru = cpage->prev;
	if(cpage->prev)
	    cpage->prev->next = NULL;
	if(cache->lru == cpage)
	    cache->lru = NULL;
    }
    else if(cache->lru == cpage)
    {
	cache->lru = cpage->next;
	if(cpage->next)
	    cpage->next->prev = NULL;
    }
    else
    {
	cpage->prev->next = cpage->next;
	cpage->next->prev = cpage->prev;
    }

    return;
}




/* @funcstatic embBtreeCacheMruAdd *******************************************
**
** Insert a cache page at the mru position
**
** @param [w] cache [EmbPBtcache] cache structure
** @param [r] cpage [EmbPBtpage] cache page
**
** @return [void]
** @@
******************************************************************************/

static void embBtreeCacheMruAdd(EmbPBtcache cache, EmbPBtpage cpage)
{
/*    ajDebug("In embBtreeCacheMruAdd\n"); */

    cpage->prev = cache->mru;
    cpage->next = NULL;
    if(cache->mru)
	cache->mru->next = cpage;
    if(!cache->lru)
	cache->lru = cpage;
    cache->mru = cpage;

    return;
}




/* @funcstatic embBtreeCacheLruUnlink ****************************************
**
** Remove links to an LRU cache page
**
** @param [w] cache [EmbPBtcache] cache structure
**
** @return [EmbPBtpage]	pointer to unlinked cache page
** @@
******************************************************************************/

static EmbPBtpage embBtreeCacheLruUnlink(EmbPBtcache cache)
{
    EmbPBtpage ret;

/*    ajDebug("In embBtreeCacheLruUnlink\n"); */

    if(cache->lru->dirty != BT_LOCK)
    {
	if(!cache->lru)
	    ajFatal("embBtreeCacheLruUnlink: No pages nodes found");

	ret = cache->lru;
	ret->next->prev = NULL;
	cache->lru = ret->next;
	return ret;
    }
    
    for(ret=cache->lru; ret; ret=ret->next)
	if(ret->dirty != BT_LOCK)
	    break;

    if(!ret)
	ajFatal("Too many locked cache pages. Try increasing cachesize");

    embBtreeCacheUnlink(cache,ret);
    
    return ret;
}




/* @funcstatic embBtreeCacheDestage *****************************************
**
** Destage a cache page
**
** @param [r] cache [EmbPBtcache] cache
** @param [r] cpage [EmbPBtpage] cache papge 
**
** @return [EmbPBtpage]	pointer to unlinked cache page
** @@
******************************************************************************/
static void embBtreeCacheDestage(EmbPBtcache cache, EmbPBtpage cpage)
{
    ajint written = 0;
    ajint retries = 0;

/*    ajDebug("In embBtreeCacheDestage\n");*/

    if(fseek(cache->fp,cpage->pageno,SEEK_SET)==-1)
	fseek(cache->fp,0L,SEEK_END);
    
    while(written != cache->pagesize && retries != BT_MAXRETRIES)
    {
	written += fwrite((void *)cpage->buf,1,cache->pagesize-written,
			  cache->fp);
	++retries;
    }
    
    if(retries == BT_MAXRETRIES)
	ajFatal("Maximum retries (%d) reached in embBtreeCacheDestage",
		BT_MAXRETRIES);

    cpage->dirty = BT_CLEAN;
    

    return;
}




/* @funcstatic embBtreeCacheFetch *****************************************
**
** Fetch a cache page from disc
**
** @param [r] cache [EmbPBtcache] cache
** @param [w] cpage [EmbPBtpage] cache page 
** @param [r] pageno [ajlong] page number
**
** @return [void]
** @@
******************************************************************************/
static void embBtreeCacheFetch(EmbPBtcache cache, EmbPBtpage cpage,
			       ajlong pageno)
{
    ajint sum = 0;
    ajint retries = 0;

/*    ajDebug("In embBtreeCacheFetch\n");*/

    if(fseek(cache->fp,pageno,SEEK_SET))
	ajFatal("Seek error in embBtreeCachefetch");
    
    while(sum != cache->pagesize && retries != BT_MAXRETRIES)
    {
	sum += fread((void *)cpage->buf,1,cache->pagesize-sum,
		     cache->fp);
	++retries;
    }
    
    if(retries == BT_MAXRETRIES)
	ajFatal("Maximum retries (%d) reached in embBtreeCacheFetch",
		BT_MAXRETRIES);

    cpage->pageno = pageno;
    
    return;
}



/* @func embBtreeCacheDel **************************************************
**
** Close a b+tree cache
**
** @param [w] filelist [EmbPBtcache*] list of files to read
**
** @return [void]
** @@
******************************************************************************/
void embBtreeCacheDel(EmbPBtcache *thys)
{
    EmbPBtcache pthis = *thys;
    EmbPBtpage  page  = NULL;
    EmbPBtpage  temp  = NULL;

/*    ajDebug("In embBtreeCacheDel\n");*/
    

    for(page=pthis->lru;page;page=temp)
    {
	temp = page->next;
	AJFREE(page->buf);
	AJFREE(page);
    }

    ajStrDel(&pthis->replace);
    
    fclose(pthis->fp);

    AJFREE(pthis);
    *thys = NULL;
    
    return;
}




/* @funcstatic embBtreeCacheControl ******************************************
**
** Master control function for cache read/write
**
** @param [w] file [EmbPBtcache] name of file
** @param [r] pageno [ajlong] page number
** @param [r] isread [AjBool] is this a read operation?
**
** @return [EmbPBtpage] disc cache page pointer
** @@
******************************************************************************/
static EmbPBtpage embBtreeCacheControl(EmbPBtcache cache, ajlong pageno,
				       AjBool isread)
{
    EmbPBtpage ret = NULL;
    unsigned char *buf = NULL;

/*    ajDebug("In embBtreeCacheControl\n");*/
    
    ret = embBtreeCacheLocate(cache,pageno);
    
    if(ret)
	embBtreeCacheUnlink(cache,ret);
    else
    {
	if(cache->listLength == cache->cachesize)
	{
	    ret = embBtreeCacheLruUnlink(cache);

	    if(ret->dirty == BT_DIRTY)
		embBtreeCacheDestage(cache,ret);
	    if(isread || pageno!=cache->totsize)
		embBtreeCacheFetch(cache,ret,pageno);
	}
	else
	{
	    ret = embBtreePageNew(cache);
	    buf = ret->buf;
	    SBT_BLOCKNUMBER(buf,pageno);
	    if(isread || pageno!=cache->totsize)
		embBtreeCacheFetch(cache,ret,pageno);
	}

	if(!isread)
	    ret->pageno = pageno;
	else
	    ret->dirty = BT_CLEAN;
    }

    embBtreeCacheMruAdd(cache,ret);

    return ret;
}




/* @func embBtreeCacheRead ******************************************
**
** Get a pointer to a disc cache page
**
** @param [w] file [EmbPBtcache] cache
** @param [r] pageno [ajlong] page number
**
** @return [EmbPBtpage] disc cache page pointer
** @@
******************************************************************************/
EmbPBtpage embBtreeCacheRead(EmbPBtcache cache, ajlong pageno)
{
    EmbPBtpage ret = NULL;

/*    ajDebug("In embBtreeCacheRead\n");*/

    ret = embBtreeCacheControl(cache,pageno,BT_READ);

    return ret;
}




/* @func embBtreeCacheSync *********************************************
**
** Sync all dirty cache pages
**
** @param [r] file [EmbPBtcache] cache
**
** @return [void]
** @@
******************************************************************************/
void embBtreeCacheSync(EmbPBtcache cache)
{
    EmbPBtpage page = NULL;

/*    ajDebug("In embBtreeCacheSync\n");*/

    for(page=cache->lru;page;page=page->next)
	if(page->dirty == BT_DIRTY || page->dirty == BT_LOCK)
	    embBtreeCacheDestage(cache,page);

    page = embBtreeCacheLocate(cache,0L);
    page->dirty = BT_LOCK;

    return;
}



/* @func embBtreeCacheWrite ******************************************
**
** Get a pointer to a disc cache page for writing
**
** @param [w] file [EmbPBtcache] cache
** @param [r] pageno [ajlong] page number
**
** @return [EmbPBtpage] disc cache page pointer
** @@
******************************************************************************/
EmbPBtpage embBtreeCacheWrite(EmbPBtcache cache, ajlong pageno)
{
    EmbPBtpage ret = NULL;

/*    ajDebug("In embBtreeCacheWrite\n");*/

    ret = embBtreeCacheControl(cache,pageno,BT_WRITE);

    return ret;
}




/* @func embBtreeCreateRootNode ***********************************************
**
** Create and write an empty root node. Set it as root, write it to
** disc and then lock the page in the disc cache.
** The root node is at block 0L
**
** @param [w] cache [EmbPBtcache] cache
**
** @return [void]
** @@
******************************************************************************/
void embBtreeCreateRootNode(EmbPBtcache cache)
{
    EmbPBtpage page = NULL;
    unsigned char *p;
    ajint nodetype;
    ajlong blocknumber;
    ajint nkeys;
    ajint totlen;
    ajlong left;
    ajlong right;
    ajlong overflow;
    ajlong prev;

/*    ajDebug("In embBtreeCreateRootNode\n"); */

    page = embBtreeCacheWrite(cache,0);
    page->pageno = 0;
    cache->totsize += cache->pagesize;
    
    p = page->buf;

    nodetype    = BT_ROOT;
    blocknumber = 0L;
    nkeys       = 0;
    totlen      = 0;
    left        = 0L;
    right       = 0L;
    prev        = 0L;
    overflow    = 0L;

    SBT_NODETYPE(p,nodetype);
    SBT_BLOCKNUMBER(p,blocknumber);
    SBT_NKEYS(p,nkeys);
    SBT_TOTLEN(p,totlen);
    SBT_LEFT(p,left);
    SBT_RIGHT(p,right);
    SBT_PREV(p,prev);
    SBT_OVERFLOW(p,overflow);

    page->dirty = BT_DIRTY;
    embBtreeCacheSync(cache);
    page->dirty = BT_LOCK;

    return;
}




/* @func embBtreeFindInsert ***********************************************
**
** Find the node that should contain a new key for insertion
**
** @param [rw] cache [EmbPBtcache] cache
** @param [r] item [char *] key to search for 
**
** @return [EmbPBtpage] leaf node where item should be inserted
** @@
******************************************************************************/
EmbPBtpage embBtreeFindInsert(EmbPBtcache cache, char *key)
{
    EmbPBtpage root = NULL;
    EmbPBtpage ret  = NULL;

/*    ajDebug("In embBtreeFindInsert\n"); */

    /* The root node should always be in the cache (BT_LOCKed) */
    root = embBtreeCacheLocate(cache,0L);
    
    if(!cache->level)
	return root;
    
    ret = embBtreeFindINode(cache,root,key);

    return ret;
}




/* @func embBtreeFindINode ***********************************************
**
** Recursive search for insert node
**
** @param [rw] cache [EmbPBtcache] cache
** @param [rw] page [EmbPBtpage] page
** @param [r] item [char *] key to search for 
**
** @return [EmbPBtpage] leaf node where item should be inserted
** @@
******************************************************************************/
static EmbPBtpage embBtreeFindINode(EmbPBtcache cache, EmbPBtpage page,
				    char *item)
{
    EmbPBtpage     ret = NULL;
    EmbPBtpage     pg  = NULL;

    unsigned char *buf = NULL;
    ajint status       = 0;
    ajint ival         = 0;

/*    ajDebug("In embBtreeFindINode\n"); */
    
    ret = page;
    buf = page->buf;
    GBT_NODETYPE(buf,&ival);
    if(ival != BT_LEAF)
    {
	status = ret->dirty;
	ret->dirty = BT_LOCK;	/* Lock in case of lots of overflow pages */
	pg = embBtreePageFromKey(cache,buf,item);
	ret->dirty = status;
	ret = embBtreeFindINode(cache,pg,item);
    }
    
    return ret;
}




/* @funcstatic embBtreePageFromKey *******************************************
**
** Return next lower index page given a key
**
** @param [r] buf [unsigned char *] page buffer
** @param [r] key [char *] key to search for 
**
** @return [EmbPBtree] pointer to a page
** @@
******************************************************************************/

static EmbPBtpage embBtreePageFromKey(EmbPBtcache cache, unsigned char *buf,
				      char *key)
{
    unsigned char *rootbuf = NULL;
    ajint nkeys    = 0;
    ajint order    = 0;
    ajint i;
    
    ajlong blockno  = 0L;
    AjPStr *karray  = NULL;
    ajlong *parray  = NULL;
    EmbPBtpage page = NULL;
    
/*    ajDebug("In embBtreePageFromKey\n"); */
    
    rootbuf = buf;


    GBT_NKEYS(rootbuf,&nkeys);
    order = cache->order;

    AJCNEW0(karray,order);
    AJCNEW0(parray,order);
    for(i=0;i<order;++i)
	karray[i] = ajStrNew();

    embBtreeGetKeys(cache,rootbuf,&karray,&parray);
    i = 0;
    while(i!=nkeys && strcmp(key,karray[i]->Ptr)>=0)
	++i;
    if(i==nkeys)
    {
	if(strcmp(key,karray[i-1]->Ptr)<0)
	    blockno = parray[i-1];
	else
	    blockno = parray[i];
    }
    else
	blockno = parray[i];

    for(i=0;i<order;++i)
	ajStrDel(&karray[i]);
    AJFREE(karray);
    AJFREE(parray);

    page =  embBtreeCacheRead(cache,blockno);

    return page;
}




/* @func embBtreeIdNew *********************************************
**
** Constructor for index bucket ID informationn
**
**
** @return [EmbPBtId] Index ID object
** @@
******************************************************************************/

EmbPBtId embBtreeIdNew(void)
{
    EmbPBtId Id = NULL;

/*    ajDebug("In embBtreeIdNew\n"); */

    AJNEW0(Id);
    Id->id = ajStrNew();
    Id->dbno = 0;
    Id->dups = 0;
    Id->offset = 0L;

    return Id;
}




/* @func embBtreeIdDel *********************************************
**
** Destructor for index bucket ID information
**
** @param [w] thys [EmbPBtId*] index ID object
**
** @return [void]
** @@
******************************************************************************/

void embBtreeIdDel(EmbPBtId *thys)
{
    EmbPBtId Id = NULL;

/*    ajDebug("In embBtreeIdDel\n"); */

    if(!thys || !*thys)
	return;
    Id = *thys;
    

    ajStrDel(&Id->id);
    AJFREE(Id);
    *thys = NULL;

    return;
}




/* @funcstatic embBtreeReadBucket *********************************************
**
** Constructor for index bucket given a disc page number
** Creates one empty key slot for possible addition
**
** @param [rw] cache [EmbPBtcache] cache
** @param [r] pageno [ajlong] page number
**
** @return [EmbPBucket] bucket
** @@
******************************************************************************/

static EmbPBucket embBtreeReadBucket(EmbPBtcache cache, ajlong pageno)
{
    EmbPBucket bucket   = NULL;
    EmbPBtpage page     = NULL;
    EmbPBtpage lpage    = NULL;
    unsigned char *buf  = NULL;
    unsigned char *kptr = NULL;
    EmbPBtId id         = NULL;
    
    unsigned char *idptr = NULL;
    
    ajint  nodetype = 0;
    ajint  nentries = 0;
    ajlong overflow = 0L;
    ajint  dirtysave = 0;
    
    ajint  i;
    ajint  len  = 0;
    
/*    ajDebug("In embBtreeReadBucket\n");*/
    
    if(!pageno)
	ajFatal("BucketRead: cannot read bucket from root page");

    page  = embBtreeCacheRead(cache,pageno);
    lpage = page;
    dirtysave = lpage->dirty;
    lpage->dirty = BT_LOCK;

    buf = lpage->buf;

    GBT_BUCKNODETYPE(buf,&nodetype);
    if(nodetype != BT_BUCKET)
	ajFatal("ReadBucket: NodeType mismatch. Not bucket (%d)", nodetype);
    
    GBT_BUCKNENTRIES(buf,&nentries);
    if(nentries > cache->nperbucket)
	ajFatal("ReadBucket: Bucket too full\n");
    

    GBT_BUCKOVERFLOW(buf,&overflow);

    AJNEW0(bucket);
    bucket->NodeType = nodetype;
    bucket->Nentries = nentries;
    bucket->Overflow = overflow;
    
    AJCNEW0(bucket->keylen, nentries+1);
    AJCNEW0(bucket->Ids, nentries+1);

    
    kptr  = PBT_BUCKKEYLEN(buf);
    idptr = kptr + (nentries * sizeof(ajint));


    
    for(i=0;i<nentries;++i)
    {
	BT_GETAJINT(kptr,&len);
	if((idptr-buf+1) + len > cache->pagesize)	/* overflow */
	{
	    ajDebug("ReadBucket: Overflow\n");
	    page  = embBtreeCacheRead(cache,overflow);
	    buf = page->buf;
	    GBT_BUCKNODETYPE(buf,&nodetype);
	    if(nodetype != BT_BUCKET)
		ajFatal("ReadBucket: NodeType mismatch. Not bucket (%d)",
			nodetype);
	    GBT_BUCKOVERFLOW(buf,&overflow);
	    /* overflow bucket ids start at the keylen position */
	    idptr = PBT_BUCKKEYLEN(buf);
	}

	bucket->Ids[i] = embBtreeIdNew();
	id = bucket->Ids[i];

	/* Fill ID objects */
	ajStrAssC(&id->id,idptr);
	idptr += (strlen(idptr) + 1);
	BT_GETAJINT(idptr,&id->dbno);
	idptr += sizeof(ajint);
	BT_GETAJINT(idptr,&id->dups);
	idptr += sizeof(ajint);	
	BT_GETAJLONG(idptr,&id->offset);
	idptr += sizeof(ajlong);

	kptr += sizeof(ajint);
    }

    lpage->dirty = dirtysave;
    
    return bucket;
}




/* @funcstatic embBtreeWriteBucket *******************************************
**
** Write index bucket object to the cache given a disc page number
**
** @param [rw] cache [EmbPBtcache] cache
** @param [r] bucket [EmbPBucket] bucket
** @param [r] pageno [ajlong] page number
**
** @return [void]
** @@
******************************************************************************/

static void embBtreeWriteBucket(EmbPBtcache cache, EmbPBucket bucket,
				ajlong pageno)
{
    EmbPBtpage page     = NULL;
    EmbPBtpage lpage    = NULL;
    unsigned char *buf  = NULL;
    unsigned char *lbuf = NULL;
    ajint   v   = 0;
    ajint   i   = 0;
    ajint   len = 0;
    
    ajlong lv = 0L;
    EmbPBtId id     = NULL;
    ajint  nentries = 0;
    ajlong overflow = 0L;
    unsigned char *keyptr = NULL;
    unsigned char *lptr   = NULL;
    ajlong   pno = 0L;
    
/*    ajDebug("In embBtreeWriteBucket\n");*/

    if(pageno == cache->totsize)	/* Create a new page? */
    {
	pno = pageno;
	page = embBtreeCacheWrite(cache,pageno);
	page->pageno = cache->totsize;
	cache->totsize += cache->pagesize;
	buf = page->buf;
	overflow = 0L;
	SBT_BUCKOVERFLOW(buf,lv);
    }
    else
    {
	page = embBtreeCacheRead(cache,pageno);
	buf = page->buf;
	GBT_BUCKOVERFLOW(buf,&overflow);
    }

    v = BT_BUCKET;
    SBT_BUCKNODETYPE(buf,v);

    lbuf = buf;
    page->dirty = BT_LOCK;
    lpage = page;

    nentries = bucket->Nentries;
    SBT_BUCKNENTRIES(buf,nentries);

    /* Write out key lengths */
    keyptr = PBT_BUCKKEYLEN(lbuf);
    for(i=0;i<nentries;++i)
    {
	if((keyptr-lbuf+1)+sizeof(ajint) > cache->pagesize)
	    ajFatal("BucketWrite: Bucket cannot hold more than %d keys",
		    i-1);

	id = bucket->Ids[i];
	/* Need to alter this if bucket ID structure changes */
	len = BT_BUCKIDLEN(id->id);
	BT_SETAJINT(keyptr,len);
	keyptr += sizeof(ajint);
    }


    /* Write out IDs using overflow if necessary */
    lptr = keyptr;
    for(i=0;i<nentries;++i)
    {
	id = bucket->Ids[i];
	len = BT_BUCKIDLEN(id->id);
	if((lptr-buf+1)+len > cache->pagesize) /* overflow */
	{
    	    ajDebug("WriteBucket: Overflow\n");
	    if(!overflow)		/* No overflow buckets yet */
	    {
		pno = cache->totsize;
		SBT_BUCKOVERFLOW(buf,pno);
		page = embBtreeCacheWrite(cache,pno);
		page->pageno = cache->totsize;
		cache->totsize += cache->pagesize;
		buf = page->buf;
		v = BT_BUCKET;
		SBT_BUCKNODETYPE(buf,v);
		v = 0;
		SBT_BUCKNENTRIES(buf,v);
		lv = 0L;
		SBT_BUCKOVERFLOW(buf,lv);
	    }
	    else
	    {
		page = embBtreeCacheRead(cache,overflow);
		buf = page->buf;
		GBT_BUCKOVERFLOW(buf,&overflow);
	    }

	    page->dirty = BT_DIRTY;
	    lptr = PBT_BUCKKEYLEN(buf);	    
	}
	
	sprintf((char *)lptr,"%s",ajStrStr(id->id));
	lptr += (ajStrLen(id->id) + 1);
	BT_SETAJINT(lptr,id->dbno);
	lptr += sizeof(ajint);
	BT_SETAJINT(lptr,id->dups);
	lptr += sizeof(ajint);
	BT_SETAJLONG(lptr,id->offset);
	lptr += sizeof(ajlong);
	

    }

    lv = 0L;
    SBT_BUCKOVERFLOW(buf,lv);
    
    lpage->dirty = BT_DIRTY;

    return;
}




/* @funcstatic embBtreeBucketDel *********************************************
**
** Delete a bucket object
**
** @param [w] thys [EmbPBucket*] bucket
**
** @return [void] bucket
** @@
******************************************************************************/

static void embBtreeBucketDel(EmbPBucket *thys)
{
    EmbPBucket pthis = NULL;
    ajint n;
    ajint i;
    
/*    ajDebug("In embBtreeBucketDel\n"); */

    if(!thys || !*thys)
	return;

    pthis = *thys;
    n = pthis->Nentries;

    for(i=0;i<n;++i)
	embBtreeIdDel(&pthis->Ids[i]);
    
    if(n)
    {
	AJFREE(pthis->keylen);
	AJFREE(pthis->Ids);
    }
    
    AJFREE(pthis);

    *thys = NULL;
    return;
}




/* @funcstatic embBtreeAddToBucket *******************************************
**
** Add an ID to a bucket
** Only called if there is room in the bucket
**
** @param [rw] cache [EmbPBtcache] cache
** @param [r] pageno [ajlong] page number of bucket
** @param [r] id [EmbBtId] ID info
**
** @return [EmbPBucket] bucket
** @@
******************************************************************************/

static void embBtreeAddToBucket(EmbPBtcache cache, ajlong pageno,
				EmbPBtId id)
{
    EmbPBucket bucket = NULL;
    EmbPBtId   destid = NULL;
    
    ajint nentries;
    
/*    ajDebug("In embBtreeAddToBucket\n");*/

    bucket   = embBtreeReadBucket(cache,pageno);
    nentries = bucket->Nentries;

    /* Reading a bucket always gives one extra ID position */
    bucket->Ids[nentries] = embBtreeIdNew();
    destid = bucket->Ids[nentries];

    ajStrAssS(&destid->id,id->id);
    destid->dbno   = id->dbno;
    destid->offset = id->offset;
    destid->dups   = id->dups;
    
    ++bucket->Nentries;

    embBtreeWriteBucket(cache,bucket,pageno);

    embBtreeBucketDel(&bucket);
    
    return;
}









/* @funcstatic embBtreeNumInBucket *******************************************
**
** Return number of entries in a bucket
**
** @param [rw] cache [EmbPBtcache] cache
** @param [r] pageno [ajlong] page number
**
** @return [EmbPBucket] bucket
** @@
******************************************************************************/

static ajint embBtreeNumInBucket(EmbPBtcache cache, ajlong pageno)
{
    EmbPBtpage page     = NULL;
    unsigned char *buf  = NULL;
    ajint  nodetype     = 0;
    ajint  nentries     = 0;
    
/*    ajDebug("In embBtreeNumInBucket\n"); */
    
    if(!pageno)
	ajFatal("NumInBucket: Attempt to read bucket from root page\n");

    page  = embBtreeCacheRead(cache,pageno);

    buf = page->buf;

    GBT_BUCKNODETYPE(buf,&nodetype);
    if(nodetype != BT_BUCKET)
	ajFatal("ReadBucket: NodeType mismatch. Not bucket (%d)", nodetype);
    
    GBT_BUCKNENTRIES(buf,&nentries);

    return nentries;
}




/* @func embBtreeReadDir ******************************************************
**
** Read files to index
**
** @param [w] filelist [AjPStr**] list of files to read
** @param [r] fdirectory [AjPStr] Directory to scan
** @param [r] files [AjPStr] Filename to search for (or NULL)
** @param [r] exclude [AjPStr] list of files to exclude
**
** @return [ajint] number of matching files
** @@
******************************************************************************/

ajint embBtreeReadDir(AjPStr **filelist, AjPStr fdirectory, AjPStr files,
		      AjPStr exclude)
{
    AjPList lfiles = NULL;
    ajint nfiles;
    ajint nremove;
    ajint i;
    ajint j;
    AjPStr file    = NULL;
    AjPStr *remove = NULL;

/*    ajDebug("In embBtreeReadDir\n"); */

    lfiles = ajListNew();
    nfiles = ajFileScan(fdirectory,files,&lfiles,ajFalse,ajFalse,NULL,NULL,
			ajFalse,NULL);

    nremove = ajArrCommaList(exclude,&remove);
    
    for(i=0;i<nfiles;++i)
    {
	ajListPop(lfiles,(void **)&file);
	ajSysBasename(&file);
	for(j=0;j<nremove && ! ajStrMatchWild(file,remove[j]);++j);
	if(j == nremove)
	    ajListPushApp(lfiles,(void *)file);
    }

    nfiles =  ajListToArray(lfiles,(void ***)&(*filelist));
    ajListDel(&lfiles);

    for(i=0; i<nremove;++i)
	ajStrDel(&remove[i]);
    AJFREE(remove);

    return nfiles;
}




/* @funcstatic embBtreeReorderBuckets *****************************************
**
** Re-order leaf buckets
** Must only be called if one of the buckets is full
**
** @param [r] cache [EmbPBtcache] cache
** @param [r] leaf [EmbPBtpage] leaf page
**
** @return [AjPBool] true if reorder was successful i.e. leaf not full
** @@
******************************************************************************/
static AjBool embBtreeReorderBuckets(EmbPBtcache cache, EmbPBtpage leaf)
{
    ajint nkeys = 0;
    unsigned char *lbuf = NULL;
    EmbPBucket *buckets = NULL;
    AjPStr *keys        = NULL;
    ajlong *ptrs        = NULL;
    ajlong *overflows   = NULL;
    
    ajint i = 0;
    ajint j = 0;
    
    ajint order;
    ajint bentries      = 0;
    ajint totalkeys     = 0;
    ajint nperbucket    = 0;
    ajint maxnperbucket = 0;
    ajint count         = 0;
    ajint totkeylen     = 0;
    ajint keylimit      = 0;
    ajint bucketn       = 0;
    ajint bucketlimit   = 0;
    ajint nodetype      = 0;
    
    AjPList idlist     = NULL;
    ajint   dirtysave  = 0;
    EmbPBtId bid       = NULL;
    EmbPBucket cbucket = NULL;
    EmbPBtId cid       = NULL;
    
/*    ajDebug("In embBtreeReorderBuckets\n");*/

    dirtysave = leaf->dirty;

    leaf->dirty = BT_LOCK;
    lbuf = leaf->buf;

    GBT_NODETYPE(lbuf,&nodetype);

    order = cache->order;
    nperbucket = cache->nperbucket;
    

    /* Read keys/ptrs */
    AJCNEW0(keys,order);
    AJCNEW0(ptrs,order);
    AJCNEW0(overflows,order);
    
    for(i=0;i<order;++i)
	keys[i] = ajStrNew();
    embBtreeGetKeys(cache,lbuf,&keys,&ptrs);

    GBT_NKEYS(lbuf,&nkeys);

    if(!nkeys)
	ajFatal("BucketReorder: Attempt to reorder empty leaf");

    for(i=0;i<nkeys;++i)
	totalkeys += embBtreeNumInBucket(cache,ptrs[i]);
    totalkeys += embBtreeNumInBucket(cache,ptrs[i]);
    

    /* Set the number of entries per bucket to approximately half full */
    maxnperbucket = nperbucket >> 1;
    if(!maxnperbucket)
	++maxnperbucket;

    /* Work out the number of new buckets needed */
    bucketn = (totalkeys / maxnperbucket);
    if(totalkeys % maxnperbucket)
	++bucketn;
    
    if(bucketn > order)
    {
	for(i=0;i<order;++i)
	    ajStrDel(&keys[i]);
	AJFREE(keys);
	AJFREE(ptrs);
	AJFREE(overflows);
	
	leaf->dirty = dirtysave;
	return ajFalse;
    }
    

    /* Read buckets */
    AJCNEW0(buckets,nkeys+1);
    keylimit = nkeys + 1;
    
    for(i=0;i<keylimit;++i)
	buckets[i] = embBtreeReadBucket(cache,ptrs[i]);


    /* Read IDs from all buckets and push to list and sort (increasing id) */
    idlist  = ajListNew();
    
    for(i=0;i<keylimit;++i)
    {
	overflows[i] = buckets[i]->Overflow;
	bentries = buckets[i]->Nentries;
	for(j=0;j<bentries;++j)
	    ajListPush(idlist,(void *)buckets[i]->Ids[j]);
	AJFREE(buckets[i]->keylen);
	AJFREE(buckets[i]->Ids);
	AJFREE(buckets[i]);
    }
    ajListSort(idlist,embBtIdCompare);
    AJFREE(buckets);

    cbucket = embBtreeBucketNew(maxnperbucket);
    bucketlimit = bucketn - 1;
    
    for(i=0;i<bucketlimit;++i)
    {
	cbucket->Overflow = overflows[i];
	cbucket->Nentries = 0;

	count = 0;
	while(count!=maxnperbucket)
	{
	    ajListPop(idlist,(void **)&bid);
	    
	    cid = cbucket->Ids[count];
	    ajStrAssS(&cid->id,bid->id);
	    cid->dbno = bid->dbno;
	    cid->dups = bid->dups;
	    cid->offset = bid->offset;

	    cbucket->keylen[count] = BT_BUCKIDLEN(bid->id);
	    ++cbucket->Nentries;
	    ++count;
	    embBtreeIdDel(&bid);
	}


	ajListPeek(idlist,(void **)&bid);
	ajStrAssS(&keys[i],bid->id);

	totkeylen += ajStrLen(bid->id);

	if(!ptrs[i])
	    ptrs[i] = cache->totsize;
	embBtreeWriteBucket(cache,cbucket,ptrs[i]);
    }


    /* Deal with greater-than bucket */

    cbucket->Overflow = overflows[i];
    cbucket->Nentries = 0;


    
    count = 0;
    while(ajListPop(idlist,(void **)&bid))
    {
	cid = cbucket->Ids[count];
	ajStrAssS(&cid->id,bid->id);
	cid->dbno = bid->dbno;
	cid->dups = bid->dups;
	cid->offset = bid->offset;

	++cbucket->Nentries;
	++count;
	embBtreeIdDel(&bid);
    }
    
    
    if(!ptrs[i])
	ptrs[i] = cache->totsize;
    embBtreeWriteBucket(cache,cbucket,ptrs[i]);

    embBtreeBucketDel(&cbucket);

    /* Now write out a modified leaf with new keys/ptrs */

    nkeys = bucketn - 1;
    SBT_NKEYS(lbuf,nkeys);
    SBT_TOTLEN(lbuf,totkeylen);

    embBtreeWriteNode(cache,leaf,keys,ptrs,nkeys);

    leaf->dirty = dirtysave;
    if(nodetype == BT_ROOT)
	leaf->dirty = BT_LOCK;
    
    for(i=0;i<order;++i)
	ajStrDel(&keys[i]);
    AJFREE(keys);
    AJFREE(ptrs);
    AJFREE(overflows);
    

    embBtreeBucketDel(&cbucket);
    ajListDel(&idlist);

    return ajTrue;
}




/* @funcstatic embBtreeNodeIsFull *****************************************
**
** Tests whether a node is full of keys
**
** @param [rw] cache [EmbPBtcache] cache
** @param [r] page [EmbPBtpage] original page
**
** @return [AjPBool] true if full
** @@
******************************************************************************/

static AjBool embBtreeNodeIsFull(EmbPBtcache cache, EmbPBtpage page)
{
    unsigned char *buf = NULL;
    ajint nkeys = 0;

/*    ajDebug("In embBtreeNodeIsFull\n"); */

    buf = page->buf;
    GBT_NKEYS(buf,&nkeys);

    if(nkeys == cache->order - 1)
	return ajTrue;

    return ajFalse;
}




/* @funcstatic embBtreeInsertNonFull *****************************************
**
** Insert a key into a non-full node
**
** @param [rw] cache [EmbPBtcache] cache
** @param [r] page [EmbPBtpage] original page
** @param [r] key [AjPStr] key to insert
** @param [r] less [ajlong] less-than pointer
** @param [r] greater [ajlong] greater-than pointer
**
** @return [void]
** @@
******************************************************************************/

static void embBtreeInsertNonFull(EmbPBtcache cache, EmbPBtpage page,
				  AjPStr key, ajlong less, ajlong greater)
{
    unsigned char *buf = NULL;
    AjPStr *karray     = NULL;
    ajlong *parray     = NULL;
    ajint nkeys  = 0;
    ajint order  = 0;
    ajint ipos   = 0;
    ajint i;
    ajint count  = 0;

    EmbPBtpage ppage = NULL;
    ajlong pageno    = 0L;

    ajint nodetype = 0;
    
/*    ajDebug("In embBtreeInsertNonFull\n");*/

    order = cache->order;
    AJCNEW0(karray,order);
    AJCNEW0(parray,order);

    for(i=0;i<order;++i)
	karray[i] = ajStrNew();

    buf = page->buf;
    GBT_NKEYS(buf,&nkeys);
    GBT_NODETYPE(buf,&nodetype);
    
    embBtreeGetKeys(cache,buf,&karray,&parray);

    i = 0;
    while(i!=nkeys && strcmp(key->Ptr,karray[i]->Ptr) >= 0)
	++i;

    ipos = i;

    count = nkeys - ipos;
    

    if(ipos == nkeys)
    {
	ajStrAssS(&karray[ipos],key);
	parray[ipos+1] = greater;
	parray[ipos]   = less;
    }
    else
    {
	parray[nkeys+1] = parray[nkeys];

	for(i=nkeys-1; count>0; --count, --i)
	{
	    ajStrAssS(&karray[i+1],karray[i]);
	    parray[i+1] = parray[i];
	}

	ajStrAssS(&karray[ipos],key);
	parray[ipos] = less;
	parray[ipos+1] = greater;
    }

    ++nkeys;
    SBT_NKEYS(buf,nkeys);

    embBtreeWriteNode(cache,page,karray,parray,nkeys);
    if(nodetype == BT_ROOT)
	page->dirty = BT_LOCK;

    pageno = page->pageno;
    ppage = embBtreeCacheRead(cache,less);
    SBT_PREV(ppage->buf,pageno);
    ppage->dirty = BT_DIRTY;
    ppage = embBtreeCacheRead(cache,greater);
    SBT_PREV(ppage->buf,pageno);
    ppage->dirty = BT_DIRTY;
    


    for(i=0;i<order;++i)
	ajStrDel(&karray[i]);
    AJFREE(karray);
    AJFREE(parray);

    if(nodetype != BT_ROOT)
	embBtreeKeyShift(cache,page);

    return;
}




/* @funcstatic embBtreeInsertKey *****************************************
**
** Insert a key into a potentially full node
**
** @param [rw] cache [EmbPBtcache] cache
** @param [r] page [EmbPBtpage] original page
** @param [r] key [AjPStr] key to insert
** @param [r] less [ajlong] less-than pointer
** @param [r] greater [ajlong] greater-than pointer
**
** @return [void]
** @@
******************************************************************************/

static void embBtreeInsertKey(EmbPBtcache cache, EmbPBtpage page,
			      AjPStr key, ajlong less, ajlong greater)
{
    unsigned char *lbuf = NULL;
    unsigned char *rbuf = NULL;
    unsigned char *tbuf = NULL;
    AjPStr *karray      = NULL;
    ajlong *parray      = NULL;
    AjPStr *tkarray     = NULL;
    ajlong *tparray     = NULL;
    ajint nkeys    = 0;
    ajint order    = 0;
    ajint keypos   = 0;
    ajint rkeyno   = 0;
    
    ajint i = 0;
    ajint n = 0;
    
    ajint nodetype = 0;
    EmbPBtpage ipage = NULL;
    EmbPBtpage lpage = NULL;
    EmbPBtpage rpage = NULL;
    EmbPBtpage tpage = NULL;

    ajlong blockno  = 0L;
    ajlong rblockno = 0L;
    ajlong lblockno = 0L;
    AjPStr mediankey  = NULL;
    ajlong medianless = 0L;
    ajlong mediangtr  = 0L;
    ajlong overflow   = 0L;
    ajlong prev       = 0L;
    ajint  totlen     = 0;
    
/*    ajDebug("In embBtreeInsertKey\n");*/

    if(!embBtreeNodeIsFull(cache,page))
    {
	embBtreeInsertNonFull(cache,page,key,less,greater);
	return;
    }
    
    order = cache->order;
    lbuf = page->buf;
    GBT_NODETYPE(lbuf,&nodetype);

    if(nodetype == BT_ROOT)
    {
	AJCNEW0(karray,order);
	AJCNEW0(parray,order);
	for(i=0;i<order;++i)
	    karray[i] = ajStrNew();
	embBtreeSplitRoot(cache);

	embBtreeGetKeys(cache,lbuf,&karray,&parray);

	if(strcmp(key->Ptr,karray[0]->Ptr)<0)
	    blockno = parray[0];
	else
	    blockno = parray[1];
	ipage = embBtreeCacheRead(cache,blockno);
	embBtreeInsertNonFull(cache,ipage,key,less,greater);

	for(i=0;i<order;++i)
	    ajStrDel(&karray[i]);
	AJFREE(karray);
	AJFREE(parray);
	return;
    }


    AJCNEW0(karray,order);
    AJCNEW0(parray,order);
    AJCNEW0(tkarray,order);
    AJCNEW0(tparray,order);
    mediankey = ajStrNew();
    
    for(i=0;i<order;++i)
    {
	tkarray[i] = ajStrNew();
	karray[i]  = ajStrNew();
    }
    
    lpage = page;
    lbuf = lpage->buf;
    
    embBtreeGetKeys(cache,lbuf,&karray,&parray);

    GBT_BLOCKNUMBER(lbuf,&lblockno);
    rblockno = cache->totsize;
    rpage = embBtreeCacheWrite(cache,rblockno);
    rpage->pageno = cache->totsize;
    cache->totsize += cache->pagesize;
    rbuf = rpage->buf;
    SBT_BLOCKNUMBER(rbuf,rblockno);

    
    GBT_PREV(lbuf,&prev);
    SBT_PREV(rbuf,prev);

    nkeys = order - 1;
    keypos = nkeys / 2;
    if(!(nkeys % 2))
	--keypos;

    ajStrAssS(&mediankey,karray[keypos]);
    medianless = lblockno;
    mediangtr  = rblockno;


    GBT_NODETYPE(lbuf,&nodetype);
    SBT_NODETYPE(rbuf,nodetype);
    SBT_OVERFLOW(rbuf,overflow);


    totlen = 0;
    for(i=0;i<keypos;++i)
    {
	ajStrAssS(&tkarray[i],karray[i]);
	totlen += ajStrLen(karray[i]);
	tparray[i] = parray[i];
    }
    tparray[i] = parray[i];
    SBT_TOTLEN(lbuf,totlen);
    n = i;
    SBT_NKEYS(lbuf,n);
    embBtreeWriteNode(cache,lpage,tkarray,tparray,i);



    for(i=0;i<n+1;++i)
    {
	tpage = embBtreeCacheRead(cache,tparray[i]);
	tbuf = tpage->buf;
	SBT_PREV(tbuf,lblockno);
	tpage->dirty = BT_DIRTY;
    }


    totlen = 0;
    for(i=keypos+1;i<nkeys;++i)
    {
	ajStrAssS(&tkarray[i-(keypos+1)],karray[i]);
	totlen += ajStrLen(karray[i]);
	tparray[i-(keypos+1)] = parray[i];
    }
    tparray[i-(keypos+1)] = parray[i];
    SBT_TOTLEN(rbuf,totlen);
    rkeyno = (nkeys-keypos) - 1;
    SBT_NKEYS(rbuf,rkeyno);
    embBtreeWriteNode(cache,rpage,tkarray,tparray,rkeyno);


    for(i=0;i<rkeyno+1;++i)
    {
	tpage = embBtreeCacheRead(cache,tparray[i]);
	tbuf = tpage->buf;
	SBT_PREV(tbuf,rblockno);
	tpage->dirty = BT_DIRTY;
    }


    ipage = rpage;
    if(strcmp(key->Ptr,mediankey->Ptr)<0)
	ipage = lpage;

    embBtreeInsertNonFull(cache,ipage,key,less,greater);


    for(i=0;i<order;++i)
    {
	ajStrDel(&karray[i]);
	ajStrDel(&tkarray[i]);
    }
    AJFREE(karray);
    AJFREE(tkarray);
    AJFREE(parray);
    AJFREE(tparray);

    ipage = embBtreeCacheRead(cache,prev);

    embBtreeInsertKey(cache,ipage,mediankey,medianless,mediangtr);
    ajStrDel(&mediankey);

    return;
}




/* @funcstatic embBtreeSplitRoot *****************************************
**
** Split the root node
**
** @param [rw] cache [EmbPBtcache] cache
**
** @return [void]
** @@
******************************************************************************/

static void embBtreeSplitRoot(EmbPBtcache cache)
{
    EmbPBtpage rootpage = NULL;
    EmbPBtpage rpage    = NULL;
    EmbPBtpage lpage    = NULL;
    EmbPBtpage tpage    = NULL;

    AjPStr *karray      = NULL;
    AjPStr *tkarray     = NULL;
    ajlong *parray      = NULL;
    ajlong *tparray     = NULL;

    ajint order     = 0;
    ajint nkeys     = 0;
    ajint keypos    = 0;
    
    ajlong rblockno = 0L;
    ajlong lblockno = 0L;
    
    AjPStr key = NULL;
    ajint  i;

    unsigned char *rootbuf = NULL;
    unsigned char *rbuf    = NULL;
    unsigned char *lbuf    = NULL;
    unsigned char *tbuf    = NULL;
    
    ajint nodetype  = 0;
    ajlong overflow = 0L;
    ajint totlen    = 0;
    ajint rkeyno    = 0;
    ajint n         = 0;
    
/*    ajDebug("In embBtreeSplitRoot\n"); */

    order = cache->order;
    AJCNEW0(karray,order);
    AJCNEW0(parray,order);
    AJCNEW0(tkarray,order);
    AJCNEW0(tparray,order);
    key = ajStrNew();

    for(i=0;i<order;++i)
    {
	karray[i]  = ajStrNew();
	tkarray[i] = ajStrNew();
    }
    

    rootpage = embBtreeCacheLocate(cache,0L);
    rootbuf = rootpage->buf;

    nkeys = order - 1;

    keypos = nkeys / 2;
    if(!(nkeys % 2))
	--keypos;


    rblockno = cache->totsize;
    rpage = embBtreeCacheWrite(cache,rblockno);
    rpage->pageno = cache->totsize;
    cache->totsize += cache->pagesize;

    lblockno = cache->totsize;
    lpage = embBtreeCacheWrite(cache,lblockno);
    lpage->pageno = cache->totsize;
    cache->totsize += cache->pagesize;

    SBT_BLOCKNUMBER(rpage->buf,rblockno);
    SBT_BLOCKNUMBER(lpage->buf,lblockno);
    

    embBtreeGetKeys(cache,rootbuf,&karray,&parray);

    /* Get key for root node and write new root node */
    ajStrAssS(&tkarray[0],karray[keypos]);
    tparray[0] = lblockno;
    tparray[1] = rblockno;
    

    n = 1;
    SBT_NKEYS(rootbuf,n);
    embBtreeWriteNode(cache,rootpage,tkarray,tparray,1);
    rootpage->dirty = BT_LOCK;

    rbuf = rpage->buf;
    lbuf = lpage->buf;
    
    if(cache->level)
	nodetype = BT_INTERNAL;
    else
	nodetype = BT_LEAF;
    
    SBT_NODETYPE(rbuf,nodetype);
    SBT_NODETYPE(lbuf,nodetype);
    SBT_OVERFLOW(rbuf,overflow);
    SBT_PREV(rbuf,overflow);
    SBT_OVERFLOW(lbuf,overflow);
    SBT_PREV(lbuf,overflow);

    totlen = 0;
    for(i=0;i<keypos;++i)
    {
	ajStrAssS(&tkarray[i],karray[i]);
	totlen += ajStrLen(karray[i]);
	tparray[i] = parray[i];
    }
    tparray[i] = parray[i];
    SBT_TOTLEN(lbuf,totlen);
    n = i;
    SBT_NKEYS(lbuf,n);
    embBtreeWriteNode(cache,lpage,tkarray,tparray,i);

    for(i=0;i<n+1;++i)
    {
	tpage = embBtreeCacheRead(cache,tparray[i]);
	tbuf = tpage->buf;
	SBT_PREV(tbuf,lblockno);
	tpage->dirty = BT_DIRTY;
    }

    totlen = 0;
    for(i=keypos+1;i<nkeys;++i)
    {
	ajStrAssS(&tkarray[i-(keypos+1)],karray[i]);
	totlen += ajStrLen(karray[i]);
	tparray[i-(keypos+1)] = parray[i];
    }
    tparray[i-(keypos+1)] = parray[i];
    SBT_TOTLEN(rbuf,totlen);
    rkeyno = (nkeys-keypos) - 1;
    SBT_NKEYS(rbuf,rkeyno);
    embBtreeWriteNode(cache,rpage,tkarray,tparray,rkeyno);

    for(i=0;i<rkeyno+1;++i)
    {
	tpage = embBtreeCacheRead(cache,tparray[i]);
	tbuf = tpage->buf;
	SBT_PREV(tbuf,rblockno);
	tpage->dirty = BT_DIRTY;
    }


    for(i=0;i<order;++i)
    {
	ajStrDel(&karray[i]);
	ajStrDel(&tkarray[i]);
    }

    ++cache->level;

    AJFREE(tkarray);
    AJFREE(tparray);
    AJFREE(karray);
    AJFREE(parray);
    ajStrDel(&key);
    
    return;
}




/* @funcstatic embBtreeCreateFile ********************************************
**
** Open B+tree file for writing
**
** @param [r] idirectory [AjPStr] Directory for index files
** @param [r] dbname [AjPStr] name of database
** @param [r] add [char *] type of file
**
** @return [AjPFile] opened file
** @@
******************************************************************************/

static AjPFile btreeCreateFile(AjPStr idirectory, AjPStr dbname,
			       char *add)
{
    AjPStr filename = NULL;
    AjPFile fp      = NULL;
    
/*    ajDebug("In embBtreeCreateFile\n"); */

    filename = ajStrNew();

    ajFmtPrintS(&filename,"%S%S%s",idirectory,dbname,add);
    
    fp =  ajFileNewOut(filename);

    ajStrDel(&filename);
    return fp;
}




/* @func embBtreeWriteFileList ***********************************************
**
** Read files to index
**
** @param [r] filelist [AjPStr*] list of files
** @param [r] fdirectory [AjPStr] Directory to scan
** @param [r] files [AjPStr] Filename to search for (or NULL)
** @param [r] exclude [AjPStr] list of files to exclude
**
** @return [AjBool] true if success
** @@
******************************************************************************/

AjBool embBtreeWriteFileList(AjPStr *filelist, ajint nfiles,
			     AjPStr fdirectory, AjPStr idirectory, ajint order,
			     ajint fill, AjPStr dbname, AjPStr ddate,
			     AjPStr release)
{
    AjPFile entfile = NULL;
    ajint i;
    
/*    ajDebug("In embBtreeWriteFileList\n"); */

    entfile = btreeCreateFile(idirectory,dbname,BTENTRYFILE);
    if(!entfile)
	return ajFalse;
    
    ajFmtPrintF(entfile,"%d\t# order\n%d\t# fill\n",order,fill);
    ajFmtPrintF(entfile,"%S\t# dbname\n%S\t# release\n%S\t# date\n",dbname,
		release,ddate);
    ajFmtPrintF(entfile,"%d\t#Number of files\n",nfiles);
    for(i=0;i<nfiles;++i)
	ajFmtPrintF(entfile,"%S%S\n",fdirectory,filelist[i]);

    ajFileClose(&entfile);
    
    return ajTrue;
}




/* @funcstatic embBtreeGetKeys *********************************************
**
** Get Keys and Pointers from an internal node
**
** @param [rw] cache [EmbPBtcache] cache
** @param [r] buf [unsigned char *] page buffer
** @param [r] keys [AjPStr **] keys
** @param [r] ptrs [ajlong**] ptrs
**
** @return [EmbPBtree] pointer to a page
** @@
******************************************************************************/

static void embBtreeGetKeys(EmbPBtcache cache, unsigned char *buf,
			    AjPStr **keys, ajlong **ptrs)
{
    AjPStr *karray = NULL;
    ajlong *parray = NULL;
    
    ajint  m;
    unsigned char *lenptr = NULL;
    unsigned char *keyptr = NULL;
    unsigned char *tbuf = NULL;

    ajint    i;
    ajint    ival = 0;
    
    ajint    len;
    ajint    pagesize = 0;
    ajlong   overflow = 0L;

    EmbPBtpage page = NULL;

/*    ajDebug("In embBtreeGetKeys\n"); */

    karray = *keys;
    parray = *ptrs;
    
    tbuf    = buf;

    pagesize = cache->pagesize;
    
    GBT_NKEYS(tbuf,&m);
    if(!m)
	ajFatal("GetKeys: No keys in node");

    lenptr =  PBT_KEYLEN(tbuf);
    keyptr = lenptr + m * sizeof(ajint);


    for(i=0; i<m; ++i)
    {
	BT_GETAJINT(lenptr,&ival);
	len = ival+1;
	
	if((keyptr-tbuf+1) + len + sizeof(ajlong) > pagesize)
	{
    	    ajDebug("GetKeys: Overflow\n");
	    GBT_OVERFLOW(tbuf,&overflow);
	    page = embBtreeCacheRead(cache,overflow);
	    tbuf = page->buf;
	    GBT_NODETYPE(tbuf,&ival);
	    if(ival != BT_OVERFLOW)
		ajFatal("Overflow node expected but not found");
	    /*
	     ** The length pointer is restricted to the initial page.
	     ** The keyptr in overflow pages starts at the Key Lengths
	     ** position!
	     */
	    keyptr = PBT_KEYLEN(tbuf);
	}

	ajStrAssC(&karray[i],keyptr);
	keyptr += len;

	BT_GETAJLONG(keyptr,&parray[i]);
	keyptr += sizeof(ajlong);
	lenptr += sizeof(ajint);
    }
    

    if((keyptr-tbuf+1) + sizeof(ajlong) > pagesize)
    {
	ajDebug("GetKeys: Overflow\n");
	GBT_OVERFLOW(tbuf,&overflow);
	page = embBtreeCacheRead(cache,overflow);
	tbuf = page->buf;
	GBT_NODETYPE(tbuf,&ival);
	if(ival != BT_OVERFLOW)
	    ajFatal("Overflow node expected but not found");
	/*
	 ** The length pointer is restricted to the initial page.
	 ** The keyptr in overflow pages starts at the Key Lengths
	 ** position!
	 */
	keyptr = PBT_KEYLEN(tbuf);
    }
    
    BT_GETAJLONG(keyptr,&parray[i]);

    return;
}








/* @funcstatic embBtIdCompare *******************************************
**
** Comparison function for ajListSort
**
** @param [r] a [const void*] ID 1
** @param [r] b [const void*] ID 2
**
** @return [ajint] 0 = bases match
** @@
******************************************************************************/


static ajint embBtIdCompare(const void *a, const void *b)
{
    return strcmp((*(EmbPBtId*)a)->id->Ptr,
		  (*(EmbPBtId*)b)->id->Ptr);
}




/* @funcstatic embBtreeWriteNode *******************************************
**
** Write an internal node
**
** @param [rw] cache [EmbPBtcache] cache
** @param [w] spage [EmbPBtpage] buffer
** @param [r] keys [AjPStr*] keys
** @param [r] ptrs [ajlong*] page pointers
** @param [r] nkeys [ajint] number of keys

**
** @return [ajint] 0 = bases match
** @@
******************************************************************************/

static void embBtreeWriteNode(EmbPBtcache cache, EmbPBtpage spage,
			      AjPStr *keys, ajlong *ptrs, ajint nkeys)
{
    unsigned char *lbuf   = NULL;
    unsigned char *tbuf   = NULL;
    unsigned char *lenptr = NULL;
    unsigned char *keyptr = NULL;
    EmbPBtpage page       = NULL;
    
    ajlong   overflow = 0L;
    ajlong   blockno  = 0L;

    ajint    i;
    ajint    len;
    ajint    v  = 0;
    ajlong   lv = 0L;

/*    ajDebug("In embBtreeWriteNode\n"); */

    lbuf = spage->buf;
    tbuf = lbuf;
    page = spage;

    SBT_NKEYS(lbuf,nkeys);
    
    lenptr = PBT_KEYLEN(lbuf);
    keyptr = lenptr + nkeys * sizeof(ajint);

    for(i=0;i<nkeys;++i)
    {
	if((lenptr-lbuf+1) > cache->pagesize)
	    ajFatal("WriteNode: Too many key lengths for available pagesize");
	len = ajStrLen(keys[i]);
	BT_SETAJINT(lenptr,len);
	lenptr += sizeof(ajint);
    }


    GBT_OVERFLOW(lbuf,&overflow);

    for(i=0;i<nkeys;++i)
    {
	len = ajStrLen(keys[i]) + 1;
	if((keyptr-tbuf+1) + len + sizeof(ajlong) > cache->pagesize)
	{
	    ajDebug("WriteNode: Overflow\n");
	    if(!overflow)		/* No overflow buckets yet */
	    {
		page->dirty = BT_DIRTY;
		blockno = cache->totsize;
		SBT_OVERFLOW(tbuf,blockno);
		page = embBtreeCacheWrite(cache,blockno);
		page->pageno = cache->totsize;
		cache->totsize += cache->pagesize;
		tbuf = page->buf;
		v = BT_OVERFLOW;
		SBT_NODETYPE(tbuf,v);
		lv = 0L;
		SBT_OVERFLOW(tbuf,lv);
		SBT_BLOCKNUMBER(tbuf,blockno);
	    }
	    else
	    {
		page = embBtreeCacheRead(cache,overflow);
		tbuf = page->buf;
		GBT_OVERFLOW(tbuf,&overflow);
	    }
	    keyptr = PBT_KEYLEN(tbuf);
	    page->dirty = BT_DIRTY;
	}


	sprintf((char *)keyptr,"%s",ajStrStr(keys[i]));
	keyptr += len;
	BT_SETAJLONG(keyptr,ptrs[i]);
	keyptr += sizeof(ajlong);
    }
    



    if((keyptr-tbuf+1) + sizeof(ajlong) > cache->pagesize)
    {
	ajDebug("WriteNode: Overflow\n");
	page->dirty = BT_DIRTY;
	if(!overflow)			/* No overflow buckets yet */
	{
	    blockno = cache->totsize;
	    SBT_OVERFLOW(tbuf,blockno);
	    page = embBtreeCacheWrite(cache,blockno);
	    page->pageno = cache->totsize;
	    cache->totsize += cache->pagesize;
	    tbuf = page->buf;
	    v = BT_OVERFLOW;
	    SBT_NODETYPE(tbuf,v);
	    SBT_BLOCKNUMBER(tbuf,blockno);
	}
	else
	{
	    page = embBtreeCacheRead(cache,overflow);
	    tbuf = page->buf;
	}
	keyptr = PBT_KEYLEN(tbuf);
    }
    
    page->dirty = BT_DIRTY;

    overflow = 0L;
    SBT_OVERFLOW(tbuf,overflow);

    BT_SETAJLONG(keyptr,ptrs[i]);

    return;
}




/* @func embBtreeInsertId *********************************************
**
** Insert an ID structure into the tree
**
** @param [rw] cache [EmbPBtcache] cache
** @param [r] id [EmbPBtId] Id object
**
** @return [void] pointer to a page
** @@
******************************************************************************/

void embBtreeInsertId(EmbPBtcache cache, EmbPBtId id)
{
    EmbPBtpage spage   = NULL;
    EmbPBtpage parent  = NULL;
    AjPStr key         = NULL;
    char *ckey         = NULL;
    EmbPBucket lbucket = NULL;
    EmbPBucket rbucket = NULL;
    ajlong lblockno = 0L;
    ajlong rblockno = 0L;
    ajlong blockno  = 0L;
    ajlong shift    = 0L;

    ajint nkeys = 0;
    ajint order = 0;

    ajint nodetype = 0;
    
    unsigned char *buf = NULL;
 
    AjPStr *karray = NULL;
    ajlong *parray = NULL;
    
    ajint i;
    ajint n;
    
/*    ajDebug("In embBtreeInsertId\n"); */
    
    key = ajStrNew();
    

    ajStrAssS(&key,id->id);
    if(!ajStrLen(key))
    {
	ajStrDel(&key);
	return;
    }


    ckey = ajStrStr(key);
    spage = embBtreeFindInsert(cache,ckey);
    buf = spage->buf;

    GBT_NKEYS(buf,&nkeys);
    GBT_NODETYPE(buf,&nodetype);
    
    order = cache->order;
    AJCNEW0(karray,order);
    AJCNEW0(parray,order);
    for(i=0;i<order;++i)
	karray[i] = ajStrNew();
    
    if(!nkeys)
    {
	lbucket  = embBtreeBucketNew(0);
	rbucket  = embBtreeBucketNew(0);

	lblockno = cache->totsize;
	embBtreeWriteBucket(cache,lbucket,lblockno);

	rblockno = cache->totsize;
	embBtreeWriteBucket(cache,rbucket,rblockno);	

	parray[0] = lblockno;
	parray[1] = rblockno;
	ajStrAssS(karray,key);
	
	embBtreeWriteNode(cache,spage,karray,parray,1);

	embBtreeBucketDel(&lbucket);
	embBtreeBucketDel(&rbucket);

	embBtreeAddToBucket(cache,rblockno,id);

	for(i=0;i<order;++i)
	    ajStrDel(&karray[i]);
	AJFREE(karray);
	AJFREE(parray);
	ajStrDel(&key);
	return;
    }
    
    embBtreeGetKeys(cache,buf,&karray,&parray);

    i=0;
    while(i!=nkeys && strcmp(key->Ptr,karray[i]->Ptr)>=0)
	++i;
    if(i==nkeys)
    {
	if(strcmp(key->Ptr,karray[i-1]->Ptr)<0)
	    blockno = parray[i-1];
	else
	    blockno = parray[i];
    }
    else
	blockno = parray[i];

    if(nodetype != BT_ROOT)
	if((shift = embBtreeInsertShift(cache,spage,key->Ptr)))
	    blockno = shift;

    n = embBtreeNumInBucket(cache,blockno);
    if(n == cache->nperbucket)
    {
	if(embBtreeReorderBuckets(cache,spage))
	{
	    GBT_NKEYS(buf,&nkeys);	    
	    embBtreeGetKeys(cache,buf,&karray,&parray);
	    i=0;
	    while(i!=nkeys && strcmp(key->Ptr,karray[i]->Ptr)>=0)
		++i;
	    if(i==nkeys)
	    {
		if(strcmp(key->Ptr,karray[i-1]->Ptr)<0)
		    blockno = parray[i-1];
		else
		    blockno = parray[i];
	    }
	    else
		blockno = parray[i];
	}
	else
	{
	    parent = embBtreeSplitLeaf(cache,spage);
	    spage = embBtreeFindInsert(cache,ckey);
	    buf = spage->buf;

	    embBtreeGetKeys(cache,buf,&karray,&parray);

	    GBT_NKEYS(buf,&nkeys);
	    i=0;
	    while(i!=nkeys && strcmp(key->Ptr,karray[i]->Ptr)>=0)
		++i;
	    if(i==nkeys)
	    {
		if(strcmp(key->Ptr,karray[i-1]->Ptr)<0)
		    blockno = parray[i-1];
		else
		    blockno = parray[i];
	    }
	    else
		blockno = parray[i];

/*
	    spage = embBtreeCacheRead(cache,blockno);
	    buf = spage->buf;
	    embBtreeGetKeys(cache,buf,&karray,&parray);
	    GBT_NKEYS(buf,&nkeys);
	    i=0;
	    while(i!=nkeys && strcmp(key->Ptr,karray[i]->Ptr)>=0)
		++i;
	    if(i==nkeys)
	    {
		if(strcmp(key->Ptr,karray[i-1]->Ptr)<0)
		    blockno = parray[i-1];
		else
		    blockno = parray[i];
	    }
	    else
		blockno = parray[i];
*/
	}
    }

    embBtreeAddToBucket(cache,blockno,id);
    for(i=0;i<order;++i)
	ajStrDel(&karray[i]);
    AJFREE(karray);
    AJFREE(parray);
    ajStrDel(&key);

    return;
}




/* @func embBtreeIdFromKey ************************************************
**
** Get an ID structure from a leaf node given a key
**
** @param [rw] cache [EmbPBtcache] cache
** @param [r] key [char *] key
** @param [r] incdup [AjBool] increment duplicate count if true
**
** @return [EmbPBtId] pointer to an ID structure or NULL if not found
** @@
******************************************************************************/

EmbPBtId embBtreeIdFromKey(EmbPBtcache cache, char *key, AjBool incdup)
{
    EmbPBtpage page   = NULL;
    EmbPBucket bucket = NULL;
    EmbPBtId   id     = NULL;
    EmbPBtId   tid    = NULL;
    
    AjPStr *karray  = NULL;
    ajlong *parray  = NULL;
    
    unsigned char *buf = NULL;

    ajint nentries = 0;
    ajint nkeys    = 0;
    ajint order    = 0;
    
    ajint i;
    
    ajlong blockno = 0L;
    AjBool found   = ajFalse;


    page = embBtreeFindInsert(cache,key);
    buf = page->buf;
    order = cache->order;

    AJCNEW0(karray,order);
    AJCNEW0(parray,order);
    for(i=0;i<order;++i)
	karray[i] = ajStrNew();

    embBtreeGetKeys(cache,buf,&karray,&parray);

    GBT_NKEYS(buf,&nkeys);

    i=0;
    while(i!=nkeys && strcmp(key,karray[i]->Ptr)>=0)
	++i;
    
    if(i==nkeys)
    {
	if(strcmp(key,karray[i-1]->Ptr)<0)
	    blockno = parray[i-1];
	else
	    blockno = parray[i];
    }
    else
	blockno = parray[i];

    bucket = embBtreeReadBucket(cache,blockno);
    
    nentries = bucket->Nentries;

    found = ajFalse;

    for(i=0;i<nentries;++i)
    {
	if(!strcmp(key,bucket->Ids[i]->id->Ptr))
	{
	    found = ajTrue;
	    break;
	}
    }

    if(found)
    {
	id  = embBtreeIdNew();
	tid = bucket->Ids[i];
	ajStrAssS(&id->id,tid->id);
	id->dups = tid->dups;
	id->dbno = tid->dbno;
	id->offset = tid->offset;
	
	if(incdup)
	{
	    ++tid->dups;
	    ++id->dups;
	    embBtreeWriteBucket(cache,bucket,blockno);
	}
    }

    embBtreeBucketDel(&bucket);
    for(i=0;i<order;++i)
	ajStrDel(&karray[i]);
    AJFREE(karray);
    AJFREE(parray);

    if(!found)
	return NULL;

    return id;
}




/* @func embBtreeWriteParams ************************************************
**
** Write B+ tree parameters to file
**
** @param [r] cache [EmbPBtcache] cache
** @param [r] fn [char *] file
**
** @return [void]
** @@
******************************************************************************/

void embBtreeWriteParams(EmbPBtcache cache, char *fn)
{
    AjPStr  fname = NULL;
    AjPFile outf  = NULL;
    
    fname = ajStrNewC(fn);
    ajStrAppC(&fname,".param");

    if(!(outf = ajFileNewOut(fname)))
	ajFatal("Cannot open param file %S\n",fname);

    ajFmtPrintF(outf,"Order     %d\n",cache->order);
    ajFmtPrintF(outf,"Fill      %d\n",cache->nperbucket);
    ajFmtPrintF(outf,"Pagesize  %d\n",cache->pagesize);
    ajFmtPrintF(outf,"Level     %d\n",cache->level);
    ajFmtPrintF(outf,"Cachesize %d\n",cache->cachesize);

    ajFileClose(&outf);
    ajStrDel(&fname);

    return;
}




/* @func embBtreeReadParams ************************************************
**
** Read B+ tree parameters from file
**
** @param [r] fn [char *] file
** @param [w] order [ajint*] tree order
** @param [w] nperbucket [ajint*] bucket fill
** @param [w] pagesize [ajint*] size of pages
** @param [w] level [ajint*] depth of tree (0 = root leaf)
** @param [w] cachesize [ajint*] cachesize
**
** @return [void]
** @@
******************************************************************************/

void embBtreeReadParams(char *fn, ajint *order, ajint *nperbucket,
			ajint *pagesize, ajint *level, ajint *cachesize)
{
    AjPStr fname = NULL;
    AjPStr line = NULL;
    AjPFile inf  = NULL;
    
    line  = ajStrNew();
    fname = ajStrNewC(fn);
    ajStrAppC(&fname,".param");

    if(!(inf = ajFileNewIn(fname)))
	ajFatal("Cannot open param file %S\n",fname);

    while(ajFileReadLine(inf,&line))
    {
	if(ajStrPrefixC(line,"Order"))
	    ajFmtScanS(line,"%*s%d",order);
	if(ajStrPrefixC(line,"Fill"))
	    ajFmtScanS(line,"%*s%d",nperbucket);
	if(ajStrPrefixC(line,"Pagesize"))
	    ajFmtScanS(line,"%*s%d",pagesize);
	if(ajStrPrefixC(line,"Level"))
	    ajFmtScanS(line,"%*s%d",level);
	if(ajStrPrefixC(line,"Cachesize"))
	    ajFmtScanS(line,"%*s%d",cachesize);
    }

    ajFileClose(&inf);
    ajStrDel(&fname);
    ajStrDel(&line);
    
    return;
}




/* @funcstatic embBtreeSplitLeaf *********************************************
**
** Split a leaf and propagate up if necessary
**
** @param [rw] cache [EmbPBtcache] cache
** @param [r] page [EmbPBtPage] page
**
** @return [EmbPBtpage] pointer to a parent page
** @@
******************************************************************************/

static EmbPBtpage embBtreeSplitLeaf(EmbPBtcache cache, EmbPBtpage spage)
{
    ajint nkeys     = 0;
    ajint order     = 0;
    ajint totalkeys = 0;
    ajint bentries  = 0;
    ajint keylimit  = 0;
    ajint nodetype  = 0;

    ajint rootnodetype  = 0;
    
    ajint i;
    ajint j;
    
    EmbPBtpage lpage = NULL;
    EmbPBtpage rpage = NULL;
    EmbPBtpage page  = NULL;
    
    AjPStr mediankey  = NULL;
    ajlong mediangtr  = 0L;
    ajlong medianless = 0L;
    

    EmbPBtId bid        = NULL;
    EmbPBtId cid        = NULL;
    unsigned char *buf  = NULL;
    unsigned char *lbuf = NULL;
    unsigned char *rbuf = NULL;

    AjPList idlist      = NULL;

    EmbPBucket *buckets = NULL;
    EmbPBucket cbucket  = NULL;
    
    AjPStr *karray    = NULL;
    ajlong *parray    = NULL;
    
    ajint keypos = 0;
    ajint lno    = 0;
    ajint rno    = 0;

    ajint bucketlimit   = 0;
    ajint maxnperbucket = 0;
    ajint nperbucket    = 0;
    ajint bucketn       = 0;
    ajint count         = 0;
    ajint totkeylen     = 0;
    
    ajlong lblockno = 0L;
    ajlong rblockno = 0L;
    ajlong prev     = 0L;
    ajlong overflow = 0L;
    
/*    ajDebug("In embBtreeSplitLeaf\n"); */

    order = cache->order;
    nperbucket = cache->nperbucket;

    mediankey = ajStrNew();
    AJCNEW0(karray,order);
    AJCNEW0(parray,order);
    
    for(i=0;i<order;++i)
	karray[i] = ajStrNew();

    buf = spage->buf;
    lbuf = buf;

    GBT_NKEYS(buf,&nkeys);
    GBT_NODETYPE(buf,&rootnodetype);

    if(rootnodetype == BT_ROOT)
    {
	lblockno = cache->totsize;
	lpage = embBtreeCacheWrite(cache,lblockno);
	lpage->pageno = cache->totsize;
	cache->totsize += cache->pagesize;
	lbuf = lpage->buf;
	SBT_PREV(lbuf,prev);
    }
    else
    {
	lblockno = spage->pageno;
	lpage = spage;
    }


    rblockno = cache->totsize;
    rpage = embBtreeCacheWrite(cache,rblockno);
    rpage->pageno = cache->totsize;
    cache->totsize += cache->pagesize;
    rbuf = rpage->buf;

    embBtreeGetKeys(cache,buf,&karray,&parray);


    keylimit = nkeys+1;
    AJCNEW0(buckets,keylimit);
    for(i=0;i<keylimit;++i)
	buckets[i] = embBtreeReadBucket(cache,parray[i]);

    idlist = ajListNew();
    for(i=0;i<keylimit;++i)
    {
	bentries = buckets[i]->Nentries;
	for(j=0;j<bentries;++j)
	    ajListPush(idlist,(void *)buckets[i]->Ids[j]);
	AJFREE(buckets[i]->keylen);
	AJFREE(buckets[i]->Ids);
	AJFREE(buckets[i]);
    }
    ajListSort(idlist,embBtIdCompare);
    AJFREE(buckets);


    totalkeys = ajListLength(idlist);

    keypos = totalkeys / 2;

    lno = keypos;
    rno = totalkeys - lno;

    maxnperbucket = nperbucket >> 1;
    if(!maxnperbucket)
	++maxnperbucket;

    cbucket = embBtreeBucketNew(maxnperbucket);

    bucketn = lno / maxnperbucket;
    if(lno % maxnperbucket)
	++bucketn;
    bucketlimit = bucketn - 1;


    totkeylen = 0;
    count = 0;
    for(i=0;i<bucketlimit;++i)
    {
	cbucket->Nentries = 0;
	for(j=0;j<maxnperbucket;++j)
	{
	    ajListPop(idlist,(void **)&bid);
	    
	    cid = cbucket->Ids[j];
	    ajStrAssS(&cid->id,bid->id);
	    cid->dbno = bid->dbno;
	    cid->dups = bid->dups;
	    cid->offset = bid->offset;

	    cbucket->keylen[j] = BT_BUCKIDLEN(bid->id);
	    ++count;
	    ++cbucket->Nentries;
	    embBtreeIdDel(&bid);
	}
	ajListPeek(idlist,(void **)&bid);
	
	ajStrAssS(&karray[i],bid->id);
	totkeylen += ajStrLen(bid->id);

	if(!parray[i])
	    parray[i] = cache->totsize;
	embBtreeWriteBucket(cache,cbucket,parray[i]);
    }

    cbucket->Nentries = 0;

    j = 0;
    while(count != lno)
    {
	ajListPop(idlist,(void **)&bid);
	cid = cbucket->Ids[j];
	++j;
	++count;

	ajStrAssS(&cid->id,bid->id);
	cid->dbno = bid->dbno;
	cid->dups = bid->dups;
	cid->offset = bid->offset;
	++cbucket->Nentries;
	embBtreeIdDel(&bid);
    }

    if(!parray[i])
	parray[i] = cache->totsize;
    embBtreeWriteBucket(cache,cbucket,parray[i]);

    nkeys = bucketn - 1;
    SBT_NKEYS(lbuf,nkeys);
    SBT_TOTLEN(lbuf,totkeylen);
    nodetype = BT_LEAF;
    SBT_NODETYPE(lbuf,nodetype);
    lpage->dirty = BT_DIRTY;
    embBtreeWriteNode(cache,lpage,karray,parray,nkeys);

    ajListPeek(idlist,(void **)&bid);
    ajStrAssS(&mediankey,bid->id);

    totkeylen = 0;
    bucketn = rno / maxnperbucket;
    if(rno % maxnperbucket)
	++bucketn;
    bucketlimit = bucketn - 1;

    for(i=0;i<bucketlimit;++i)
    {
	cbucket->Nentries = 0;
	for(j=0;j<maxnperbucket;++j)
	{
	    ajListPop(idlist,(void **)&bid);
	    
	    cid = cbucket->Ids[j];
	    ajStrAssS(&cid->id,bid->id);
	    cid->dbno = bid->dbno;
	    cid->dups = bid->dups;
	    cid->offset = bid->offset;

	    cbucket->keylen[j] = BT_BUCKIDLEN(bid->id);
	    ++cbucket->Nentries;
	    embBtreeIdDel(&bid);
	}

	ajListPeek(idlist,(void **)&bid);
	ajStrAssS(&karray[i],bid->id);
	totkeylen += ajStrLen(bid->id);

	parray[i] = cache->totsize;
	embBtreeWriteBucket(cache,cbucket,parray[i]);
    }

    cbucket->Nentries = 0;

    j = 0;
    while(ajListPop(idlist,(void**)&bid))
    {
	cid = cbucket->Ids[j];
	++j;

	ajStrAssS(&cid->id,bid->id);
	cid->dbno = bid->dbno;
	cid->dups = bid->dups;
	cid->offset = bid->offset;
	++cbucket->Nentries;
	embBtreeIdDel(&bid);
    }
    
    parray[i] = cache->totsize;
    embBtreeWriteBucket(cache,cbucket,parray[i]);

    nkeys = bucketn - 1;
    
    SBT_NKEYS(rbuf,nkeys);
    SBT_TOTLEN(rbuf,totkeylen);
    nodetype = BT_LEAF;
    SBT_NODETYPE(rbuf,nodetype);
    GBT_PREV(lbuf,&prev);
    SBT_PREV(rbuf,prev);
    SBT_OVERFLOW(rbuf,overflow);
    
    embBtreeWriteNode(cache,rpage,karray,parray,nkeys);
    rpage->dirty = BT_DIRTY;

    cbucket->Nentries = maxnperbucket;
    embBtreeBucketDel(&cbucket);
    ajListDel(&idlist);



    medianless = lblockno;
    mediangtr  = rblockno;


    if(rootnodetype == BT_ROOT)
    {
	ajStrAssS(&karray[0],mediankey);
	parray[0]=lblockno;
	parray[1]=rblockno;
	nkeys = 1;
	embBtreeWriteNode(cache,spage,karray,parray,nkeys);	
	spage->dirty = BT_LOCK;
	for(i=0;i<order;++i)
	    ajStrDel(&karray[i]);
	AJFREE(karray);
	AJFREE(parray);
	ajStrDel(&mediankey);
	++cache->level;
	return spage;
    }


    for(i=0;i<order;++i)
	ajStrDel(&karray[i]);
    AJFREE(karray);
    AJFREE(parray);

    page = embBtreeCacheRead(cache,prev);
    embBtreeInsertKey(cache,page,mediankey,medianless,mediangtr);
    ajStrDel(&mediankey);

    page = embBtreeCacheRead(cache,prev);

    return page;
}




/* @func embBtreeDeleteId *********************************************
**
** Entry point for ID deletion
**
** @param [rw] cache [EmbPBtcache] cache
** @param [r] id [EmbPBtId] page
**
** @return [AjBool] True if found and deleted
** @@
******************************************************************************/

AjBool embBtreeDeleteId(EmbPBtcache cache, EmbPBtId id)
{
    EmbPBtpage rootpage = NULL;
    unsigned char *buf  = NULL;
    ajlong balanceNode  = 0L;
    ajlong blockno      = 0L;

    
/*    ajDebug("In embBtreeDeleteId\n"); */
    
    rootpage = embBtreeCacheLocate(cache,0L);
    rootpage->dirty = BT_LOCK;
    
    buf = rootpage->buf;
    
    balanceNode = BTNO_BALANCE;

    blockno = embBtreeFindBalance(cache,0L,BTNO_NODE,BTNO_NODE,BTNO_NODE,
				  BTNO_NODE,id);

    
    if(!cache->deleted)
	return ajFalse;

    return ajTrue;
}




/* @funcstatic embBtreeFindBalance *******************************************
**
** Master routine for entry deletion
**
** @param [rw] cache [EmbPBtcache] cache
** @param [r] id [EmbPBtId] id
**
** @return [ajlong] page number or BTNO_NODE
** @@
******************************************************************************/

static ajlong embBtreeFindBalance(EmbPBtcache cache, ajlong thisNode,
				  ajlong leftNode, ajlong rightNode,
				  ajlong lAnchor, ajlong rAnchor,
				  EmbPBtId id)
{
    unsigned char *buf  = NULL;
    unsigned char *buf1 = NULL;
    
    ajlong nextNode   = BTNO_NODE;
    ajlong nextLeft   = BTNO_NODE;
    ajlong nextRight  = BTNO_NODE;
    ajlong nextAncL   = BTNO_NODE;
    ajlong nextAncR   = BTNO_NODE;
    ajlong done       = 0L;
    
    ajint  nkeys      = 0;
    ajint  order      = 0;
    ajint  minkeys    = 0;
    ajint  i;
    ajint  nodetype   = 0;

    ajint n1keys      = 0;
    
    EmbPBtpage page  = NULL;
    EmbPBtpage page1 = NULL;

    ajlong balanceNode = 0L;
    ajlong blockno     = 0L;
    ajlong ptrSave     = 0L;

    AjPStr *karray  = NULL;
    ajlong *parray  = NULL;
    AjPStr *k1array = NULL;
    ajlong *p1array = NULL;

    
    char *key = NULL;
    AjBool existed = ajFalse;
    
/*    ajDebug("In embBtreeFindBalance\n"); */

    if(thisNode)
	page = embBtreeCacheRead(cache,thisNode);
    else
    {
	page = embBtreeCacheLocate(cache,thisNode);
	page->dirty = BT_LOCK;
    }

    cache->deleted = ajFalse;

    buf = page->buf;
    GBT_NKEYS(buf,&nkeys);
    
    order = cache->order;
    minkeys = (order-1) / 2;
    if((order-1)%2)
	++minkeys;

    if(nkeys >= minkeys)
	balanceNode = BTNO_BALANCE;
    else
	balanceNode = page->pageno;

    AJCNEW0(karray,order);
    AJCNEW0(parray,order);
    AJCNEW0(k1array,order);
    AJCNEW0(p1array,order);
    
    for(i=0;i<order;++i)
    {
	k1array[i] = ajStrNew();
	karray[i]  = ajStrNew();
    }

    key = id->id->Ptr;

    embBtreeGetKeys(cache,buf,&karray,&parray);
    i=0;
    while(i!=nkeys && strcmp(key,karray[i]->Ptr)>=0)
	++i;
    if(i==nkeys)
    {
	if(strcmp(key,karray[i-1]->Ptr)<0)
	    blockno = parray[i-1];
	else
	    blockno = parray[i];
    }
    else
	blockno = parray[i];

    nextNode = blockno;
    ptrSave = i;

    GBT_NODETYPE(buf,&nodetype);
    if(!(nodetype == BT_LEAF) && !(nodetype == BT_ROOT && !cache->level))
    {
	if(nextNode == parray[0])
	{
	    if(leftNode != BTNO_NODE)
	    {
		page1 = embBtreeCacheRead(cache,leftNode);
		buf1 = page1->buf;
/*		page1->dirty = BT_CLEAN; */
		GBT_NKEYS(buf1,&n1keys);
		embBtreeGetKeys(cache,buf1,&k1array,&p1array);
		nextLeft = p1array[n1keys];
	    }
	    else
		nextLeft = BTNO_NODE;
	    
	    if(!thisNode)
		nextAncL = thisNode;
	    else
		nextAncL = lAnchor;
	}
	else
	{
	    nextLeft = parray[ptrSave-1];
	    nextAncL = thisNode;
	}

	if(nextNode == parray[nkeys])
	{
	    if(rightNode != BTNO_NODE)
	    {
		page1 = embBtreeCacheRead(cache,rightNode);
		buf1 = page1->buf;
/*		page1->dirty = BT_CLEAN; */
		GBT_NKEYS(buf1,&n1keys);
		embBtreeGetKeys(cache,buf1,&k1array,&p1array);
		nextRight = p1array[0];
	    }
	    else
		nextRight = BTNO_NODE;

	    if(!thisNode)
		nextAncR = thisNode;
	    else
		nextAncR = rAnchor;
	}
	else
	{
	    nextRight = parray[ptrSave+1];
	    nextAncR  = thisNode;
	}



	/* Check to see whether key exists in an internal node */
	if(nodetype != BT_LEAF && cache->level)
	{
	    i=0;
	    while(i!=nkeys && strcmp(key,karray[i]->Ptr))
		++i;
	    if(i!=nkeys)
	    {
		embBtreeFindMin(cache,parray[i+1],key);
		ajStrAssS(&karray[i],cache->replace);
		embBtreeWriteNode(cache,page,karray,parray,nkeys);
	    }
	
	}
	
	embBtreeFindBalance(cache,nextNode,nextLeft,nextRight,
			    nextAncL,nextAncR,id);

	if(thisNode)
	    page = embBtreeCacheRead(cache,thisNode);
	else
	{
	    page = embBtreeCacheLocate(cache,thisNode);
	    page->dirty = BT_LOCK;
	}
	buf = page->buf;

    }
    else
    {
	if(nodetype == BT_LEAF || (nodetype==BT_ROOT && !cache->level))
	{
	    existed = embBtreeRemoveEntry(cache,thisNode,id);
	    
	    if(existed)
		cache->deleted = ajTrue;
	    GBT_NKEYS(buf,&nkeys);
	    if(nkeys >= minkeys || (nodetype==BT_ROOT && !cache->level))
		balanceNode = BTNO_BALANCE;
	    else
		balanceNode = page->pageno;
	}
    }



    
	
    if(balanceNode == BTNO_BALANCE || thisNode == 0L)
	done = BTNO_NODE;
    else
	done = embBtreeRebalance(cache,thisNode,leftNode,rightNode,
				 lAnchor,rAnchor);
    


    for(i=0;i<order;++i)
    {
	ajStrDel(&k1array[i]);
	ajStrDel(&karray[i]);
    }
    AJFREE(k1array);
    AJFREE(karray);
    AJFREE(p1array);
    AJFREE(parray);

    return done;
}




/* @funcstatic embBtreeRemoveEntry *******************************************
**
** Find and delete an ID from a given leaf node if necessary
**
** @param [rw] cache [EmbPBtcache] cache
** @param [r] pageno [ajlong] leaf node page
** @param [r] id [EmbPBtId] id
**
** @return [AjBool] True if found (and deleted)
** @@
******************************************************************************/

static AjBool embBtreeRemoveEntry(EmbPBtcache cache,ajlong pageno,EmbPBtId id)
{
    unsigned char *buf = NULL;
    EmbPBtpage page    = NULL;
    EmbPBucket bucket  = NULL;
    
    AjPStr *karray = NULL;
    ajlong *parray = NULL;
    ajlong blockno = 0L;
    
    ajint order    = 0;
    ajint nkeys    = 0;
    ajint nentries = 0;
    ajint i;
    ajint dirtysave = 0;
    
    AjBool found = ajFalse;
    char   *key  = NULL;

/*    ajDebug("In embBtreeRemoveEntry\n"); */

    page = embBtreeCacheRead(cache,pageno);
    buf = page->buf;
    dirtysave = page->dirty;
    page->dirty = BT_LOCK;
    order = cache->order;
    
    GBT_NKEYS(buf,&nkeys);
    if(!nkeys)
	return ajFalse;

    AJCNEW0(karray,order);
    AJCNEW0(parray,order);
    for(i=0;i<order;++i)
	karray[i] = ajStrNew();

    embBtreeGetKeys(cache,buf,&karray,&parray);
    
    key = id->id->Ptr;

    i=0;
    while(i!=nkeys && strcmp(key,karray[i]->Ptr)>=0)
	++i;
    if(i==nkeys)
    {
	if(strcmp(key,karray[i-1]->Ptr)<0)
	    blockno = parray[i-1];
	else
	    blockno = parray[i];
    }
    else
	blockno = parray[i];
    
    bucket = embBtreeReadBucket(cache,blockno);


    nentries = bucket->Nentries;
    found = ajFalse;

    for(i=0;i<nentries;++i)
	if(!strcmp(key,bucket->Ids[i]->id->Ptr))
	{
	    found = ajTrue;
	    break;
	}
    

    if(found)
    {
	/* Perform the deletion */
	if(nentries == 1)
	{
	    bucket->Nentries = 0;
	    embBtreeIdDel(&bucket->Ids[0]);
	}
	else
	{
	    embBtreeIdDel(&bucket->Ids[i]);
	    bucket->Ids[i] = bucket->Ids[nentries-1];
	    --bucket->Nentries;
	}

	embBtreeWriteBucket(cache,bucket,blockno);
	embBtreeAdjustBuckets(cache,page);
	page->dirty = BT_DIRTY;
    }
    else
	page->dirty = dirtysave;

    embBtreeBucketDel(&bucket);

    for(i=0;i<order;++i)
	ajStrDel(&karray[i]);
    AJFREE(karray);
    AJFREE(parray);

    
    if(!found)
	return ajFalse;

    return ajTrue;
}




/* @funcstatic embBtreeAdjustBuckets *****************************************
**
** Re-order leaf buckets
** Can be called whatever the state of a leaf (used by deletion funcs)
**
** @param [r] cache [EmbPBtcache] cache
** @param [r] leaf [EmbPBtpage] leaf page
**
** @return [void]
** @@
******************************************************************************/
static void embBtreeAdjustBuckets(EmbPBtcache cache, EmbPBtpage leaf)
{
    ajint nkeys = 0;
    unsigned char *lbuf = NULL;
    EmbPBucket *buckets = NULL;
    AjPStr *keys        = NULL;
    ajlong *ptrs        = NULL;
    ajlong *overflows   = NULL;
    
    ajint i = 0;
    ajint j = 0;
    
    ajint order;
    ajint bentries      = 0;
    ajint totalkeys     = 0;
    ajint nperbucket    = 0;
    ajint maxnperbucket = 0;
    ajint count         = 0;
    ajint totkeylen     = 0;
    ajint keylimit      = 0;
    ajint bucketn       = 0;
    ajint bucketlimit   = 0;
    ajint nodetype      = 0;
    ajint nids          = 0;
    ajint totnids       = 0;
    
    AjPList idlist     = NULL;
    ajint   dirtysave  = 0;
    EmbPBtId bid       = NULL;
    EmbPBucket cbucket = NULL;
    EmbPBtId cid       = NULL;
    
/*    ajDebug("In embBtreeAdjustBuckets\n"); */

    dirtysave = leaf->dirty;

    leaf->dirty = BT_LOCK;
    lbuf = leaf->buf;

    GBT_NKEYS(lbuf,&nkeys);
    if(!nkeys)
    {
	leaf->dirty = dirtysave;
	return;
    }


    GBT_NODETYPE(lbuf,&nodetype);

    order = cache->order;
    nperbucket = cache->nperbucket;
    

    /* Read keys/ptrs */
    AJCNEW0(keys,order);
    AJCNEW0(ptrs,order);
    AJCNEW0(overflows,order);
    
    for(i=0;i<order;++i)
	keys[i] = ajStrNew();

    embBtreeGetKeys(cache,lbuf,&keys,&ptrs);


    for(i=0;i<nkeys;++i)
	totalkeys += embBtreeNumInBucket(cache,ptrs[i]);
    totalkeys += embBtreeNumInBucket(cache,ptrs[i]);


    /* Set the number of entries per bucket to approximately half full */
    maxnperbucket = nperbucket >> 1;
    if(!maxnperbucket)
	++maxnperbucket;

    if(!leaf->pageno)
	maxnperbucket = nperbucket;

    /* Work out the number of new buckets needed */
    bucketn = (totalkeys / maxnperbucket);
    if(totalkeys % maxnperbucket)
	++bucketn;

    if(bucketn == 1)
	++bucketn;
    
    
    if(bucketn > order)
	ajFatal("AdjustBuckets: bucket number greater than order");
    

    /* Read buckets */
    AJCNEW0(buckets,nkeys+1);
    keylimit = nkeys + 1;
    
    for(i=0;i<keylimit;++i)
	buckets[i] = embBtreeReadBucket(cache,ptrs[i]);


    /* Read IDs from all buckets and push to list and sort (increasing id) */
    idlist  = ajListNew();

    for(i=0;i<keylimit;++i)
    {
	overflows[i] = buckets[i]->Overflow;
	bentries = buckets[i]->Nentries;
	for(j=0;j<bentries;++j)
	    ajListPush(idlist,(void *)buckets[i]->Ids[j]);
	
	AJFREE(buckets[i]->keylen);
	AJFREE(buckets[i]->Ids);
	AJFREE(buckets[i]);
    }
    ajListSort(idlist,embBtIdCompare);
    AJFREE(buckets);

    cbucket = embBtreeBucketNew(maxnperbucket);
    bucketlimit = bucketn - 1;

    totnids = 0;
    nids = ajListLength(idlist);


    if(!totalkeys)
    {
	SBT_NKEYS(lbuf,totalkeys);
	for(i=0;i<order;++i)
	    ajStrDel(&keys[i]);
	AJFREE(keys);
	AJFREE(ptrs);
	AJFREE(overflows);
	ajListDel(&idlist);
	leaf->dirty = BT_DIRTY;
	return;
    }
    
    if(nids <= maxnperbucket)
    {
	cbucket->Overflow = overflows[1];
	cbucket->Nentries = 0;
	ajListPeek(idlist,(void **)&bid);
	ajStrAssS(&keys[0],bid->id);

	count = 0;
	while(count!=maxnperbucket && totnids != nids)
	{
	    ajListPop(idlist,(void **)&bid);
	    
	    cid = cbucket->Ids[count];
	    ajStrAssS(&cid->id,bid->id);
	    cid->dbno = bid->dbno;
	    cid->dups = bid->dups;
	    cid->offset = bid->offset;

	    cbucket->keylen[count] = BT_BUCKIDLEN(bid->id);
	    ++cbucket->Nentries;
	    ++count;
	    ++totnids;
	    embBtreeIdDel(&bid);
	}


	totkeylen += ajStrLen(keys[0]);

	if(!ptrs[1])
	    ptrs[1] = cache->totsize;
	embBtreeWriteBucket(cache,cbucket,ptrs[1]);

	cbucket->Overflow = overflows[0];
	cbucket->Nentries = 0;
	if(!ptrs[0])
	    ptrs[0] = cache->totsize;
	embBtreeWriteBucket(cache,cbucket,ptrs[0]);
    }
    else
    {
	for(i=0;i<bucketlimit;++i)
	{
	    cbucket->Overflow = overflows[i];
	    cbucket->Nentries = 0;

	    count = 0;
	    while(count!=maxnperbucket && totnids != nids)
	    {
		ajListPop(idlist,(void **)&bid);

		cid = cbucket->Ids[count];
		ajStrAssS(&cid->id,bid->id);
		cid->dbno = bid->dbno;
		cid->dups = bid->dups;
		cid->offset = bid->offset;

		cbucket->keylen[count] = BT_BUCKIDLEN(bid->id);
		++cbucket->Nentries;
		++count;
		embBtreeIdDel(&bid);
	    }


	    ajListPeek(idlist,(void **)&bid);
	    ajStrAssS(&keys[i],bid->id);


	    totkeylen += ajStrLen(bid->id);

	    if(!ptrs[i])
		ptrs[i] = cache->totsize;
	    embBtreeWriteBucket(cache,cbucket,ptrs[i]);
	}
	
	
	/* Deal with greater-than bucket */
	
	cbucket->Overflow = overflows[i];
	cbucket->Nentries = 0;
	
	
	
	count = 0;
	while(ajListPop(idlist,(void **)&bid))
	{
	    cid = cbucket->Ids[count];
	    ajStrAssS(&cid->id,bid->id);
	    cid->dbno = bid->dbno;
	    cid->dups = bid->dups;
	    cid->offset = bid->offset;

	    ++cbucket->Nentries;
	    ++count;
	    embBtreeIdDel(&bid);
	}
	
	
	if(!ptrs[i])
	    ptrs[i] = cache->totsize;
	
	embBtreeWriteBucket(cache,cbucket,ptrs[i]);
    }
    

    cbucket->Nentries = maxnperbucket;
    embBtreeBucketDel(&cbucket);

    /* Now write out a modified leaf with new keys/ptrs */

    nkeys = bucketn - 1;
    SBT_NKEYS(lbuf,nkeys);
    SBT_TOTLEN(lbuf,totkeylen);

    embBtreeWriteNode(cache,leaf,keys,ptrs,nkeys);

    leaf->dirty = dirtysave;
    if(nodetype == BT_ROOT)
	leaf->dirty = BT_LOCK;


    for(i=0;i<order;++i)
	ajStrDel(&keys[i]);
    AJFREE(keys);
    AJFREE(ptrs);
    AJFREE(overflows);
    

    embBtreeBucketDel(&cbucket);
    ajListDel(&idlist);

    return;
}




/* @funcstatic embBtreeCollapseRoot *******************************************
**
** Master routine for entry deletion
**
** @param [rw] cache [EmbPBtcache] cache
** @param [r] pageno [ajlong] page number to make new root
**
** @return [ajlong] page number or BTNO_NODE
** @@
******************************************************************************/

static ajlong embBtreeCollapseRoot(EmbPBtcache cache, ajlong pageno)
{
    unsigned char *buf  = NULL;
    unsigned char *lbuf = NULL;

    AjPStr *karray      = NULL;
    ajlong *parray      = NULL;

    EmbPBtpage rootpage = NULL;
    EmbPBtpage page     = NULL;
    
    ajint nodetype = 0;
    ajint nkeys    = 0;
    ajint order    = 0;
    ajint i;

    ajlong prev   = 0L;

/*    ajDebug("In embBtreeCollapseRoot\n"); */
    
    if(!cache->level)
	return BTNO_NODE;

    rootpage = embBtreeCacheLocate(cache,0L);
    buf = rootpage->buf;
    page = embBtreeCacheRead(cache,pageno);


    order = cache->order;
    AJCNEW0(karray,order);
    AJCNEW0(parray,order);
    for(i=0;i<order;++i)
	karray[i] = ajStrNew();

    /*
    ** Swap pageno values to make root the child and child the root
    ** Update nodetypes and mark the original root as a clean page
    */

    /* At this point page->pageno could be added to a free list */

    rootpage->pageno = page->pageno;
    rootpage->dirty = BT_CLEAN;
    nodetype = BT_INTERNAL;
    SBT_NODETYPE(buf,nodetype);

    page->pageno = 0;
    page->dirty  = BT_LOCK;
    buf = page->buf;
    nodetype = BT_ROOT;
    SBT_NODETYPE(buf,nodetype);
    
    --cache->level;

    if(cache->level)
    {
	/*
	 ** Update the PREV pointers of the new root's children
	 */
	GBT_NKEYS(buf,&nkeys);
	embBtreeGetKeys(cache,buf,&karray,&parray);
	for(i=0;i<nkeys+1;++i)
	{
	    page = embBtreeCacheRead(cache,parray[i]);
	    lbuf = page->buf;
	    SBT_PREV(lbuf,prev);
	    page->dirty = BT_DIRTY;
	}
    }


    for(i=0;i<order;++i)
	ajStrDel(&karray[i]);
    AJFREE(karray);
    AJFREE(parray);

    
    return 0L;
}




/* @funcstatic embBtreeRebalance *******************************************
**
** Master routine for entry deletion
**
** @param [rw] cache [EmbPBtcache] cache
** @param [r] thisNode [ajlong] Node to rebalance
** @param [r] leftNode [ajlong] left node
** @param [r] rightNode [ajlong] right node
** @param [r] lAnchor [ajlong] left anchor
** @param [r] rAnchor [ajlong] right anchor
**
** @return [ajlong] page number or BTNO_NODE
** @@
******************************************************************************/

static ajlong embBtreeRebalance(EmbPBtcache cache, ajlong thisNode,
				ajlong leftNode, ajlong rightNode,
				ajlong lAnchor, ajlong rAnchor)
{
    unsigned char *lbuf = NULL;
    unsigned char *rbuf = NULL;
    unsigned char *tbuf = NULL;

    ajlong anchorNode   = 0L;
    ajlong balanceNode  = 0L;
    ajlong mergeNode    = 0L;
    ajlong done         = 0L;
    ajlong parent       = 0L;
    
    EmbPBtpage lpage    = NULL;
    EmbPBtpage rpage    = NULL;
    EmbPBtpage tpage    = NULL;
    
    ajint lnkeys        = 0;
    ajint rnkeys        = 0;
    ajint size          = 0;
    ajint order         = 0;
    ajint minsize       = 0;

    AjBool leftok  = ajFalse;
    AjBool rightok = ajFalse;
    
    
/*    ajDebug("In embBtreeRebalance\n"); */

    if(leftNode!=BTNO_NODE && lAnchor!=BTNO_NODE)
	leftok = ajTrue;
    if(rightNode!=BTNO_NODE && rAnchor!=BTNO_NODE)
	rightok = ajTrue;

    if(!leftok && !rightok)
	return BTNO_NODE;
    

    if(leftok)
    {
	lpage = embBtreeCacheRead(cache,leftNode);
	lbuf  = lpage->buf;
	GBT_NKEYS(lbuf,&lnkeys);
    }
    

    if(rightok)
    {
	rpage = embBtreeCacheRead(cache,rightNode);
	rbuf  = rpage->buf;
	GBT_NKEYS(rbuf,&rnkeys);
    }
    


    if(leftok && rightok)
    {
	size = (lnkeys >= rnkeys) ? lnkeys : rnkeys;
	balanceNode = (lnkeys >= rnkeys) ? leftNode : rightNode;
    }
    else if(leftok)
    {
	size = lnkeys;
	balanceNode = leftNode;
    }
    else
    {
	size = rnkeys;
	balanceNode = rightNode;
    }

    
    order = cache->order;
    minsize = (order-1) / 2;
    if((order-1)%2)
	++minsize;

    /* Changed from size > minsize on Saturday. else get too many keys in
       merge */
    if(size >= minsize)
    {
	if(leftok && rightok)
	    anchorNode = (lnkeys >= rnkeys) ? lAnchor : rAnchor;
	else if(leftok)
	    anchorNode = lAnchor;
	else
	    anchorNode = rAnchor;
	done = embBtreeShift(cache,thisNode,balanceNode,anchorNode);
    }
	    
    else
    {
	tpage = embBtreeCacheRead(cache,thisNode);
	tbuf  = tpage->buf;
	GBT_PREV(tbuf,&parent);
	if(leftok && rightok)
	{
	    anchorNode = (parent == lAnchor) ? lAnchor : rAnchor;
	    mergeNode  = (anchorNode  == lAnchor) ? leftNode : rightNode;
	}
	else if(leftok)
	{
	    anchorNode = lAnchor;
	    mergeNode  = leftNode;
	}
	else
	{
	    anchorNode = rAnchor;
	    mergeNode  = rightNode;
	}
	done = embBtreeMerge(cache,thisNode,mergeNode,anchorNode);
    }

    return done;
}




/* @funcstatic embBtreeShift *************************************************
**
** Shift spare entries from one node to another
**
** @param [rw] cache [EmbPBtcache] cache
** @param [r] thisNode [ajlong] master node
** @param [r] balanceNode [ajlong] balance node
** @param [r] anchorNode [ajlong] anchor node
**
** @return [void] page number or BTNO_NODE
** @@
******************************************************************************/

static ajlong embBtreeShift(EmbPBtcache cache, ajlong thisNode,
			    ajlong balanceNode, ajlong anchorNode)
{
    unsigned char *tbuf = NULL;
    unsigned char *abuf = NULL;
    unsigned char *bbuf = NULL;
    unsigned char *buf  = NULL;
    
    AjPStr *kTarray = NULL;
    AjPStr *kAarray = NULL;
    AjPStr *kBarray = NULL;
    ajlong *pTarray = NULL;
    ajlong *pAarray = NULL;
    ajlong *pBarray = NULL;
    
    ajint  nAkeys = 0;
    ajint  nBkeys = 0;
    ajint  nTkeys = 0;
    ajint  order  = 0;
    ajint  count  = 0;
    ajint  i;
    
    EmbPBtpage pageA = NULL;
    EmbPBtpage pageB = NULL;
    EmbPBtpage pageT = NULL;
    EmbPBtpage page  = NULL;
    
    EmbPBtpage leftpage  = NULL;

    ajint  anchorPos   = 0;
    ajlong prev        = 0L;
    ajint  nodetype    = 0;
    
/*    ajDebug("In embBtreeShift\n"); */

    order = cache->order;
    AJCNEW0(kTarray,order);
    AJCNEW0(kAarray,order);
    AJCNEW0(kBarray,order);
    AJCNEW0(pTarray,order);
    AJCNEW0(pAarray,order);
    AJCNEW0(pBarray,order);
    for(i=0;i<order;++i)
    {
	kTarray[i] = ajStrNew();
	kAarray[i] = ajStrNew();
	kBarray[i] = ajStrNew();
    }


    pageA = embBtreeCacheRead(cache,anchorNode);
    pageA->dirty = BT_LOCK;
    abuf = pageA->buf;
    pageB = embBtreeCacheRead(cache,balanceNode);
    pageB->dirty = BT_LOCK;
    bbuf = pageB->buf;
    pageT = embBtreeCacheRead(cache,thisNode);
    pageT->dirty = BT_LOCK;
    tbuf = pageT->buf;

    GBT_NKEYS(abuf,&nAkeys);
    GBT_NKEYS(bbuf,&nBkeys);
    GBT_NKEYS(tbuf,&nTkeys);

    embBtreeGetKeys(cache,abuf,&kAarray,&pAarray);
    embBtreeGetKeys(cache,bbuf,&kBarray,&pBarray);
    embBtreeGetKeys(cache,tbuf,&kTarray,&pTarray);
    
    if(strcmp(kTarray[nTkeys-1]->Ptr,kBarray[nBkeys-1]->Ptr)<0)
	leftpage = pageT;
    else
	leftpage = pageB;


    if(leftpage == pageT)
    {
	/* Find anchor key position */
	i=0;
	while(i!=nAkeys && strcmp(kTarray[nTkeys-1]->Ptr,kAarray[i]->Ptr)>=0)
	    ++i;
	anchorPos = i;

	/* Move down anchor key to thisNode */
	ajStrAssS(&kTarray[nTkeys],kAarray[anchorPos]);
	++nTkeys;

	/* Shift extra */
	while(nTkeys < nBkeys)
	{
	    ajStrAssS(&kTarray[nTkeys],kBarray[0]);
	    pTarray[nTkeys] = pBarray[0];
	    ++nTkeys;
	    --nBkeys;

	    for(i=0;i<nBkeys;++i)
	    {
		ajStrAssS(&kBarray[i],kBarray[i+1]);
		pBarray[i] = pBarray[i+1];
	    }
	    pBarray[i] = pBarray[i+1];
	}

	/* Adjust anchor key */
	ajStrAssS(&kAarray[anchorPos],kTarray[nTkeys-1]);
	--nTkeys;
    }
    else	/* thisNode on the right */
    {
	/* Find anchor key position */
	i=0;
	while(i!=nAkeys && strcmp(kBarray[nBkeys-1]->Ptr,kAarray[i]->Ptr)>=0)
	    ++i;
	anchorPos = i;

	/* Move down anchor key to thisNode */
	pTarray[nTkeys+1] = pTarray[nTkeys];
	for(i=nTkeys-1;i>-1;--i)
	{
	    ajStrAssS(&kTarray[i+1],kTarray[i]);
	    pTarray[i+1] = pTarray[i];
	}
	ajStrAssS(&kTarray[0],kAarray[anchorPos]);
	++nTkeys;

	/* Shift extra */
	count = 0;
	while(nTkeys < nBkeys)
	{
	    pTarray[nTkeys+1] = pTarray[nTkeys];
	    for(i=nTkeys-1;i>-1;--i)
	    {
		ajStrAssS(&kTarray[i+1],kTarray[i]);
		pTarray[i+1] = pTarray[i];
	    }
	    ajStrAssS(&kTarray[0],kBarray[nBkeys-1]);
	    pTarray[1] = pBarray[nBkeys];
	    ++nTkeys;
	    --nBkeys;
	}


	/* Adjust anchor key */
	ajStrAssS(&kAarray[anchorPos],kTarray[0]);
	--nTkeys;
	for(i=0;i<nTkeys;++i)
	{
	    ajStrAssS(&kTarray[i],kTarray[i+1]);
	    pTarray[i] = pTarray[i+1];
	}
	pTarray[i] = pTarray[i+1];
    }
    

    /* Adjust PREV pointers for thisNode */
    prev = pageT->pageno;
    for(i=0;i<nTkeys+1;++i)
    {
	page = embBtreeCacheRead(cache,pTarray[i]);
	buf = page->buf;
	GBT_NODETYPE(buf,&nodetype);
	if(nodetype != BT_BUCKET)
	{
	    SBT_PREV(buf,prev);
	    page->dirty = BT_DIRTY;
	}
    }



    embBtreeWriteNode(cache,pageA,kAarray,pAarray,nAkeys);
    embBtreeWriteNode(cache,pageB,kBarray,pBarray,nBkeys);
    embBtreeWriteNode(cache,pageT,kTarray,pTarray,nTkeys);

    if(!anchorNode)
	pageA->dirty = BT_LOCK;

    for(i=0;i<order;++i)
    {
	ajStrDel(&kTarray[i]);
	ajStrDel(&kAarray[i]);
	ajStrDel(&kBarray[i]);
    }
    AJFREE(kTarray);
    AJFREE(kAarray);
    AJFREE(kBarray);
    AJFREE(pTarray);
    AJFREE(pAarray);
    AJFREE(pBarray);


    return BTNO_NODE;
}




/* @funcstatic embBtreeMerge *************************************************
**
** Merge two nodes
**
** @param [rw] cache [EmbPBtcache] cache
** @param [r] thisNode [ajlong] master node
** @param [r] mergeNode [ajlong] merge node
** @param [r] anchorNode [ajlong] anchor node
**
** @return [void] page number or BTNO_NODE
** @@
******************************************************************************/
static ajlong embBtreeMerge(EmbPBtcache cache, ajlong thisNode,
			    ajlong mergeNode, ajlong anchorNode)
{
    unsigned char *tbuf = NULL;
    unsigned char *abuf = NULL;
    unsigned char *nbuf = NULL;
    unsigned char *buf  = NULL;
    
    AjPStr *kTarray = NULL;
    AjPStr *kAarray = NULL;
    AjPStr *kNarray = NULL;
    ajlong *pTarray = NULL;
    ajlong *pAarray = NULL;
    ajlong *pNarray = NULL;

    ajlong thisprev  = 0L;
    ajlong mergeprev = 0L;
    
    
    ajint  nAkeys = 0;
    ajint  nNkeys = 0;
    ajint  nTkeys = 0;
    ajint  order  = 0;
    ajint  count  = 0;
    ajint  i;
    ajint  nodetype = 0;
    
    ajint saveA = 0;
    ajint saveN = 0;
    ajint saveT = 0;
    
    EmbPBtpage pageA = NULL;
    EmbPBtpage pageN = NULL;
    EmbPBtpage pageT = NULL;
    EmbPBtpage page  = NULL;
    
    EmbPBtpage leftpage  = NULL;

    ajint  anchorPos = 0;
    ajlong prev      = 0L;

    AjBool collapse = ajFalse;
    
/*    ajDebug("In embBtreeMerge\n"); */

    order = cache->order;


    pageA = embBtreeCacheRead(cache,anchorNode);
    saveA = pageA->dirty;
    pageA->dirty = BT_LOCK;
    abuf = pageA->buf;
    pageN = embBtreeCacheRead(cache,mergeNode);
    saveN = pageN->dirty;
    pageN->dirty = BT_LOCK;
    nbuf = pageN->buf;
    pageT = embBtreeCacheRead(cache,thisNode);
    saveT = pageT->dirty;
    pageT->dirty = BT_LOCK;
    tbuf = pageT->buf;

    GBT_PREV(tbuf,&thisprev);
    GBT_PREV(nbuf,&mergeprev);

    GBT_NKEYS(abuf,&nAkeys);
    GBT_NKEYS(nbuf,&nNkeys);
    GBT_NKEYS(tbuf,&nTkeys);

    GBT_NODETYPE(nbuf,&nodetype);


    if(nAkeys == 1)
    {
	if(!anchorNode && !thisprev && !mergeprev)
	    collapse = ajTrue;
	else
	{
	    pageA->dirty = saveA;
	    pageN->dirty = saveN;
	    pageT->dirty = saveT;
	    return thisNode;
	}
    }

    AJCNEW0(kTarray,order);
    AJCNEW0(kAarray,order);
    AJCNEW0(kNarray,order);
    AJCNEW0(pTarray,order);
    AJCNEW0(pAarray,order);
    AJCNEW0(pNarray,order);
    for(i=0;i<order;++i)
    {
	kTarray[i] = ajStrNew();
	kAarray[i] = ajStrNew();
	kNarray[i] = ajStrNew();
    }

    embBtreeGetKeys(cache,abuf,&kAarray,&pAarray);
    embBtreeGetKeys(cache,nbuf,&kNarray,&pNarray);
    embBtreeGetKeys(cache,tbuf,&kTarray,&pTarray);

    if(strcmp(kTarray[nTkeys-1]->Ptr,kNarray[nNkeys-1]->Ptr)<0)
	leftpage = pageT;
    else
	leftpage = pageN;


    if(leftpage == pageT)
    {
	/* Find anchor key position */
	i=0;
	while(i!=nAkeys && strcmp(kTarray[nTkeys-1]->Ptr,kAarray[i]->Ptr)>=0)
	    ++i;
	anchorPos = i;

	/* Move down anchor key to neighbour Node */
	pNarray[nNkeys+1] = pNarray[nNkeys];
	for(i=nNkeys-1;i>-1;--i)
	{
	    ajStrAssS(&kNarray[i+1],kNarray[i]);
	    pNarray[i+1] = pNarray[i];
	}
	ajStrAssS(&kNarray[0],kAarray[anchorPos]);
	++nNkeys;


	/* Adjust anchor node keys/ptrs */
	++anchorPos;
	if(anchorPos==nAkeys)
	    pAarray[nAkeys-1] = pAarray[nAkeys];
	else
	{
	    for(i=anchorPos;i<nAkeys;++i)
	    {
		ajStrAssS(&kAarray[i-1],kAarray[i]);
		pAarray[i-1] = pAarray[i];
	    }
	    pAarray[i-1] = pAarray[i];
	}
	--nAkeys;
	

	/* Merge this to neighbour */

	while(nTkeys)
	{
	    pNarray[nNkeys+1] = pNarray[nNkeys];
	    for(i=nNkeys-1;i>-1;--i)
	    {
		ajStrAssS(&kNarray[i+1],kNarray[i]);
		pNarray[i+1] = pNarray[i];
	    }
	    ajStrAssS(&kNarray[0],kTarray[nTkeys-1]);
	    pNarray[1] = pTarray[nTkeys];
	    pNarray[0] = pTarray[nTkeys-1];
	    --nTkeys;
	    ++nNkeys;
	}

	/* At this point the 'this' node could be added to a freelist */
    }
    else
    {
	/* Find anchor key position */
	i=0;
	while(i!=nAkeys && strcmp(kNarray[nNkeys-1]->Ptr,kAarray[i]->Ptr)>=0)
	    ++i;
	anchorPos = i;

	/* Move down anchor key to neighbourNode */
	ajStrAssS(&kNarray[nNkeys],kAarray[anchorPos]);
	++nNkeys;

	/* Adjust anchor node keys/ptrs */
	++anchorPos;
	if(anchorPos!=nAkeys)
	    for(i=anchorPos;i<nAkeys;++i)
	    {
		ajStrAssS(&kAarray[i-1],kAarray[i]);
		pAarray[i] = pAarray[i+1];
	    }
	--nAkeys;

	/* merge extra */
	count = 0;
	while(nTkeys)
	{
	    ajStrAssS(&kNarray[nNkeys],kTarray[count]);
	    pNarray[nNkeys] = pTarray[count];
	    ++nNkeys;
	    ++count;
	    --nTkeys;
	    pNarray[nNkeys] = pTarray[count];
	
	}

	/* At this point the 'this' node could be added to a freelist */
    }
    
    
    /* Adjust PREV pointers for neighbour Node */
    prev = pageN->pageno;
    for(i=0;i<nNkeys+1;++i)
    {
	page = embBtreeCacheRead(cache,pNarray[i]);
	buf = page->buf;
	GBT_NODETYPE(buf,&nodetype);
	if(nodetype != BT_BUCKET)
	{
	    SBT_PREV(buf,prev);
	    page->dirty = BT_DIRTY;
	}
    }

    pageT->dirty = BT_CLEAN;
    embBtreeWriteNode(cache,pageA,kAarray,pAarray,nAkeys);
    embBtreeWriteNode(cache,pageN,kNarray,pNarray,nNkeys);

    if(!anchorNode)
	pageA->dirty = BT_LOCK;

    
    for(i=0;i<order;++i)
    {
	ajStrDel(&kTarray[i]);
	ajStrDel(&kAarray[i]);
	ajStrDel(&kNarray[i]);
    }
    AJFREE(kTarray);
    AJFREE(kAarray);
    AJFREE(kNarray);
    AJFREE(pTarray);
    AJFREE(pAarray);
    AJFREE(pNarray);

    if(collapse)
	embBtreeCollapseRoot(cache,mergeNode);

    return thisNode;
}




/* @funcstatic embBtreeFindMin ***********************************************
**
** Find minimum key in subtree and store in cache
**
** @param [rw] cache [EmbPBtcache] cache
** @param [r] pageno [ajlong] page
** @param [r] key [char *] key
**
** @return [void]
** @@
******************************************************************************/
static void embBtreeFindMin(EmbPBtcache cache, ajlong pageno, char *key)
{
    unsigned char *buf = NULL;
    EmbPBtpage page    = NULL;
    EmbPBucket bucket  = NULL;
    AjPStr *karray     = NULL;
    ajlong *parray     = NULL;

    ajint nkeys        = 0;
    ajint nodetype     = 0;
    ajint order        = 0;
    ajint nentries     = 0;
    ajint i;

/*    ajDebug("In embBtreeFindMin\n"); */

    order = cache->order;
    AJCNEW0(karray,order);
    AJCNEW0(parray,order);
    for(i=0;i<order;++i)
	karray[i] = ajStrNew();
    
    page = embBtreeCacheRead(cache,pageno);
    buf  = page->buf;
    GBT_NODETYPE(buf,&nodetype);
    GBT_NKEYS(buf,&nkeys);

    embBtreeGetKeys(cache,buf,&karray,&parray);

    if(nodetype == BT_LEAF)
    {
	bucket = embBtreeReadBucket(cache,parray[0]);
	nentries = bucket->Nentries;
	if(nentries<2)
	{
	    embBtreeBucketDel(&bucket);
	    bucket = embBtreeReadBucket(cache,parray[1]);
	    nentries = bucket->Nentries;
	}
	if(nentries<1)	
	    ajFatal("FindMin: Too few entries in bucket Nkeys=%d\n",nkeys);
	ajStrAssS(&cache->replace,bucket->Ids[0]->id);
	if(!strcmp(cache->replace->Ptr,key))
	    ajStrAssS(&cache->replace,bucket->Ids[1]->id);
	for(i=1;i<nentries;++i)
	    if(strcmp(bucket->Ids[i]->id->Ptr,cache->replace->Ptr)<0 &&
	       strcmp(bucket->Ids[i]->id->Ptr,key))
		ajStrAssS(&cache->replace,bucket->Ids[i]->id);
	embBtreeBucketDel(&bucket);
    }
    else
    {
	pageno = parray[0];
	(void) embBtreeFindMin(cache,pageno,key);
	
    }
    

    for(i=0;i<order;++i)
	ajStrDel(&karray[i]);
    AJFREE(karray);
    AJFREE(parray);

    return;
}




/* @funcstatic embBtreeInsertShift ********************************************
**
** Rebalance buckets on insertion
**
** @param [rw] cache [EmbPBtcache] cache
** @param [r] tpage [EmbPBtpage] page
** @param [r] key [char *] key
**
** @return [ajlong] bucket block or 0L if shift not posible 
** @@
******************************************************************************/
static ajlong embBtreeInsertShift(EmbPBtcache cache, EmbPBtpage tpage,
				  char *key)
{
    unsigned char *tbuf = NULL;
    unsigned char *pbuf = NULL;
    unsigned char *sbuf = NULL;
    EmbPBtpage ppage    = NULL;
    EmbPBtpage spage    = NULL;
    ajint tkeys         = 0;
    ajint pkeys         = 0;
    ajint skeys         = 0;
    ajint order         = 0;
    
    ajint i;
    ajint n;
    
    ajlong parent  = 0L;
    ajlong blockno = 0L;
    
    AjPStr *kTarray = NULL;
    AjPStr *kParray = NULL;
    AjPStr *kSarray = NULL;
    ajlong *pTarray = NULL;
    ajlong *pParray = NULL;
    ajlong *pSarray = NULL;

    AjPStr *karray = NULL;
    ajlong *parray = NULL;

    ajint ppos    = 0;
    ajint pkeypos = 0;
    ajint minsize = 0;
    
/*    ajDebug("In embBtreeInsertShift\n"); */

    tbuf = tpage->buf;

    GBT_PREV(tbuf,&parent);
    GBT_NKEYS(tbuf,&tkeys);

    order = cache->order;
    minsize = order / 2;
    if(order % 2)
	++minsize;

    if(tkeys <= minsize)
	return 0L;

    
    ppage = embBtreeCacheRead(cache,parent);
    pbuf = ppage->buf;
    GBT_NKEYS(pbuf,&pkeys);
    
    


    AJCNEW0(kParray,order);
    AJCNEW0(pParray,order);
    AJCNEW0(kSarray,order);
    AJCNEW0(pSarray,order);
    AJCNEW0(kTarray,order);
    AJCNEW0(pTarray,order);
    
    for(i=0;i<order;++i)
    {
	kSarray[i] = ajStrNew();
	kParray[i] = ajStrNew();
	kTarray[i] = ajStrNew();
    }

    embBtreeGetKeys(cache,pbuf,&kParray,&pParray);
    i=0;
    while(i!=pkeys && strcmp(key,kParray[i]->Ptr)>=0)
	++i;
    pkeypos = i;
    
    if(i==pkeys)
    {
	if(strcmp(key,kParray[i-1]->Ptr)<0)
	    ppos = i-1;
	else
	    ppos = i;
    }
    else
	ppos = i;

    
    if(ppos) /* There is another leaf to the left */
    {
	spage = embBtreeCacheRead(cache,pParray[ppos-1]);
	sbuf = spage->buf;
	GBT_NKEYS(sbuf,&skeys);
    }

    if(i && skeys != order-1) /* There is space in the left leaf */
    {
	embBtreeGetKeys(cache,tbuf,&kTarray,&pTarray);
	if(skeys)
	    embBtreeGetKeys(cache,sbuf,&kSarray,&pSarray);

	while(i!=pkeys && strcmp(kParray[i]->Ptr,kTarray[0]->Ptr)<0)
	++i;
	if(i==pkeys)
	    --i;

	pkeypos = i;

	ajStrAssS(&kSarray[skeys],kParray[pkeypos]);
	pSarray[skeys+1] = pTarray[0];
	++skeys;
	--tkeys;
	ajStrAssS(&kParray[pkeypos],kTarray[0]);
	for(i=0;i<tkeys;++i)
	{
	    ajStrAssS(&kTarray[i],kTarray[i+1]);
	    pTarray[i] = pTarray[i+1];
	}
	pTarray[i] = pTarray[i+1];
	pTarray[i+1] = 0L;
	
	embBtreeWriteNode(cache,spage,kSarray,pSarray,skeys);
	embBtreeWriteNode(cache,tpage,kTarray,pTarray,tkeys);
	embBtreeWriteNode(cache,ppage,kParray,pParray,pkeys);
	if(!ppage->pageno)
	    ppage->dirty = BT_LOCK;

	i = 0;
	while(i!=pkeys && strcmp(key,kParray[i]->Ptr)>=0)
	    ++i;
	if(i==pkeys)
	{
	    if(strcmp(key,kParray[i-1]->Ptr)<0)
		blockno = pParray[i-1];
	    else
		blockno = pParray[i];
	}
	else
	    blockno = pParray[i];

	if(blockno == spage->pageno)
	{
	    karray = kSarray;
	    parray = pSarray;
	    n = skeys;
	}
	else
	{
	    karray = kTarray;
	    parray = pTarray;
	    n = tkeys;
	}
	

	i = 0;
	while(i!=n && strcmp(key,karray[i]->Ptr)>=0)
	    ++i;
	if(i==n)
	{
	    if(strcmp(key,karray[i-1]->Ptr)<0)
		blockno = parray[i-1];
	    else
		blockno = parray[i];
	}
	else
	    blockno = parray[i];

	for(i=0;i<order;++i)
	{
	    ajStrDel(&kTarray[i]);
	    ajStrDel(&kParray[i]);
	    ajStrDel(&kSarray[i]);
	}
	AJFREE(kTarray);
	AJFREE(kSarray);
	AJFREE(kParray);
	AJFREE(pTarray);
	AJFREE(pSarray);
	AJFREE(pParray);

	return blockno;
    }
    

    if(ppos != pkeys)	/* There is a right node */
    {
	spage = embBtreeCacheRead(cache,pParray[ppos+1]);
	sbuf = spage->buf;
	GBT_NKEYS(sbuf,&skeys);
    }


    if(ppos != pkeys && skeys != order-1) /* Space in the right leaf */
    {
	embBtreeGetKeys(cache,tbuf,&kTarray,&pTarray);
	embBtreeGetKeys(cache,sbuf,&kSarray,&pSarray);

	while(i!=pkeys && strcmp(kTarray[tkeys-1]->Ptr,kParray[i]->Ptr)>=0)
	++i;
	pkeypos = i;


	pSarray[skeys+1] = pSarray[skeys];
	for(i=skeys-1;i>-1;--i)
	{
	    ajStrAssS(&kSarray[i+1],kSarray[i]);
	    pSarray[i+1] = pSarray[i];
	}
	ajStrAssS(&kSarray[0],kParray[pkeypos]);
	pSarray[0] = pTarray[tkeys];
	ajStrAssS(&kParray[pkeypos],kTarray[tkeys-1]);
	++skeys;
	--tkeys;
	pTarray[tkeys+1] = 0L;
	
	embBtreeWriteNode(cache,spage,kSarray,pSarray,skeys);
	embBtreeWriteNode(cache,tpage,kTarray,pTarray,tkeys);
	embBtreeWriteNode(cache,ppage,kParray,pParray,pkeys);
	if(!ppage->pageno)
	    ppage->dirty = BT_LOCK;

	i = 0;
	while(i!=pkeys && strcmp(key,kParray[i]->Ptr)>=0)
	    ++i;
	if(i==pkeys)
	{
	    if(strcmp(key,kParray[i-1]->Ptr)<0)
		blockno = pParray[i-1];
	    else
		blockno = pParray[i];
	}
	else
	    blockno = pParray[i];

	if(blockno == spage->pageno)
	{
	    karray = kSarray;
	    parray = pSarray;
	    n = skeys;
	}
	else
	{
	    karray = kTarray;
	    parray = pTarray;
	    n = tkeys;
	}
	
	i = 0;
	while(i!=n && strcmp(key,karray[i]->Ptr)>=0)
	    ++i;
	if(i==n)
	{
	    if(strcmp(key,karray[i-1]->Ptr)<0)
		blockno = parray[i-1];
	    else
		blockno = parray[i];
	}
	else
	    blockno = parray[i];

	for(i=0;i<order;++i)
	{
	    ajStrDel(&kTarray[i]);
	    ajStrDel(&kParray[i]);
	    ajStrDel(&kSarray[i]);
	}
	AJFREE(kTarray);
	AJFREE(kSarray);
	AJFREE(kParray);
	AJFREE(pTarray);
	AJFREE(pSarray);
	AJFREE(pParray);

	return blockno;
    }


    for(i=0;i<order;++i)
    {
	ajStrDel(&kTarray[i]);
	ajStrDel(&kParray[i]);
	ajStrDel(&kSarray[i]);
    }
    AJFREE(kTarray);
    AJFREE(kSarray);
    AJFREE(kParray);
    AJFREE(pTarray);
    AJFREE(pSarray);
    AJFREE(pParray);

    return 0L;
}




/* @funcstatic embBtreeKeyShift ********************************************
**
** Rebalance Nodes on insertion
**
** @param [rw] cache [EmbPBtcache] cache
** @param [r] tpage [EmbPBtpage] page
**
** @return [void]
** @@
******************************************************************************/
static void embBtreeKeyShift(EmbPBtcache cache, EmbPBtpage tpage)
{
    unsigned char *tbuf = NULL;
    unsigned char *pbuf = NULL;
    unsigned char *sbuf = NULL;
    unsigned char *buf  = NULL;
    EmbPBtpage ppage    = NULL;
    EmbPBtpage spage    = NULL;
    EmbPBtpage page     = NULL;
    ajint tkeys         = 0;
    ajint pkeys         = 0;
    ajint skeys         = 0;
    ajint order         = 0;
    
    ajint i;
    
    ajlong parent  = 0L;
    
    AjPStr *kTarray = NULL;
    AjPStr *kParray = NULL;
    AjPStr *kSarray = NULL;
    ajlong *pTarray = NULL;
    ajlong *pParray = NULL;
    ajlong *pSarray = NULL;

    ajint pkeypos = 0;
    ajint minsize = 0;

/*    ajDebug("In embBtreeKeyShift\n"); */
    
    tbuf = tpage->buf;

    GBT_PREV(tbuf,&parent);
    GBT_NKEYS(tbuf,&tkeys);

    order = cache->order;
    minsize = order / 2;
    if(order % 2)
	++minsize;

    if(tkeys <= minsize)
	return;

    
    ppage = embBtreeCacheRead(cache,parent);
    pbuf = ppage->buf;
    GBT_NKEYS(pbuf,&pkeys);
    
    


    AJCNEW0(kParray,order);
    AJCNEW0(pParray,order);
    AJCNEW0(kSarray,order);
    AJCNEW0(pSarray,order);
    AJCNEW0(kTarray,order);
    AJCNEW0(pTarray,order);
    
    for(i=0;i<order;++i)
    {
	kSarray[i] = ajStrNew();
	kParray[i] = ajStrNew();
	kTarray[i] = ajStrNew();
    }

    embBtreeGetKeys(cache,tbuf,&kTarray,&pTarray);
    GBT_NKEYS(tbuf,&tkeys);


    embBtreeGetKeys(cache,pbuf,&kParray,&pParray);
    i=0;
    while(i!=pkeys && strcmp(kParray[i]->Ptr,kTarray[0]->Ptr)<0)
	++i;
    if(i)
	pkeypos = i-1;
    
    if(pkeypos) /* There is another leaf to the left */
    {
	spage = embBtreeCacheRead(cache,pParray[pkeypos]);
	sbuf = spage->buf;
	GBT_NKEYS(sbuf,&skeys);
    }

    if(pkeypos && skeys != order-1) /* There is space in the left leaf */
    {
	if(skeys)
	    embBtreeGetKeys(cache,sbuf,&kSarray,&pSarray);

	ajStrAssS(&kSarray[skeys],kParray[pkeypos]);
	pSarray[skeys+1] = pTarray[0];
	++skeys;
	--tkeys;
	ajStrAssS(&kParray[pkeypos],kTarray[0]);
	for(i=0;i<tkeys;++i)
	{
	    ajStrAssS(&kTarray[i],kTarray[i+1]);
	    pTarray[i] = pTarray[i+1];
	}
	pTarray[i] = pTarray[i+1];
	pTarray[i+1] = 0L;
	
	embBtreeWriteNode(cache,spage,kSarray,pSarray,skeys);
	embBtreeWriteNode(cache,tpage,kTarray,pTarray,tkeys);
	embBtreeWriteNode(cache,ppage,kParray,pParray,pkeys);
	if(!ppage->pageno)
	    ppage->dirty = BT_LOCK;

	page = embBtreeCacheRead(cache,pSarray[skeys]);
	buf = page->buf;
	SBT_PREV(buf,spage->pageno);
	page->dirty = BT_DIRTY;


	for(i=0;i<order;++i)
	{
	    ajStrDel(&kTarray[i]);
	    ajStrDel(&kParray[i]);
	    ajStrDel(&kSarray[i]);
	}
	AJFREE(kTarray);
	AJFREE(kSarray);
	AJFREE(kParray);
	AJFREE(pTarray);
	AJFREE(pSarray);
	AJFREE(pParray);

	return;
    }

    if(pkeypos != pkeys-1)	/* There is a right node */
    {
	spage = embBtreeCacheRead(cache,pParray[pkeypos+1]);
	sbuf = spage->buf;
	GBT_NKEYS(sbuf,&skeys);
    }


    if(pkeypos != pkeys-1 && skeys != order-1) /* Space in the right node */
    {
	if(skeys)
	    embBtreeGetKeys(cache,sbuf,&kSarray,&pSarray);

	pSarray[skeys+1] = pSarray[skeys];
	for(i=skeys-1;i>-1;--i)
	{
	    ajStrAssS(&kSarray[i+1],kSarray[i]);
	    pSarray[i+1] = pSarray[i];
	}
	ajStrAssS(&kSarray[0],kParray[pkeypos]);
	pSarray[0] = pTarray[tkeys];
	ajStrAssS(&kParray[pkeypos],kTarray[tkeys-1]);
	++skeys;
	--tkeys;
	pTarray[tkeys+1] = 0L;
	
	embBtreeWriteNode(cache,spage,kSarray,pSarray,skeys);
	embBtreeWriteNode(cache,tpage,kTarray,pTarray,tkeys);
	embBtreeWriteNode(cache,ppage,kParray,pParray,pkeys);
	if(!ppage->pageno)
	    ppage->dirty = BT_LOCK;

	page = embBtreeCacheRead(cache,pSarray[0]);
	buf = page->buf;
	SBT_PREV(buf,spage->pageno);
	page->dirty = BT_DIRTY;

	for(i=0;i<order;++i)
	{
	    ajStrDel(&kTarray[i]);
	    ajStrDel(&kParray[i]);
	    ajStrDel(&kSarray[i]);
	}
	AJFREE(kTarray);
	AJFREE(kSarray);
	AJFREE(kParray);
	AJFREE(pTarray);
	AJFREE(pSarray);
	AJFREE(pParray);

	return;
    }


    for(i=0;i<order;++i)
    {
	ajStrDel(&kTarray[i]);
	ajStrDel(&kParray[i]);
	ajStrDel(&kSarray[i]);
    }
    AJFREE(kTarray);
    AJFREE(kSarray);
    AJFREE(kParray);
    AJFREE(pTarray);
    AJFREE(pSarray);
    AJFREE(pParray);

    return;
}




/* @funcstatic embBtreeTraverseLeaves *****************************************
**
** Find the next leaf by traversing the tree
**
** @param [rw] cache [EmbPBtcache] cache
** @param [r] thys [EmbPBtpage] current leaf page
**
** @return [EmbPBtpage] next leaf or NULL
** @@
******************************************************************************/
static EmbPBtpage embBtreeTraverseLeaves(EmbPBtcache cache, EmbPBtpage thys)
{
    unsigned char *buf = NULL;
    EmbPBtpage page    = NULL;
    
    ajlong pageno      = 0L;
    ajlong prev        = 0L;
    
    AjPStr *karray     = NULL;
    ajlong *parray     = NULL;

    ajint nodetype = 0;
    ajint nkeys    = 0;
    ajint apos     = 0;
    ajint order    = 0;
    ajint i;
    

    if(!cache->level)
	return NULL;

    order = cache->order;
    AJCNEW0(karray,order);
    AJCNEW0(parray,order);
    for(i=0;i<order;++i)
	karray[i] = ajStrNew();

    pageno = thys->pageno;
    buf = thys->buf;
    GBT_PREV(buf,&prev);

    page = embBtreeCacheRead(cache,prev);
    buf = page->buf;
    GBT_NKEYS(buf,&nkeys);
    GBT_NODETYPE(buf,&nodetype);
    embBtreeGetKeys(cache,buf,&karray,&parray);
    apos = 0;
    while(parray[apos] != pageno)
	++apos;

    while(apos == nkeys)
    {
	if(nodetype == BT_ROOT)
	{
	    for(i=0;i<order;++i)
		ajStrDel(&karray[i]);
	    AJFREE(karray);
	    AJFREE(parray);
	    return NULL;
	}

	GBT_PREV(buf,&prev);
	pageno = page->pageno;
	page = embBtreeCacheRead(cache,prev);
	buf = page->buf;
	GBT_NKEYS(buf,&nkeys);
	GBT_NODETYPE(buf,&nodetype);
	embBtreeGetKeys(cache,buf,&karray,&parray);
	apos = 0;
	while(parray[apos] != pageno)
	    ++apos;
    }

    page = embBtreeCacheRead(cache,parray[apos+1]);
    buf = page->buf;
    GBT_NODETYPE(buf,&nodetype);
    embBtreeGetKeys(cache,buf,&karray,&parray);
    
    while(nodetype != BT_LEAF)
    {
	page = embBtreeCacheRead(cache,parray[0]);
	buf = page->buf;
	embBtreeGetKeys(cache,buf,&karray,&parray);
	GBT_NODETYPE(buf,&nodetype);
    }

    for(i=0;i<order;++i)
	ajStrDel(&karray[i]);
    AJFREE(karray);
    AJFREE(parray);

    return page;
}




/* @func embBtreeJoinLeaves **************************************************
**
** Update all Left/Right Leaf Pointers
**
** @param [rw] cache [EmbPBtcache] cache
**
** @return [void] next leaf or NULL
** @@
******************************************************************************/
void embBtreeJoinLeaves(EmbPBtcache cache)
{
    unsigned char *buf = NULL;
    EmbPBtpage page    = NULL;
    EmbPBtpage newpage = NULL;
    
    ajint nodetype = 0;
    ajint order    = 0;
    ajint i;

    AjPStr *karray = NULL;
    ajlong *parray = NULL;

    ajlong left    = 0L;
    ajlong right   = 0L;
    
    if(!cache->level)
	return;

    order = cache->order;

    AJCNEW0(karray,order);
    AJCNEW0(parray,order);
    for(i=0;i<order;++i)
	karray[i] = ajStrNew();

    page = embBtreeCacheLocate(cache,0L);
    buf = page->buf;
    embBtreeGetKeys(cache,buf,&karray,&parray);
    GBT_NODETYPE(buf,&nodetype);

    while(nodetype != BT_LEAF)
    {
	page = embBtreeCacheRead(cache,parray[0]);
	buf = page->buf;
	embBtreeGetKeys(cache,buf,&karray,&parray);
	GBT_NODETYPE(buf,&nodetype);
    }
    
    SBT_LEFT(buf,left);
    
    while((newpage = embBtreeTraverseLeaves(cache,page)))
    {
	right = newpage->pageno;
	SBT_RIGHT(buf,right);
	page->dirty = BT_DIRTY;
	left = page->pageno;
	buf = newpage->buf;
	SBT_LEFT(buf,left);
	page = newpage;
    }

    right = 0L;
    SBT_RIGHT(buf,right);
    page->dirty = BT_DIRTY;

    for(i=0;i<order;++i)
	ajStrDel(&karray[i]);
    AJFREE(karray);
    AJFREE(parray);

    return;
}




/* @func embBtreeWildNew *********************************************
**
** Construct a wildcard search object
**
** @param [rw] cache [EmbPBtcache] cache
** @param [r] wild [AjPStr] wildcard id prefix (without asterisk)
**
** @return [EmbPBtWild] b+ tree wildcard object
** @@
******************************************************************************/

EmbPBtWild embBtreeWildNew(EmbPBtcache cache, AjPStr wild)
{
    EmbPBtWild thys = NULL;
    
    AJNEW0(thys);

    thys->id   = ajStrNewC(wild->Ptr);
    thys->list = ajListNew();

    thys->first = ajTrue;

    return thys;
}




/* @func embBtreeWildDel *********************************************
**
** Destroy a wildcard search object
**
** @param [rw] thys [EmbPBtWild*] b+ tree wildcard structure
**
** @return [void]
** @@
******************************************************************************/

void embBtreeWildDel(EmbPBtWild *thys)
{
    EmbPBtWild pthis = NULL;
    EmbPBtId   id    = NULL;
    
    pthis = *thys;
    if(!thys || !pthis)
	return;

    ajStrDel(&pthis->id);

    while(ajListPop(pthis->list,(void **)&id))
	embBtreeIdDel(&id);

    ajListDel(&pthis->list);

    *thys = NULL;
    AJFREE(pthis);

    return;
}




/* @func embBtreeFindInsertW ***********************************************
**
** Find the node that should contain a key (wild)
**
** @param [rw] cache [EmbPBtcache] cache
** @param [r] item [char *] key to search for 
**
** @return [EmbPBtpage] leaf node where item should be inserted
** @@
******************************************************************************/
EmbPBtpage embBtreeFindInsertW(EmbPBtcache cache, char *key)
{
    EmbPBtpage root = NULL;
    EmbPBtpage ret  = NULL;

/*    ajDebug("In embBtreeFindInsertW\n"); */

    /* The root node should always be in the cache (BT_LOCKed) */
    root = embBtreeCacheLocate(cache,0L);
    
    if(!cache->level)
	return root;
    
    ret = embBtreeFindINodeW(cache,root,key);

    return ret;
}




/* @func embBtreeFindINodeW ***********************************************
**
** Recursive search for node (wild)
**
** @param [rw] cache [EmbPBtcache] cache
** @param [rw] page [EmbPBtpage] page
** @param [r] item [char *] key to search for 
**
** @return [EmbPBtpage] leaf node where item should be inserted
** @@
******************************************************************************/
static EmbPBtpage embBtreeFindINodeW(EmbPBtcache cache, EmbPBtpage page,
				     char *item)
{
    EmbPBtpage     ret = NULL;
    EmbPBtpage     pg  = NULL;

    unsigned char *buf = NULL;
    ajint status       = 0;
    ajint ival         = 0;

/*    ajDebug("In embBtreeFindINodeW\n"); */
    
    ret = page;
    buf = page->buf;
    GBT_NODETYPE(buf,&ival);
    if(ival != BT_LEAF)
    {
	status = ret->dirty;
	ret->dirty = BT_LOCK;	/* Lock in case of lots of overflow pages */
	pg = embBtreePageFromKeyW(cache,buf,item);
	ret->dirty = status;
	ret = embBtreeFindINodeW(cache,pg,item);
    }
    
    return ret;
}




/* @funcstatic embBtreePageFromKeyW *******************************************
**
** Return next lower index page given a key (wild)
**
** @param [r] buf [unsigned char *] page buffer
** @param [r] key [char *] key to search for 
**
** @return [EmbPBtree] pointer to a page
** @@
******************************************************************************/

static EmbPBtpage embBtreePageFromKeyW(EmbPBtcache cache, unsigned char *buf,
				       char *key)
{
    unsigned char *rootbuf = NULL;
    ajint nkeys    = 0;
    ajint keylen   = 0;
    ajint order    = 0;
    ajint i;
    
    ajlong blockno  = 0L;
    AjPStr *karray  = NULL;
    ajlong *parray  = NULL;
    EmbPBtpage page = NULL;
    
/*    ajDebug("In embBtreePageFromKeyW\n"); */
    
    rootbuf = buf;


    GBT_NKEYS(rootbuf,&nkeys);
    order = cache->order;

    AJCNEW0(karray,order);
    AJCNEW0(parray,order);
    for(i=0;i<order;++i)
	karray[i] = ajStrNew();

    keylen = strlen(key);

    embBtreeGetKeys(cache,rootbuf,&karray,&parray);
    i = 0;
    while(i!=nkeys && strncmp(key,karray[i]->Ptr,keylen)>0)
	++i;
    if(i==nkeys)
    {
	if(strncmp(key,karray[i-1]->Ptr,keylen)<=0)
	    blockno = parray[i-1];
	else
	    blockno = parray[i];
    }
    else
	blockno = parray[i];

    for(i=0;i<order;++i)
	ajStrDel(&karray[i]);
    AJFREE(karray);
    AJFREE(parray);

    page =  embBtreeCacheRead(cache,blockno);

    return page;
}




/* @funcstatic embBtreeReadLeaf ***********************************************
**
** Read all leaf Ids into a list
**
** @param [rw] cache [EmbPBtcache] cache
** @param [rw] page [EmbPBtpage] page
** @param [w] list [AjPList] list
**
** @return [void]
** @@
******************************************************************************/

static void embBtreeReadLeaf(EmbPBtcache cache, EmbPBtpage page,
			     AjPList list)
{
    unsigned char *buf = NULL;
    AjPStr *karray     = NULL;
    ajlong *parray     = NULL;

    ajint keylimit = 0;
    ajint order    = 0;
    ajint nkeys    = 0;
    ajint bentries =0;
    
    ajint i;
    ajint j;
    
    EmbPBucket *buckets = NULL;
    
    buf = page->buf;
    order = cache->order;

    AJCNEW0(parray,order);
    AJCNEW0(karray,order);
    for(i=0;i<order;++i)
	karray[i] = ajStrNew();

    embBtreeGetKeys(cache,buf,&karray,&parray);

    GBT_NKEYS(buf,&nkeys);
    
    keylimit = nkeys+1;
    AJCNEW0(buckets,keylimit);

    for(i=0;i<keylimit;++i)
	buckets[i] = embBtreeReadBucket(cache,parray[i]);
    
    for(i=0;i<keylimit;++i)
    {
	bentries = buckets[i]->Nentries;
	for(j=0;j<bentries;++j)
	    ajListPush(list,(void *)buckets[i]->Ids[j]);
	AJFREE(buckets[i]->keylen);
	AJFREE(buckets[i]->Ids);
	AJFREE(buckets[i]);
    }
    ajListSort(list,embBtIdCompare);
    AJFREE(buckets);

    for(i=0;i<order;++i)
	ajStrDel(&karray[i]);
    AJFREE(karray);
    AJFREE(parray);

    return;
}




/* @func embBtreeIdFromKeyW ********************************************
**
** Wildcard retrieval of entries
**
** @param [rw] cache [EmbPBtcache] cache
** @param [rw] page [EmbPBtpage] page
** @param [w] list [AjPList] list
**
** @return [EmbPBtId] next matching Id or NULL
** @@
******************************************************************************/

EmbPBtId embBtreeIdFromKeyW(EmbPBtcache cache, EmbPBtWild wild)
{
    unsigned char *buf = NULL;
    EmbPBtId id        = NULL;
    EmbPBtpage page    = NULL;
    char *key          = NULL;
    AjPList list       = NULL;
    AjBool found       = ajFalse;

    ajlong pageno = 0L;
    ajint keylen  = 0;
    

    key  = wild->id->Ptr;
    list = wild->list;

    keylen = strlen(key);

    found = ajFalse;
    
    if(wild->first)
    {
	page = embBtreeFindInsertW(cache,key);
	page->dirty = BT_LOCK;
	wild->pageno = page->pageno;
	
	embBtreeReadLeaf(cache,page,list);

	page->dirty = BT_CLEAN;
	
	if(!ajListLength(list))
	    return NULL;

	while(ajListPop(list,(void **)&id))
	{
	    if(!strncmp(id->id->Ptr,key,keylen))
	    {
		found = ajTrue;
		break;
	    }
	    embBtreeIdDel(&id);
	}

	wild->first = ajFalse;


	if(!found)	/* Check the next leaf just in case key==internal */
	{
	    buf = page->buf;
	    GBT_RIGHT(buf,&pageno);
	    if(!pageno)
		return NULL;
	    page = embBtreeCacheRead(cache,pageno);
	    wild->pageno = pageno;
	    page->dirty = BT_LOCK;
	    
	    embBtreeReadLeaf(cache,page,list);	
	    
	    page->dirty = BT_CLEAN;
	    
	    if(!ajListLength(list))
		return NULL;
	    
	    found = ajFalse;
	    while(ajListPop(list,(void **)&id))
	    {
		if(!strncmp(id->id->Ptr,key,keylen))
		{
		    found = ajTrue;
		    break;
		}
		embBtreeIdDel(&id);
	    }
	}


	if(!found)
	    return NULL;

	return id;
    }


    if(!ajListLength(list))
    {
	page = embBtreeCacheRead(cache,wild->pageno); 
	buf = page->buf;
	GBT_RIGHT(buf,&pageno);
	if(!pageno)
	    return NULL;
	page = embBtreeCacheRead(cache,pageno);
	wild->pageno = pageno;
	page->dirty = BT_LOCK;

	embBtreeReadLeaf(cache,page,list);	

	page->dirty = BT_CLEAN;
	
	if(!ajListLength(list))
	    return NULL;
    }
    


    while(ajListPop(list,(void **)&id))
    {
	if(!strncmp(id->id->Ptr,key,keylen))
	{
	    found = ajTrue;
	    break;
	}
	embBtreeIdDel(&id);
    }
    
    
    if(!found)
	return NULL;
    
    return id;
}




/* @func embBtreeReplaceId ************************************************
**
** Replace an ID structure in a leaf node given a key
**
** @param [rw] cache [EmbPBtcache] cache
** @param [r] rid [EmbPBtId] replacement id object
**
** @return [AjBool] true if success
** @@
******************************************************************************/

AjBool embBtreeReplaceId(EmbPBtcache cache, EmbPBtId rid)
{
    EmbPBtpage page   = NULL;
    EmbPBucket bucket = NULL;
    EmbPBtId   id     = NULL;
    char *key         = NULL;
    
    AjPStr *karray  = NULL;
    ajlong *parray  = NULL;
    
    unsigned char *buf = NULL;

    ajint nentries = 0;
    ajint nkeys    = 0;
    ajint order    = 0;
    
    ajint i;
    
    ajlong blockno = 0L;
    AjBool found   = ajFalse;


    key = rid->id->Ptr;

    page = embBtreeFindInsert(cache,key);
    buf = page->buf;
    order = cache->order;

    AJCNEW0(karray,order);
    AJCNEW0(parray,order);
    for(i=0;i<order;++i)
	karray[i] = ajStrNew();

    embBtreeGetKeys(cache,buf,&karray,&parray);

    GBT_NKEYS(buf,&nkeys);

    i=0;
    while(i!=nkeys && strcmp(key,karray[i]->Ptr)>=0)
	++i;
    
    if(i==nkeys)
    {
	if(strcmp(key,karray[i-1]->Ptr)<0)
	    blockno = parray[i-1];
	else
	    blockno = parray[i];
    }
    else
	blockno = parray[i];

    bucket = embBtreeReadBucket(cache,blockno);
    
    nentries = bucket->Nentries;

    found = ajFalse;

    for(i=0;i<nentries;++i)
    {
	if(!strcmp(key,bucket->Ids[i]->id->Ptr))
	{
	    found = ajTrue;
	    break;
	}
    }

    if(found)
    {
	id->dbno = rid->dbno;
	id->dups = rid->dups;
	id->offset = rid->offset;
	embBtreeWriteBucket(cache,bucket,blockno);
    }

    embBtreeBucketDel(&bucket);
    for(i=0;i<order;++i)
	ajStrDel(&karray[i]);
    AJFREE(karray);
    AJFREE(parray);

    if(!found)
	return ajFalse;

    return ajTrue;
}
