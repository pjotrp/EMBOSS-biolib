#include "ajax.h"

/* @datastatic SeqPType *******************************************************
**
** Sequence types data structure, used to test input sequence against
** a defined sequence type
**
** @alias SeqSType
** @alias SeqOType
**
** @attr Name [char*] sequence type name
** @attr Gaps [AjBool] allow gap characters
** @attr Type [ajint] enumerated ISANY=0 ISNUC=1 ISPROT=2 
** @attr Test [(char*)] Test function
** @attr Desc [char*] Description for documentation purposes
** @@
******************************************************************************/

typedef struct SeqSType {
  char *Name;
  AjBool Gaps;
  ajint Type;
  char (*Test) (AjPStr* pthys);
  char *Desc;
} SeqOType, *SeqPType;

enum ProtNuc {ISANY=0, ISNUC=1, ISPROT=2};

/* gaps only allowed if it says so
** gap conversion is a separate attribute, along with case convserion
*/

/* @funclist seqType **********************************************************
**
** Functions to test each sequence type
**
******************************************************************************/

static SeqOType seqType[] = {
  {"any",            AJFALSE, ISANY, ajSeqTypeAnyS,
                     "any valid sequence"}, /* reset type */
  {"dna",            AJFALSE, ISNUC, ajSeqTypeDnaS,
                     "DNA sequence"},
  {"rna",            AJFALSE, ISNUC, ajSeqTypeRnaS,
                     "RNA sequence"},
  {"puredna",        AJFALSE, ISNUC, ajSeqTypePurednaS,
                     "DNA, bases ACGT only"},
  {"purerna",        AJFALSE, ISNUC, ajSeqTypePurernaS,
                     "RNA, bases ACGU only"},
  {"nucleotide",     AJFALSE, ISNUC, ajSeqTypeNucS,
                     "nucleotide sequence"},
  {"purenucleotide", AJFALSE, ISNUC, ajSeqTypePurenucS,
                     "nucleotide, bases ACGTU only"},
  {"gapnucleotide",  AJFALSE, ISNUC, ajSeqTypeGapnucS,
                     "nucleotide, bases ACGTU with gaps"},
  {"gapdna",         AJTRUE,  ISNUC, ajSeqTypeGapdnaS,
                     "DNA sequence with gaps"},
  {"gaprna",         AJTRUE,  ISNUC, ajSeqTypeGaprnaS,
                     "RNA sequence with gaps"},
  {"protein",        AJFALSE, ISPROT,  ajSeqTypeProtS,
                     "protein sequence"},
  {"gapprotein",     AJTRUE,  ISPROT,  ajSeqTypeGapprotS,
                     "protein sequence with gaps"},
  {"pureprotein",    AJFALSE, ISPROT,  ajSeqTypePureprotS,
                     "protein sequence without BZ or X"},
  {"stopprotein",    AJFALSE, ISPROT,  ajSeqTypeStopprotS,
                     "protein sequence with a possible stop"},
  {"gapany",         AJTRUE,  ISANY, ajSeqTypeGapanyS,
                     "any valid sequence with gaps"}, /* reset type */
  {NULL,             AJFALSE, ISANY, NULL,
                     NULL}
};

static void       seqGapSL (AjPStr* seq, char gapc, char padc, ajint ilen);
static AjBool     seqTypeStopTrimS (AjPStr* pthys);
static void       seqTypeSet (AjPSeq thys, AjPStr Type);
static char       seqTypeTest (AjPStr thys, char* seqchar);

