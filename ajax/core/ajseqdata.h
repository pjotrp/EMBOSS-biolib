#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ajseqdata_h
#define ajseqdata_h

#define NULLFPOS -1

enum AjEQryType {QRY_UNKNOWN, QRY_ENTRY, QRY_QUERY, QRY_ALL};

typedef struct SeqSAccess SeqSAccess;

/* @data AjPSeqQuery **********************************************************
**
** Ajax Sequence Query object.
**
** Holds data needed to interpret the entry specification part of a USA.
** This can refer to an entry name (or "id"), and accession number or
** other queriable items.
**
** ajpseqquery is created with the entry specification part of a USA
** (Uniform Sequence Address). The syntax is currently related to that
** used by SRS release 5.1.
**
** @new ajSeqQueryNew Default constructor
** @delete ajSeqQueryDel Default destructor
** @set ajSeqQueryClear Clears all contents
** @set seqQueryMatch Compares an AjPSeq to a query.
** @set ajSeqQueryStarclear Clears fully wild elements of a query because
**                          empty elements are the same.
** @cast seqQueryWild Tests whether a query includes wildcards
** @cast seqQueryIs Tests whether a query has been defined
**
** @attr DbName [AjPStr] Database name
** @attr DbType [AjPStr] Database type
** @attr Id [AjPStr] ID Wildcard
** @attr Acc [AjPStr] Accession Wildcard
** @attr Des [AjPStr] Description Wildcard
** @attr Key [AjPStr] Keyword Wildcard
** @attr Org [AjPStr] Taxonomy Wildcard
** @attr Sv [AjPStr] SeqVersion Wildcard
** @attr Method [AjPStr] Name of access method
** @attr Formatstr [AjPStr] Name of input sequence format
** @attr IndexDir [AjPStr] Index directory
** @attr Directory [AjPStr] Data directory
** @attr Filename [AjPStr] Individual filename
** @attr Exclude [AjPStr] File wildcards to exclude (spaced)
** @attr DbFields [AjPStr] Query fields (plus id and acc)
** @attr DbProxy [AjPStr] Proxy host
** @attr DbHttpVer [AjPStr] HTTP Version
** @attr Field [AjPStr] Query field
** @attr QryString [AjPStr] Query term
** @attr Application [AjPStr] External application command
** @attr Type [enum AjEQryType] Enumerated query type
** @attr Fpos [ajlong] File position from fseek
** @attr QryDone [AjBool] Has the query been done yet
** @attr Access [SeqSAccess*] Access function : see ajseqread.h
** @attr QryData [void*] Private data for access function
** @@
******************************************************************************/

typedef struct AjSSeqQuery {
  AjPStr DbName;
  AjPStr DbType;
  AjPStr Id;
  AjPStr Acc;
  AjPStr Des;
  AjPStr Key;
  AjPStr Org;
  AjPStr Sv;
  AjPStr Method;
  AjPStr Formatstr;
  AjPStr IndexDir;
  AjPStr Directory;
  AjPStr Filename;
  AjPStr Exclude;
  AjPStr DbFields;
  AjPStr DbProxy;
  AjPStr DbHttpVer;
  AjPStr Field;
  AjPStr QryString;
  AjPStr Application;
  enum AjEQryType Type;
  ajlong Fpos;
  AjBool QryDone;
  SeqSAccess* Access;
  void* QryData;
} AjOSeqQuery;

#define AjPSeqQuery AjOSeqQuery*


/* @data AjPSelexSQ ***********************************************************
**
** Ajax Selex object for #=SQ information.
**
** @new ajSelexSQNew Default constructor
** @delete ajSelexSQDel Default destructor
**
** @attr name [AjPStr] Object name
** @attr source [AjPStr] Source file
** @attr ac [AjPStr] accession
** @attr de [AjPStr] description
** @attr wt [float] weight (default 1.0)
** @attr start [ajint] start position
** @attr stop [ajint] end position
** @attr len [ajint] length
** @@
******************************************************************************/

typedef struct AjSSelexSQ
{
    AjPStr name;
    AjPStr source;
    AjPStr ac;
    AjPStr de;
    float  wt;
    ajint  start;
    ajint  stop;
    ajint  len;
} AjOSelexSQ;

