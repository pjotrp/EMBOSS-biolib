/* @source wordmatch application
**
** Finds matching words in DNA sequences
**
** @author
** @@
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

/* wordmatch
** Create a word table for the sequences in the sequence set.
** Then iterate over the sequences in the seqall set
** checking to see if the word matches.  If word matches then
** check to see if the position lines up with the last position if it
** does continue else stop.
**
*/

#include "emboss.h"
#include <string.h>


/*
**
** Modulus (q, a large prime) and radix are used for calculating hash values.
** We should be able to replace binary search with direct search
** by selecting a small q, but in this case we should always make sure that
** a hit is a correct hit.
** radix has a relation with alphabet size
**
*/

static ajulong q = 1073741789;
static ajulong radix =256;




/* @data EmbPWordWrap *********************************************************
**
** data structure that wraps EmbPWord objects for efficient access
**
** @attr word [EmbPWord] Original word object
** @attr seqs [AjPSeq*] List of sequences word has been seen
** @attr seqindxs [ajuint*] Positions in the seqset
**                          for each sequence in the seqs array
** @attr nnseqlocs [ajuint**] Number of word start positions for each sequence
** @attr locs [ajuint**] List of word start positions for each sequence
** @attr hash [ajulong] Hash value for the word
** @attr nseqs [ajuint] Number of sequences word has been seen
** @attr minskip [ajuint] We can scan all other words to find out a minimum
**                        skip when this word is matched (not yet used)
** @@
******************************************************************************/

typedef struct EmbSWordWrap {
    const EmbPWord word;
    const AjPSeq * seqs;
    ajuint* seqindxs;
    ajuint* nnseqlocs;
    ajuint** locs;
    ajulong hash;
    ajuint nseqs;
    ajuint minskip;
} EmbOWordWrap;

#define EmbPWordWrap EmbOWordWrap*



static ajuint embPatRabinKarpSearchMultiPattern(const AjPStr sseq,
        const EmbPWordWrap* patterns,
        ajuint plen, ajuint nwords, AjPList* l,
        ajuint* lastlocation);

static ajuint getWords(const AjPTable table, EmbPWordWrap**, ajuint wordlen,
        const AjPSeqset seqset);




