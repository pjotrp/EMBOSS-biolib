#ifdef __cplusplus
extern "C"
{
#endif

#ifndef embWord_h
#define embWord_h

/* @data EmbPWordMatch ********************************************************
**
** NUCLEUS data structure for word matches
**
** @attr seq1start [ajint] match start point in original sequence
** @attr seq2start [ajint] match start point in comparison sequence
** @attr length [ajint] length of match
** @attr sequence [AjPSeq] need in case we build multiple matches here
**                         so we know which one the match belongs to
** @@
******************************************************************************/

typedef struct EmbSWordMatch {
  ajint seq1start;
  ajint seq2start;
  ajint length;
  AjPSeq sequence;
} EmbOWordMatch, *EmbPWordMatch;

/* @data EmbPWord *************************************************************
**
** NUCLEUS data structure for words
**
** @attr count [ajint] Size of list
** @attr fword [char*] Original word
** @attr list [AjPList] List of words
** @@
******************************************************************************/

typedef struct EmbSWord {
  ajint count;
  char *fword;
  AjPList list;
} EmbOWord, *EmbPWord;

/* @data EmbPWord2 ************************************************************
**
** NUCLEUS data structure for words (part 2)
**
** @attr name [char*] Name
** @attr fword [EmbPWord] Word structure
** @@
******************************************************************************/

typedef struct EmbSWord2 {
  char *name;
  EmbPWord fword;
} EmbOWord2, *EmbPWord2;



AjPList embWordBuildMatchTable (AjPTable *seq1MatchTable,
				AjPSeq seq2, ajint orderit);
void    embWordClear (void);
void    embWordFreeTable( AjPTable table);
ajint   embWordGetTable (AjPTable *table, AjPSeq seq);
void    embWordLength (ajint wordlen);
AjBool  embWordMatchIter (AjIList iter, ajint* start1, ajint* start2,
			  ajint* len);
void    embWordMatchListDelete (AjPList* plist);
void    embWordMatchListPrint (AjPFile file, AjPList list);
void    embWordPrintTable  (AjPTable table);
void    embWordPrintTableF (AjPTable table, AjPFile outf);
void    embWordMatchListConvToFeat(AjPList list,
				   AjPFeattable *tab1, AjPFeattable *tab2,
				   AjPSeq seq1, AjPSeq seq2);

void    embWordMatchMin(AjPList matchlist, ajint seq1length, int
        			seq2length);
#endif

#ifdef __cplusplus
}
#endif