#define AjPSelexSQ AjOSelexSQ*

/* @data AjPSelex *************************************************************
**
** Ajax Selex object.
**
** @new ajSelexNew Default constructor
** @delete ajSelexDel Default destructor
**
** @attr id [AjPStr] identifier
** @attr ac [AjPStr] accession
** @attr de [AjPStr] description
** @attr au [AjPStr] author
** @attr cs [AjPStr] Undocumented
** @attr rf [AjPStr] Undocumented
** @attr name [AjPStr*] Undocumented
** @attr str [AjPStr*] Undocumented
** @attr ss [AjPStr*] Undocumented
** @attr ga [float[2]] Undocumented
** @attr tc [float[2]] Undocumented
** @attr nc [float[2]] Undocumented
** @attr sq [AjPSelexSQ*] Selex sequence objects
** @attr n [ajint] Number of AjPSelexSQ sequence objects
** @attr Count [ajint] Count
** @@
******************************************************************************/

typedef struct AjSSelex
{
    AjPStr id;
    AjPStr ac;
    AjPStr de;
    AjPStr au;
    AjPStr cs;
    AjPStr rf;
    AjPStr *name;
    AjPStr *str;
    AjPStr *ss;
    float  ga[2];
    float  tc[2];
    float  nc[2];
    AjPSelexSQ *sq;
    ajint  n;
    ajint  Count;
} AjOSelex;

#define AjPSelex AjOSelex*


/* @data AjPSelexdata *********************************************************
**
** Ajax Selex data object (individual sequences)
**
** @new ajSelexdataNew Default constructor
** @delete ajSelexdataDel Default destructor
**
** @attr id [AjPStr] identifier
** @attr ac [AjPStr] accession
** @attr de [AjPStr] description
** @attr au [AjPStr] author
** @attr cs [AjPStr] Undocumented
** @attr rf [AjPStr] Undocumented
** @attr name [AjPStr] Undocumented
** @attr str [AjPStr] Undocumented
** @attr ss [AjPStr] Undocumented
** @attr ga [float[2]] Undocumented
** @attr tc [float[2]] Undocumented
** @attr nc [float[2]] Undocumented
** @attr sq [AjPSelexSQ] Selex sequence object
** @@
******************************************************************************/

typedef struct AjSSelexdata
{
    AjPStr id;
    AjPStr ac;
    AjPStr de;
    AjPStr au;
    AjPStr cs;
    AjPStr rf;
    AjPStr name;
    AjPStr str;
    AjPStr ss;
    float  ga[2];
    float  tc[2];
    float  nc[2];
    AjPSelexSQ sq;
} AjOSelexdata;

#define AjPSelexdata AjOSelexdata*


/* @data AjPStockholm *********************************************************
**
** Ajax Stockholm object.
**
** @new ajStockholmNew Default constructor
** @delete ajStockholmDel Default destructor
**
** @attr id [AjPStr] identifier
** @attr ac [AjPStr] accession
** @attr de [AjPStr] description
** @attr au [AjPStr] author
** @attr al [AjPStr] Undocumented
** @attr tp [AjPStr] Undocumented
** @attr se [AjPStr] Undocumented
** @attr ga [ajint[2]] Undocumented
** @attr tc [float[2]] Undocumented
** @attr nc [float[2]] Undocumented
** @attr bm [AjPStr] Undocumented
** @attr ref [AjPStr] Undocumented
** @attr dc [AjPStr] Undocumented
** @attr dr [AjPStr] Undocumented
** @attr cc [AjPStr] Undocumented
** @attr sacons [AjPStr] Undocumented
** @attr sscons [AjPStr] Undocumented
** @attr gs [AjPStr] Undocumented
** @attr name [AjPStr*] Undocumented
** @attr str [AjPStr*] Undocumented
** @attr n [ajint] Undocumented
** @attr Count [ajint] Count
** @@
******************************************************************************/

