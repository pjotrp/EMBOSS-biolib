/****************************************************************************
**
** @source ajpdb.h
**
** AJAX objects for handling protein structural data.  
** Atom, Chain & Pdb objects.
** Hetent, Het, Vdwres, Vdwall, Cmap and Pdbtosp objects.
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

#ifndef ajpdb_h
#define ajpdb_h





/* @data AjPAtom ************************************************************
**
** Ajax atom object.
**
** Holds protein atom data
**
**  Variables for coordinate and related data.
** @attr  Mod     [ajint]  Model number.
** @attr  Chn     [ajint]  Chain number. 
** @attr  Gpn     [ajint]  Group number. 
** @attr  Type    [char]   'P' (protein atom), 'H' ("heterogens") or 'W' 
**                         (water).
** @attr  Idx     [ajint]  Residue number - index into sequence.
** @attr  Pdb     [AjPStr] Residue number - according to original PDB file.
** @attr  Id1     [char]   Standard residue identifier or 'X' for unknown 
**                         types or '.' for heterogens and water. 
** @attr  Id3     [AjPStr] Residue or group identifier. 
** @attr  Atm     [AjPStr] Atom identifier. 
** @attr  X       [float]  X coordinate. 
** @attr  Y       [float]  Y coordinate. 
** @attr  Z       [float]  Z coordinate. 
** @attr  O       [float]  Occupancy. 
** @attr  B       [float]  B value thermal factor. 
** @attr  Phi     [float]  Phi angle. 
** @attr  Psi     [float]  Psi angle. 
** @attr  Area    [float]  Residue solvent accessible area. 
**  
**
**  Variables for secondary structure from the PDB file.
** @attr  eNum    [ajint]  Element serial number.
** @attr  eId     [AjPStr] Element identifier. 
** @attr  eType   [char]   Element type COIL ('C'), HELIX ('H'), SHEET ('E')
**                         or TURN ('T'). Has a default value of COIL. 
** @attr  eClass  [ajint]  Class of helix, an int from 1-10,  from 
**	  http://www.rcsb.org/pdb/docs/format/pdbguide2.2/guide2.2_frame.html
**
**
**  Variables for data derived by using the STRIDE program.
** @attr  eStrideNum   [ajint]  Number of the element (sequential count from 
**                              N-term) 
** @attr  eStrideType  [char]   Element type:  ALPHA HELIX ('H'), 3-10 HELIX
**                              ('G'), PI-HELIX ('I'), EXTENDED CONFORMATION
**                              ('E'), ISOLATED BRIDGE ('B' or 'b'), TURN 
**                              ('T') or COIL (none of the above) ('C') (from 
**                              STRIDE).
**
** 
**  Variables for data derived by using the NACCESS program.
** @attr  all_abs   [float]  Absolute accessibility, all atoms. 
** @attr  all_rel   [float]  Relative accessibility, all atoms. 
** @attr  side_abs  [float]  Absolute accessibility, atoms in sidechain. 
** @attr  side_rel  [float]  Relative accessibility, atoms in sidechain. 
** @attr  main_abs  [float]  Absolute accessibility, atoms in mainchain. 
** @attr  main_rel  [float]  Relative accessibility, atoms in mainchain. 
** @attr  npol_abs  [float]  Absolute accessibility, nonpolar atoms. 
** @attr  npol_rel  [float]  Relative accessibility, nonpolar atoms. 
** @attr  pol_abs   [float]  Absolute accessibility, polar atoms. 
** @attr  pol_rel   [float]  Relative accessibility, polar atoms. 
**
** 
** AjPAtom is implemented as a pointer to a C data structure.
**
** @alias AjSAtom
** @alias AjOAtom
**
** @@
****************************************************************************/

typedef struct AjSAtom
{
    ajint   Mod;    
    ajint   Chn;    
    ajint   Gpn;    
    char    Type;  
    ajint   Idx;    
    AjPStr  Pdb;   
    char    Id1;   
    AjPStr  Id3;   
    AjPStr  Atm;   
    float   X;     
    float   Y;     
    float   Z;     
    float   O;     
    float   B;     
    float   Phi;   
    float   Psi;   
    float   Area;  

    ajint   eNum;  
    AjPStr  eId;   
    char    eType;    
    ajint   eClass;   
 
    ajint   eStrideNum; 
    char    eStrideType;

    float   all_abs;  
    float   all_rel;  
    float   side_abs;    
    float   side_rel;    
    float   main_abs;    
    float   main_rel;    
    float   npol_abs;    
    float   npol_rel;    
    float   pol_abs;  
    float   pol_rel;  
} AjOAtom, *AjPAtom;