/* @prog wordmatch ************************************************************
**
** Finds all exact matches of a given size between 2 sequences
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjPSeqset seqset;
    AjPSeqall seqall;
    AjPSeq seqofseqall;
    const AjPSeq seqofseqset;
    ajint wordlen;
    AjPTable seq1MatchTable = 0;
    AjPList* matchlist = NULL;
    AjPFile logfile;
    AjPFeattable ftableofseqsetseq = NULL;
    AjPFeattable ftableofseqallseq = NULL;
    AjPFeattabOut ftoutforseqsetseq = NULL;
    AjPFeattabOut ftoutforseqallseq = NULL;
    AjPAlign align = NULL;
    AjIList iter = NULL;
    ajint start1;
    ajint start2;
    ajint len;
    ajuint i, j;
    ajulong nAllMatches = 0;
    ajulong sumAllScore = 0;
    AjBool dumpAlign = ajTrue;
    AjBool dumpFeature = ajTrue;
    EmbPWordWrap* wordswrapped = NULL;
    ajuint npatterns=0;

    /** cursors for the current sequence being scanned
     ** until which location it was scanned,
     ** we have an entry for each sequence in the seqset
     **
     */
    ajuint* lastlocation;


    embInit("wordmatch", argc, argv);

    wordlen = ajAcdGetInt("wordsize");
    seqset  = ajAcdGetSeqset("asequence");
    seqall  = ajAcdGetSeqall("bsequence");
    logfile = ajAcdGetOutfile("logfile");
    dumpAlign = ajAcdGetToggle("dumpalign");
    dumpFeature = ajAcdGetToggle("dumpfeat");

    ajSeqsetTrim(seqset);
    AJCNEW0(matchlist, ajSeqsetGetSize(seqset));

    if(dumpAlign)
    {
        align = ajAcdGetAlign("outfile");
        ajAlignSetExternal(align, ajTrue);
    }

    if (dumpFeature)
    {
        ftoutforseqsetseq =  ajAcdGetFeatout("aoutfeat");
        ftoutforseqallseq =  ajAcdGetFeatout("boutfeat");
    }
    embWordLength(wordlen);

    for(i=0;i<ajSeqsetGetSize(seqset);i++)
    {
        const AjPSeq seq;
        seq = ajSeqsetGetseqSeq(seqset, i);
        embWordGetTable(&seq1MatchTable, seq);
    }
    AJCNEW0(lastlocation, i);

    if(ajTableGetLength(seq1MatchTable)>0)
    {
        npatterns = getWords(seq1MatchTable, &wordswrapped, wordlen, seqset);

        while(ajSeqallNext(seqall,&seqofseqall))
        {
            ajuint nmatches;
            for(i=0;i<ajSeqsetGetSize(seqset);i++)
            {
                lastlocation[i]=0;
                matchlist[i] = ajListstrNew();
            }
            nmatches = embPatRabinKarpSearchMultiPattern(
                    ajSeqGetSeqS(seqofseqall),
                    (const EmbPWordWrap*)wordswrapped, wordlen, npatterns,
                    matchlist, lastlocation);


            for(i=0;i<ajSeqsetGetSize(seqset);i++)
            {
                if(nmatches>0)
                {
                    iter = ajListIterNewread(matchlist[i]) ;

                    while(embWordMatchIter(iter, &start1, &start2, &len,
                            &seqofseqset))
                    {
                        if (dumpAlign){
                            /* TODO: check any possible improvements using
                             * if(ajAlignFormatShowsSequences(align)
                             */
                            ajAlignDefineSS(align, seqofseqset, seqofseqall);
                            ajAlignSetScoreI(align, len);
                            /* ungapped alignment
                             * so same length for both sequences */
                            ajAlignSetSubRange(align, start1, 1, len,
                                    ajSeqIsReversed(seqofseqset),
                                    ajSeqGetLen(seqofseqset),
                                    start2, 1, len,
                                    ajSeqIsReversed(seqofseqall),
                                    ajSeqGetLen(seqofseqall));
                            ajAlignWrite(align);
                            ajAlignReset(align);
                        }
                        sumAllScore += len;
                    }

                    if(ajListGetLength(matchlist[i])>0 && dumpFeature)
                    {
                        /*
                         ** TODO: this section needs to be updated
                         ** for multi sequence mode
                         */
                        embWordMatchListConvToFeat(matchlist[i],
                                &ftableofseqsetseq,&ftableofseqallseq,
                                seqofseqset,seqofseqall);
                        ajFeattableWrite(ftoutforseqsetseq, ftableofseqsetseq);
                        ajFeattableWrite(ftoutforseqallseq, ftableofseqallseq);
                    }

                    ajListIterDel(&iter);
                }
                embWordMatchListDelete(&matchlist[i]);
            }
            nAllMatches += nmatches;
        }
        ajFmtPrintF(logfile, "number of patterns = %u\n", npatterns);
        ajFmtPrintF(logfile, "total number matches = %Lu\n", nAllMatches);
        ajFmtPrintF(logfile, "sum of score values = %Lu\n", sumAllScore);
    }

    embWordFreeTable(&seq1MatchTable);	/* free table of words */

    for(i=0;i<npatterns;i++)
    {
        AJFREE(wordswrapped[i]->seqindxs);
        AJFREE(wordswrapped[i]->seqs);
        for(j=0;j<wordswrapped[i]->nseqs;j++)
            AJFREE(wordswrapped[i]->locs[j]);
        AJFREE(wordswrapped[i]->nnseqlocs);
        AJFREE(wordswrapped[i]->locs);
        AJFREE(wordswrapped[i]);
    }
    AJFREE(wordswrapped);
    AJFREE(matchlist);
    AJFREE(lastlocation);

    if(dumpAlign)
    {
        ajAlignClose(align);
        ajAlignDel(&align);
    }

    if(dumpFeature)
    {
        ajFeattableDel(&ftableofseqsetseq);
        ajFeattableDel(&ftableofseqallseq);
        ajFeattabOutDel(&ftoutforseqsetseq);
        ajFeattabOutDel(&ftoutforseqallseq);
    }
    ajFileClose(&logfile);

    ajSeqallDel(&seqall);
    ajSeqsetDel(&seqset);
    ajSeqDel(&seqofseqall);

    embExit();

    return 0;
}




