/* motifnormal.h */

#ifndef PROFILENORMAL_H 
#define PROFILENORMAL_H 

#include "macros.h"
#include "nrutil.h"
#include "rtarray.h"
#include "curves.h"
#include "chi.h"
#include "user.h"

#define		ERROR 	0
#define 	NOERROR	1 

typedef struct {
        float SeqLen;		/* length of sequence */
        BOOLEAN Skip;		/* ignore sequence if true */
	long  fp;		/* file pointer to beginning of sequence rec. */
	float Mscores[MAXG];	/* maximum scores for each motif */
        float Score;		/* holds score for motif being normalized */
	float Calc;		/* expected score for sum of Mscores */
        float Zscore;		/* Z-score for sum of Mscores (MAXSUM) */
	float EVPvalue;		/* product of p-values for Mscores */
	float RVsum;		/* sum of reduced variates */
        BOOLEAN known_pos;	/* known positive if true (for ROC) */
        } SEQUENCE;

int motifnormal(    int mmode,	int sdmode,
                        int nmotifs,    float *mu,      float *sigma,   int *w,
			int *mn,
			FILE *DatFile, 	FILE *OutFile, 	FILE *ZScoreFile,
			int MinNorm, 	float PtCut,
			SEQUENCE *sequences, 	int Count,	BOOLEAN raw 
			);

BOOLEAN	MrqMin(	float	X[],	float	Y[],
		float	Sig[],	int	NData,
		float	A[],	int	MA,
		int	ListA[],int	MFit,
		float	**Covar,float	**Alpha,
		float	*ChiSq,
		float	*Alamda,float *delChiSq,
		FTYPE   Funcs);

void 	MrqCof(    float   X[],    float   Y[],
                float   Sig[],  int     NData,
                float   A[],    int     MA,
                int     ListA[],int     MFit,
                float   **Alpha,float   *Beta,
                float   *ChiSq,	FTYPE 	Funcs);

void Gaussj(    float   **A,    int     N,
                float   **B,    int     M ,
                BOOLEAN *MrqMin         );

void CovSrt(    float   **Covar,        int     MA,
                int     ListA[],        int     MFit );

#endif

/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/INCLUDE/motifnormal.h,v 1.1 2000/11/05 21:47:56 ajb Exp $ */
