
#ifndef _H_CKIT
#define _H_CKIT

/***  CKit.h  ***********************************************************
**
**
****************************************************************************/
#include <stdio.h>
#include "ckittypes.h"
#include "seqspec.h"
#include "seqentry.h"

/*
** NextSeqEntry.c
*/

extern  SeqEntry *NextSeqEntry(SeqSpec *spec);

/*
** DataFiles.c
*/

extern     FILE *FindDataFile(char *fileName);
extern     FILE *FindPIRFile(char *fileName);
extern     FILE *FindGCGFile(char *fileName, Choice *where);
extern     void  ShowGCGEnzymes(char *enzFName, Boolean doSchiz, Choice *where);

/*
** Error.c
*/

extern void Notify(ProcPtr pNotify, ProcPtr pError, ProcPtr pFatal);
extern void PostError(Choice severity, char *errMsg);
extern  int ClearErrors(void);
extern  int Errors(void);

/*
** SeqEntry.c
*/

extern SeqEntry *NewSeqEntry(void);
extern     void  DeleteSeqEntry(SeqEntry *seq);
extern     char *MakeSeqDesc(SeqEntry *seq);
extern   Choice  EncodeType(char *string);
extern     char *DecodeType(Choice type);

/*
** SeqSpec.c
*/

extern SeqSpec *NewSeqSpec(void);                           
extern SeqSpec *CopySeqSpec(SeqSpec *to, SeqSpec *from);
extern    void  DeleteSeqSpec(SeqSpec *spec);
extern    void  MakeSeqSpec(SeqSpec *spec, char *, char *defaultDB); 
extern  Choice  EncodeFormat(char *string);
extern    char *DecodeFormat(Choice format);

/*
** ReadEntry.h
*/

extern SeqEntry *ReadEntry(SeqSpec *spec);
extern  Boolean  SetDataBase(char *dbName);

/*
** Sequence.c
*/

extern     int  CheckSeq(char *strand);
extern    char  CompBase(char base);
extern    char *RevComp(char *strand, Choice type);
extern    char *DNAtoRNA(char *strand);
extern    char *RNAtoDNA(char *strand);
extern    long  NucToProtein(Choice geneticCode, char *strand, long *length);
extern  double  Weight(char *strand, Choice type);

/*
** Strings.c
*/

extern    char *BooleanToStr(Boolean value, Choice style);
extern    char *NextToken(char *string, char *cPos, char *token, char *sep);
extern    char *StrCollapse(char *string);
extern    char *StrCompress(char *string);
extern    char *StrIndex(char *pattern, char *string);
extern Boolean  StrIsBlank(char *string);
extern    char *StrToLower(char *string );
extern    char *StrToUpper(char *string );
extern    char *StrChange(char *string, char before, char after);
extern    char *StrTrim(char *string);
extern    char *StrTruncate(char *string);
extern    char *StrUnComment(char *string);
extern Boolean  StrWildMatch(char *pattern, char *string);

/*
** <OS Specific.c>
*/

extern Boolean  CopyFile(char *toFName, char *fromFName);
extern Boolean  RenameFile(char *newFName, char *oldFName);
extern    void  PurgeFile(char *fileName);
extern Boolean  DeleteFile(char *fileName);
extern    char *DePath(char *fileName);
extern    char *GetTime(Choice style);
extern Boolean  TransName(char *translation, char *logicalName);
extern Boolean  NeedsFileType(char *fileName);
extern Boolean  NewFileType(char *fileName, char *type);
extern Boolean  NextFile(char *wildFileName, char *thisFName);
extern Boolean  GetOSSymbol(char *symbol, char *value);
extern Boolean  SetOSSymbol(char *symbol, char *value);
extern Boolean  Spawn(char *command, char *control);
extern    void  Launch(char *command);
                                 

/*
** WriteSeq.c
*/

extern    char *SeqToSpecName(SeqEntry *seq, Choice format);
extern    char *WriteSeq(char *outSpec, SeqEntry *seq, Choice format);

#endif
