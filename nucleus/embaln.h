#ifdef __cplusplus
extern "C"
{
#endif

#ifndef embaln_h
#define embaln_h

#define PAZ  26
#define PAZ1 27

void embAlignCalcSimilarity(const AjPStr m, const AjPStr n,
			    const AjFloatArray *sub, const AjPSeqCvt cvt,
			    ajint lenm, ajint lenn, float *id, float *sim,
			    float *idx, float *simx);

void embAlignPathCalc(const char *a, const char *b,
		      ajint lena, ajint lenb, float gapopen,
		      float gapextend, float *path,
		      const AjFloatArray *sub, const AjPSeqCvt cvt,
		      ajint *compass, AjBool show);

void embAlignPathCalcSW(const char *a, const char *b, ajint lena, ajint lenb,
			float gapopen, float gapextend, float *path,
			const AjFloatArray *sub, const AjPSeqCvt cvt, ajint *compass,
			AjBool show);

void embAlignPrintGlobal(AjPFile outf, const char *a, const char *b,
			 const AjPStr m, const AjPStr n,
			 ajint start1, ajint start2, float score, AjBool mark,
			 const AjFloatArray *sub, const AjPSeqCvt cvt,
			 const char *namea,
			 const char *nameb, ajint begina, ajint beginb);
void embAlignPrintLocal(AjPFile outf, const char *a, const char *b,
			const AjPStr m, const AjPStr n,
			ajint start1, ajint start2, float score, AjBool mark,
			const AjFloatArray *sub, const AjPSeqCvt cvt,
			const char *namea,
			const char *nameb, ajint begina, ajint beginb);
void embAlignPrintProfile(AjPFile outf, const char *a, const char *b,
			  const AjPStr m, const AjPStr n,
			  ajint start1, ajint start2, float score, AjBool mark,
			  const AjFloatArray *fmatrix, const char *namea,
			  const char *nameb, ajint begina, ajint beginb);

void embAlignProfilePathCalc(const char *a,
			     ajint mlen, ajint slen, float gapopen,
			     float gapextend, float *path,
			     const AjFloatArray *fmatrix,
			     ajint *compass, AjBool show);

void embAlignReportGlobal (AjPAlign align,
			   const AjPSeq seqa, const AjPSeq seqb,
			   const AjPStr m, const AjPStr n,
			   ajint start1, ajint start2,
			   float gapopen, float gapextend,
			   float score, const AjPMatrixf matrix,
			   ajint offset1, ajint offset2);
void embAlignReportLocal (AjPAlign align,
			  const AjPSeq seqa, const AjPSeq seqb,
			  const AjPStr m, const AjPStr n,
			  ajint start1, ajint start2,
			  float gapopen, float gapextend,
			  float score, const AjPMatrixf matrix,
			  ajint offset1, ajint offset2);
void embAlignReportProfile(AjPAlign align, const AjPSeqset seqset,
			   const char *a, const char *b,
			   const AjPStr m, const AjPStr n,
			   ajint start1, ajint start2,
			   float score, AjBool mark,
			   const AjFloatArray *fmatrix, const char *namea,
			   const char *nameb, ajint begina, ajint beginb);

float embAlignScoreNWMatrix(float *path, const AjPSeq a, const AjPSeq b,
			    const AjFloatArray *fmatrix,
			    const AjPSeqCvt cvt, ajint lena, ajint lenb,
			    float gapopen, ajint *compass,
			    float gapextend, ajint *start1, ajint *start2);

float embAlignScoreProfileMatrix(float *path, ajint *compass, float gapopen,
				 float gapextend, const AjPStr b,
				 ajint clen, ajint slen, const AjFloatArray *fmatrix,
				 ajint *start1, ajint *start2);
float embAlignScoreSWMatrix(float *path, ajint *compass, float gapopen,
			    float gapextend,  const AjPSeq a, const AjPSeq b,
			    ajint lena, ajint lenb, const AjFloatArray *sub,
			    const AjPSeqCvt cvt, ajint *start1, ajint *start2);

void embAlignWalkNWMatrix(float *path, const AjPSeq a, const AjPSeq b,
			  AjPStr *m,
			  AjPStr *n, ajint lena, ajint lenb, ajint *start1,
			  ajint *start2, float gapopen,
			  float gapextend, const AjPSeqCvt cvt,
			  ajint *compass, const AjFloatArray *sub);
void embAlignWalkProfileMatrix(float *path, ajint *compass, float gapopen,
			       float gapextend,
			       const AjPStr cons, const AjPStr b,
			       AjPStr *m, AjPStr *n, ajint clen, ajint slen,
			       const AjFloatArray *fmatrix,
			       ajint *start1, ajint *start2);
void embAlignWalkSWMatrix(float *path, ajint *compass, float gapopen,
			  float gapextend, const AjPSeq a, const AjPSeq b,
			  AjPStr *m,
			  AjPStr *n, ajint lena, ajint lenb, const AjFloatArray *sub,
			  const AjPSeqCvt cvt, ajint *start1, ajint *start2);

void embAlignPathCalcFast(const char *a, const char *b, ajint lena, ajint lenb,
			  float gapopen, float gapextend, float *path,
			  const AjFloatArray *sub, const AjPSeqCvt cvt,
			  ajint *compass, AjBool show, ajint width);

float embAlignScoreSWMatrixFast(float *path, ajint *compass, float gapopen,
				float gapextend,
				const AjPSeq a, const AjPSeq b,
				ajint lena, ajint lenb, const AjFloatArray *sub,
				const AjPSeqCvt cvt,
				ajint *start1, ajint *start2,
				ajint width);

void embAlignWalkSWMatrixFast(float *path, ajint *compass, float gapopen,
			      float gapextend, const AjPSeq a, const AjPSeq b,
			      AjPStr *m,
			      AjPStr *n, ajint lena, ajint lenb,
			      const AjFloatArray *sub,
			      const AjPSeqCvt cvt,
			      ajint *start1, ajint *start2,
			      ajint width);

#endif

#ifdef __cplusplus
}
#endif
