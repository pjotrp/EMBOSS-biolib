#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ajxyz_h
#define ajxyz_h

#define XRAY  0    /*Structure was determined by X-ray crystallography*/
#define NMR   1    /*Structure was determined by NMR or is a model*/
#define OUT_BLK       75






typedef struct AjSAtom
{
  int        Mod;        /*Model number*/
  int        Chn;        /*Chain number*/
  char       Type;       /*'P' (protein atom), 'H' ("heterogens") or 'w' 
			   (water)*/
  int        Idx;        /*Residue number - index into sequence*/
  int        Pdb;        /*Residue number - according to original PDB file*/
  char       Id1;        /*Standard residue identifier or '?' for unknown 
			   types or '.' for heterogens and water*/
  AjPStr     Id3;        /*Residue or group identifier*/
  AjPStr     Atm;        /*Atom identifier*/
  float      X;          /*X coordinate*/
  float      Y;          /*Y coordinate*/
  float      Z;          /*Z coordinate*/
}AjSAtom, *AjPAtom;

typedef struct AjSChain
{
  char       Id;         /*Chain id, ('.' if one wasn't specified in the 
			   original PDB file)*/
  int        Nres;       /*No. of amino acid residues*/
  int        Nhet;       /*No. of atoms which are non-covalently associated 
			   with the chain, excluding water ("heterogens")*/
  int        Nwat;       /*No. of water atoms which are associated with the 
			   chain*/
  AjPStr     Seq;	 /* sequence as string */
  AjPList    Atoms;      /*List of Atoms */
  AjPInt     Models;     /*Offsets to the first first atom of each model*/
                         /*in the chain. */
}AjOChain, *AjPChain;



/* @data AjPPdb *******************************************************
**
** Ajax pdb object.
**
** Holds arrays describing pdb data
**
** AjPPdb is implemented as a pointer to a C data structure.
**
** @alias AjSPdb
** @alias AjOPdb
**
** @@
******************************************************************************/

typedef struct AjSPdb
{
  AjPStr     Pdb;        /*PDB code*/
  AjPStr     Compnd;     /*Text from COMPND records in PDB file*/
  AjPStr     Source;     /*Text from SOURCE records in PDB file*/
  int        Method;     /*Exp. type, value is either XRAY or NMR*/
  float      Reso;       /*Resolution of an XRAY structure or 0*/
  int        Nmod;       /*No. of models (always 1 for XRAY structures)*/
  int        Nchn;       /*No. polypeptide chains */
  AjPChain  *Chains;     /*Array of pointers to AjSChain structures*/
}AjOPdb, *AjPPdb;



AjPAtom  ajAtomNew(void);
void     ajAtomDel(AjPAtom *thys);
AjPChain ajChainNew(int n);
void     ajChainDel(AjPChain *thys);
AjPPdb   ajPdbNew(int n, int m);
void     ajPdbDel(AjPPdb *thys);

AjBool   ajCpdbRead(AjPStr str, AjPPdb *thys);
AjBool   ajCpdbWriteAll(AjPFile out, AjPPdb pdb);


#endif

#ifdef __cplusplus
}
#endif
