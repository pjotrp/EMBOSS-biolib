#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ajxyz_h
#define ajxyz_h

#define ajXRAY  0    /* Structure was determined by X-ray crystallography */
#define ajNMR   1    /* Structure was determined by NMR or is a model     */


#define ajESCOP "Escop.dat" /* Scop data file */




/* @data AjPDomConts **********************************************************
**
** Ajax DomConts object.
**
** Holds the domain contact data
**
** AjPDomConts is implemented as a pointer to a C data structure.
**
** @alias AjSDomConts
** @alias AjODomConts
**
** @attr het_name [AjPStr] 3-character code of heterogen 
** @attr scop_name [AjPStr] 7-character scop id domain name
** @attr no_keyres [ajint] number of key binding residues
** @attr aa_code [AjPStr*] Array for 3-character amino acid codes
** @attr res_pos [AjPInt] Array of ints for residue positions in domain file
** @attr res_pos2 [AjPStr*] Array of residue positions in complete protein 
**			    coordinate file - exist as strings
** @@
******************************************************************************/

typedef struct AjSDomConts
{
  AjPStr het_name;
  AjPStr scop_name;
  ajint  no_keyres;
  AjPStr *aa_code;
  AjPInt res_pos;
  AjPStr *res_pos2;
}AjODomConts, *AjPDomConts;


/* @data AjPDbaseEnt **********************************************************
**
** Ajax DbaseEnt object.
**
** Holds the data required for the database of functional sites
**
** AjPDbaseDat is implemented as a pointer to a C data structure.
**
** @alias AjSDbaseEnt
** @alias AjODbaseEnt
**
** @attr abv [AjPStr] 3-letter abbreviation of heterogen
** @attr ful [AjPStr] Full name
** @attr no_dom [ajint] number of domains
** @attr cont_data [AjPDomConts*] array of domain contact data
**                               (derived from tmp)
** @attr tmp [AjPList]  Temp. list of domain contact data
** @@
******************************************************************************/

typedef struct AjSDbaseEnt
{
  AjPStr      abv;
  AjPStr      ful;
  ajint       no_dom;
  AjPDomConts *cont_data;
  AjPList     tmp;
} AjODbaseEnt, *AjPDbaseEnt;


/* @data AjPDbase ***********************************************************
**
** Ajax Dbase object.
**
** Holds a Database of functional residues.
**
** AjPDbase is implemented as a pointer to a C data structure.
**
** @alias AjSDbase
** @alias AjODbase  
**
** @attr n [ajint] Number of entries
** @attr entries [AjPDbaseEnt*] Array of entries
** @@
******************************************************************************/

typedef struct AjSDbase
{
  ajint         n;
  AjPDbaseEnt *entries;
} AjODbase, *AjPDbase;




/* @data AjPCath **************************************************************
**
** Ajax cath object
**
** Holds cath database data
**
** @alias AjSCath
** @alias AjOCath
**
** The variables have the following meaning:
**
** @attr DomainID  [AjPStr]      Domain identifer code        
** @attr Pdb        [AjPStr]     Corresponding pdb identifer code
** @attr Class     [AjPStr]      CATH class name as an AjPStr
** @attr Architecture [AjPStr]    CATH architecture name as an AjPStr
** @attr Topology    [AjPStr]   CATH topology name as an AjPStr
** @attr Superfamily [AjPStr]    CATH homologous superfamily name as an AjPStr
** @attr Length  [ajint]       No. of residues in domain
** @attr Chain  [char]        Chain identifier
** @attr NSegment [ajint]      No. of chain segments domain is comprised of
** @attr Start  [AjPStr*]      PDB residue number of first residue in segment
**                             or '.' 
** @attr End   [AjPStr*]        PDB residue number of last residue in segment
**                             or '.' 
** @attr Class_Id  [ajint]      CATH class no. as an ajint
** @attr Arch_Id  [ajint]       CATH architecture no.as an ajint
** @attr Topology_Id [ajint]    CATH topology no. as an ajint
** @attr Superfamily_Id [ajint] CATH superfamily no. as an ajint
** @attr Family_Id  [ajint]     CATH family no. as an ajint 
** @attr NIFamily_Id  [ajint]  CATH near identical family no. as an ajint 
** @attr IFamily_Id  [ajint]   CATH identical family no. as an ajint 
**
**  @@
******************************************************************************/

typedef struct AjSCath
{
    AjPStr DomainID;       
    AjPStr Pdb;            
    AjPStr Class;          
    AjPStr Architecture;   
    AjPStr Topology;       
    AjPStr Superfamily;    
    
    ajint  Length;         
    char   Chain;          
    
    ajint  NSegment;       
    AjPStr *Start;
    AjPStr *End;          
    
    ajint Class_Id;        
    ajint Arch_Id;         
    ajint Topology_Id;     
    ajint Superfamily_Id;  
    ajint Family_Id;      
    ajint NIFamily_Id;     
    ajint IFamily_Id;     
} AjOCath, *AjPCath;


/* @data AjPPdbtosp *******************************************************
**
** Ajax Pdbtosp object.
**
** Holds swissprot codes and accession numbers for a PDB code.
**
** AjPPdbtosp is implemented as a pointer to a C data structure.
**
** @alias AjSPdbtosp
** @alias AjOPdbtosp
**
**
** @attr Pdb [AjPStr] PDB code
** @attr n [ajint] No. entries for this pdb code
** @attr Acc [AjPStr*] Accession numbers
** @attr Spr [AjPStr*] Swissprot codes
** @@
******************************************************************************/

typedef struct AjSPdbtosp
{   	
    AjPStr     Pdb;
    ajint      n;
    AjPStr    *Acc;
    AjPStr    *Spr;
} AjOPdbtosp, *AjPPdbtosp;




/* @data AjPScorealg *******************************************************
**
** Ajax Scorealg object.
**
** Holds scores associated with the 5 scoring methods used in siggen
**
** AjPScorealg is implemented as a pointer to a C data structure.
**
** @alias AjSScorealg
** @alias AjOScorealg
**
** @attr seqmat_score [AjPFloat]  Array of scores based on residue convervation
** @attr seqvar_score [AjPFloat] Array of scores based on residue variability
** @attr post_similar [AjPInt] Array of scores based on stamp pij value
** @attr positions [AjPInt] Array of integers from 'Position' line in
**                          alignment, used for manual specification of
**                          signature positions
** @attr ncon_score [AjPFloat] Array of scores based on number of contacts
** @attr ccon_score [AjPFloat] Array of scores based on convervation of
**                           contacts
** @attr nccon_score [AjPInt] Array of total score based on convervation
**                            and number of contacts
** @attr combi_score [AjPInt] Array of total score based on users scoring
**                             criteria
** @attr ncon_thresh [AjPInt] Array of positions with > threshold number
**                              of contacts
** @attr seqmat_do [AjBool] Whether to use score based on residue convervation
** @attr seqvar_do [AjBool] Whether to use score based on residue variablility
** @attr filterpsim [AjBool] Whether to filter on basis of post_similar line
** @attr filtercon [AjBool] Whether to filter on basis of number of contacts
** @attr conthresh [ajint] Threshold number of contacts for filtercon
** @attr ncon_do [AjBool] Whether to use score based on number of contacts
** @attr ccon_do [AjBool] Whether to use score based on convervation
**                              of contacts
** @attr nccon_do [AjBool] Whether to use score based on convervation
**                               and number of contacts
** @attr random [AjBool] Whether to generate a randomised signature
** @attr manual [AjBool] Whether signature positions were taken from
**                              alignment file (manual selection)
** @@
******************************************************************************/
typedef struct AjSScorealg
{   
    AjPFloat  seqmat_score;
    AjPFloat  seqvar_score;
    AjPInt    post_similar;
    AjPInt    positions;
    AjPFloat  ncon_score;
    AjPFloat  ccon_score;
    AjPInt    nccon_score;
    AjPInt    combi_score;
    AjPInt    ncon_thresh;
    AjBool    seqmat_do;
    AjBool    seqvar_do;
    AjBool    filterpsim;
    AjBool    filtercon;
    ajint     conthresh;
    AjBool    ncon_do;
    AjBool    ccon_do;
    AjBool    nccon_do;

    AjBool    random;
    AjBool    manual;
    
} AjOScorealg, *AjPScorealg;