typedef struct AjSStockholm
{
    AjPStr id;
    AjPStr ac;
    AjPStr de;
    AjPStr au;
    AjPStr al;
    AjPStr tp;
    AjPStr se;
    ajint  ga[2];
    float  tc[2];
    float  nc[2];
    AjPStr bm;
    AjPStr ref;
    AjPStr dc;
    AjPStr dr;
    AjPStr cc;
    AjPStr sacons;
    AjPStr sscons;
    AjPStr gs;
    AjPStr *name;
    AjPStr *str;
    ajint  n;
    ajint  Count;
} AjOStockholm;

#define AjPStockholm AjOStockholm*


/* @data AjPStockholmdata *****************************************************
**
** Ajax Stockholm data object (individual sequences)
**
** @new ajStockholmdataNew Default constructor
** @delete ajStockholmdataDel Default destructor
**
** @attr id [AjPStr] identifier
** @attr ac [AjPStr] accession
** @attr de [AjPStr] description
** @attr au [AjPStr] author
** @attr al [AjPStr] Undocumented
** @attr tp [AjPStr] Undocumented
** @attr se [AjPStr] Undocumented
** @attr bm [AjPStr] Undocumented
** @attr sscons [AjPStr] Undocumented
** @attr sacons [AjPStr] Undocumented
** @attr ref [AjPStr] Undocumented
** @attr dc [AjPStr] Undocumented
** @attr dr [AjPStr] Undocumented
** @attr cc [AjPStr] Undocumented
** @attr gs [AjPStr] Undocumented
** @attr ga [float[2]] Undocumented
** @attr tc [float[2]] Undocumented
** @attr nc [float[2]] Undocumented
** @@
******************************************************************************/

typedef struct AjSStockholmdata
{
    AjPStr id;
    AjPStr ac;
    AjPStr de;
    AjPStr au;
    AjPStr al;
    AjPStr tp;
    AjPStr se;
    AjPStr bm;
    AjPStr sscons;
    AjPStr sacons;
    AjPStr ref;
    AjPStr dc;
    AjPStr dr;
    AjPStr cc;
    AjPStr gs;
    float  ga[2];
    float  tc[2];
    float  nc[2];
} AjOStockholmdata;

#define AjPStockholmdata AjOStockholmdata*


/* @data AjPSeqin *************************************************************
**
** Ajax Sequence Input object.
**
** Holds the sequence specification and information needed to read
** the sequence and possible further sequences.
**
** Also holds information on a selected sequence range and other
** options.
**
** @new ajSeqinNew Default constructor
** @delete ajSeqinDel Default destructor
** @set ajSeqinUsa Resets using a new USA
** @set ajSeqinClear Resets ready for reuse.
** @set ajSeqinSetRange Sets a sequence range for all input sequences
** @use ajSeqRead Reading a sequence.
** @use ajSeqsetRead Reading a sequence set.
** @use ajSeqAllRead Reading a sequence stream.
** @other AjPSeq Sequences
** @other AjPSeqset Sequence sets
** @other AjPSeqall Sequence streams
**
** @attr Name [AjPStr] Sequence name (replace on reading)
** @attr Acc [AjPStr] Sequence accession number (replace on reading)
** @attr Inputtype [AjPStr] Sequence type from ACD
** @attr Type [AjPStr] Sequence type N or P
** @attr Db [AjPStr] Database name (replace on reading)
** @attr Setdb [AjPStr] Database name for output (replace on reading)
** @attr Full [AjPStr] Full name
** @attr Date [AjPStr] Date
** @attr Desc [AjPStr] One-line description
** @attr Doc [AjPStr] Full text
** @attr Inseq [AjPStr] Temporary input sequence holder
** @attr Begin [ajint] Start position
** @attr End [ajint] End position
** @attr Rev [AjBool] Reverse/complement if true
** @attr List [AjPList] List of USAs to be read
** @attr Usa [AjPStr] USA for the sequence
** @attr Ufo [AjPStr] UFO for features (if any)
** @attr Fttable [AjPFeattable] Input feature table (why in AjPSeqin?)
** @attr Ftquery [AjPFeattabIn] Feature table input spec
** @attr Formatstr [AjPStr] Sequence input format name
** @attr Format [AjEnum] Sequence input format enum
** @attr Filename [AjPStr] Original filename
** @attr Entryname [AjPStr] Entry name
** @attr Filebuff [AjPFileBuff] Input sequence buffered file
** @attr Search [AjBool] Search for more entries (always true?)
** @attr Single [AjBool] Read single entries
** @attr Features [AjBool] true: read features if any
** @attr IsNuc [AjBool] true: known to be nucleic
** @attr IsProt [AjBool] true: known to be protein
** @attr multi [AjBool] ???? see also Single
** @attr multiset [AjBool] true: seqsetall input
** @attr multidone [AjBool] seqsetall input: true when set completed
** @attr Lower [AjBool] true: convert to lower case -slower
** @attr Upper [AjBool] true: convert to upper case -supper
** @attr Text [AjBool] true: save full text of entry
** @attr Count [ajint] count of entries so far. Used when ACD reads first
**                     sequence and we need to reuse it in a Next loop
** @attr Filecount [ajint] Number of files read - used by seqsetall input
** @attr Fileseqs [ajint] Number of seqs in file - used by seqsetall input
** @attr Fpos [ajlong] File position (fseek) for building USA
** @attr Query [AjPSeqQuery] Query data - see AjPSeqQuery
** @attr Selex [AjPSelex] Selex data
** @attr Stockholm [AjPStockholm] Stokholm data
** @attr Data [void*] Format data for reuse, e.g. multiple sequence input
** @@
******************************************************************************/