/* gap characters known are:
**
** . GCG and most others
** - Phylip and some alignment output
** ~ GCG for gaps at ends
** * Staden for DNA but stop for protein (fix on input?)
** O Phylip (fix on input?)
*/
char seqCharProt[] = "ACDEFGHIKLMNPQRSTVWYacdefghiklmnpqrstvwyBXZbxz*";
char seqCharProtPure[] = "ACDEFGHIKLMNPQRSTVWYacdefghiklmnpqrstvwy";
char seqCharProtAmbig[] = "BXZbxz";
char seqCharProtStop[] = "*";
char seqCharNuc[] = "ACGTUacgtuBDHKMNRSVWXYbdhkmnrsvwxy";
char seqCharNucPure[] = "ACGTUacgtu";
char seqCharNucAmbig[] = "BDHKMNRSVWXYbdhkmnrsvwxy";
char seqCharGap[] = ".-~Oo";	/* phylip uses O */
char seqCharNucDNA[] = "Tt";
char seqCharNucRNA[] = "Uu";
char seqCharGapany[] = ".-~Oo";	/* phylip uses O */
char seqCharGapdash[] = "-";
char seqCharGapdot[] = ".";
char seqGap = '-';		/* the (only) EMBOSS gap character */
char seqCharGapTest[] = " .-~Oo";   /* phylip uses O - don't forget space */

/* @func ajSeqTypeTest ********************************************************
**
** Tests the type of a sequence is compatible with a defined type.
** If the type can have gaps, also tests for gap characters.
** Used only for testing, so never writes any error message
**
** @param [P] pthys [AjPStr*] Sequence string
** @param [P] Type [AjPStr] Sequence type
** @return [AjBool] ajTrue if compatible.
** @@
******************************************************************************/

AjBool ajSeqTypeTest (AjPSeq thys, AjPStr Type) {

  ajint i = 0;
  char ret;

  ajDebug ("testing sequence '%s' type '%S'\n", thys, Type);

  if (!ajStrLen(Type)) {		/* nothing given - anything goes */
    ajSeqGap (thys, seqGap, 0);
    return ajTrue;
  }

  for (i = 0; seqType[i].Name; i++) {
    if (!ajStrMatchCaseC(Type, seqType[i].Name)) continue;
    ajDebug ("type '%s' found (%s)\n", seqType[i].Name, seqType[i].Desc);
    if (seqType[i].Type == ISPROT && !ajSeqIsProt(thys)) {
      ajDebug("Sequence is not a protein\n");
      return ajFalse;
    }
    if (seqType[i].Type == ISNUC && !ajSeqIsNuc(thys)) {
      ajDebug("Sequence is not nucleic\n");
      return ajFalse;
    }

    /* Calling funclist seqType() */
    ret = seqType[i].Test (&thys->Seq);
    if (ret) {
      ajDebug ("Sequence must be %s,\n found bad character '%c'",
	     seqType[i].Desc, ret);
      return ajFalse;
    }
    else {
      if (seqType[i].Gaps)
	ajSeqGap (thys, seqGap, 0);
      return ajTrue;
    }
  }

  ajErr ("Sequence type '%S' unknown", Type);
  return ajFalse;
}

/* @funcstatic seqTypeSet *****************************************************
**
** Sets the sequence type. Uses the first character of the type
** which can be N or P
**
** @param [P] thys [AjPSeq] Sequence object
** @param [P] Type [AjPStr] Sequence type
** @return [void]
** @@
******************************************************************************/

static void seqTypeSet (AjPSeq thys, AjPStr Type) {

  char* cp = ajStrStr(Type);

  ajDebug ("seqTypeSet '%S'\n", Type);

  switch (*cp) {
  case 'P':
  case 'p':
    ajSeqSetProt(thys);
    break;
  case 'N':
  case 'n':
    ajSeqSetNuc(thys);
    break;
  case '\0':
    break;
  default:
    ajErr("Unknown sequence type '%c'", *cp);
  }

  return;
}

/* @func ajSeqTypeCheckS ******************************************************
**
** Tests the type of a sequence is compatible with a defined type.
** If the type can have gaps, also tests for gap characters.
** Used for input validation - writes error message if the type check fails
**
** @param [P] pthys [AjPStr*] Sequence string
** @param [P] type [AjPStr] Sequence type
** @return [AjBool] ajTrue if compatible.
** @@
******************************************************************************/