/* @data AjPVdwres *******************************************************
**
** Ajax Vdwres object.
**
** Holds the Van der Waals radius for atoms in a residue 
**
** AjPVdwres is implemented as a pointer to a C data structure.
**
** @alias AjSVdwres
** @alias AjOVdwres
**
** @attr Id1 [char] Standard residue identifier or 'X' for unknown
** @attr Id3 [AjPStr] 3 character residue identifier
** @attr N [ajint] Number of atoms in residue
** @attr Atm [AjPStr*] Array of atom identifiers
** @attr Rad [float*] Array of van der Waal's radii
** @@
******************************************************************************/
typedef struct AjSVdwres
{
    char       Id1;
    AjPStr     Id3;
    ajint      N;
    AjPStr    *Atm;
    float     *Rad;
} AjOVdwres, *AjPVdwres;







/* @data AjPVdwall *******************************************************
**
** Ajax Vdwall object.
**
** Holds the Van der Waals radii for all types of protein atoms
**
** AjPVdwall is implemented as a pointer to a C data structure.
**
** @alias AjSVdwall
** @alias AjOVdwall
**
** @attr N [ajint] Number of residues
** @attr Res [AjPVdwres*] Array of Vdwres structures
** @@
******************************************************************************/
typedef struct AjSVdwall
{
    ajint       N;
    AjPVdwres  *Res;
} AjOVdwall, *AjPVdwall;






/* @data AjPCmap *******************************************************
**
** Ajax Cmap object.
**
** Holds a contact map and associated data for a protein domain / chain.
**
** AjPCmap is implemented as a pointer to a C data structure.
**
** @alias AjSCmap
** @alias AjOCmap
**
** @attr Id [AjPStr] Protein id code
** @attr Seq [AjPStr] The sequence of the domain or chain
** @attr Mat [AjPInt2d] Contact map
** @attr Dim [ajint] Dimension of contact map
** @attr Ncon [ajint] No. of contacts (1's in contact map)
** @@
******************************************************************************/
typedef struct AjSCmap
{
    AjPStr      Id;
    AjPStr      Seq;
    AjPInt2d    Mat;
    ajint       Dim;
    ajint       Ncon;
} AjOCmap, *AjPCmap;





/* @data AjPScopalg *******************************************************
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
** @attr Class [AjPStr] SCOP class
** @attr Fold [AjPStr] SCOP fold
** @attr Superfamily [AjPStr] SCOP superfamily
** @attr Family [AjPStr] SCOP family
** @attr Sunid_Family [ajint] SCOP sunid for family
** @attr width [ajint] Width (residues) of widest part of alignment
** @attr N [ajint] No. of sequences in alignment
** @attr Codes [AjPStr*] Array of domain id codes of sequences
** @attr Seqs [AjPStr*] Array of sequences
** @attr Post_similar [AjPStr] Post_similar line from alignment
** @attr Positions [AjPStr] Array of integers from 'Position' line in
**                          alignment, used for manual specification
**                          of signature positions
** @@
******************************************************************************/
typedef struct AjSScopalg
{
    AjPStr   Class;
    AjPStr   Fold;
    AjPStr   Superfamily;
    AjPStr   Family;
    ajint    Sunid_Family;
    ajint    width;
    ajint    N;
    AjPStr  *Codes;
    AjPStr  *Seqs;
    AjPStr   Post_similar;
    AjPStr   Positions;
} AjOScopalg, *AjPScopalg;






/* @data AjPScophit *******************************************************
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
** @attr Class [AjPStr] SCOP class
** @attr Fold [AjPStr] SCOP fold
** @attr Superfamily [AjPStr] SCOP superfamily
** @attr Family [AjPStr] SCOP family
** @attr Sunid_Family [ajint] SCOP sunid for family
** @attr Seq [AjPStr] Sequence as string
** @attr Start [ajint] Start of sequence or signature alignment relative 
**                     to full length
** @attr End [ajint] End of sequence or signature alignment relative 
**                   to full length
** @attr Acc [AjPStr] Accession number of sequence entry
** @attr Spr [AjPStr] Swissprot code of sequence entry
** @attr Typeobj [AjPStr] Bibliographic information ... objective
** @attr Typesbj [AjPStr] Bibliographic information ... subjective
** @attr Model [AjPStr] String for model type (HMM, Gribskov etc)
** @attr Group [AjPStr] 'REDUNDANT' or 'NON_REDUNDANT'
** @attr Rank [ajint] Rank order of hit
** @attr Score [float] Score of hit 
** @attr Eval [float] E-value of hit
** @attr Pval [float] p-value of hit
** @attr Alg [AjPStr] Alignment, e.g. of a signature to the sequence
** @attr Target [AjBool] True if the Scophit is targetted for removal from
**                       a list of Scophit objects
** @attr Target2 [AjBool] Also used for garbage collection
** @attr Priority [AjBool] True if the Scop hit is high priority
** @@
******************************************************************************/

typedef struct AjSScophit
{
    AjPStr    Class;
    AjPStr    Fold;
    AjPStr    Superfamily;
    AjPStr    Family;
    ajint    Sunid_Family;
    AjPStr    Seq;
    ajint     Start;
    ajint     End;
    AjPStr    Acc;
    AjPStr    Spr;
    AjPStr    Typeobj;
    AjPStr    Typesbj;
    AjPStr    Model;
    AjPStr    Group;
    ajint     Rank;
    float     Score;
    float     Eval;
    float     Pval;
    AjPStr    Alg;
    AjBool    Target;
    AjBool    Target2;
    AjBool    Priority;
} AjOScophit, *AjPScophit;




