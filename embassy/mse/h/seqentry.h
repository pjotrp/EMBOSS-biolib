#ifndef _H_SEQENTRY
#define _H_SEQENTRY

/***  SeqEntry.h  **********************************************************
**
** Sequence Entry - composed of Sequence Specification, Header, Title
**   Description, flags and sequence
**
**  William A. Gilbert, Whitehead Institute
**
****************************************************************************/
#include "seqspec.h"
#include "seqdef.h"
#include "ckittypes.h"

typedef struct SeqEntry SeqEntry;
struct SeqEntry {
    SeqSpec *spec;      /* Pointer to a Sequence Spec structure           */
       char *name;      /* Sequence Name, varies with format              */
       char *title;     /* Sequence title, may be blank for some formats  */
       char *desc;      /* Sequence description                           */
     Choice  type;      /* Sequence type, Protein, DNA, RNA, etc,         */
    Boolean  circular;  /* "true" if this is a circular molecule          */
        int  checkSum;  /* Checksum for sequence                          */
       long  length;    /* Length of sequence                             */
       char *mem;       /* Memory pointer to start of sequence            */
       long  size;      /* Size of memory block                           */
       char *strand;    /* Pointer to *Mem-1 such that Strand[1] = &Mem   */
       char *text;      /* Pointer to text comments, seperated by \n's    */
       long  tSize;     /* Size of "Text", used by cfree                  */
};


#endif