typedef struct AjSSeqin {
  AjPStr Name;
  AjPStr Acc;
  AjPStr Inputtype;
  AjPStr Type;
  AjPStr Db;
  AjPStr Setdb;
  AjPStr Full;
  AjPStr Date;
  AjPStr Desc;
  AjPStr Doc;
  AjPStr Inseq;
  ajint Begin;
  ajint End;
  AjBool Rev;
  AjPList List;
  AjPStr Usa;
  AjPStr Ufo;
  AjPFeattable Fttable;
  AjPFeattabIn Ftquery;
  AjPStr Formatstr;
  AjEnum Format;
  AjPStr Filename;
  AjPStr Entryname;
  AjPFileBuff Filebuff;
  AjBool Search;
  AjBool Single;
  AjBool Features;
  AjBool IsNuc;
  AjBool IsProt;
  AjBool multi;
  AjBool multiset;
  AjBool multidone;
  AjBool Lower;
  AjBool Upper;
  AjBool Text;
  ajint Count;
  ajint Filecount;
  ajint Fileseqs;
  ajlong Fpos;
  AjPSeqQuery Query;
  AjPSelex Selex;
  AjPStockholm Stockholm;
  void *Data;
} AjOSeqin;

#define AjPSeqin AjOSeqin*



/* @data AjPSeq ***************************************************************
**
** Ajax Sequence object.
**
** Holds the sequence itself, plus associated information such as a
** sequence name, accession number, format, type.
**
** Also holds information on a selected sequence range and other
** options.
**
** Sequence features can also be stored, but for efficiency reasons
** features are turned off by default.
**
** @new ajSeqNew Default constructor
** @new ajSeqNewL Constructor with expected maximum size.
** @new ajSeqNewS Constructor with sequence object to be cloned.
** @delete ajSeqDel Default destructor
** @set ajSeqRead Master sequence input, calls specific functions for
**                file access type and sequence format.
** @set ajSeqAllRead Master sequence stream input, reads first sequence
**                   from an open input stream.
** @set ajSeqallNext Master sequence stream input, reads next sequence
**                   from an open input stream.
** @set ajSeqMod Sets a sequence as modifiable by making its sequence
**               into a unique AjPStr.
** @set ajSeqReplace Replaces a sequence with a string containing a modified
**                   version.
** @set ajSeqReplaceC Replaces a sequence with a char* containing a modified
**                   version.
** @set ajSeqSetRange Sets a sequence using specified start and end positions.
** @set ajSeqType Sets the sequence type
** @set ajSeqSetNuc Sets sequence to be nucleotide
** @set ajSeqSetProt Sets sequence to be protein
** @set ajSeqToLower Converts a sequence to lower case
** @set ajSeqToUpper Converts a sequence to upper case
** @set ajSeqReverse Reverse complements a nucleotide sequence
** @set ajSeqReverseStr Reverse complements a nucleotide sequence as an AjPStr
** @set ajSeqRevOnly Reverses a sequence (does not complement)
** @set ajSeqCompOnly Complements a nucleotide sequence (does not reverse)
** @set ajSeqCompOnlyStr Complements a nucleotide sequence AjPStr
**                       (does not reverse)
** @cast ajSeqChar Returns the actual char* holding the sequence.
** @cast ajSeqCharCopy Returns a copy of the sequence as char*.
** @cast ajSeqCharCopyL Returns a copy of the sequence as char* with
**                      a specified minimum reserved length.
** @cast ajSeqStr Returns the actual AjPStr holding the sequence.
** @cast ajSeqCopyStr Returns a copy of the AjPStr holding the sequence.
** @cast ajSeqName Returns the sequence name as char*
** @cast ajSeqLen Returns the sequence length
** @cast ajSeqBegin Returns the sequence start position
** @cast ajSeqEnd Returns the sequence end position
** @cast ajSeqCheckGcg Calculates the GCG checksum for a sequence.
** @use ajSeqWrite Writes a sequence out.
** @use ajSeqIsNuc tests whether a sequence is nucleotide
** @use ajSeqIsProt tests whether a sequence is protein
** @use ajSeqNum Converts a sequence to numbers
** @use ajSeqTrace Reports the contents of a sequence
** @other AjPSeqset Sequence sets
** @other AjPSeqall Sequence streams
**
** @attr Name [AjPStr] Name (ID)
** @attr Acc [AjPStr] Accession number (primary only)
** @attr Sv [AjPStr] SeqVersion number
** @attr Gi [AjPStr] GI NCBI version number
** @attr Tax [AjPStr] Main taxonomy (species)
** @attr Type [AjPStr] Type N or P
** @attr EType [AjEnum] unused, obsolete
** @attr Db [AjPStr] Database name from input
** @attr Setdb [AjPStr] Database name from command line
** @attr Full [AjPStr] Full name
** @attr Date [AjPStr] Date
** @attr Desc [AjPStr] One-line description
** @attr Doc [AjPStr] Obsolete - see TextPtr
** @attr Rev [AjBool] true: to be reverse-complemented
** @attr Reversed [AjBool] true: has been reverse-complemented
** @attr Begin [ajint] start position (processed on reading)
** @attr End [ajint] end position (processed on reading)
** @attr Offset [ajint] offset from start
** @attr Offend [ajint] offset from end 
** @attr Fpos [ajlong] File position (fseek) for USA
** @attr Usa [AjPStr] USA fo re-reading
** @attr Ufo [AjPStr] UFO for re-reading
** @attr Fttable [AjPFeattable] Feature table
** @attr Formatstr [AjPStr] Input format name
** @attr Format [AjEnum] Input format enum
** @attr Filename [AjPStr] Original filename
** @attr Entryname [AjPStr] Entryname (ID)
** @attr TextPtr [AjPStr] Full text
** @attr Weight [float] Weight from multiple alignment
** @attr Acclist [AjPList] Secondary accessions
** @attr Keylist [AjPList] Keyword list
** @attr Taxlist [AjPList] Taxonomy list (just species for now)
** @attr Seq [AjPStr] The sequence
** @attr Selexdata [AjPSelexdata] Selex data
** @attr Stock [AjPStockholmdata] Stockholm data
** @@
******************************************************************************/
typedef struct AjSSeq {
  AjPStr Name;
  AjPStr Acc;
  AjPStr Sv;
  AjPStr Gi;
  AjPStr Tax;
  AjPStr Type;
  AjEnum EType;
  AjPStr Db;
  AjPStr Setdb;
  AjPStr Full;
  AjPStr Date;
  AjPStr Desc;
  AjPStr Doc;
  AjBool Rev;
  AjBool Reversed;
  ajint Begin;
  ajint End;
  ajint Offset;
  ajint Offend;
  ajlong Fpos;
  AjPStr Usa;
  AjPStr Ufo;
  AjPFeattable Fttable;
  AjPStr Formatstr;
  AjEnum Format;
  AjPStr Filename;
  AjPStr Entryname;
  AjPStr TextPtr;
  float Weight;
  AjPList Acclist;
  AjPList Keylist;
  AjPList Taxlist;
  AjPStr Seq;
  AjPSelexdata Selexdata;
  AjPStockholmdata Stock;
} AjOSeq;