/* @data AjPChain ***********************************************************
**
** Ajax chain object.
**
** Holds protein chain data
**
** @attr  Id          [char]     Chain id, ('.' if one wasn't specified in 
**                               the PDB file).
** @attr  Nres        [ajint]    No. of amino acid residues.
** @attr  Nlig        [ajint]    No. of groups which are non-covalently 
**                               associated with the chain, excluding water 
**                               ("heterogens").
** @attr  numHelices  [ajint]    No. of helices in the chain according to the
**                               PDB file.
** @attr  numStrands  [ajint]    No. of strands in the chain according to the
**                               PDB file.
** @attr  Seq         [AjPStr]   Protein sequence as string.
** @attr  Atoms       [AjPList]  List of Atom objects for (potentially multiple
**                               models) of the polypeptide chain and any 
**                               groups (ligands) that could be uniquely 
**                               associated with a chain.
**
** AjPChain is implemented as a pointer to a C data structure.
**
** @alias AjSChain
** @alias AjOChain
**
** @@
****************************************************************************/

typedef struct AjSChain
{
    char     Id;       
    ajint    Nres;   
    ajint    Nlig;   
    ajint    numHelices; 
    ajint    numStrands;  
    AjPStr   Seq;      
    AjPList  Atoms;    
} AjOChain, *AjPChain;





/* @data AjPPdb *************************************************************
**
** Ajax pdb object.
**
** Holds arrays describing pdb data
**
** @attr  Pdb     [AjPStr]  PDB code.
** @attr  Compnd  [AjPStr]  Text from COMPND records in PDB file.
** @attr  Source  [AjPStr]  Text from SOURCE records in PDB file.
** @attr  Method  [ajint]   Experiment type, either XRAY or NMR. 
** @attr  Reso    [float]   Resolution of an XRAY structure or 0. 
** @attr  Nmod    [ajint]   No. of models (always 1 for XRAY structures).
** @attr  Nchn    [ajint]   No. polypeptide chains.
** @attr  Chains  [AjPStr]  Array of pointers to AjSChain structures.
** @attr  Ngp     [ajint]   No. groups that could not be uniquely 
**                          associated with a chain in the SEQRES records.
** @attr  gpid    [AjPChar] Array of chain (group) id's for groups that 
**                          could not be uniquely associated with a chain.
** @attr  Groups  [AjPList] List of Atom objects for groups that could not 
**                          be uniquely associated with a chain.
** @attr  Water   [AjPList] List of Atom objects for water molecules.
**
** AjPPdb is implemented as a pointer to a C data structure.
**
** @alias AjSPdb
** @alias AjOPdb
**
** @@
****************************************************************************/

typedef struct AjSPdb
{
    AjPStr     Pdb;       
    AjPStr     Compnd;    
    AjPStr     Source;    
    ajint      Method;    
    float      Reso;      
    ajint      Nmod;      
    ajint      Nchn;      
    AjPChain  *Chains;    
    ajint      Ngp;       
    AjPChar    gpid;	
    AjPList    Groups;    
    AjPList    Water;     
}AjOPdb, *AjPPdb;





/* @data AjPHetent **********************************************************
**
** Ajax Hetent object.
**
** Holds a single entry from a dictionary of heterogen groups.
**
** @attr  abv [AjPStr]  3-letter abbreviation of heterogen.
** @attr  syn [AjPStr]  Synonym.
** @attr  ful [AjPStr]  Full name. 
** @attr  cnt [ajint]   No. of occurences (files) of this heterogen in a 
**                      directory.
** 
** AjPHetent is implemented as a pointer to a C data structure.
**
** @alias AjSHetent
** @alias AjOHetent
**
** @@
****************************************************************************/
typedef struct AjSHetent
{
    AjPStr   abv;   
    AjPStr   syn;   
    AjPStr   ful;   
    ajint    cnt;   
} AjOHetent, *AjPHetent;





/* @data AjPHet *************************************************************
**
** Ajax Het object.
** Holds a dictionary of heterogen groups.
**
** @attr  n        [ajint]     Number of entries.
** @attr  entries  [AjPHetent] Array of entries. 
**
** AjPHet is implemented as a pointer to a C data structure.
**
** @alias AjSHet
** @alias AjOHet
**
** @@
****************************************************************************/
typedef struct AjSHet
{
    ajint         n;   
    AjPHetent *entries;
} AjOHet, *AjPHet;





/* @data AjPVdwres **********************************************************
**
** Ajax Vdwres object.
**
** Holds the Van der Waals radius for atoms in a residue 
**
** @attr  Id1  [char]   Standard residue identifier or 'X' for unknown.
** @attr  Id3  [AjPStr] 3 character residue identifier.
** @attr  N    [ajint]  Nummber of atoms in residue. 
** @attr  Atm  [AjPStr] Array of atom identifiers.
** @attr  Rad  [float]  Array of van der Waals radii.
** 
** AjPVdwres is implemented as a pointer to a C data structure.
**
** @alias AjSVdwres
** @alias AjOVdwres
**
** @@
****************************************************************************/
typedef struct AjSVdwres
{
    char     Id1;     
    AjPStr   Id3;     
    ajint    N;       
    AjPStr  *Atm;     
    float   *Rad;     
} AjOVdwres, *AjPVdwres;





