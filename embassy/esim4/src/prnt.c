/* MATHOG, replaced the ckprintf() stuff with fprintf(SIM4_OUTDEV, so that
   EMBOSS file redirection would work. ckprintf was only used in this one routine
   and seems to have been a holdover from some previous use. */
#ifdef EMBASSY
#include "emboss.h"
#endif
#include "libc.h"
#include "types.h"
#include "args.h"
#include "seq.h"
#include "dna.h"
#include "misc.h"
#include "prnt.h"

#ifndef __lint
static const char rcsid[] =
"$Id: prnt.c,v 1.3 2004/08/05 16:02:32 rice Exp $";
#endif

/* XXX */
extern FILE* SIM4_OUTDEV; /* set in sim4.init.c to stdout or by -outfile in EMBASSY mode */
static int offset1;
static int offset2;

enum { BUFSIZE=128 };

static char *subseq_label(char *buf, unsigned int size, int n);


#ifdef    EMBASSY

static const char* revflag(AjPSeq  s);
static const char* revlabel(AjPSeq  s);

#undef   SEQ_FROM
#undef   SEQ_TO
#undef   SEQ_HEAD
#undef   SEQ_NAME
#define  SEQ_FROM(s) ((s)->Offset)
#define  SEQ_TO(s)   ((s)->Offend)
#define  SEQ_HEAD(s) (MAJSTRSTR((s)->Name))
#define  SEQ_NAME(s) (MAJSTRSTR(ajSeqGetUsa(s)))

static void print_align_header_n(AjPSeq seq1, AjPSeq seq2, argv_scores_t *ds, int n)
{
        int f, t, F, T;
        char buf[BUFSIZE];

        (void) fprintf(SIM4_OUTDEV,"#:lav\n\nd {\n  \"");
        /* ck_argc("print_align_header"); */
        fprintf_argv(SIM4_OUTDEV);
        (void) fprintf(SIM4_OUTDEV,"\n   M = %d, I = %d, V = %d", ds->M, ds->I, ds->V);
        (void) fprintf(SIM4_OUTDEV,", O = %d, E = %g", ds->O, ds->E);
        (void) fprintf(SIM4_OUTDEV,"\"\n}\n");

        /* Mathog: these flags were not documented in the original, they are
           ignored if they are zero.  Map t,T to "to1,to2" and f,F to "offset1,offset2" 
           Probably there is some valid combination where some are zero and this will
           either throw the fatfail there. */
        t      = ajAcdGetInt ("toA");
        T      = ajAcdGetInt ("toB");
        f      = ajAcdGetInt ("offsetA");
        F      = ajAcdGetInt ("offsetB");
        if (f != 0){
                if (F == 0 || t == 0 || T == 0){
                    fatal("Inconsistent use of `offset1,2` and 'to1,2'.");
                }
                offset1 = SEQ_FROM(seq1) - f;
                offset2 = SEQ_FROM(seq2) - F;
        } else {
                f = SEQ_FROM(seq1); t = SEQ_TO(seq1);
                F = SEQ_FROM(seq2); T = SEQ_TO(seq2);
                offset1 = offset2 = 0;
        }
        (void) fprintf(SIM4_OUTDEV,"s {\n  \"%s%s\" %d %d\n  \"%s%s\" %d %d\n}\n",
                        SEQ_NAME(seq1), revflag(seq1), f, t,
                        SEQ_NAME(seq2), revflag(seq2), F, T);
        (void) fprintf(SIM4_OUTDEV,"h {\n   \"%s%s\"\n   \"%s%s%s\"\n}\n",
                SEQ_HEAD(seq1),
                revlabel(seq1),
                SEQ_HEAD(seq2),
                revlabel(seq2),
                subseq_label(buf, sizeof buf, n));
}

/* print_align_header  -------------  print the top part of an alignment file */
void print_align_header(AjPSeq seq1, AjPSeq seq2, argv_scores_t *ds)
{
	print_align_header_n(seq1, seq2, ds, 0);
}

#else  /* EMBASSY */

static const char* revflag(SEQ *s);
static const char* revlabel(SEQ *s);