#define AjPSeq AjOSeq*

/* @data AjPSeqset ************************************************************
**
** Ajax Sequence set object. A sequence set contains one or more
** sequences together in memory, for example as a sequence alignment.
**
** Holds the sequence set itself, plus associated information such as a
** sequence names, accession number, format, type.
**
** Also holds information on a selected sequence range and other
** options.
**
** Sequence set features can also be stored, but for efficiency reasons
** features are turned off by default.
**
** @new ajSeqsetNew Default constructor
** @delete none Typically left until the program ends so there
**              is no destructor used.
** @set ajSeqsetRead Master input routine for a sequence set
** @set ajSeqsetToLower Converts a sequence set to lower case
** @set ajSeqsetToUpper Converts a sequence set to upper case
** @cast ajSeqsetLen Returns the maximum length of a sequence set
** @cast ajSeqsetSize Returns the number of sequences in a sequence set
** @cast ajSeqsetName Returns the name of a sequence in a set
** @cast ajSeqsetSeq Returns the char* pointer to a sequence in a set
** @cast ajSeqsetIsNuc Tests whether the sequence set is nucleotide
** @cast ajSeqsetIsProt Tests whether the sequence set is protein
** @use ajSeqsetWrite Writes out all sequences in a set
** @other AjPSeq Sequences
** @other AjPSeqall Sequence streams
**
** @attr Size [ajint] Number of sequences
** @attr Len [ajint] Maximum sequence length
** @attr Begin [ajint] start position
** @attr End [ajint] end position
** @attr Rev [AjBool] true: reverse-complemented
** @attr Totweight [float] total weight (usually 1.0 * Size)
** @attr Type [AjPStr] Type N or P
** @attr EType [AjEnum] enum type obsolete
** @attr Formatstr [AjPStr] Input format name
** @attr Format [AjEnum] Input format enum
** @attr Filename [AjPStr] Original filename
** @attr Full [AjPStr] Full name
** @attr Name [AjPStr] Name
** @attr Usa [AjPStr] USA for re-reading
** @attr Ufo [AjPStr] UFO for re-reading
** @attr Seq [AjPSeq*] Sequence array (see Size)
** @attr Seqweight [float*] Sequence weights (see also AjPSeq)
** @@
******************************************************************************/

