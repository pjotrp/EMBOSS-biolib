/****************************************************************************
**
** @source embdomain.h
**
** Algorithms for reading SCOP and CATH parsable files, for reading and 
** writing dcf (domain classification file) format and for handling protein
** domain data.
** For Scop and Cath objects defined in ajdomain.h
** 
** Copyright (c) 2004 Jon Ison (jison@hgmp.mrc.ac.uk) 
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
**
****************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef embdomain_h
#define embdomain_h





/* ======================================================================= */
/* ========================== Cath object ================================ */
/* ======================================================================= */
AjPCath  embCathReadCNew(AjPFile inf, char *entry);
AjPCath  embCathReadNew(AjPFile inf, AjPStr entry);
AjPList  embCathReadAllNew(AjPFile inf); 
AjPList  embCathReadAllRawNew(AjPFile cathf, AjPFile domf, 
			      AjPFile namesf, AjPFile logf);
AjBool   embCathWrite(AjPFile outf, AjPCath obj);





/* ======================================================================= */
/* ========================== Scop object ================================ */
/* ======================================================================= */
AjPScop  embScopReadCNew(AjPFile inf, char *entry);
AjPScop  embScopReadNew(AjPFile inf, AjPStr entry);
AjPList  embScopReadAllNew(AjPFile inf); 
AjPList  embScopReadAllRawNew(AjPFile claf, AjPFile desf, AjBool outputall);
AjBool   embScopWrite(AjPFile outf, AjPScop obj);
AjPStr   embScopToPdbid(AjPStr scop, AjPStr *pdb);
AjBool   embScopToSp(AjPStr scop, AjPStr *spr, AjPList list);
AjBool   embScopToAcc(AjPStr scop, AjPStr *acc, AjPList list);





#endif

#ifdef __cplusplus
}
#endif














