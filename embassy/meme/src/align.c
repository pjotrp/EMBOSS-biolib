/***********************************************************************
*								       *
*	MEME++							       *
*	Copyright 1994, The Regents of the University of California    *
*	Author: Timothy L. Bailey				       *
*								       *
***********************************************************************/
/* align.c */

#include "meme.h"
#include "align.h"

/**********************************************************************/
/*
	align

	Align all sites that score over the threshold in a sequence.
	Returns the number of scores above threshold.
	The scores of the positions along the sequence are also
	returned in the scores array.
*/
/**********************************************************************/
extern int align(
  int imotif,				/* motif number; <0 if last sample */
  LOGODDS logodds,			/* log-odds array */
  int seqno,				/* sequence number (from 1) 
					   if <= 0, print alignment, 
					   otherwise, print .motif file */
  double threshold,			/* align sites above this */
  char *sample_name,			/* name of sample */
  char *eseq,				/* integer-coded sequence */
  BOOLEAN d[4],				/* strand directions to use */
  int lseq,				/* length of sequence */
  int w,				/* length of site */
  double *scores,			/* array to put scores in */
  FILE *outfile				/* stream for output */
)
{
  int i, j, k, ii, jj, line_len;
  int positives = 0;
  int nscores = 0;
  char site[MAXSITE+1], pre[10+1], post[10+1];
  char *strand1 = "";
  char *strand2 = "";
  char *strand3 = "";
  BOOLEAN use_strands = FALSE;

  /* non-default strands used ? */
  if (d[1] || d[2] || d[3]) {
    use_strands = TRUE;
    strand1 = " strand";
    strand2 = " ------";
    strand3 = "       ";
  } 

  /* get total width of printed line */
  line_len = MSN + strlen(strand2) + 1 + 6 + 1 + 6 + 1 + 10 + 1 + w + 1 + 10;

  if (seqno == 0) {
    if (VERBOSE) {
      ajFmtPrintF(outf, "Alignment of sites with IC scores over %g:\n",
	threshold);
    } 

    for (i=0; i<line_len; i++)
	ajFmtPrintF(outf,"-");
    ajFmtPrintF(outf,"\n");

    ajFmtPrintF(outf,
      "\tPossible examples of motif %d in the training set\n", abs(imotif));

    for (i=0; i<line_len; i++)
	ajFmtPrintF(outf,"-");
    ajFmtPrintF(outf,"\n");

/*
    fprintf(outfile, "%-*.*s%s ", MSN, MSN, "-------------", strand2); 
    fprintf(outfile, "%6s %6s %10s %-*s %-10s\n", 
      "-----", "--", "---", w, "----", "----");
*/

    ajFmtPrintF(outf, "%-*.*s%s ", MSN, MSN, "Sequence name", strand1);
    ajFmtPrintF(outf, "%6s %6s %10s %*sSite%*s\n", 
      "Start", "Score", "", w/2 - 2, "", w - w/2 - 4, "");

    ajFmtPrintF(outf, "%-*.*s%s ", MSN, MSN, "-------------", strand2); 
    ajFmtPrintF(outf, "%6s %6s %10s ", "-----", "-----", "");

    for (i=0; i<w; i++)
	ajFmtPrintF(outf,"-");
    ajFmtPrintF(outf,"\n");
    
  }

  for (j=0; j <= lseq - w; j++) {	        /* site start */
    double score[4];				/* scores for diff directions */
    int lastk;

    /* compute the score for each direction of strand */
    for (k=0; k<4; k++) score[k] = 0;
    lastk = j+w;
    for (k=j, ii=0, jj=w-1; k<lastk; k++, ii++, jj--) {	/* position */
      int r = eseq[k];
      int rc = dna_comp(r);
      score[0] += logodds(ii, r);
      if (d[1]) score[1] += logodds(jj, r);
      if (d[2]) score[2] += logodds(jj, rc);
      if (d[3]) score[3] += logodds(ii, rc);
    }
    /*printf("%d %d %f\n", seqno, j, score[0]);*/

    for (k=0; k<4; k++) {

      if (d[k]) {
 
        /* save score */
        scores[nscores++] = score[k];

        /* print alignment if over threshold */
	if (score[k] > threshold) {
	  positives++;

	  if (seqno <= 0) {			/* print an alignment line */

	    ajFmtPrintF(outf, "%-*.*s%s ", MSN, MSN, sample_name, 
	      use_strands ? dir[k] : strand3);
            ajFmtPrintF(outf, "%6d %6.2f", j+1, score[k]);

            if (k==0 || k==3) {			/* w53 or c35 */
              /* left to right strands */
              for (i=j-10, ii=0; i<j; i++) {
                char r = eseq[i];
                if (i<0) continue;
                pre[ii++] = (k==0) ? unhash(r) : unhash(dna_comp(r));
              }
              pre[ii] = 0;
              for (i=j, ii=0; ii<w; i++) {
                char r = eseq[i];
                site[ii++] = (k==0) ? unhash(r) : unhash(dna_comp(r));
              }
              site[ii] = 0;
              for (i=j+w, ii=0; ii<10 && i<lseq; i++) {
                char r = eseq[i];
                post[ii++] = (k==0) ? unhash(r) : unhash(dna_comp(r));
              }
              post[ii] = 0;

            } else {
              /* right to left strands */
              for (i=j+w+9, ii=0; i>=j+w; i--) {
                char r = eseq[i];
                if (i>=lseq) continue;
                pre[ii++] = (k==1) ? unhash(r) : unhash(dna_comp(r));
              }
              pre[ii] = 0;
              for (i=j+w-1, ii=0; ii<w; i--) {
                char r = eseq[i];
                site[ii++] = (k==1) ? unhash(r) : unhash(dna_comp(r));
              }
              site[ii] = 0;
              for (i=j-1, ii=0; ii<10 && i>=0; i--) {
                char r = eseq[i];
                post[ii++] = (k==1) ? unhash(r) : unhash(dna_comp(r));
              }
              post[ii] = 0;
            }

	    ajFmtPrintF(outf, " %10s %-*s %-10s\n", pre, w, site, post);

	  } else {				/* print a .motif file line */
	    ajFmtPrintF(outf, "%d %d %s\n", seqno, j+1, sample_name);
	  }
        }
      }
    } 
  }

  /* last sample; print line of hyphens */
  if (imotif < 0) {
    for (i=0; i<line_len; i++)
	ajFmtPrintF(outf,"-");
    ajFmtPrintF(outf,"\n\n");
  }

  return positives;
}
/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/align.c,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
