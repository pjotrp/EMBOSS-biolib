#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ajmatrices_h
#define ajmatrices_h

/* @data AjPMatrix ************************************************************
**
** AJAX data structure for sequence comparison matrices with integer values
**
** @alias AjSMatrix
** @alias AjOMatrix
**
** @attr Size [ajint] Matrix size (size of 2D array)
** @attr Name [AjPStr] Matrix name
** @attr Codes [AjPStr*] Row/column codes
** @attr Matrix [ajint**] Matrix as 2D array
** @attr Cvt [AjPSeqCvt] Conversion table
** @@
******************************************************************************/

typedef struct AjSMatrix {
  ajint Size;
  AjPStr Name;
  AjPStr* Codes;
  ajint** Matrix;
  AjPSeqCvt Cvt;
} AjOMatrix;
#define AjPMatrix AjOMatrix*

/* @data AjPMatrixf ***********************************************************
**
** AJAX data structure for sequence comparison matrices with floating
** point values
**
** @alias AjSMatrixf
** @alias AjOMatrixf
**
** @attr Size [ajint] Matrix size (size of 2D array)
** @attr Name [AjPStr] Matrix name
** @attr Codes [AjPStr*] Row/column codes
** @attr Matrixf [float**] Matrix as 2D array
** @attr Cvt [AjPSeqCvt] Conversion table
** @@
******************************************************************************/

typedef struct AjSMatrixf {
  ajint Size;
  AjPStr Name;
  AjPStr* Codes;
  float** Matrixf;
  AjPSeqCvt Cvt;
} AjOMatrixf;
#define AjPMatrixf AjOMatrixf*

ajint**    ajMatrixArray (AjPMatrix thys);
void       ajMatrixChar (AjPMatrix thys, ajint i, AjPStr *label);
AjPSeqCvt  ajMatrixCvt (AjPMatrix thys);
void       ajMatrixDel (AjPMatrix *thys);
AjPStr     ajMatrixName (AjPMatrix thys);
AjPMatrix  ajMatrixNew (AjPStr* codes, ajint n, AjPStr filename);
AjBool     ajMatrixSeqNum (AjPMatrix thys, AjPSeq seq, AjPStr* numseq);
AjBool     ajMatrixRead (AjPMatrix* pthis, AjPStr filename);
ajint      ajMatrixSize (AjPMatrix thys);

float**    ajMatrixfArray (AjPMatrixf thys);
void       ajMatrixfChar (AjPMatrixf thys, ajint i, AjPStr *label);
AjPSeqCvt  ajMatrixfCvt (AjPMatrixf thys);
void       ajMatrixfDel (AjPMatrixf *thys);
AjPStr     ajMatrixfName (AjPMatrixf thys);
AjPMatrixf ajMatrixfNew (AjPStr* codes, ajint n, AjPStr filename);
AjBool     ajMatrixfSeqNum (AjPMatrixf thys, AjPSeq seq, AjPStr* numseq);
AjBool     ajMatrixfRead (AjPMatrixf* pthis, AjPStr filename);
ajint      ajMatrixfSize (AjPMatrixf thys);

AjBool     ajSeqsetConsStats(AjPSeqset thys, AjPMatrix mymatrix, AjPStr *cons,
			     ajint* retident, ajint* retsim, ajint* retgap,
			     ajint* retlen);

#endif

#ifdef __cplusplus
}
#endif
