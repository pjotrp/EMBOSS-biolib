/* ProfileNormal.h */

#ifndef PROFILENORMAL_H 
#define PROFILENORMAL_H 

#include <macros.h>
#include <nrutil.h>
#include <rtarray.h>
#include <curves.h>
#include <chi.h>

#define		ERROR 	0
#define 	NOERROR	1 

typedef struct {
        float Score;
	float Calc;
        float SeqLen;
        float Zscore;
	float EVPvalue;		/* p-value based on Gaussian extreme value */
        int   Order;
	long  fp;		/* file pointer to beginning of sequence */
        } SEQUENCE;

BOOLEAN ProfileNormal(  int mmode,	int sdmode,
                        int nmotifs,    float *mu,      float *sigma,   int *w,
			FILE *DatFile, 	FILE *OutFile, 	FILE *ZScoreFile,
			int MinNorm, 	float PtCut,
			SEQUENCE *sequences, 	int Count      
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

/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/INCLUDE/ProfileNormal.h,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