AjBool ajSeqTypeCheckS (AjPStr* pthys, AjPStr type) {

  ajint i = 0;
  char ret;

  ajDebug ("ajSeqTypeCheckS type '%S' seq '%S'\n", type, *pthys);

  if (!ajStrLen(type)) {		/* nothing given - anything goes */
    ajSeqGapS (pthys, seqGap);
    return ajTrue;
  }

  for (i = 0; seqType[i].Name; i++) {
    if (!ajStrMatchCaseC(type, seqType[i].Name)) continue;
    ajDebug ("type '%s' found (%s)\n", seqType[i].Name, seqType[i].Desc);

    /* no need to test sequence type - we will test every character below */

    /* Calling funclist seqType() */
    ret = seqType[i].Test (pthys);
    if (ret) {
      ajErr ("Sequence must be %s,\n found bad character '%c'",
	     seqType[i].Desc, ret);
      return ajFalse;
    }
    else {
      if (seqType[i].Gaps)
	ajSeqGapS (pthys, seqGap);
      return ajTrue;
    }
  }

  ajErr ("Seqout type '%S' unknown", type);
  return ajFalse;
}

/* @func ajSeqTypeCheckIn *****************************************************
**
** Tests the type of a sequence is compatible with a defined type.
** If the type can have gaps, also tests for gap characters.
** Used for input validation - writes error message if the type check fails
**
** @param [P] thys [AjPSeq] Sequence object
** @param [P] seqin [AjPSeqin] Sequence input object
** @return [AjBool] ajTrue if compatible.
** @@
******************************************************************************/

AjBool ajSeqTypeCheckIn (AjPSeq thys, AjPSeqin seqin) {

  ajint i = 0;
  char ret;

  AjPStr Type = seqin->Inputtype; /* ACD file had a predefined seq type */

  ajDebug ("testing sequence '%s' type '%S' IsNuc %B IsProt %B\n",
	   ajSeqName(thys), seqin->Inputtype, seqin->IsNuc, seqin->IsProt);

  if (seqin->IsNuc)
    ajSeqSetNuc(thys);

  if (seqin->IsProt)
    ajSeqSetProt(thys);

  if (seqin->Query && ajStrLen(seqin->Query->DbType)) {
    seqTypeSet(thys, seqin->Query->DbType);
  }

  if (!ajStrLen(Type)) {		/* nothing given - anything goes */
    ajSeqGap (thys, seqGap, 0);
    return ajTrue;
  }

  for (i = 0; seqType[i].Name; i++) {
    if (!ajStrMatchCaseC(Type, seqType[i].Name)) continue;
    ajDebug ("type '%s' found (%s)\n", seqType[i].Name, seqType[i].Desc);
    if (seqType[i].Type == ISPROT && !ajSeqIsProt(thys)) {
      ajErr("Sequence is not a protein\n");
      return ajFalse;
    }
    if (seqType[i].Type == ISNUC && !ajSeqIsNuc(thys)) {
      ajErr("Sequence is not nucleic\n");
      return ajFalse;
    }
    /* Calling funclist seqType() */
    ret = seqType[i].Test (&thys->Seq);
    if (ret) {
      ajErr ("Sequence %s must be %s,\n found bad character '%c'",
	     ajSeqName(thys), seqType[i].Desc, ret);
      return ajFalse;
    }
    else {
      if (seqType[i].Gaps)
	ajSeqGap (thys, seqGap, 0);
      return ajTrue;
    }
  }

  ajErr ("Sequence type '%S' unknown", Type);
  return ajFalse;
}

/* @func ajSeqTypeAnyS ********************************************************
**
** Checks sequence type for any valid sequence character (but no gaps)
**
** @param [r] pthys [AjPStr*] Sequence string (not changed at present)
** @return [char] bad character if found, or null.
** @@
******************************************************************************/