static void print_align_header_n(SEQ *seq1, SEQ *seq2, argv_scores_t *ds, int n)
{
        int f, t, F, T;
        char buf[BUFSIZE];

        (void) fprintf(SIM4_OUTDEV,"#:lav\n\nd {\n  \"");
        ck_argc("print_align_header");
        fprintf_argv(SIM4_OUTDEV);
        (void) fprintf(SIM4_OUTDEV,"\n   M = %d, I = %d, V = %d", ds->M, ds->I, ds->V);
        (void) fprintf(SIM4_OUTDEV,", O = %d, E = %g", ds->O, ds->E);
        (void) fprintf(SIM4_OUTDEV,"\"\n}\n");

        if (get_argval('f', &f)) {
                if (!get_argval('t',&t) || !get_argval('F',&F) ||
                    !get_argval('T',&T))
                        fatal("Inconsistent use of `f`, `t`, `F', `T' args.");
                offset1 = SEQ_FROM(seq1) - f;
                offset2 = SEQ_FROM(seq2) - F;
        } else {
                f = SEQ_FROM(seq1); t = SEQ_TO(seq1);
                F = SEQ_FROM(seq2); T = SEQ_TO(seq2);
                offset1 = offset2 = 0;
        }
        (void) fprintf(SIM4_OUTDEV,"s {\n  \"%s%s\" %d %d\n  \"%s%s\" %d %d\n}\n",
                        SEQ_NAME(seq1), revflag(seq1), f, t,
                        SEQ_NAME(seq2), revflag(seq2), F, T);
        (void) fprintf(SIM4_OUTDEV,"h {\n   \"%s%s\"\n   \"%s%s%s\"\n}\n",
                SEQ_HEAD(seq1),
                revlabel(seq1),
                SEQ_HEAD(seq2),
                revlabel(seq2),
                subseq_label(buf, sizeof buf, n));
}

/* print_align_header  -------------  print the top part of an alignment file */
void print_align_header(SEQ *seq1, SEQ *seq2, argv_scores_t *ds)
{
	print_align_header_n(seq1, seq2, ds, 0);
}
#endif /* EMBASSY */


static char *subseq_label(char *buf, unsigned int size, int n)
{
    AjPStr buffer = NULL;
    

    
	assert(size > 0);

    buffer = ajStrNewC("");
    ajFmtPrintS(&buffer," (subsequence #%d)", n);
	buf[0] = 0;
    if (n > 0)
    {
	if(ajStrLen(buffer)>size)
	    strncpy(buf,ajStrStr(buffer),size);
	else
	    strcpy(buf,ajStrStr(buffer));
    }
    ajStrDel(&buffer);
    

    /* The following snprintf replaced by above code as Tru64 V4 doesn't have it */
    /* snprintf(buf, size, " (subsequence #%d)", n); */

	return buf;
}

#ifdef    EMBASSY
static const char* revflag(AjPSeq s)
{
	return (s->Rev) ? "-" : "";
}

static const char* revlabel(AjPSeq s)
{
	return (s->Rev) ? " (reverse complement)" : "";
}
#else  /* EMBASSY */
static const char* revflag(SEQ *s)
{
	return (s->flags & SEQ_IS_REVCOMP) ? "-" : "";
}

static const char* revlabel(SEQ *s)
{
	return (s->flags & SEQ_IS_REVCOMP) ? " (reverse complement)" : "";
}
#endif /* EMBASSY */


/* print_align  ----------------------------------- print a general alignment */
void print_align(int score, sim4_uchar *seq1, sim4_uchar *seq2, int beg1, int end1, int beg2, int end2,int *S)
{
        int M, N, i, j, op, start_i, start_j, match, run, pct;
        sim4_uchar *P, *p, *q;

        beg1 += offset1;
        end1 += offset1;
        beg2 += offset2;
        end2 += offset2;

        M = end1 - beg1 + 1;
        N = end2 - beg2 + 1;
        (void) fprintf(SIM4_OUTDEV,"a {\n  s %d\n  b %d %d\n  e %d %d\n",
                score, beg1, beg2, end1, end2);
        for (i = j = 0; i < M || j < N; ) {
                start_i = i;
                start_j = j;
                match = 0;
                P= p = seq1 + beg1 + i - 1;
                q = seq2 + beg2 + j - 1;
                while (i < M && j < N && *S == 0) {
                        if (*p++ == *q++)
                                ++match;
                        ++i;
                        ++j;
                        ++S;
                }
                run = p - P;
                pct = (run > 0) ? ((100*match + run/2)/run) : 0; /* round */
                (void) fprintf(SIM4_OUTDEV,"  l %d %d %d %d %d\n",
                        beg1+start_i, beg2+start_j, beg1+i-1, beg2+j-1, pct);
                if (i < M || j < N) {
                        if ((op = *S++) > 0) j += op; else i -= op;
                }       
        }       
        (void) fprintf(SIM4_OUTDEV,"}\n");
}