/* @data AjPVdwall **********************************************************
**
** Ajax Vdwall object.
**
** Holds the Van der Waals radii for all types of protein atoms
**
** @attr  N    [ajint]      Number of residues.
** @attr  Res  [AjPVdwres]  Array of Vdwres structures.
** 
** AjPVdwall is implemented as a pointer to a C data structure.
**
** @alias AjSVdwall
** @alias AjOVdwall
**
** @@
****************************************************************************/
typedef struct AjSVdwall
{
    ajint       N;    
    AjPVdwres  *Res;  
} AjOVdwall, *AjPVdwall;





/* @data AjPCmap ************************************************************
**
** Ajax Cmap object.
**
** Holds a contact map and associated data for a protein domain / chain.
**
** @attr  Id    [AjPStr]   Protein id code. 
** @attr  Seq   [AjPStr]   The sequence of the domain or chain. 
** @attr  Mat   [AjPInt2d] Contact map. 
** @attr  Dim   [ajint]    Dimension of contact map. 
** @attr  Ncon  [ajint]    No. of contacts (1's in contact map). 
** 
** AjPCmap is implemented as a pointer to a C data structure.
**
** @alias AjSCmap
** @alias AjOCmap
**
** @@
****************************************************************************/
typedef struct AjSCmap
{
    AjPStr    Id;     
    AjPStr    Seq;    
    AjPInt2d  Mat;    
    ajint     Dim;    
    ajint     Ncon;   
} AjOCmap, *AjPCmap;





/* @data AjPPdbtosp *******************************************************
**
** Ajax Pdbtosp object.
**
** Holds swissprot codes and accession numbers for a PDB code.
**
** @attr  Pdb  [AjPStr]   PDB code
** @attr  n    [ajint]    No. entries for this pdb code 
** @attr  Acc  [AjPStr*]  Accession numbers 
** @attr  Spr  [AjPStr*]  Swissprot codes 
** 
** AjPPdbtosp is implemented as a pointer to a C data structure.
**
** @alias AjSPdbtosp
** @alias AjOPdbtosp
**
** @@
****************************************************************************/

typedef struct AjSPdbtosp
{   	
    AjPStr   Pdb;
    ajint    n;  
    AjPStr  *Acc;
    AjPStr  *Spr;
} AjOPdbtosp, *AjPPdbtosp;





/* ======================================================================= */
/* ============================ Het objects ============================== */
/* ======================================================================= */
AjPHet       ajHetNew(ajint n);
void         ajHetDel(AjPHet *ptr);





/* ======================================================================= */
/* ============================ Hetent object ============================ */
/* ======================================================================= */
AjPHetent    ajHetentNew(void);
void         ajHetentDel(AjPHetent *ptr);





/* ======================================================================= */
/* ============================ Vdwall object ============================ */
/* ======================================================================= */
AjPVdwall    ajVdwallNew(ajint n);
void         ajVdwallDel(AjPVdwall *ptr);





/* ======================================================================= */
/* ============================ Vdwres object ============================ */
/* ======================================================================= */
AjPVdwres    ajVdwresNew(ajint n);
void         ajVdwresDel(AjPVdwres *ptr);





/* ======================================================================= */
/* ============================ Atom object ============================== */
/* ======================================================================= */
AjPAtom      ajAtomNew(void);
void         ajAtomDel(AjPAtom *ptr);
AjBool       ajAtomCopy(AjPAtom *to, AjPAtom from);
AjBool       ajAtomListCopy(AjPList *to, AjPList from);





/* ======================================================================= */
/* ============================ Cmap object ============================== */
/* ======================================================================= */
AjPCmap      ajCmapNew(ajint n);
void         ajCmapDel(AjPCmap *ptr);





/* ======================================================================= */
/* ============================ Pdbtosp object =========================== */
/* ======================================================================= */
AjPPdbtosp   ajPdbtospNew(ajint n);
void         ajPdbtospDel(AjPPdbtosp *ptr);
ajint        ajPdbtospArrFindPdbid(AjPStr id, AjPPdbtosp *arr, ajint siz);





/* ======================================================================= */
/* ============================ Chain object ============================= */
/* ======================================================================= */
AjPChain     ajChainNew(void);
void         ajChainDel(AjPChain *ptr);





/* ======================================================================= */
/* ============================ Pdb object =============================== */
/* ======================================================================= */
AjPPdb       ajPdbNew(ajint n);
void         ajPdbDel(AjPPdb *ptr);
AjBool       ajPdbCopy(AjPPdb *to, AjPPdb from);
AjBool       ajPdbChnidToNum(char id, AjPPdb pdb, ajint *chn);





#endif

#ifdef __cplusplus
}
#endif

