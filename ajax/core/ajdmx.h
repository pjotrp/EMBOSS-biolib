/***************************************************************************
**
** @source ajdmx.h
**
** AJAX objects for some of the DOMAINATRIX EMBASSY applications. 
** Scophit and Scopalg objects. 
** 
** @author: Copyright (C) 2004 Ranjeeva Ranasinghe (rranasin@hgmp.mrc.ac.uk)
** @author: Copyright (C) 2004 Jon Ison (jison@hgmp.mrc.ac.uk)
** @version 1.0 
** @@
** 
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
** 
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
** 
** You should have received a copy of the GNU Library General Public
** License along with this library; if not, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
***************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ajdmx_h
#define ajdmx_h





/* @data AjPScophit *********************************************************
**
** Ajax Scophit object.
**
** Holds data associated with a protein / domain sequence that is generated 
** / manipulated by the EMBOSS applications psiblasts, swissparse, seqsort, 
** seqmerge and groups.  Includes SCOP classification records.
**
** AjPScophit is implemented as a pointer to a C data structure.
**
** @alias AjSScophit
** @alias AjOScophit
**
** @@
****************************************************************************/

typedef struct AjSScophit
{
    AjPStr    Class;
    AjPStr    Fold;
    AjPStr    Superfamily;
    AjPStr    Family;
    ajint     Sunid_Family; /* SCOP sunid for family */
    AjPStr    Seq;	  /* Sequence as string */
    ajint     Start;      /* Start of sequence or signature alignment 
			     relative to full length swissprot sequence */
    ajint     End;        /* End of sequence or signature alignment relative 
			     to full length swissprot sequence */
    AjPStr    Acc;        /* Accession number of sequence entry  */
    AjPStr    Spr;        /* Swissprot code of sequence entry */
    AjPStr    Typeobj;    /* Bibliographic information ... objective*/ 
    AjPStr    Typesbj;    /* Bibliographic information ... subjective */ 
    AjPStr    Model;      /* String for model type (HMM, Gribskov etc) */
    AjPStr    Group;      /* 'REDUNDANT' or 'NON_REDUNDANT' */
    ajint     Rank;       /* Rank order of hit */	
    float     Score;      /* Score of hit */
    float     Eval;       /* E-value of hit */
    float     Pval;       /* p-value of hit */
    AjPStr    Alg;        /* Alignment, e.g. of a signature to the 
			     sequence */
    AjBool    Target;     /* True if the Scophit is targetted for removal 
			     from a list of Scophit objects */
    AjBool    Target2;    /* Also used for garbage collection */
    AjBool    Priority;   /* True if the Scop hit is high priority. */
} AjOScophit, *AjPScophit;





/* @data AjPScopalg *********************************************************
**
** Ajax Scopalg object.
**
** Holds data associated with a structure alignment that is generated 
** by the EMBOSS applications scopalign.
**
** AjPScopalg is implemented as a pointer to a C data structure.
**
** @alias AjSScopalg
** @alias AjOScopalg
**
** @@
****************************************************************************/
typedef struct AjSScopalg
{
    AjPStr   Class;
    AjPStr   Fold;
    AjPStr   Superfamily;
    AjPStr   Family;
    ajint    Sunid_Family; /* SCOP sunid for family */
    ajint    width;        /* Width (residues) of widest part of alignment */
    ajint    N;            /* No. of sequences in alignment */
    AjPStr  *Codes;        /* Array of domain id codes of sequences */
    AjPStr  *Seqs;         /* Array of sequences */
    AjPStr   Post_similar; /* Post_similar line from alignment */
    AjPStr   Positions;    /* Array of integers from 'Position' line in 
			      alignment, used for manual specification of 
			      signature positions */
} AjOScopalg, *AjPScopalg;





/* ======================================================================= */
/* =========================== Scophit object ============================ */
/* ======================================================================= */
AjPScophit    ajDmxScophitNew(void);
void          ajDmxScophitDel(AjPScophit *pthis);
void          ajDmxScophitDelWrap(const void  **ptr);
AjPList       ajDmxScophitListCopy(AjPList ptr);

AjBool        ajDmxScophitCheckTarget(AjPScophit ptr);
AjBool        ajDmxScophitTarget(AjPScophit *h);
AjBool        ajDmxScophitTarget2(AjPScophit *h);
AjBool        ajDmxScophitTargetLowPriority(AjPScophit *h);

ajint         ajDmxScophitCompSpr(const void *hit1, const void *hit2);
ajint         ajDmxScophitCompStart(const void *hit1, const void *hit2);
ajint         ajDmxScophitCompEnd(const void *hit1, const void *hit2);
ajint         ajDmxScophitCompFold(const void *hit1, const void *hit2);
ajint         ajDmxScophitCompSfam(const void *hit1, const void *hit2);
ajint         ajDmxScophitCompFam(const void *hit1, const void *hit2);
ajint         ajDmxScophitCompAcc(const void *hit1, const void *hit2);
ajint         ajDmxScophitCompSunid(const void *entry1, const void *entry2);
ajint         ajDmxScophitCompScore(const void *hit1, const void *hit2); 
ajint         ajDmxScophitCompPval(const void *hit1, const void *hit2); 

AjBool        ajDmxScopSeqFromSunid(ajint id, AjPStr *seq, AjPList list);
AjBool        ajDmxScophitsWrite(AjPFile outf, AjPList list);
AjBool        ajDmxScophitCopy(AjPScophit *to, AjPScophit from);





/* ======================================================================= */
/* ========================== Scopalg object ============================= */
/* ======================================================================= */
AjBool        ajDmxScopalgRead(AjPFile inf, AjPScopalg *thys);
AjBool        ajDmxScopalgWrite(AjPFile outf, AjPScopalg scop);
AjBool        ajDmxScopalgWriteClustal(AjPScopalg align, AjPFile* outf);
AjBool        ajDmxScopalgWriteClustal2(AjPScopalg align, AjPFile* outf);
AjPScopalg    ajDmxScopalgNew(ajint n);
void          ajDmxScopalgDel(AjPScopalg *pthis);
ajint         ajDmxScopalgGetseqs(AjPScopalg thys, AjPStr **arr);






#endif

#ifdef __cplusplus
}
#endif