/* @func compEmbWordWrap ******************************************************
**
** Comparison function for EmbPWordWrap objects, based on their hash values
**
** @param [r] a [const void *] First EmbPWordWrap object
** @param [r] b [const void *] Second EmbPWordWrap object
**
** @return [ajint] difference of hash values
******************************************************************************/

static ajint compEmbWordWrap(const void *a, const void *b)
{
    const EmbPWordWrap ww1;
    const EmbPWordWrap ww2;

    ww1 = *(const EmbPWordWrap const *) a;
    ww2 = *(const EmbPWordWrap const *) b;

    return ww1->hash - ww2->hash;
}




/* @func precomputeRM ********************************************************
**
** Precomputes a value that helps recalculating consecutive hash values
** with less computation. Uses q and radix variables defined above.
**
** @param [r] m [ajuint] word length
** @return [ajulong] radix^(m-1) % q
** @@
******************************************************************************/

static ajulong precomputeRM(ajuint m){
    ajulong rm;
    ajuint i;
    rm = 1;
    for (i = 1; i <= m-1; i++)
    {
        rm = (radix * rm) % q;
    }
    return rm;
}




/* @func embPatRabinKarpSearchMultiPattern*************************************
**
** Rabin Karp search for multiple patterns.
**
** @param [r] sseq [const AjPStr] Sequence to be scanned for multiple patterns
** @param [r] patterns [const EmbPWordWrap*] Patterns to be searched
** @param [r] plen [ajuint] Length of patterns
** @param [r] npatterns [ajuint] Number of patterns
** @param [u] matchlist [AjPList] List of matches for each sequence
**                                in the sequence set
** @param [m] lastlocation [ajuint*] Position of the search for each sequence
**                                in the sequence set
** @return [ajuint] number of matches
** @@
******************************************************************************/

static ajuint embPatRabinKarpSearchMultiPattern(const AjPStr sseq,
             const EmbPWordWrap* patterns,
             ajuint plen, ajuint npatterns, AjPList* matchlist,
             ajuint* lastlocation)
{
    const char *text;
    const AjPSeq seq;
    ajuint i, j, tlen, ii, k, seqsetindx, indxloc, maxloc;
    ajuint matches=0;
    ajuint slen;
    EmbPWordWrap* bsres;
    EmbPWordWrap cursor;
    ajulong rm;
    ajulong textHash = 0;
    ajuint seq2start;

    AJNEW0(cursor);

    rm = precomputeRM(plen);
    text = ajStrGetPtr(sseq);
    tlen  = ajStrGetLen(sseq);

    for (i=0; i<plen; i++)
    {
        textHash    = (ajulong)(radix * textHash   + text[i]) %q;
    }

    /* Scan the sequence */
    for (i=plen; i<=tlen;)
    {
        cursor->hash = textHash;
        bsres = bsearch(&cursor, patterns, npatterns,
                sizeof(EmbPWordWrap), compEmbWordWrap);
        if(bsres!=NULL)
        {
            seq2start = i-plen;

            for (k=0;k<(*bsres)->nseqs;k++)
            {
                seq= (*bsres)->seqs[k];
                seqsetindx = (*bsres)->seqindxs[k];
                if (lastlocation[seqsetindx] < i)
                {
                    maxloc = 0;
                    for(indxloc=0; indxloc < (*bsres)->nnseqlocs[k]; indxloc++)
                    {
                        ajuint pos = (*bsres)->locs[k][indxloc];
                        const char* seq_ = ajSeqGetSeqC(seq);
                        j=0;
                        /* following loop is to make sure we never have
                         * false positives, after we are confident that
                         * we don't get false hits we can delete this loop
                         */
                        while(j<plen)
                        {
                            if(seq_[pos+j] != text[i+j-plen])
                            {
                                char tmp[plen+1];
                                tmp[plen] = '\0';
                                memcpy(tmp, text+i-plen, plen);
                                ajWarn("unexpected match:   pat:%s  pat-pos:%u,"
                                        " txt-pos:%u text:%s hash:%u\n",
                                        (*bsres)->word->fword, pos, i+j-plen,
                                        tmp, textHash);
                                break;
                            }
                            j++;
                        }

                        if(j<plen)
                            continue;

                        j=0;
                        ++matches;
                        ii = seq2start+plen;
                        while(ii<tlen  && pos+plen+j<ajSeqGetLen(seq))
                        {
                            if(seq_[pos+plen+j] != text[ii++])
                                break;
                            else
                                ++j;
                        }
                        embWordMatchListAppend(matchlist[seqsetindx],
                                seq, pos, seq2start, plen+j);
                        if ( ii > maxloc )
                            maxloc = ii;
                    }
                    lastlocation[seqsetindx] = maxloc;
                }
            }
        }
        textHash = ((textHash +text[i-plen] *(q-rm))*radix+ text[i]) % q;
        ++i;
    }
    AJFREE(cursor);
    return matches;
}




