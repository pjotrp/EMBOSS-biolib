#ifdef __cplusplus
extern "C"
{
#endif




#ifndef embcontact_h
#define embcontact_h

/* @data AjPContact **********************************************************
**
** NUCLEUS contact between two residues in a structure (from a cmap file)
**
** Holds attributes of a single contact between two residues in a cmap file
**
** AjPContact is implemented as a pointer to a C data structure.
**
** @alias AjSContact
** @alias AjOContact
**
** @new ajContactNew default constructor
**
** @delete ajContactDel default destructor
**
**
** @attr ajIntFirstPosition [ajint] position of first residue in contact
** @attr ajIntSecondPosition [ajint] position of second residue in contact
** @attr ajpStrFirstResType [AjPStr] 1st residue type in contact (3 lett code)
** @attr ajpStrSecondResType [AjPStr] 2nd residue type in contact (3 lett code)
** @@
******************************************************************************/


typedef struct AjContact
{
    ajint ajIntFirstPosition;
    ajint ajIntSecondPosition;
    AjPStr ajpStrFirstResType; 
    AjPStr ajpStrSecondResType;
} AjOContact;
#define AjPContact AjOContact*



/* @data AjPCmapHeader ********************************************************
**
** NUCLEUS header of a cmap file
**
** holds elements of the header text in a cmap file
**
** AjPCmapHeader is implemented as a pointer to a C data structure.
**
** @alias AjSCmapHeader
** @alias AjOCmapHeader
**
** @new ajCmapHeaderNew default constructor
**
** @delete ajCmapHeaderDel default destructor
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



typedef struct AjCmapHeader
{
    AjPStr ajpStrCmapId; 
    AjPStr ajpStrCmapDe;
    AjPStr ajpStrCmapEx;
    AjPStr ajpStrCmapMo;
    AjPStr ajpStrCmapCn;
    AjPStr ajpStrCmapIn;
    AjPStr ajpStrCmapSq;
} AjOCmapHeader;
#define AjPCmapHeader AjOCmapHeader*




/* ========================================================================= */
/* =================== All functions in alphabetical order ================= */
/* ========================================================================= */

AjPContact ajContactNew (void);

AjBool embContactWriteScoringMatrix (AjPInt2d ajpInt2dCounts,
				     AjPFile ajpFileScoringMatrix);

ajint embCharToScoringMatrixIndex (char cResType);

ajint embScoringMatrixIndexToChar (ajint ajIntBlosumIndex);

AjBool embString3ToAjint1 (AjPStr ajpStrThreeLetterCode);

AjPStr embAjint1ToString3 (ajint ajIntCode);

AjBool embReadCmapFile (AjPFile ajpFileOriginalCmap,
			ajint ajIntSeqLen,
			AjPInt2d *pAjPInt2dCmapSummary,
			AjPCmapHeader *pAjpFileOriginalCmapHeader,
			AjPInt2d *pAjPInt2dCmapResTypes,
			AjPInt2d *pAjPInt2dCmapPositions);

AjPStr embReadCmapLine (AjPFile ajpFileOriginalCmap);

ajint embTypeCmapLine (AjPStr ajpStrCmapLine);

AjBool embLoadHeaderLine (AjPStr *pAjpStrCmapLine,
			  AjPCmapHeader *pAjpHeaderToLoad);

AjBool embLoadContactLine (AjPStr *pAjpStrCmapLine,
			   AjPContact *pAjpContactLoaded);

AjBool embWriteCmapFile (AjPFile ajpFileUpdatedCmap,
			       ajint ajIntSeqLen,
			       AjPInt2d *pAjPInt2dSummary,
			       AjPCmapHeader *pAjpFileUpdatedCmapHeader,
			       AjPInt2d *pAjPInt2dCmapResTypes,
			       AjPInt2d *pAjPInt2dCmapPositions);

AjBool embPrintContact (AjPContact ajpContactToPrint);

AjBool embWriteCmapHeader (AjPFile ajpFileUpdatedCmap,
				 AjPCmapHeader ajpHeaderToWrite);

AjBool  embWriteCmapLine (AjPFile ajpFileUpdatedCmap,
			       AjPStr *pAjpStrLineToWrite);

void embWriteContact (AjPFile ajpFileUpdatedCmap,
			   AjPContact ajpContactToWrite);

AjPCmapHeader ajCmapHeaderNew ();

void ajCmapHeaderDel (AjPCmapHeader *pAjpCmapHeaderToDelete);

AjPInt2d embGetIntMap (ajint ajIntAcrossSeqLen);

AjPInt2d embGetCmapSummary (char *pcSeq);


/* DEBUGGING FUNCTIONS */
AjBool debug_cmap_header (AjPCmapHeader *pAjpCmapHeader);


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
