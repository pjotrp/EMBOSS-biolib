#ifdef __cplusplus
extern "C"
{
#endif




#ifndef embcontact_h
#define embcontact_h

/* @data EmbPContact **********************************************************
**
** NUCLEUS contact between two residues in a structure (from a cmap file)
**
** Holds attributes of a single contact between two residues in a cmap file
**
** EmbPContact is implemented as a pointer to a C data structure.
**
** @alias EmbSContact
** @alias EmbOContact
**
** @new embContactNew default constructor
**
** @delete embContactDel default destructor
**
**
** @attr ajIntFirstPosition [ajint] position of first residue in contact
** @attr ajIntSecondPosition [ajint] position of second residue in contact
** @attr ajpStrFirstResType [AjPStr] 1st residue type in contact (3 lett code)
** @attr ajpStrSecondResType [AjPStr] 2nd residue type in contact (3 lett code)
** @attr fContactScore [AjPStr] frequency-based score of contact
** @@
******************************************************************************/


typedef struct EmbSContact
{
    ajint ajIntFirstPosition;
    ajint ajIntSecondPosition;
    AjPStr ajpStrFirstResType; 
    AjPStr ajpStrSecondResType;
    float fContactScore;
    
} EmbOContact;
#define EmbPContact EmbOContact*



/* @data EmbPCmapHeader *******************************************************
**
** NUCLEUS header of a cmap file
**
** holds elements of the header text in a cmap file
**
** EmbPCmapHeader is implemented as a pointer to a C data structure.
**
** @alias EmbSCmapHeader
** @alias EmbOCmapHeader
**
** @new embCmapHeaderNew default constructor
**
** @delete embCmapHeaderDel default destructor
**
**
** @attr ajpStrCmapId [AjPStr] text of protein ID code
** @attr ajpStrCmapDe [AjPStr] text of 
** @attr ajpStrCmapEx [AjPStr] 1st residue type in contact (3 lett code)
** @attr ajpStrCmapMo [AjPStr] MO line data
** @attr ajpStrCmapCn [AjPStr] CN line data
** @attr ajpStrCmapIn [AjPStr]  IN line data
** @attr ajpStrCmapSq [AjPStr]  Sequence data
** @@
******************************************************************************/



typedef struct EmbSCmapHeader
{
    AjPStr ajpStrCmapId; 
    AjPStr ajpStrCmapDe;
    AjPStr ajpStrCmapEx;
    AjPStr ajpStrCmapMo;
    AjPStr ajpStrCmapCn;
    AjPStr ajpStrCmapIn;
    AjPStr ajpStrCmapSq;
} EmbOCmapHeader;
#define EmbPCmapHeader EmbOCmapHeader*




/* ========================================================================= */
/* =================== All functions in alphabetical order ================= */
/* ========================================================================= */

EmbPContact embContactNew (void);

AjBool embContactWriteScoringMatrix (const AjPInt2d ajpInt2dCounts,
				     AjPFile ajpFileScoringMatrix);

ajint embAjintToScoringMatrixIndex (ajint ajIntResType);

ajint embCharToScoringMatrixIndex (char cResType);

ajint  embScoringMatrixIndexToChar (ajint ajIntBlosumIndex);

AjBool embString3ToAjint1 (const AjPStr ajpStrThreeLetterCode);

AjPStr embAjint1ToString3 (ajint ajIntCode);

AjBool embReadCmapFile (AjPFile ajpFileOriginalCmap,
			ajint ajIntSeqLen,
			AjPInt2d *AjPInt2dCmapSummary,
			EmbPCmapHeader *pEmbpFileOriginalCmapHeader,
			AjPInt2d *pAjPInt2dCmapResTypes,
			AjPInt2d *pAjPInt2dCmapPositions);

AjBool embReadAndReviseCmapFile (AjPFile ajpFileCmap,
				 ajint ajIntSeqLen,
				 AjPInt2d *pAjpInt2dCmapSummary,
				 EmbPCmapHeader *pAjpCmapHeader,
				 AjPInt2d *pAjpInt2dCmapResTypes,
				 AjPInt2d *pAjpInt2dCmapPositions,
				 char *pcUpdatedSeqAcross);

AjPStr embReadCmapLine (AjPFile ajpFileOriginalCmap);

ajint  embTypeCmapLine (const AjPStr ajpStrCmapLine);

ajint  embTypeCmapLine (const AjPStr ajpStrCmapLine);

AjBool embLoadHeaderLine (const AjPStr ajpStrCmapLine,
			  EmbPCmapHeader embpHeaderToLoad);

AjBool embLoadContactLine (const AjPStr pAjpStrCmapLine,
			   EmbPContact embpContactLoaded);

AjBool embWriteCmapFile (AjPFile ajpFileCmap,
			 ajint ajIntSeqLen,
			 AjPInt2d *pEmbpInt2dSummary,
			 EmbPCmapHeader *pEmbpFileCmapHeader,
			 AjPInt2d *pAjpInt2dCmapResTypes,
			 AjPInt2d *pAjpInt2dCmapPositions);

AjBool embWriteUpdatedCmapFile (AjPFile ajpFileUpdatedCmap,
				ajint ajIntSeqLen,
				AjPInt2d *pAjpInt2dSummary,
				EmbPCmapHeader *pEmbpFileUpdatedCmapHeader,
				AjPInt2d *pAjpInt2dCmapResTypes,
				AjPInt2d *pAjpInt2dCmapPositions,
				const AjPMatrixf ajpMatrixfContactScoring);

AjBool embPrintContact (const EmbPContact embpContactToPrint);

AjBool embWriteCmapHeader (AjPFile ajpFileUpdatedCmap,
			   const EmbPCmapHeader embpHeaderToWrite);

AjBool embWriteCmapLine (AjPFile ajpFileUpdatedCmap,
			 const AjPStr ajpStrLineToWrite);

void embWriteContact (AjPFile ajpFileUpdatedCmap,
		      const EmbPContact embpContactToWrite);

void embWriteUpdatedContact (AjPFile ajpFileUpdatedCmap,
			     const EmbPContact embpContactToWrite);

EmbPCmapHeader embCmapHeaderNew (void);

void embCmapHeaderDel (EmbPCmapHeader *pEmbpCmapHeaderToDelete);

AjPInt2d embGetIntMap (ajint ajIntAcrossSeqLen);

AjPInt2d embGetCmapSummary (const char *pcSeq);


/* DEBUGGING FUNCTIONS */
AjBool debug_cmap_header (const EmbPCmapHeader embpCmapHeader);


void debug_int_map (AjPInt2d *pAjpInt2dSummary,
		    AjPInt2d *ajpInt2dResTypes,
		    AjPInt2d *ajpInt2dPositions,
		    char *pcSeq,
		    ajint ajIntSeqLen);

void debug_cmap_summary (AjPInt2d *pAjpInt2dSummary,
			 ajint ajIntSeqLen);
#endif




#ifdef __cplusplus
}
#endif
