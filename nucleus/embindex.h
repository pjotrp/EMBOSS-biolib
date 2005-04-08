#ifdef __cplusplus
extern "C"
{
#endif

#ifndef embindex_h
#define embindex_h

#define BTREE_DEF_ORDER     20
#define BTREE_DEF_FILL      15
#define BTREE_DEF_SECORDER  20
#define BTREE_DEF_SECFILL   20
#define BTREE_DEF_KWLIMIT   15
#define BTREE_DEF_CACHESIZE 100
#define BTREE_DEF_PAGESIZE  0

typedef struct EmbSBtreeEntry
{
    AjBool do_id;
    AjBool do_accession;
    AjBool do_sv;
    AjBool do_description;
    AjBool do_keyword;
    AjBool do_taxonomy;
    AjPStr dbname;
    AjPStr dbrs;
    AjPStr release;
    AjPStr date;
    AjPStr dbtype;

    AjPStr directory;
    AjPStr idirectory;

    AjPList files;
    ajint   nfiles;

    ajint order;
    ajint fill;
    ajint secorder;
    ajint secfill;
    ajint kwlimit;
    ajint cachesize;
    ajint pagesize;

    AjPBtcache idcache;
    AjPBtcache accache;
    AjPBtcache svcache;
    AjPBtcache kwcache;
    AjPBtcache decache;
    AjPBtcache txcache;

    ajlong fpos;

    AjPStr id;
    AjPList ac;
    AjPList sv;
    AjPList tx;
    AjPList kw;
    AjPList de;
} EmbOBtreeEntry;
#define EmbPBtreeEntry EmbOBtreeEntry*
 



void   embBtreeEmblAC(const AjPStr acline, AjPList aclist);
void   embBtreeEmblKW(const AjPStr kwline, AjPList kwlist, ajint maxlen);
void   embBtreeEmblDE(const AjPStr deline, AjPList delist, ajint maxlen);
void   embBtreeGenBankAC(const AjPStr acline, AjPList aclist);
void   embBtreeGenBankKW(const AjPStr kwline, AjPList kwlist, ajint maxlen);
void   embBtreeGenBankDE(const AjPStr kwline, AjPList kwlist, ajint maxlen);
void   embBtreeGenBankTX(const AjPStr kwline, AjPList kwlist, ajint maxlen);

void   embBtreeFastaDE(const AjPStr kwline, AjPList kwlist, ajint maxlen);


ajint  embBtreeReadDir(AjPStr **filelist, const AjPStr fdirectory,
		       const AjPStr files, const AjPStr exclude);
AjBool embBtreeWriteFileList(const AjPStr *filelist, ajint nfiles,
			     const AjPStr fdirectory, const AjPStr idirectory,
			     const AjPStr dbname);
EmbPBtreeEntry embBtreeEntryNew(void);
ajint          embBtreeSetFields(EmbPBtreeEntry entry, AjPStr* fields);
void           embBtreeEntryDel(EmbPBtreeEntry *thys);
void           embBtreeSetDbInfo(EmbPBtreeEntry entry, const AjPStr name,
				 const AjPStr dbrs,
		                 const AjPStr date, const AjPStr release,
		                 const AjPStr type, const AjPStr directory,
		                 const AjPStr idirectory);
ajint          embBtreeGetFiles(EmbPBtreeEntry entry, const AjPStr fdirectory,
				const AjPStr files, const AjPStr exclude);
AjBool         embBtreeWriteEntryFile(const EmbPBtreeEntry entry);
void           embBtreeGetRsInfo(EmbPBtreeEntry entry);
AjBool         embBtreeOpenCaches(EmbPBtreeEntry entry);
AjBool         embBtreeCloseCaches(EmbPBtreeEntry entry);
AjBool         embBtreeDumpParameters(EmbPBtreeEntry entry);


#endif

#ifdef __cplusplus
}
#endif