/* @data AjPHit *******************************************************
**
** Ajax hit object.
**
** Holds data associated with a protein / domain sequence that is generated 
** / manipulated by the EMBOSS applications psiblasts, swissparse, seqsort, 
** seqmerge, groups and sigscan.
**
** AjPHit is implemented as a pointer to a C data structure.
**
** @alias AjSHit
** @alias AjOHit
**
** @attr Seq [AjPStr] Sequence as string
** @attr Start [ajint] Start of sequence or signature alignment relative 
**                     to full length
** @attr End [ajint] End of sequence or signature alignment relative to  
		     full length swissprot sequence, this is an index  
		     so starts at 0
** @attr Acc [AjPStr] Accession number of sequence entry
** @attr Spr [AjPStr] Swissprot code of sequence entry
** @attr Typeobj [AjPStr] Primary classification of hit - objective
** @attr Typesbj [AjPStr] Secondary classification of hit - subjective
** @attr Model [AjPStr] String for model type (HMM, Gribskov etc)
** @attr Alg [AjPStr] Alignment, e.g. of a model to the sequence
** @attr Group [AjPStr] 'REDUNDANT' or 'NON_REDUNDANT'
** @attr Rank [ajint] Rank order of hit
** @attr Score [float] Score of hit 
** @attr Eval [float] E-value of hit
** @attr Pval [float] p-value of hit
** @attr Target [AjBool] True if the Scophit is targetted for removal from
**                       a list of Scophit objects
** @attr Target2 [AjBool] Also used for garbage collection
** @attr Priority [AjBool] True if the Scop hit is high priority
** @@
******************************************************************************/

typedef struct AjSHit
{
  AjPStr    Seq;
  ajint     Start;
  ajint     End;
  AjPStr    Acc;
  AjPStr    Spr;
  AjPStr    Typeobj;
  AjPStr    Typesbj;
  AjPStr    Model;
  AjPStr    Alg;
  AjPStr    Group;
  ajint     Rank;
  float     Score;
  float     Eval;
  float     Pval;
  AjBool    Target;
  AjBool    Target2;
  AjBool    Priority;
} AjOHit, *AjPHit;


/* @data AjPHitlist *******************************************************
**
** Ajax hitlist object.
**
** Holds an array of hit structures and associated SCOP classification 
** records.
**
** AjPHitlist is implemented as a pointer to a C data structure.
**
** @alias AjSHitlist
** @alias AjOHitlist
**
** @attr Class [AjPStr] SCOP class
** @attr Fold [AjPStr] SCOP fold
** @attr Superfamily [AjPStr] SCOP superfamily
** @attr Family [AjPStr] SCOP family
** @attr Model [AjPStr] String for model type (HMM, Gribskov etc)
** @attr Sunid_Family [ajint] SCOP sunid for family
** @attr Priority [AjBool] True if the Hitlist is high priority
** @attr N [ajint] No. of hits
** @attr hits [AjPHit*] Array of hits
** @@
******************************************************************************/

typedef struct AjSHitlist
{
    AjPStr  Class;
    AjPStr  Fold;
    AjPStr  Superfamily;
    AjPStr  Family;
    AjPStr  Model;
    ajint    Sunid_Family;
    AjBool  Priority;
    ajint   N;
    AjPHit *hits;
} AjOHitlist, *AjPHitlist;





/* @data AjPHitidx *******************************************************
**
** Ajax Hitidx object.
**
** Holds data for an indexing Hit and Hitlist objects
**
** AjPHitidx is implemented as a pointer to a C data structure.
**
** @alias AjSHitidx
** @alias AjOHitidx
**
** @attr Id [AjPStr] Identifier
** @attr hptr [AjPHit] Pointer to AjPHit structure
** @attr lptr [AjPHitlist] Pointer to AjPHitlist structure
** @@
******************************************************************************/
typedef struct AjSHitidx
{  
    AjPStr      Id;
    AjPHit      hptr;
    AjPHitlist  lptr;
}AjOHitidx, *AjPHitidx;






/* @data AjPAtom *******************************************************
**
** Ajax atom object.
**
** Holds protein atom data
**
** AjPAtom is implemented as a pointer to a C data structure.
**
** @alias AjSAtom
** @alias AjOAtom
**
** @attr Mod [ajint] Model number
** @attr Chn [ajint] Chain number
** @attr Gpn [ajint] Group number
** @attr Type [char] 'P' (protein atom), 'H' ("heterogens") or 'w' (water)
** @attr Idx [ajint] Residue number - index into sequence
** @attr Pdb [AjPStr] Residue number - according to original PDB file
** @attr Id1 [char] Standard residue identifier or 'X' for unknown 
**                  types or '.' for heterogens and water
** @attr Id3 [AjPStr] Residue or group identifier
** @attr Atm [AjPStr] Atom identifier
** @attr X [float] X coordinate
** @attr Y [float] Y coordinate
** @attr Z [float] Z coordinate
** @attr O [float] Occupancy
** @attr B [float] B value thermal factor
** @attr Phi [float] 18:Phi angle
** @attr Psi [float] 19:Psi angle
** @attr Area [float] 20:Residue solvent accessible area
**
** @cc Secondary structure-specific variables from the PDB file
**
** @attr eNum [ajint] Serial number of the element
** @attr eId [AjPStr] Element identifier
** @attr eType [char] Element type COIL ('C'), HELIX ('H'), SHEET ('E')
**                    or TURN ('T'). Has a default value of COIL.
** @attr eClass [ajint] Class of helix, an int from 1-10,  from 
**	  http://www.rcsb.org/pdb/docs/format/pdbguide2.2/guide2.2_frame.html
**        (see below)
**
** @cc Variables for data derived from stride
**
** @attr eStrideNum [ajint] Number of the element (sequential count,
**                         from N-term)
** @attr eStrideType [char] 8: Element type: ALPHA HELIX ('H'),
**                         3-10 HELIX ('G')
**			   PI-HELIX ('I'), EXTENDED CONFORMATION ('E'), 
**			   ISOLATED BRIDGE ('B' or 'b'), TURN ('T') or COIL 
**			   (none of the above) ('C'). (from STRIDE)
**
** @cc Variables for data derived from naccess
**
** @attr all_abs [float] Absolute accessibility, all atoms
** @attr all_rel [float] Relative accessibility, all atoms
** @attr side_abs [float] Absolute accessibility, atoms in sidechain
** @attr side_rel [float] Relative accessibility, atoms in sidechain
** @attr main_abs [float] Absolute accessibility, atoms in mainchain
** @attr main_rel [float] Relative accessibility, atoms in mainchain
** @attr npol_abs [float] Absolute accessibility, nonpolar atoms
** @attr npol_rel [float] Relative accessibility, nonpolar atoms
** @attr pol_abs [float] Absolute accessibility, polar atoms
** @attr pol_rel [float] Relative accessibility, polar atoms
** @@
******************************************************************************/

typedef struct AjSAtom
{
  ajint        Mod;
  ajint        Chn;
  ajint        Gpn;
  char       Type;
  ajint        Idx;
  AjPStr     Pdb;
  char       Id1;
  AjPStr     Id3;
  AjPStr     Atm;
  float      X;
  float      Y;
  float      Z;
  float      O;
  float      B;
  float      Phi;
  float      Psi;
  float      Area;
  
  ajint      eNum;
  AjPStr     eId;
  char       eType;
  ajint      eClass;

  ajint      eStrideNum;
  char       eStrideType;

  float      all_abs;
  float      all_rel;
  float      side_abs;
  float      side_rel;
  float      main_abs;
  float      main_rel;
  float      npol_abs;
  float      npol_rel;
  float      pol_abs;
  float      pol_rel;
} AjOAtom, *AjPAtom;


