#ifdef __cplusplus
extern "C"
{
#endif

#ifndef enscache_h
#define enscache_h

#include "ajax.h"
#include "enstable.h"




/*******************************************************************************
**
** Ensembl Cache Type enumeration.
**
******************************************************************************/

enum EnsECacheType
{
    ensECacheTypeNULL,
    ensECacheTypeNumeric,
    ensECacheTypeAlphaNumeric
};




/* @data EnsPCache ************************************************************
**
** Ensembl Cache.
**
** @alias EnsSCache
** @alias EnsOCache
**
** @attr List [AjPList] AJAX List implementing LRU functionality
** @attr Table [AjPTable] AJAX Table implementing lookup functionality
** @attr Label [AjPStr] Cache label for statistics output
** @attr Reference [(void**)] Object-specific referencing function
** @attr Delete [(void*)] Object-specific deletion function
** @attr Size [(ajuint*)] Object-specific memory sizing function
** @attr Read [(void**)] Object-specific reading function
** @attr Write [(AjBool*)] Object-specific writing function
** @attr Type [AjEnum] Ensembl Cache type (EnsECacheNumeric, ...)
** @attr Syncron [AjBool] ajTrue: Immediately write-back value data
**                       ajFalse: Write-back value data later
** @cc Cache limits
** @attr MaxBytes [ajuint] Maximum number of allowed bytes
** @attr MaxCount [ajuint] Maximum number of allowed entries
** @attr MaxSize [ajuint] Maximum memory size of an object
** @cc Cache size
** @attr Bytes [ajuint] Current number of cached bytes
** @attr Count [ajuint] Current number of cached entry
** @cc Cache performance statistics
** @attr Dropped [ajuint] Number of entries dropped by the LRU algorithm
** @attr Removed [ajuint] Number of entries explicitly removed
** @attr Stored [ajuint] Number of entries currently stored
** @attr Hit [ajuint] Number of cache hits
** @attr Miss [ajuint] Number of cache misses
** @@
******************************************************************************/

typedef struct EnsSCache
{
    AjPList List;
    AjPTable Table;
    AjPStr Label;
    void* (*Reference)(void *);
    void (*Delete)(void **);
    ajuint (*Size)(const void *);
    void* (*Read)(const void *key);
    AjBool (*Write)(const void *);
    AjEnum Type;
    AjBool Syncron;
    ajuint MaxBytes;
    ajuint MaxCount;
    ajuint MaxSize;
    ajuint Bytes;
    ajuint Count;
    ajuint Dropped;
    ajuint Removed;
    ajuint Stored;
    ajuint Hit;
    ajuint Miss;
} EnsOCache;

#define EnsPCache EnsOCache*




/*
** Prototype definitions
*/

/* Ensembl Cache */

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
                      const char *label);

void ensCacheDel(EnsPCache* Pcache);

void *ensCacheFetch(EnsPCache cache, void *key);

AjBool ensCacheStore(EnsPCache cache, void* key, void** value);

AjBool ensCacheRemove(EnsPCache cache, const void* key);

AjBool ensCacheSyncronise(EnsPCache cache);

void ensCacheTrace(const EnsPCache cache);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
