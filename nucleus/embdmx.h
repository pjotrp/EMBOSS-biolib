/****************************************************************************
**
** @source embdmx.h
** 
** @source Algorithms for some of the DOMAINATRIX EMBASSY applications. 
** For Scophit and Scopalign objects defined in ajxyz.h
** The functionality will eventually be subsumed by other AJAX and NUCLEUS 
** libraries. 
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
****************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef embdmx_h
#define embdmx_h





AjBool        embDmxHitlistToScophits(AjPList in, AjPList *out);
AjBool        embDmxScophitsToHitlist(AjPList in, AjPHitlist *out, 
				      AjIList *iter);
AjBool        embDmxScophitToHit(AjPHit *to, AjPScophit from);
AjBool        embDmxScophitsAccToHitlist(AjPList in, AjPHitlist *out,   
					 AjIList *iter);
AjBool        embDmxHitsWrite(AjPFile outf, AjPHitlist hits, ajint maxhits);
AjBool        embDmxScopToScophit(AjPScop source, AjPScophit* target);
AjBool        embDmxScopalgToScop(AjPScopalg align, AjPScop *scop_arr, 
				  ajint dim, AjPList* list);
AjBool        embDmxScophitsOverlap(AjPScophit h1, AjPScophit h2, ajint n);
AjBool        embDmxScophitsOverlapAcc(AjPScophit h1, AjPScophit h2, ajint n);
AjPScophit    embDmxScophitMerge(AjPScophit hit1, AjPScophit hit2);
AjBool        embDmxScophitMergeInsertThis(AjPList list, AjPScophit hit1, 
					   AjPScophit hit2,  AjIList iter);
AjBool        embDmxScophitMergeInsertThisTarget(AjPList list, 
						 AjPScophit hit1, 
						 AjPScophit hit2,  
						 AjIList iter);
AjBool        embDmxScophitMergeInsertThisTargetBoth(AjPList list, 
						     AjPScophit hit1, 
						     AjPScophit hit2,  
						     AjIList iter);
AjBool        embDmxScophitMergeInsertOther(AjPList list, AjPScophit hit1, 
					    AjPScophit hit2);
AjBool        embDmxScophitMergeInsertOtherTargetBoth(AjPList list, 
						      AjPScophit hit1, 
						      AjPScophit hit2);
AjBool        embDmxScophitMergeInsertOtherTarget(AjPList list, 
						  AjPScophit hit1, 
						  AjPScophit hit2);
AjBool        embDmxScopalgWriteFasta(AjPScopalg align, AjPFile* outf);
AjBool        embDmxSeqNR(AjPList input, AjPInt *keep, ajint *nset,
			  AjPMatrixf matrix, float gapopen, float gapextend,
			  float thresh);
AjBool        embDmxSeqNRRange(AjPList input, AjPInt *keep, ajint *nset,
			       AjPMatrixf matrix, float gapopen, 
			       float gapextend,
			       float thresh1, float thresh2);

#endif

#ifdef __cplusplus
}
#endif
