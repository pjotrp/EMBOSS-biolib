#ifdef __cplusplus
extern "C"
{
#endif

#ifndef embxyz_h
#define embxyz_h

AjBool embXyzSeqsetNR(const AjPList input, AjPInt *keep, ajint *nset,
		      const AjPMatrixf matrix, float gapopen, float gapextend,
		      float thresh);

AjBool embXyzSeqsetNRRange(const AjPList input, AjPInt *keep, ajint *nset,
			   const AjPMatrixf matrix,
			   float gapopen, float gapextend,
			   float thresh1, float thresh2);

#endif

#ifdef __cplusplus
}
#endif
