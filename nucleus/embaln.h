#ifdef __cplusplus
extern "C"
{
#endif

#ifndef embaln_h
#define embaln_h

#define PAZ  26
#define PAZ1 27




/*
** Prototype definitions
*/

void embAlignCalcSimilarity(const AjPStr m, const AjPStr n,
			    const AjFloatArray *sub, const AjPSeqCvt cvt,
			    ajint lenm, ajint lenn, float *id, float *sim,
			    float *idx, float *simx);

float embAlignPathCalc(const char *a, const char *b,
                       ajint lena, ajint lenb, float gapopen,
                       float gapextend, float *path,
                       float * const *sub, const AjPSeqCvt cvt,
                       ajint *compass, AjBool show);

void embAlignPathCalcWithEndGapPenalties(const char *a, const char *b,
                       const ajint lena, const ajint lenb,
                       const float gapopen, const float gapextend,
                       const float endgapopen, const float endgapextend,
                       float *path, float * const *sub, const AjPSeqCvt cvt,
                       float *m, float *ix, float *iy,
                       ajint *compass, const AjBool show,
                       const AjBool endweight);

float embAlignPathCalcSW(const char *a, const char *b, ajint lena, ajint lenb,
                         float gapopen, float gapextend, float *path,
                         float * const *sub, const AjPSeqCvt cvt,
                         ajint *compass,
                         AjBool show);

void embAlignPrintGlobal(AjPFile outf, const char *a, const char *b,
			 const AjPStr m, const AjPStr n,
			 ajint start1, ajint start2, float score, AjBool mark,
			 float * const *sub, const AjPSeqCvt cvt,
			 const char *namea,
			 const char *nameb, ajint begina, ajint beginb);
void embAlignPrintLocal(AjPFile outf,
			const AjPStr m, const AjPStr n,
			ajint start1, ajint start2, float score, AjBool mark,
			float * const *sub, const AjPSeqCvt cvt,
			const char *namea,
			const char *nameb, ajint begina, ajint beginb);
void embAlignPrintProfile(AjPFile outf,
			  const AjPStr m, const AjPStr n,
			  ajint start1, ajint start2, float score, AjBool mark,
			  float * const *fmatrix, const char *namea,
			  const char *nameb, ajint begina, ajint beginb);

float embAlignProfilePathCalc(const char *a,
                              ajint mlen, ajint slen, float gapopen,
                              float gapextend, float *path,
                              float * const *fmatrix,
                              ajint *compass, AjBool show);

void embAlignReportGlobal (AjPAlign align,
			   const AjPSeq seqa, const AjPSeq seqb,
			   const AjPStr m, const AjPStr n,
			   ajint start1, ajint start2,
			   float gapopen, float gapextend,
			   float score, AjPMatrixf matrix,
			   ajint offset1, ajint offset2);
void embAlignReportLocal (AjPAlign align,
			  const AjPSeq seqa, const AjPSeq seqb,
			  const AjPStr m, const AjPStr n,
			  ajint start1, ajint start2,
			  float gapopen, float gapextend,
			  float score, AjPMatrixf matrix,
			  ajint offset1, ajint offset2);
void embAlignReportProfile(AjPAlign align,
			   const AjPStr m, const AjPStr n,
			   ajint start1, ajint start2,
			   float score,
			   const char *namea, const char *nameb);

/*float embAlignScoreProfileMatrix(const float *path, const ajint *compass,
				 float gapopen,
				 float gapextend, const AjPStr b,
				 ajint clen, ajint slen,
				 float * const *fmatrix,
				 ajint *start1, ajint *start2);
*/
void embAlignWalkNWMatrix(const float *path,
                          const AjPSeq a, const AjPSeq b,
			  AjPStr *m, AjPStr *n,
                          ajint lena, ajint lenb,
                          ajint *start1, ajint *start2,
                          float gapopen, float gapextend,
                          const ajint *compass);

float embAlignWalkNWMatrixUsingCompass(const float *path,
        const AjPSeq a, const AjPSeq b,
        AjPStr *m, AjPStr *n,
        const ajint lena, const ajint lenb,
        ajint *start1, ajint *start2,
        const ajint *compass, const AjBool endweight);

float embAlignGetScoreNWMatrix(const float *path,
        const ajint lena, const ajint lenb,
        ajint *xpos, ajint *ypos,
        const AjBool endweight);

void embAlignWalkProfileMatrix(const float *path, const ajint *compass,
			       float gapopen, float gapextend,
			       const AjPStr cons, const AjPStr b,
			       AjPStr *m, AjPStr *n,
                               ajint clen, ajint slen,
			       float * const *pmatrix,
			       ajint *start1, ajint *start2);

void embAlignWalkSWMatrix(const float *path, const ajint *compass,
			  float gapopen, float gapextend,
                          const AjPSeq a, const AjPSeq b,
			  AjPStr *m, AjPStr *n,
                          ajint lena, ajint lenb,
			  ajint *start1, ajint *start2);

float embAlignPathCalcSWFast(const char *a, const char *b,
                             ajint lena, ajint lenb,
                             ajint offset, ajint width,
                             float gapopen, float gapextend, float *path,
                             float * const *sub, const AjPSeqCvt cvt,
                             ajint *compass, AjBool show);

void embAlignWalkSWMatrixFast(const float *path, const ajint *compass,
			      float gapopen, float gapextend,
                              const AjPSeq a, const AjPSeq b,
			      AjPStr *m, AjPStr *n,
                              ajint lena, ajint lenb,
			      ajint offset, ajint width,
                              ajint *start1, ajint *start2);

void embAlignUnused(void);

/*
** End of prototype definitions
*/

#endif

#ifdef __cplusplus
}
#endif
