#ifndef _H_SEQDEF
#define _H_SEQDEF

/****  SeqDef.h  *********************************************************
**
** Define sequence formats and sequence type.  "x" may be "L" or "C" if
** NBRF sequence format headers are used. "L" indicates a linear sequence
** "C" denotes a circular sequence, either flag may be specified by the 
** the user to override the NBRF flag or used with other formats
**
**  William Gilbert
**  Copyright (c) Whitehead Institute, MIT
**
************************************************************************/

#define UNDEF      0      /* Undefined, pick up from system logical       */
#define NBRF       1      /* NBRF/MIT sequence format                     */
#define PIR        1      /* NBRF/MIT sequence format                     */
#define RAW        2      /* sequence-only using IUPAC symbols            */
#define STADEN     3      /* Staden                                       */
#define GCG        4      /* UWGCG sequence format                        */
#define IG         5      /* Intellegentics (IG) format                   */
#define IBI        6      /* Pustell/IBI (GenBANK) format                 */
#define STRIDER    7      /* Chistian Marck's DNA Strider for Macintosh   */
#define DI         8      /* DNA Inspector IIe from TEXTCO                */
#define EMBL       9      /* EMBL format                                  */
#define PEARSON   10      /* Bill Pearson's FASTA format                  */
#define SWISSPROT 11      /* SwissProt format                             */
#define CODATA    12      /* CODATA format                                */


                        /* Use UNDEF from above for undefined type      */
#define FRAGMENT -2     /* Protein fragment, NBRF header "F1"           */
#define PROTEIN  -1     /* Complete Protein, NBRF header "P1"           */
#define DNA       1     /*  DNA, NBRF header "Dx" or user specified     */
#define RNA       2     /*  RNA, NBRF header "Rx"       "               */
#define RRNA      3     /* rRNA, NBRF header "N1"      "                */
#define TRNA      4     /* tRNA, NBRF header "N3"      "                */
#define URNA      5     /* uRNA                                         */
#define MRNA      6     /* mRNA                                         */


#endif