char ajSeqTypeAnyS (AjPStr* pthys) {
  static char seqchars[256] = "";

  if (!*seqchars) {
    (void) strcpy (seqchars, seqCharProtPure);
    (void) strcat (seqchars, seqCharProtAmbig);
    (void) strcat (seqchars, seqCharProtStop);
    (void) strcat (seqchars, seqCharNucPure);
    (void) strcat (seqchars, seqCharNucAmbig);
  }
  ajDebug ("seqTypeAnyS test for '%s'\n", seqchars);

  return seqTypeTest (*pthys, seqchars);
}

/* @func ajSeqTypeDnaS ********************************************************
**
** Checks sequence type for DNA.
**
** RNA codes are accepted but are converted to DNA.
**
** @param [r] pthys [AjPStr*] Sequence string
** @return [char] invalid character if any.
** @@
******************************************************************************/

char ajSeqTypeDnaS (AjPStr* pthys) {
  static char seqchars[256] = "";

  if (!*seqchars) {
    (void) strcpy(seqchars, seqCharNucPure);
    (void) strcat(seqchars, seqCharNucAmbig);
  }

  ajDebug ("seqTypeDnaS test for '%s'\n", seqchars);

  (void) ajStrConvertCC (pthys, seqCharNucRNA, seqCharNucDNA);

  return seqTypeTest (*pthys, seqchars);
}

/* @func ajSeqTypeRnaS ********************************************************
**
** Checks sequence type for RNA.
**
** DNA codes are accepted but are converted to RNA.
**
** @param [r] pthys [AjPStr*] Sequence string
** @return [char] invalid character if any.
** @@
******************************************************************************/

char ajSeqTypeRnaS (AjPStr* pthys) {
  static char seqchars[256] = "";

  if (!*seqchars) {
    (void) strcpy(seqchars, seqCharNucPure);
    (void) strcat(seqchars, seqCharNucAmbig);
  }

  ajDebug ("seqTypeRnaS test for '%s'\n", seqchars);

  (void) ajStrConvertCC (pthys, seqCharNucDNA, seqCharNucRNA);

  return seqTypeTest (*pthys, seqchars);
}

/* @func ajSeqTypePurednaS ****************************************************
**
** Checks sequence type for pure (unambiguous) DNA.
**
** RNA codes are accepted but are converted to DNA.
**
** @param [r] pthys [AjPStr*] Sequence string
** @return [char] invalid character if any.
** @@
******************************************************************************/

char ajSeqTypePurednaS (AjPStr *pthys) {

  ajDebug ("seqTypePurednaS test for '%s'\n", seqCharNucPure);

  (void) ajStrConvertCC (pthys, seqCharNucRNA, seqCharNucDNA);

  return seqTypeTest (*pthys, seqCharNucPure);
}

/* @func ajSeqTypePurernaS ****************************************************
**
** Checks sequence type for pure (unambiguous) RNA.
**
** DNA codes are accepted but are converted to RNA.
**
** @param [r] pthys [AjPStr*] Sequence string
** @return [char] invalid character if any.
** @@
******************************************************************************/

char ajSeqTypePurernaS (AjPStr* pthys) {

  ajDebug ("seqTypePurernaS test for '%s'\n", seqCharNucPure);

  (void) ajStrConvertCC (pthys, seqCharNucDNA, seqCharNucRNA);

  return seqTypeTest (*pthys, seqCharNucPure);
}

/* @func ajSeqTypeNucS ********************************************************
**
** Checks sequence type for nucleotide.
**
** RNA and DNA codes are accepted as is.
**
** @param [r] pthys [AjPStr*] Sequence string (unchanged at present)
** @return [char] invalid character if any.
** @@
******************************************************************************/