typedef struct AjSSeqset {
  ajint Size;
  ajint Len;
  ajint Begin;
  ajint End;
  AjBool Rev;
  float Totweight;
  AjPStr Type;
  AjEnum EType;
  AjPStr Formatstr;
  AjEnum Format;
  AjPStr Filename;
  AjPStr Full;
  AjPStr Name;
  AjPStr Usa;
  AjPStr Ufo;
  AjPSeq* Seq;
  float* Seqweight;
} AjOSeqset;

#define AjPSeqset AjOSeqset*



/* @data AjPSeqall ************************************************************
**
** Ajax Sequence all (stream) object.
**
** Inherits an AjPSeq but allows more sequences to be read from the
** same input by also inheriting the AjPSeqin input object.
**
** @new ajSeqallNew Default constructor
** @delete none Typically left until the program ends so there
**              is no destructor used.
** @set ajSeqallNext Master sequence stream input.
** @other AjPSeq Sequences
** @other AjPSeqin Sequence input
** @other AjPSeqset Sequence sets
**
** @attr Seq [AjPSeq] Current sequence
** @attr Seqin [AjPSeqin] Sequence input for reading next
** @attr Count [ajint] Count of sequences so far
** @attr Begin [ajint] start position
** @attr End [ajint] end position
** @attr Rev [AjBool] if true: reverse-complement
** @@
******************************************************************************/

typedef struct AjSSeqall {
  AjPSeq Seq;
  AjPSeqin Seqin;
  ajint Count;
  ajint Begin;
  ajint End;
  AjBool Rev;
} AjOSeqall;

#define AjPSeqall AjOSeqall*

#endif

#ifdef __cplusplus
}
#endif