/* @func getWords ********************************************************
**
** Preprocesses word/pattern table and repackages the words in EmbPWordWrap
** objects to improve access efficiency. Also computes hash values
** for each pattern.
**
** @param [r] table [const AjPTable] table of patterns
** @param [u] newwords [const EmbPWordWrap*] Patterns to be searched
** @param [r] wordlen [ajuint] Length of words/patterns
** @param [u] seqset [AjPSeqset] Sequence set that patterns derived from
** @return [ajuint] number of words
** @@
******************************************************************************/

static ajuint getWords(const AjPTable table, EmbPWordWrap** newwords,
        ajuint wordlen, const AjPSeqset seqset)
{
    ajuint i, j, k, l;
    EmbPWord* embwords = NULL;
    const EmbPWord embword;
    ajulong patternHash;
    EmbPWordWrap newword;
    AjIList iterp;
    EmbPWordSeqLocs* seqlocs;
    ajuint nseqlocs;
    const AjPSeq seq;
    const char* word;
    ajuint nseqs = ajSeqsetGetSize(seqset);
    ajuint nwords = ajTableToarrayValues(table, (void***)&embwords);

    AJCNEW(*newwords, nwords);

    for (i=0; i<nwords; i++)
    {
        seqlocs=NULL;
        embword = embwords[i];
        word = embword->fword;

        AJNEW0(newword);

        patternHash = 0;
        for (j=0; j<wordlen; j++)
        {
            patternHash = (ajulong)(radix * patternHash + word[j]) % q;
        }

        nseqlocs = ajTableToarrayValues(embword->seqlocs, (void***)&seqlocs);
        newword->nseqs = nseqlocs;
        newword->hash  = patternHash;
        newword->word = embword;
        AJCNEW(newword->seqindxs, nseqlocs);
        AJCNEW(newword->locs, nseqlocs);
        AJCNEW(newword->seqs, nseqlocs);
        AJCNEW(newword->nnseqlocs, nseqlocs);

        for(j=0; j<nseqlocs; j++)
        {
            seq= seqlocs[j]->seq;
            for(l=0;l<nseqs;l++)
            {
                if (seq == ajSeqsetGetseqSeq(seqset,l))
                {
                    newword->seqindxs[j] = l;
                    break;
                }
            }
            if (l==nseqs)
            {
                ajErr("something wrong, sequence not found in seqset");
                ajExitBad();
            }
            newword->seqs[j] = seq;
            iterp = ajListIterNewread(seqlocs[j]->locs);
            k=0;
            newword->nnseqlocs[j]= ajListGetLength(seqlocs[j]->locs);
            AJCNEW(newword->locs[j],newword->nnseqlocs[j]);
            while(!ajListIterDone(iterp))
            {
                ajuint pos = *(ajuint *) ajListIterGet(iterp);
                newword->locs[j][k++] = pos;
            }
            ajListIterDel(&iterp);
        }

        AJFREE(seqlocs);

        (*newwords)[i] = newword;

    }

    AJFREE(embwords);

    qsort(*newwords, nwords, sizeof(EmbPWordWrap), compEmbWordWrap);

    return nwords;
}