char ajSeqTypeNucS (AjPStr* pthys) {
  static char seqchars[256] = "";

  if (!*seqchars) {
    (void) strcpy(seqchars, seqCharNucPure);
    (void) strcat(seqchars, seqCharNucAmbig);
  }

  ajDebug ("seqTypeNuc test for '%s'\n", seqchars);

  return seqTypeTest (*pthys, seqchars);
}

/* @func ajSeqTypePurenucS ****************************************************
**
** Checks sequence type for pure (unambiguous) nucleotide.
**
** RNA and DNA codes are accepted as is.
**
** @param [r] pthys [AjPStr*] Sequence string (unchanged at present)
** @return [char] invalid character if any.
** @@
******************************************************************************/

char ajSeqTypePurenucS (AjPStr* pthys) {

  ajDebug ("seqTypePurenucS test for '%s'\n", seqCharNucPure);

  return seqTypeTest (*pthys, seqCharNucPure);
}

/* @func ajSeqTypeGapnucS *****************************************************
**
** Checks sequence type for nucleotide with gaps.
**
** RNA and DNA codes are accepted as is.
**
** @param [r] pthys [AjPStr*] Sequence string (unchanged at present)
** @return [char] invalid character if any.
** @@
******************************************************************************/

char ajSeqTypeGapnucS (AjPStr* pthys) {
  static char seqchars[256] = "";

  if (!*seqchars) {
    (void) strcpy(seqchars, seqCharNucPure);
    (void) strcat(seqchars, seqCharNucAmbig);
    (void) strcat(seqchars, seqCharGap);
  }

  ajDebug ("seqTypeGapnucS test for '%s'\n", seqchars);

  return seqTypeTest (*pthys, seqchars);
}


/* @func ajSeqTypeGapdnaS *****************************************************
**
** Checks sequence type for DNA with gaps.
**
** RNA codes are accepted an converted to DNA.
**
** @param [r] pthys [AjPStr*] Sequence string (unchanged at present)
** @return [char] invalid character if any.
** @@
******************************************************************************/

char ajSeqTypeGapdnaS (AjPStr* pthys) {
  static char seqchars[256] = "";

  if (!*seqchars) {
    (void) strcpy(seqchars, seqCharNucPure);
    (void) strcat(seqchars, seqCharNucAmbig);
    (void) strcat(seqchars, seqCharGap);
  }

  ajDebug ("seqTypeGapdnaS test for '%s'\n", seqchars);

  (void) ajStrConvertCC (pthys, seqCharNucRNA, seqCharNucDNA);

  return seqTypeTest (*pthys, seqchars);
}

/* @func ajSeqTypeGaprnaS *****************************************************
**
** Checks sequence type for RNA with gaps.
**
** DNA codes are accepted an converted to RNA.
**
** @param [r] pthys [AjPStr*] Sequence string
** @return [char] invalid character if any.
** @@
******************************************************************************/

char ajSeqTypeGaprnaS (AjPStr* pthys) {
  static char seqchars[256] = "";

  if (!*seqchars) {
    (void) strcpy(seqchars, seqCharNucPure);
    (void) strcat(seqchars, seqCharNucAmbig);
    (void) strcat(seqchars, seqCharGap);
  }

  ajDebug ("seqTypeGaprnaS test for '%s'\n", seqchars);

  (void) ajStrConvertCC (pthys, seqCharNucDNA, seqCharNucRNA);

  return seqTypeTest (*pthys, seqchars);
}

/* @func ajSeqTypeProtS *******************************************************
**
** Checks sequence type for protein.
**
** A stop at the end is allowed (but is removed).
**
** @param [r] pthys [AjPStr*] Sequence string
** @return [char] invalid character if any.
** @@
******************************************************************************/