/* @data AjPChain ***********************************************************
**
** Ajax chain object.
**
** Holds protein chain data
**
** AjPChain is implemented as a pointer to a C data structure.
**
** @alias AjSChain
** @alias AjOChain
**
** @attr Id [char] Chain id, ('.' if one wasn't specified in the 
**		   original PDB file)
** @attr Nres [ajint] No. of amino acid residues
** @attr Nlig [ajint] No. of groups which are non-covalently associated 
**		   with the chain, excluding water ("heterogens")
** @attr numHelices [ajint] No. of helices in the chain according to the
**                          PDB file
** @attr numStrands [ajint] No. of strands in the chain according to the
**                          PDB file
** @attr Seq [AjPStr] sequence as string
** @attr Atoms [AjPList] List of Atom objects for (potentially multiple models)
**		  of the polypeptide chain and any groups (ligands) that 
**		  could be uniquely associated with a chain
** @@
******************************************************************************/

typedef struct AjSChain
{
    char       Id;
    ajint      Nres;
    ajint      Nlig;

    ajint      numHelices;
    ajint      numStrands;

    AjPStr     Seq;
    AjPList    Atoms;
} AjOChain, *AjPChain;



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
** @attr Pdb [AjPStr] PDB code
** @attr Compnd [AjPStr] Text from COMPND records in PDB file
** @attr Source [AjPStr] Text from SOURCE records in PDB file
** @attr Method [ajint] Exp. type, value is either XRAY or NMR
** @attr Reso [float] Resolution of an XRAY structure or 0.0
** @attr Nmod [ajint] No. of models (always 1 for XRAY structures)
** @attr Nchn [ajint] No. polypeptide chains
** @attr Chains [AjPChain*] Array of pointers to AjSChain structures
** @attr Ngp [ajint] No. groups that could not be uniquely associated 
**		     with a chain in the SEQRES records
** @attr gpid [AjPChar] Array of chain (group) id's for groups that 
**		     could not be uniquely associated with a chain
** @attr Groups [AjPList] List of Atom objects for groups that could not 
**		   be uniquely associated with a chain
** @attr Water [AjPList] List of Atom objects for water molecules (which can 
**		   never be uniquely associated with a chain
** @@
******************************************************************************/

typedef struct AjSPdb
{
    AjPStr     Pdb;
    AjPStr     Compnd;
    AjPStr     Source;
    ajint        Method;
    float      Reso;
    ajint        Nmod;
    ajint        Nchn;
    AjPChain  *Chains;
    ajint      Ngp;
    AjPChar    gpid;
    AjPList   Groups;
    AjPList   Water;
}AjOPdb, *AjPPdb;




/* @data AjPScop *******************************************************
**
** Ajax scop object.
**
** Holds scop database data
**
** The variables have the following meaning:
**
**  AjPStr Entry;          Domain identifer code 
**  AjPStr Pdb;            Corresponding pdb identifier code
**  AjPStr Class;          SCOP class name as an AjPStr 
**  AjPStr Fold;           SCOP fold  name as an AjPStr  
**  AjPStr Superfamily;    SCOP superfamily name as an AjPStr 
**  AjPStr Family;         SCOP family name as an AjPStr 
**  AjPStr Domain;         SCOP domain name as an AjPStr 
**  AjPStr Source;         SCOP source (species) as an AjPStr 
**  ajint    N;            No. chains from which this domain is comprised 
**  char   *Chain;         Chain identifiers 
**  AjPStr *Start;         PDB residue number of first residue in domain 
**  AjPStr *End;           PDB residue number of last residue in domain 
**
** AjPScop is implemented as a pointer to a C data structure.
**
** @alias AjSScop
** @alias AjOScop
**
** @attr Entry [AjPStr] SCOP Domain identifer code
** @attr Pdb [AjPStr] Corresponding pdb identifier code
** @attr Class [AjPStr] SCOP class name as an AjPStr
** @attr Fold [AjPStr] SCOP fold  name as an AjPStr
** @attr Superfamily [AjPStr] SCOP superfamily name as an AjPStr
** @attr Family [AjPStr] SCOP family name as an AjPStr
** @attr Domain [AjPStr] SCOP domain name as an AjPStr
** @attr Source [AjPStr] SCOP source (species) as an AjPStr
** @attr N [ajint] No. chain or segments from which this domain is comprised
** @attr Chain [char*] Chain identifiers
** @attr Start [AjPStr*] PDB residue number of first residue in domain
** @attr End [AjPStr*] PDB residue number of last residue in domain
** @attr Sunid_Class [ajint] SCOP sunid for class
** @attr Sunid_Fold [ajint] SCOP sunid for fold
** @attr Sunid_Superfamily [ajint] SCOP sunid for superfamily
** @attr Sunid_Family [ajint] SCOP sunid for family
** @attr Sunid_Domain [ajint] SCOP sunid for domain
** @attr Sunid_Source [ajint] SCOP sunid for species
** @attr Sunid_Domdat [ajint] SCOP sunid for domain data
** @attr Acc [AjPStr] Accession number of sequence entry
** @attr Spr [AjPStr] Swissprot code of sequence entry
** @attr SeqPdb [AjPStr] Sequence (from pdb) as string
** @attr SeqSpr [AjPStr] Sequence (from swissprot) as string
** @attr Startd [ajint] Start of sequence relative to full length 
**		    swissprot sequence
** @attr Endd [ajint] End of sequence relative to full length 
**		    swissprot sequence
** @@
******************************************************************************/
typedef struct AjSScop
{
    AjPStr Entry;
    AjPStr Pdb;
    AjPStr Class;
    AjPStr Fold;
    AjPStr Superfamily;
    AjPStr Family;
    AjPStr Domain;
    AjPStr Source;
    ajint    N;
    char   *Chain;
    AjPStr *Start;
    AjPStr *End;

    ajint  Sunid_Class;
    ajint  Sunid_Fold;
    ajint  Sunid_Superfamily;
    ajint  Sunid_Family;
    ajint  Sunid_Domain;
    ajint  Sunid_Source;
    ajint  Sunid_Domdat;

    AjPStr Acc;
    AjPStr Spr;
    AjPStr SeqPdb;
    AjPStr SeqSpr;
    ajint  Startd;
    ajint  Endd;
} AjOScop,*AjPScop;






/* @data AjPScopcla *******************************************************
**
** Ajax scopcla object.
**
** Holds scop database data from raw file (dir.cla.scop.txt from SCOP authors)
**
** AjPScopcla is implemented as a pointer to a C data structure.
**
** @alias AjSScopcla
** @alias AjOScopcla
**
** @attr Entry [AjPStr] Domain identifer code
** @attr Pdb [AjPStr] Corresponding pdb identifier code
** @attr Sccs [AjPStr] Scop compact classification string
** @attr Class [ajint] SCOP sunid for class 
** @attr Fold [ajint] SCOP sunid for fold 
** @attr Superfamily [ajint] SCOP sunid for superfamily 
** @attr Family [ajint] SCOP sunid for family 
** @attr Domain [ajint] SCOP sunid for domain   
** @attr Source [ajint] SCOP sunid for species 
** @attr Domdat [ajint] SCOP sunid for domain data
** @attr N [ajint] No. chains from which this domain is comprised 
** @attr Chain [char*] Chain identifiers 
** @attr Start [AjPStr*] PDB residue number of first residue in domain 
** @attr End [AjPStr*] PDB residue number of last residue in domain 
** @@
******************************************************************************/

typedef struct AjSScopcla
{
    AjPStr Entry;
    AjPStr Pdb;
    AjPStr Sccs;

    ajint  Class;
    ajint  Fold;
    ajint  Superfamily;
    ajint  Family;  
    ajint  Domain;
    ajint  Source;
    ajint  Domdat;
        
    ajint    N;
    char   *Chain;
    AjPStr *Start;
    AjPStr *End;
} AjOScopcla,*AjPScopcla;



/* @data AjPScopdes *******************************************************
**
** Ajax scopdes object.
**
** Holds scop database data from raw file (dir.des.scop.txt from SCOP authors)
**
** AjPScopdes is implemented as a pointer to a C data structure.
**
** @alias AjSScopdes
** @alias AjOScopdes
**
** @attr Sunid [ajint] SCOP sunid for node
** @attr Type [AjPStr] Type of node, either 'px' (domain data), 'cl' (class),
**  'cf' (fold), 'sf' (superfamily), 'fa' (family), 'dm' (domain) or 
**  'sp' (species)
** @attr Sccs [AjPStr] Scop compact classification string
** @attr Entry [AjPStr] Domain identifer code (or '-' if Type!='px')
** @attr Desc [AjPStr] Description in english of the node
** @@
******************************************************************************/

typedef struct AjSScopdes
{
    ajint  Sunid;
    AjPStr Type;
    AjPStr Sccs;
    AjPStr Entry;
    AjPStr Desc;
} AjOScopdes,*AjPScopdes;





/* @data AjPSigcell *******************************************************
**
** Ajax Sigcell object.
**
** Holds data for a cell of a path matrix for signature:sequence alignment.
**
** AjPSigcell is implemented as a pointer to a C data structure.
**
** @alias AjSSigcell
** @alias AjOSigcell
**
** @attr val [float] Value for this cell
** @attr prev [ajint] Index in path matrix of prev. highest value
** @attr visited [AjBool] ajTrue if this cell has been visited
** @@
******************************************************************************/
typedef struct AjSSigcell
{
    float  val;
    ajint  prev;
    AjBool visited;
} AjOSigcell, *AjPSigcell;





/* @data AjPSigdat *******************************************************
**
** Ajax Sigdat object.
**
** Holds empirical data for an (uncompiled) signature position. 
** Important: Functions which manipulate this structure rely on the data in 
** the gap arrays (gsiz and grfq) being filled in order of increasing gap size.
**
** AjPSigdat is implemented as a pointer to a C data structure.
**
** @alias AjSSigdat
** @alias AjOSigdat
**
** @attr nres [ajint] No. diff. types of residue
** @attr rids [AjPChar] Residue ids
** @attr rfrq [AjPInt] Residue frequencies
** @attr ngap [ajint] No. diff. sizes of empirical gap
** @attr gsiz [AjPInt] Gap sizes
** @attr gfrq [AjPInt] Frequencies of gaps of each size
** @attr wsiz [ajint] Window size for this gap
** @@
******************************************************************************/
typedef struct AJSSigdat
{
    ajint       nres;
    AjPChar     rids;
    AjPInt      rfrq;

    ajint       ngap;
    AjPInt      gsiz;
    AjPInt      gfrq;
    ajint       wsiz;
} AJOSigdat, *AjPSigdat;






/* @data AjPSigpos *******************************************************
**
** Ajax Sigpos object.
**
** Holds data for compiled signature position
**
** AjPSigpos is implemented as a pointer to a C data structure.
**
** @alias AjSSigpos
** @alias AjOSigpos
**
** @attr ngaps [ajint] No. of gaps
** @attr gsiz [ajint*] Gap sizes
** @attr gpen [float*] Gap penalties
** @attr subs [float*] Residue match values
** @@
******************************************************************************/
typedef struct AJSSigpos
{
    ajint    ngaps;
    ajint   *gsiz;
    float   *gpen;
    float   *subs;
} AJOSigpos, *AjPSigpos;




/* @data AjPSignature *******************************************************
**
** Ajax Signature object.
**
** Holds data for 
**
** AjPSignature is implemented as a pointer to a C data structure.
**
** @alias AjSSignature
** @alias AjOSignature
**
** @attr Class [AjPStr] SCOP class
** @attr Fold [AjPStr] SCOP fold
** @attr Superfamily [AjPStr] SCOP superfamily
** @attr Family [AjPStr] SCOP family
** @attr Sunid_Family [ajint] SCOP sunid for family
** @attr npos [ajint] No. of signature positions
** @attr pos [AjPSigpos*] Array of derived data for purposes of alignment
** @attr dat [AjPSigdat*] Array of empirical data
** @@
******************************************************************************/
typedef struct AjSSignature
{
    AjPStr     Class;
    AjPStr     Fold;
    AjPStr     Superfamily;
    AjPStr     Family;
    ajint    Sunid_Family;
    ajint      npos;
    AjPSigpos *pos;
    AjPSigdat *dat;
} AjOSignature, *AjPSignature;







/* @data AjPHetent ***********************************************************
**
** Ajax Hetent object.
**
** Holds a single entry from a dictionary of heterogen groups.
**
** AjPHetent is implemented as a pointer to a C data structure.
**
** @alias AjSHetent
** @alias AjOHetent
**
** @attr abv [AjPStr] 3-letter abbreviation of heterogen
** @attr syn [AjPStr] Synonym
** @attr ful [AjPStr] Full name
** @attr cnt [ajint] No. of occurences (files) of this heterogen in a directory
** @@
******************************************************************************/
typedef struct AjSHetent
{
    AjPStr   abv;
    AjPStr   syn;
    AjPStr   ful;
    ajint    cnt;
} AjOHetent, *AjPHetent;




/* @data AjPHet ***********************************************************
**
** Ajax Het object.
** Holds a dictionary of heterogen groups.
**
** AjPHet is implemented as a pointer to a C data structure.
**
** @alias AjSHet
** @alias AjOHet
**
** @attr n [ajint] Number of entries
** @attr entries [AjPHetent*] Array of entries
** @@
******************************************************************************/
typedef struct AjSHet
{
    ajint         n;
    AjPHetent *entries;
} AjOHet, *AjPHet;


/* @data AjPCoord *******************************************************
**
** Undocumented
**
** @alias AjOCoord
** @alias AjSCoord
**
** @attr Class [AjPStr] SCOP class
** @attr Fold [AjPStr] SCOP fold
** @attr Superfamily [AjPStr] SCOP superfamily
** @attr Family [AjPStr] SCOP family
** @attr Sunid_Family [ajint] the sun_id for a given SCOP family
** @attr Model_Type [AjPStr] The type of model used to generate the scores
** @attr Acc [AjPStr] Accession number of sequence entry
** @attr Spr [AjPStr] Swissprot code of sequence entry
** @attr x [ajint] The score interval
** @attr y [ajint] Frequency of scores
** @@
******************************************************************************/

typedef struct AjSCoord
{
    AjPStr   Class;
    AjPStr   Fold;
    AjPStr   Superfamily;
    AjPStr   Family;
    ajint    Sunid_Family;
    AjPStr   Model_Type;
    AjPStr   Acc;
    AjPStr   Spr;
    ajint    x;
    ajint    y;
} AjOCoord, *AjPCoord;

/* @data AjPDatapoint *******************************************************
**
** Undocumented
**
** @alias AjODatapoint
** @alias AjSDatapoint
**
** @attr Acc [AjPStr] Accession number of sequence entry
** @attr Spr [AjPStr] Swissprot code of sequence entry
** @attr x [ajint] The score interval
** @attr y [ajint] Frequency of scores
** @@
******************************************************************************/



typedef struct AjSDatapoint
{
    AjPStr    Acc;
    AjPStr    Spr;
    ajint     x;
    ajint     y;
} AjODatapoint, *AjPDatapoint;


/* @data AjPDiscord *******************************************************
**
** Undocumented
**
** @alias AjODiscord
** @alias AjSDiscord
**
** @attr Class [AjPStr] SCOP class
** @attr Fold [AjPStr] SCOP fold
** @attr Superfamily [AjPStr] SCOP superfamily
** @attr Family [AjPStr] SCOP family
** @attr Sunid_Family [ajint] the sun_id for a given SCOP family
** @attr Model_Type [AjPStr] The type of model used to generate the scores
** @attr N [ajint] No. of data points
** @attr Points [AjPDatapoint*] an array of coordinates
** @@
******************************************************************************/

typedef struct AjSDiscord
{
    AjPStr        Class;
    AjPStr        Fold;
    AjPStr        Superfamily;
    AjPStr        Family;
    ajint         Sunid_Family;
    AjPStr        Model_Type;
    ajint         N;

    AjPDatapoint  *Points;
} AjODiscord, *AjPDiscord;








/* Waqas funky */
AjBool      ajXyzFunkyRead(AjPFile funky_fptr, AjPList *all_entries);
AjPDbase    ajXyzDbaseNew(ajint n);
void        ajXyzDbaseDel(AjPDbase *ptr);

AjPDbaseEnt ajXyzDbaseEntNew(ajint n);
void        ajXyzDbaseEntDel(AjPDbaseEnt *ptr);

AjPDomConts ajXyzDomContsNew(ajint n);
void        ajXyzDomContsDel(AjPDomConts *ptr);


/*  Ranjeeva stuff */

ajint ajXyzCoordBinSearchScore(float score, AjPCoord *arr, ajint siz);
float ajXyzScoreToPvalue (float score, AjPList list);
float ajXyzPvalueFromDist (float score, AjPList list);
AjBool ajXyzDiscordWrite(AjPFile outf, AjPDiscord thys);
AjBool ajXyzDiscordRead(AjPFile inf, char *delim, AjPDiscord *thys);
void ajXyzDatapointDel(AjPDatapoint *thys);
void ajXyzDiscordDel(AjPDiscord *pthis);
AjPDatapoint ajXyzDatapointNew(void);
AjPDiscord  ajXyzDiscordNew(ajint n);
AjBool ajXyzDiscordToCoords(AjPDiscord dis_cord, AjPList *out);
void ajXyzCoordDel(AjPCoord *pthis);
AjPCoord ajXyzCoordNew(void);

AjBool ajXyzSunidToScopInfo (ajint sunid, AjPStr *family, AjPStr *superfamily, AjPStr *fold, AjPStr *klass, AjPList list);


AjPPdbtosp ajXyzPdbtospNew(ajint n);
void ajXyzPdbtospDel(AjPPdbtosp *thys);
AjBool ajXyzPdbtospRead(AjPFile inf, AjPStr entry, AjPPdbtosp *thys);
AjBool ajXyzPdbtospReadC(AjPFile inf, char *entry, AjPPdbtosp *thys);
AjBool ajXyzPdbtospReadAll(AjPFile inf, AjPList *list);
ajint ajXyzPdbtospBinSearch(AjPStr id, AjPPdbtosp *arr, ajint siz);
ajint ajXyzSortPdbtospPdb(const void *ptr1, const void *ptr2);


AjPAtom       ajXyzAtomNew(void);
void          ajXyzAtomDel(AjPAtom *thys);
AjBool        ajXyzAtomCopy(AjPAtom *to, AjPAtom from);
AjPList       ajXyzAtomListCopy(AjPList ptr);
AjBool        ajXyzInContact(AjPAtom atm1, AjPAtom atm2, float thresh,
			    AjPVdwall vdw);
float         ajXyzAtomDistance(AjPAtom atm1, AjPAtom atm2, AjPVdwall vdw);

AjPChain      ajXyzChainNew(void);
void          ajXyzChainDel(AjPChain *thys);


AjPPdb        ajXyzPdbNew(ajint chains);
void          ajXyzPdbDel(AjPPdb *thys);
AjBool        ajXyzPdbWriteAll(AjPFile errf, AjPFile outf, AjPPdb pdb);
AjBool        ajXyzPdbWriteDomain(AjPFile errf, AjPFile outf, AjPPdb pdb,
				AjPScop scop); 
AjBool        ajXyzPdbChain(char id, AjPPdb pdb, ajint *chn);
AjBool        ajXyzPdbAtomIndexI(AjPPdb pdb, ajint chn, AjPInt *idx);
AjBool        ajXyzPdbAtomIndexC(AjPPdb pdb, char chn, AjPInt *idx);
AjBool        ajXyzPdbAtomIndexICA(AjPPdb pdb, ajint chn, AjPInt *idx, ajint *nres);
AjBool        ajXyzPdbAtomIndexCCA(AjPPdb pdb, char chn, AjPInt *idx, ajint *nres);
AjBool        ajXyzPdbToSp(AjPStr pdb, AjPStr *spr, AjPList list);
AjBool        ajXyzPdbToAcc(AjPStr pdb, AjPStr *acc, AjPList list);
AjBool      ajXyzPdbToScop(AjPPdb pdb, AjPList list_allscop, 
			   AjPList *list_pdbscopids);
AjBool  ajXyzPdbToIdx(ajint *idx, AjPPdb pdb, AjPStr res, ajint chn);




AjBool        ajXyzCpdbRead(AjPFile inf, AjPPdb *thys);
AjBool        ajXyzCpdbReadFirstModel(AjPFile inf, AjPPdb *thys);
AjBool        ajXyzCpdbReadOld(AjPFile inf, AjPPdb *thys);
AjBool ajXyzCpdbCopy(AjPPdb *to, AjPPdb from);
AjBool        ajXyzCpdbWriteAll(AjPFile out, AjPPdb thys);
AjBool        ajXyzCpdbWriteDomain(AjPFile errf, AjPFile outf, AjPPdb pdb,
			      AjPScop scop);
AjBool      ajXyzCpdbListHeterogens(AjPPdb pdb, AjPList *list_heterogens, 
					    AjPInt *siz_heterogens, ajint *nhet, 
					    AjPFile logf );




void ajXyzCathWrite(AjPFile outf, AjPCath ptr);
void ajXyzCathDel(AjPCath *ptr);
AjPCath ajXyzCathNew(ajint NSegment);
AjBool ajXyzCathReadC(AjPFile inf, char *entry, AjPCath *thys);


AjPScop       ajXyzScopNew(ajint n);
void          ajXyzScopDel(AjPScop *pthis);
AjBool        ajXyzScopRead(AjPFile inf, AjPStr entry, AjPScop *thys);
AjBool        ajXyzScopReadC(AjPFile inf, char *entry, AjPScop *thys);
AjBool        ajXyzScopReadAll(AjPFile fptr, AjPList *list); 
void          ajXyzScopWrite(AjPFile outf, AjPScop thys);
AjPStr        ajXyzScopToPdb(AjPStr scop, AjPStr *pdb);
AjBool        ajXyzScopToSp(AjPStr scop, AjPStr *spr, AjPList list);
AjBool        ajXyzScopToAcc(AjPStr scop, AjPStr *acc, AjPList list);
AjBool        ajXyzScopToScophit(AjPScop source, AjPScophit* target);
ajint         ajXyzScopBinSearchScop(AjPStr id, AjPScop *arr, ajint siz);
ajint         ajXyzScopCompId(const void *hit1, const void *hit2);
ajint         ajXyzScopCompPdbId(const void *hit1, const void *hit2);
AjBool        ajXyzScopCopy(AjPScop *to, AjPScop from);
ajint ajXyzScopBinSearchSunid(ajint id, AjPScop *arr, ajint siz);
ajint ajXyzScopCompSunid(const void *entry1, const void *entry2);
AjBool ajXyzScopSeqFromSunid(ajint id, AjPStr *seq, AjPList list);
ajint ajXyzScopCompPdbId(const void *hit1, const void *hit2);
ajint ajXyzScopBinSearchPdb(AjPStr id, AjPScop *arr, ajint siz);



ajint ajXyzCathCompPdbId(const void *hit1, const void *hit2);
ajint ajXyzCathBinSearchPdb(AjPStr id, AjPCath *arr, ajint siz);


AjPScopcla    ajXyzScopclaNew(ajint chains);
void          ajXyzScopclaDel(AjPScopcla *thys);
AjBool        ajXyzScopclaRead(AjPFile inf, AjPStr entry, AjPScopcla *thys);
AjBool        ajXyzScopclaReadC(AjPFile inf, char *entry, AjPScopcla *thys);

AjPScopdes    ajXyzScopdesNew(void);
void          ajXyzScopdesDel(AjPScopdes *ptr);
AjBool        ajXyzScopdesRead(AjPFile inf, AjPStr entry, AjPScopdes *thys);
AjBool        ajXyzScopdesReadC(AjPFile inf, char *entry, AjPScopdes *thys);
ajint ajXyzScopdesBinSearch(ajint id, AjPScopdes *arr, ajint siz);
ajint ajXyzScopdesCompSunid(const void *scop1, const void *scop2);





AjPScophit    ajXyzScophitNew(void);
void          ajXyzScophitDel(AjPScophit *pthis);
void          ajXyzScophitDelWrap(const void  **ptr);
AjBool        ajXyzScophitsToHitlist(AjPList in, AjPHitlist *out,   AjIList *iter);
AjBool ajXyzScophitsWrite(AjPFile outf, AjPList list);
AjBool ajXyzScophitsAccToHitlist(AjPList in, AjPHitlist *out,   AjIList *iter);
AjBool        ajXyzScophitsOverlap(AjPScophit h1, AjPScophit h2, ajint n);
AjBool        ajXyzScophitsOverlapAcc(AjPScophit h1, AjPScophit h2, ajint n);
AjBool        ajXyzScophitCopy(AjPScophit *to, AjPScophit from);
AjPList       ajXyzScophitListCopy(AjPList ptr);
AjBool        ajXyzScophitToHit(AjPHit *to, AjPScophit from);
AjBool        ajXyzScophitCheckTarget(AjPScophit ptr);
AjBool        ajXyzScophitTarget(AjPScophit *h);
AjBool        ajXyzScophitTarget2(AjPScophit *h);
AjBool        ajXyzScophitTargetLowPriority(AjPScophit *h);
AjBool        ajXyzScophitMergeInsertThis(AjPList list, AjPScophit hit1, 
				   AjPScophit hit2,  AjIList iter);
AjBool        ajXyzScophitMergeInsertThisTarget(AjPList list, AjPScophit hit1, 
				   AjPScophit hit2,  AjIList iter);
AjBool ajXyzScophitMergeInsertThisTargetBoth(AjPList list, AjPScophit hit1, 
					     AjPScophit hit2,  AjIList iter);
AjBool        ajXyzScophitMergeInsertOther(AjPList list, AjPScophit hit1, AjPScophit hit2);
AjBool        ajXyzScophitMergeInsertOtherTargetBoth(AjPList list, AjPScophit hit1, AjPScophit hit2);
AjBool        ajXyzScophitMergeInsertOtherTarget(AjPList list, AjPScophit hit1, AjPScophit hit2);
AjPScophit    ajXyzScophitMerge(AjPScophit hit1, AjPScophit hit2);
ajint         ajXyzScophitCompSpr(const void *hit1, const void *hit2);
ajint         ajXyzScophitCompStart(const void *hit1, const void *hit2);
ajint         ajXyzScophitCompEnd(const void *hit1, const void *hit2);
ajint         ajXyzScophitCompFold(const void *hit1, const void *hit2);
ajint         ajXyzScophitCompSfam(const void *hit1, const void *hit2);
ajint         ajXyzScophitCompFam(const void *hit1, const void *hit2);
ajint         ajXyzScophitCompAcc(const void *hit1, const void *hit2);
ajint         ajXyzScophitCompSunid(const void *entry1, const void *entry2);
ajint         ajXyzScophitCompScore(const void *hit1, const void *hit2); 
ajint         ajXyzScophitCompPval(const void *hit1, const void *hit2); 

AjPHit        ajXyzHitNew(void);
void          ajXyzHitDel(AjPHit *pthis);
AjPHit        ajXyzHitMerge(AjPHit hit1, AjPHit hit2);
AjBool        ajXyzHitsOverlap(AjPHit h1, AjPHit h2, ajint n);
ajint         ajXyzCompScore(const void *hit1, const void *hit2);
ajint         ajXyzCompScoreInv(const void *hit1, const void *hit2);
ajint         ajXyzCompId(const void *hit1, const void *hit2);

AjPHitlist    ajXyzHitlistNew(ajint n);
void          ajXyzHitlistDel(AjPHitlist *pthis);
AjBool        ajXyzHitlistRead(AjPFile inf, char *delim, AjPHitlist *thys);
AjBool        ajXyzHitlistReadNode(AjPFile scopf, AjPList *list, AjPStr fam, AjPStr sfam, AjPStr fold, AjPStr klass);
AjBool        ajXyzHitlistReadFam(AjPFile scopf, AjPStr fam, AjPStr sfam, AjPStr fold, AjPStr klass, AjPList* list);
AjBool        ajXyzHitlistReadSfam(AjPFile scopf, AjPStr fam, AjPStr sfam, AjPStr fold, AjPStr klass,AjPList* list);
AjBool        ajXyzHitlistReadFold(AjPFile scopf, AjPStr fam, AjPStr sfam, AjPStr fold, AjPStr klass,AjPList* list);
AjBool        ajXyzHitlistWrite(AjPFile outf, AjPHitlist thys);
AjBool ajXyzHitlistWriteSubset(AjPFile outf, AjPHitlist thys, AjPInt ok);

AjBool        ajXyzHitlistToScophits(AjPList in, AjPList *out);
AjBool        ajXyzHitlistClassify(AjPHitlist *hits, AjPList targets, 
				   ajint thresh);
AjBool        ajXyzHitlistClassifyLigand(AjPHitlist *hits, AjPList targets, 
				   ajint thresh);
AjBool        ajXyzHitlistPriorityHigh(AjPHitlist *list);
AjBool        ajXyzHitlistPriorityLow(AjPHitlist *list);
AjBool        ajXyzHitlistToThreeScophits(AjPList in, AjPList *fam, AjPList *sfam, AjPList *fold);

AjBool        ajXyzHitlistsWriteFasta(AjPList *list, AjPFile *outf);
ajint         ajXyzHitlistCompFold(const void *hit1, const void *hit2);



AjBool        ajXyzPrintPdbSeqresChain(AjPFile errf, AjPFile outf, AjPPdb pdb,
				  ajint chn);
AjBool        ajXyzPrintPdbSeqresDomain(AjPFile errf, AjPFile outf, AjPPdb pdb,
				   AjPScop scop);
AjBool        ajXyzPrintPdbAtomChain(AjPFile outf, AjPPdb pdb, ajint mod, ajint chn);
AjBool        ajXyzPrintPdbAtomDomain(AjPFile errf, AjPFile outf, AjPPdb pdb,
				 AjPScop scop, ajint mod);
AjBool        ajXyzPrintPdbHeterogen(AjPFile outf, AjPPdb pdb, ajint mod);
AjBool        ajXyzPrintPdbText(AjPFile outf, AjPStr str, char *prefix);
AjBool        ajXyzPrintPdbHeader(AjPFile outf, AjPPdb pdb);
AjBool        ajXyzPrintPdbHeaderScop(AjPFile outf, AjPScop scop);
AjBool        ajXyzPrintPdbTitle(AjPFile outf, AjPPdb pdb);
AjBool        ajXyzPrintPdbCompnd(AjPFile outf, AjPPdb pdb);
AjBool        ajXyzPrintPdbSource(AjPFile outf, AjPPdb pdb);
AjBool        ajXyzPrintPdbEmptyRemark(AjPFile outf, AjPPdb pdb);
AjBool        ajXyzPrintPdbResolution(AjPFile outf, AjPPdb pdb);


AjBool        ajXyzScopalgRead(AjPFile inf, AjPScopalg *thys);
AjBool        ajXyzScopalgWrite(AjPFile outf, AjPScopalg scop);
AjBool        ajXyzScopalgWriteClustal(AjPScopalg align, AjPFile* outf);
AjBool        ajXyzScopalgWriteClustal2(AjPScopalg align, AjPFile* outf);
AjPScopalg    ajXyzScopalgNew(ajint n);
void          ajXyzScopalgDel(AjPScopalg *pthis);
ajint         ajXyzScopalgGetseqs(AjPScopalg thys, AjPStr **arr);
AjBool ajXyzScopalgToScop(AjPScopalg align, AjPScop *scop_arr, ajint dim, AjPList* list);

AjPCmap       ajXyzCmapNew(ajint dim);
void          ajXyzCmapDel(AjPCmap *pthis);
AjBool        ajXyzCmapRead(AjPFile inf, ajint mode, ajint chn, ajint mod, AjPCmap *thys);
AjBool        ajXyzCmapReadC(AjPFile inf, char chn, ajint mod, AjPCmap *thys);
AjBool        ajXyzCmapReadI(AjPFile inf, ajint chn, ajint mod, AjPCmap *thys);


float         ajXyzVdwRad(AjPAtom atm, AjPVdwall vdw);
 
AjPVdwall     ajXyzVdwallNew(ajint n);
void          ajXyzVdwallDel(AjPVdwall *pthis);
AjBool        ajXyzVdwallRead(AjPFile inf, AjPVdwall *thys);


AjPVdwres     ajXyzVdwresNew(ajint n);
void          ajXyzVdwresDel(AjPVdwres *pthis);


AjPScorealg   ajXyzScorealgNew(ajint len);
void          ajXyzScorealgDel(AjPScorealg *pthis);


AjPSigdat     ajXyzSigdatNew(ajint nres, ajint ngap);
AjPSigpos     ajXyzSigposNew(ajint ngap);


void          ajXyzSigdatDel(AjPSigdat *pthis);
void          ajXyzSigposDel(AjPSigpos *thys);


AjPHitidx     ajXyzHitidxNew(void);
void          ajXyzHitidxDel(AjPHitidx *pthis);
ajint         ajXyzHitidxBinSearch(AjPStr id, AjPHitidx *arr, ajint siz);


AjPSignature  ajXyzSignatureNew(ajint n);
void          ajXyzSignatureDel(AjPSignature *pthis);
AjBool        ajXyzSignatureRead(AjPFile inf, AjPSignature *thys);
AjBool        ajXyzSignatureWrite(AjPFile outf, AjPSignature thys);
AjBool        ajXyzSignatureCompile(AjPSignature *S, float gapo, float gape, 	
				    AjPMatrixf matrix);
AjBool        ajXyzSignatureAlignSeq(AjPSignature S, AjPSeq seq, AjPHit *hit, 
				     ajint nterm);
AjBool        ajXyzSignatureAlignSeqall(AjPSignature sig, AjPSeqall db, ajint n, 
					AjPHitlist *hits, ajint nterm);
AjBool        ajXyzSignatureHitsWrite(AjPFile outf, AjPSignature sig, 
				      AjPHitlist hits, ajint n);
AjBool        ajXyzSignatureHitsWriteHitlist(AjPFile outf, 
					     AjPHitlist hits, ajint n);

AjPHitlist   ajXyzSignatureHitsRead(AjPFile inf);
AjBool        ajXyzSignatureAlignWrite(AjPFile outf, AjPSignature sig, 
				       AjPHitlist hits);
AjBool        ajXyzSignatureAlignWriteBlock(AjPFile outf, AjPSignature sig, 
				       AjPHitlist hits);


AjPHetent     ajXyzHetentNew(void);
void          ajXyzHetentDel(AjPHetent *ptr);

AjPHet        ajXyzHetNew(ajint n);
void          ajXyzHetDel(AjPHet *ptr);
AjBool        ajXyzHetRawRead(AjPFile fptr, AjPHet *ptr);
AjBool        ajXyzHetRead(AjPFile fptr, AjPHet *ptr);
AjBool        ajXyzHetWrite(AjPFile fptr, AjPHet ptr, AjBool dogrep);


ajint StrBinSearchScop(AjPStr id, AjPStr *arr, ajint siz);
ajint StrComp(const void *str1, const void *str2);




#endif

#ifdef __cplusplus
}
#endif



