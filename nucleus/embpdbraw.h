/****************************************************************************
**
** @source embpdbraw.h
** 
** Algorithms for reading and writing PDB format.  
** For Pdb object defined in ajpdb.h
** 
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

#ifndef embpdbraw_h
#define embpdbraw_h





/* ======================================================================= */
/* =========================== Pdb object ================================ */
/* ======================================================================= */
AjPPdb  embPdbReadRawNew(AjPFile inf, AjPStr pdbid, ajint min_chain_size, 
			 ajint max_mismatch, AjBool camask, AjBool camask1, 
			 AjBool atommask, AjPFile logf);

AjBool  embPdbWriteAllRaw(ajint mode, AjPPdb pdb, AjPFile outf, 
			  AjPFile errf);

AjBool  embPdbWriteDomainRaw(ajint mode, AjPPdb pdb, AjPScop scop, 
			     AjPFile outf, AjPFile errf); 

AjBool  embPdbWriteRecordRaw(ajint mode, AjPPdb pdb, ajint mod, 
			     ajint chn, AjPFile outf, AjPFile errf); 

AjBool  embPdbWriteDomainRecordRaw(ajint mode, AjPPdb pdb, ajint mod,
				   AjPScop scop, AjPFile outf, AjPFile errf);





#endif

#ifdef __cplusplus
}
#endif