char ajSeqTypeProtS (AjPStr* pthys) {
  static char seqchars[256] = "";

  if (!*seqchars) {
    (void) strcpy(seqchars, seqCharProtPure);
    (void) strcat(seqchars, seqCharProtAmbig);
  }

  ajDebug ("seqTypeProtS test for '%s'\n", seqchars);

  (void) seqTypeStopTrimS(pthys);

  return seqTypeTest (*pthys, seqchars);
}

/* @func ajSeqTypePureprotS ***************************************************
**
** Checks sequence type for (unambiguous) protein.
**
** A stop at the end is allowed (but is removed).
**
** @param [r] pthys [AjPStr*] Sequence string
** @return [char] invalid character if any.
** @@
******************************************************************************/

char ajSeqTypePureprotS (AjPStr* pthys) {

  ajDebug ("seqTypePureprotS test for '%s'\n", seqCharProtPure);

  (void) seqTypeStopTrimS(pthys);

  return seqTypeTest (*pthys, seqCharProtPure);
}

/* @func ajSeqTypeAnyprotS ****************************************************
**
** Checks sequence type for anything that can be in a protein sequence
**
** Stop codes are replaced with gaps.
**
** @param [r] pthys [AjPStr*] Sequence string (unchanged at present)
** @return [char] invalid character if any.
** @@
******************************************************************************/

char ajSeqTypeAnyprotS (AjPStr* pthys) {
  static char seqchars[256] = "";

  if (!*seqchars) {
    (void) strcpy(seqchars, seqCharProtPure);
    (void) strcat(seqchars, seqCharProtAmbig);
    (void) strcat(seqchars, seqCharGap);
    (void) strcat(seqchars, seqCharProtStop);
  }

  ajDebug ("seqTypeAnyprotS test for '%s'\n", seqchars);

  return seqTypeTest (*pthys, seqchars);
}

/* @func ajSeqTypeGapprotS ****************************************************
**
** Checks sequence type for protein with gaps.
**
** Stop codes are replaced with gaps.
**
** @param [r] pthys [AjPStr*] Sequence string
** @return [char] invalid character if any.
** @@
******************************************************************************/

char ajSeqTypeGapprotS (AjPStr *pthys) {
  static char seqchars[256] = "";

  if (!*seqchars) {
    (void) strcpy(seqchars, seqCharProtPure);
    (void) strcat(seqchars, seqCharProtAmbig);
    (void) strcat(seqchars, seqCharGap);
  }

  ajDebug ("seqTypeGapprotS test for '%s'\n", seqchars);

  (void) ajStrConvertCC (pthys, seqCharProtStop, "-");

  return seqTypeTest (*pthys, seqchars);
}

/* @func ajSeqTypeStopprotS ***************************************************
**
** Checks sequence type for protein.
**
** Stops ('*') are allowed so this could be a 3 frame translation of DNA.
**
** @param [r] pthys [AjPStr*] Sequence string
** @return [char] invalid character if any.
** @@
******************************************************************************/

char ajSeqTypeStopprotS (AjPStr* pthys) {
  static char seqchars[256] = "";

  if (!*seqchars) {
    (void) strcpy(seqchars, seqCharProtPure);
    (void) strcat(seqchars, seqCharProtAmbig);
    (void) strcat(seqchars, seqCharProtStop);
  }

  ajDebug ("seqTypeStopprotS test for '%s'\n", seqchars);

  return seqTypeTest (*pthys, seqchars);
}

/* @func ajSeqTypeGapanyS *****************************************************
**
** Checks sequence type for any sequence with gaps.
**
** Stops ('*') are allowed so this could be a 3 frame translation of DNA.
**
** @param [r] pthys [AjPStr*] Sequence string (unchanged at present)
** @return [char] invalid character if any.
** @@
******************************************************************************/

