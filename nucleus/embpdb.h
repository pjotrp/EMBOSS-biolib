/****************************************************************************
**
** @source embpdb.h
** 
** Algorithms for handling protein structural data.   
** For use with Atom, Chain and Pdb objects defined in ajpdb.h
** Also for use with Hetent, Het, Vdwres, Vdwall, Cmap and Pdbtosp objects 
** (also in ajpdb.h)
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

#ifndef embpdb_h
#define embpdb_h





/****************************************************************************
**
** @source embpdb.h
** 
** @source Algorithms for reading and writing ccf (clean coordinate file)
** format and for handling protein structural data.   
** For Atom, Chain and Pdb objects defined in ajpdb.h
** For Hetent, Het, Vdwres, Vdwall, Cmap and Pdbtosp objects (also in ajpdb.h)
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
***************************************************************************/





/* ======================================================================= */
/* =========================== Pdb object ================================ */
/* ======================================================================= */
AjBool       embAtomInContact(AjPAtom atm1, AjPAtom atm2, float thresh,
			       AjPVdwall vdw);
float        embAtomDistance(AjPAtom atm1, AjPAtom atm2, AjPVdwall vdw);
AjBool       embPdbAtomIndexI(AjPPdb pdb, ajint chn, AjPInt *idx);
AjBool       embPdbAtomIndexC(AjPPdb pdb, char chn, AjPInt *idx);
AjBool       embPdbAtomIndexICA(AjPPdb pdb, ajint chn, AjPInt *idx, 
				ajint *nres);
AjBool       embPdbAtomIndexCCA(AjPPdb pdb, char chn, AjPInt *idx, 
				ajint *nres);
AjBool       embPdbToIdx(ajint *idx, AjPPdb pdb, AjPStr res, ajint chn);
AjBool       embPdbListHeterogens(AjPPdb pdb, AjPList *list_heterogens, 
				  AjPInt *siz_heterogens, ajint *nhet, 
				  AjPFile logf );
AjBool       embPdbidToSp(AjPStr pdb, AjPStr *spr, AjPList list);
AjBool       embPdbidToAcc(AjPStr pdb, AjPStr *acc, AjPList list);
AjBool       embPdbidToScop(AjPPdb pdb, const AjPList list_allscop, 
			    AjPList *list_pdbscopids);
float        embVdwRad(AjPAtom atm, AjPVdwall vdw);





#endif

#ifdef __cplusplus
}
#endif







