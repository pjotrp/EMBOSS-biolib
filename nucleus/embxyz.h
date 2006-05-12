#ifdef __cplusplus
extern "C"
{
#endif

#ifndef embxyz_h
#define embxyz_h




/*
** Prototype definitions
*/

AjBool embXyzSeqsetNR(const AjPList input, AjPInt *keep, ajint *nset,
		      const AjPMatrixf matrix, float gapopen, float gapextend,
		      float thresh);

AjBool embXyzSeqsetNRRange(const AjPList input, AjPInt *keep, ajint *nset,
			   const AjPMatrixf matrix,
			   float gapopen, float gapextend,
			   float thresh1, float thresh2);

/*
** End of prototype definitions
*/

#endif

#ifdef __cplusplus
}
#endif