char ajSeqTypeGapanyS (AjPStr* pthys) {
  static char seqchars[512] = "";

  if (!*seqchars) {
    (void) strcpy(seqchars, seqCharProtPure);
    (void) strcat(seqchars, seqCharProtAmbig);
    (void) strcat(seqchars, seqCharProtStop);
    (void) strcat(seqchars, seqCharNucPure);
    (void) strcat(seqchars, seqCharNucAmbig);
    (void) strcat(seqchars, seqCharGap);
  }

  ajDebug ("seqTypeGapanyS test for '%s'\n", seqchars);

  return seqTypeTest (*pthys, seqchars);
}

/* @func ajSeqGap *************************************************************
**
** Sets non-sequence characters to valid gap characters,
** and pads with extra gaps if necessary to a specified length
**
** @param [u] thys [AjPSeq] Sequence
** @param [r] gapc [char] Standard gap character
** @param [r] padc [char] Gap character for ends of sequence
** @return [void]
** @@
******************************************************************************/

void ajSeqGap (AjPSeq thys, char gapc, char padc) {
  seqGapSL (&thys->Seq, gapc, padc, 0);
}

/* @func ajSeqGapLen **********************************************************
**
** Sets non-sequence characters to valid gap characters,
** and pads with extra gaps if necessary to a specified length
**
** @param [u] thys [AjPSeq] Sequence
** @param [r] gapc [char] Standard gap character
** @param [r] padc [char] Gap character for ends of sequence
** @param [r] ilen [ajint] Sequence length. Expanded if longer than
**                       current length
** @return [void]
** @@
******************************************************************************/

void ajSeqGapLen (AjPSeq thys, char gapc, char padc, ajint ilen) {
  seqGapSL (&thys->Seq, gapc, padc, ilen);
}

/* @func ajSeqGapS ************************************************************
**
** Sets non-sequence characters to valid gap characters,
** and pads with extra gaps if necessary to a specified length
**
** @param [u] seq [AjPStr*] Sequence
** @param [r] gapc [char] Standard gap character
** @return [void]
** @@
******************************************************************************/

void ajSeqGapS (AjPStr* seq, char gapc) {
  seqGapSL (seq, gapc, 0, 0);
}

/* @funcstatic seqGapSL *******************************************************
**
** Sets non-sequence characters in a string to valid gap characters,
** and pads with extra gaps if necessary to a specified length
**
** @param [u] seq [AjPStr*] String of sequence characters
** @param [r] gapc [char] Standard gap character
** @param [r] padc [char] Gap character for ends of sequence
** @param [r] ilen [ajint] Sequence length. Expanded if longer than
**                       current length
** @return [void]
** @@
******************************************************************************/

static void seqGapSL (AjPStr* seq, char gapc, char padc, ajint ilen) {

  ajint i;
  static char* newgap=NULL;
  static ajint igap;
  char* cp;
  char endc = gapc;

  igap = strlen(seqCharGapTest);
  if (!newgap){
    newgap = ajCharNewL(igap);
    newgap[0] = '\0';
  }

  /* Set the newgap string to match gapc */

  if (*newgap != gapc) {
    for (i=0; i < igap; i++)
      newgap[i] = gapc;
    newgap[i] = '\0';
  }

  /*
//  ajDebug("seqGapSL gapc '%c' padc '%c' len %d seqCharGapTest '%s'\n",
//	  gapc, padc, ilen, seqCharGapTest);
//  ajDebug("seqGapSL before '%S'\n", *seq);
  */

  if (ilen)
    (void) ajStrModL (seq, ilen+1);
  else
    (void) ajStrMod(seq);

  (void) ajStrConvertCC (seq, seqCharGapTest, newgap);

  if (padc) {			/* start and end characters updated */
    endc = padc;
    for (cp = ajStrStr(*seq);
	 strchr(seqCharGapTest, *cp); cp++) /* pad start */
    {
      *cp = padc;
    }
    cp = ajStrStr(*seq);
    for (i=ajStrLen(*seq) - 1; i && strchr(seqCharGapTest, cp[i]);  i--)
      cp[i] = padc;
  }

  if (ajStrLen(*seq) < ilen) {	/* ilen can be zero to skip this */
    cp = ajStrStr(*seq);
    for (i=ajStrLen(*seq); i < ilen; i++)
      cp[i] = endc;
    cp[ilen] = '\0';
    ajStrFix(*seq);
  }

  /*  ajDebug("seqGapSL after  '%S'\n", *seq); */

  return;
}

/* @funcstatic seqTypeStopTrimS ***********************************************
**
** Removes a trailing stop (asterisk) from a protein sequence
**
** @param [P] pthys [AjPStr*] Sequence string
** @return [AjBool] ajTrue if a stop was removed.
** @@
******************************************************************************/

static AjBool seqTypeStopTrimS (AjPStr* pthys) {

  if (strchr(seqCharProtStop,ajStrChar(*pthys, -1))) {
    ajDebug("Trailing stop removed %c\n", ajStrChar(*pthys, -1));
    (void) ajStrTrim(pthys, -1);
    return ajTrue;
  }
  return ajFalse;
}

/* @func ajSeqSetNuc **********************************************************
**
** Sets a sequence type to "nucleotide"
**
** @param [P] thys [AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

void ajSeqSetNuc (AjPSeq thys) {
  (void) ajStrAssC (&thys->Type, "N");
}

/* @func ajSeqSetProt *********************************************************
**
** Sets a sequence type to "protein"
**
** @param [P] thys [AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

void ajSeqSetProt (AjPSeq thys) {
  (void) ajStrAssC (&thys->Type, "P");
}

/* @func ajSeqType ************************************************************
**
** Sets the type of a sequence if it has not yet been defined.
**
** @param [P] thys [AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

void ajSeqType (AjPSeq thys) {

  ajDebug ("ajSeqType current: %S\n", thys->Type);

  if (ajStrLen(thys->Type))
    return;

  if (ajSeqIsNuc (thys)) {
    ajSeqSetNuc(thys);
    ajDebug ("ajSeqType nucleotide: %S\n", thys->Type);
    return;
  }
  if (ajSeqIsProt (thys)) {
    ajSeqSetProt(thys);
    ajDebug ("ajSeqType protein: %S\n", thys->Type);
    return;
  }

  ajDebug ("ajSeqType unknown: %S\n", thys->Type);
  return;
}

/* @func ajSeqPrintType *******************************************************
**
** Prints the seqType definitions.
** For EMBOSS entrails output
**
** @param [R] outf [AjPFile] Output file
** @param [R] full [AjBool] Full output
** @return [void]
******************************************************************************/

void ajSeqPrintType (AjPFile outf, AjBool full) {
  ajint i;

  char* typeName[] = {"ANY", "NUC", "PRO"};

  ajFmtPrintF (outf, "\n#Sequence Types\n");
  ajFmtPrintF (outf, "# Name            Gap N/P Desciption\n");
  ajFmtPrintF (outf, "seqType {\n");
  for (i=0; seqType[i].Name; i++) {
    ajFmtPrintF (outf, "  %-15s %3B %s \"%s\"\n",
		 seqType[i].Name, seqType[i].Gaps,
		 typeName[seqType[i].Type], seqType[i].Desc);

  }
  ajFmtPrintF (outf, "}\n");
}

/* @funcstatic seqTypeTest ****************************************************
**
** Checks sequence contains only expected characters.
**
** Returns an invalid character for failure, or a null character for success.
**
** @param [R] thys [AjPStr] Sequence string
** @param [R] seqchars [char*] Sequence characters allowed
** @return [char] invalid character if any.
******************************************************************************/

static char seqTypeTest (AjPStr thys, char* seqchars) {
  ajint i;

  if (!ajStrLen(thys))
    return '\0';

  i = strspn(ajStrStr(thys), seqchars);
  if (i < ajStrLen(thys))
    return ajStrChar(thys, i);

  return '\0';
}
