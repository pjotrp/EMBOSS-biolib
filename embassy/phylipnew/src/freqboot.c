#include "phylip.h"
#include "seq.h"

/* version 3.6. (c) Copyright 1993-2002 by the University of Washington.
   Written by Joseph Felsenstein, Akiko Fuseki, Sean Lamont, and Andrew Keeffe.
   Permission is granted to copy and use this program provided no fee is
   charged for it and provided that this copyright notice is not removed. */

typedef enum {
  seqs, morphology, restsites, genefreqs
} datatype;

typedef enum {
  dna, rna, protein
} seqtype;

AjPSeqset seqset = NULL;
AjPPhyloState phylorest = NULL;
AjPPhyloState phylostate = NULL;
AjPPhyloFreq phylofreq = NULL;
AjPPhyloProp phyloweights = NULL;
AjPPhyloProp phylofact = NULL;

boolean printdata;
steptr weight;

#ifndef OLDC
/* function prototypes */
/*void   getoptions(void);*/
void emboss_getoptions(char *pgm, int argc, char *argv[]);
void   seqboot_inputnumbersfreq(AjPPhyloFreq);
void   seqboot_inputnumbersrest(AjPPhyloState);
void   seqboot_inputnumbersstate(AjPPhyloState);
void   seqboot_inputfactors(AjPPhyloProp);
void   inputoptions(void);
void   seqboot_inputdataseq(AjPSeqset);
void   seqboot_inputdatafreq(AjPPhyloFreq);
void   seqboot_inputdatarest(AjPPhyloState);
void   seqboot_inputdatastate(AjPPhyloState);
void   allocrest(void);
void   doinput(int argc, Char *argv[]);
void   bootweights(void);
void   sppermute(long);
void   writedata(void);
void   writeweights(void);
void   bootwrite(void);
/* function prototypes */
#endif


FILE *outcatfile, *outweightfile;
AjPFile embossoutcatfile;
AjPFile embossoutweightfile;
Char infilename[FNMLNGTH], catfilename[FNMLNGTH],
   weightfilename[FNMLNGTH];
const char* outfilename;
const char* outcatfilename;
const char* outweightfilename;
long sites, loci, maxalleles, groups, newsites, newersites,
             newgroups, newergroups, nenzymes, reps, ws, blocksize, categs;
boolean bootstrap, permute, jackknife, xml, weights, factors,
          enzymes, all, justwts, progress;
datatype data;
seqtype seq;
steptr oldweight, where, how_many, newwhere, newhowmany,
             newerwhere, newerhowmany, factorr, newerfactor;
Char *factor;
long *alleles;
Char **nodep;
double **nodef;
long **sppord;
longer seed;



/************ EMBOSS GET OPTIONS ROUTINES ******************************/

void emboss_getoptions(char *pgm, int argc, char *argv[])
{
    AjStatus retval;
 
    /* initialize global variables */

    ajNamInit("emboss");
    retval =  ajAcdInitP (pgm, argc, argv, "PHYLIP");

    /* ajAcdGet */

    /* init functions for standard ajAcdGet */

    /* cleanup for clashing options */

}

/************ END EMBOSS GET OPTIONS ROUTINES **************************/

/*
//void getoptions()
//{
//  /# interactively set options #/
//  long reps0;
//  long inseed, inseed0, loopcount, loopcount2;
//  Char ch;
//  boolean done1;
//
//  data = seqs;
//  seq = dna;
//  bootstrap = true;
//  jackknife = false;
//  permute = false;
//  blocksize = 1;
//  all = false;
//  reps = 100;
//  weights = false;
//  categories = false;
//  justwts = false;
//  printdata = false;
//  dotdiff = true;
//  progress = true;
//  interleaved = true;
//  xml = false;
//  loopcount = 0;
//  for (;;) {
//    cleerhome();
//    printf("\nBootstrapping algorithm, version %s\n\n",VERSION);
//    printf("Settings for this run:\n");
//    printf("  D      Sequence, Morph, Rest., Gene Freqs?  %s\n",
//           (data == seqs       ) ? "Molecular sequences"      :
//           (data == morphology ) ? "Discrete Morphology"      :
//           (data == restsites)   ? "Restriction Sites"        :
//           (data == genefreqs)   ? "Gene Frequencies" : "");
//    if (data == restsites)
//      printf("  E                       Number of enzymes?  %s\n",
//             enzymes ? "Present in input file" :
//                       "Not present in input file");
//    if (data == genefreqs)
//      printf("  A       All alleles present at each locus?  %s\n",
//             all ? "Yes" : "No, one absent at each locus");
//
//    printf("  J  Bootstrap, Jackknife, Permute, Rewrite?  %s\n",
//           jackknife ? "Delete-half jackknife"                 :
//           permute   ? "Permute species for each character"    :
//           bootstrap ? "Bootstrap" : "Rewrite data");
//    if (!(jackknife || permute || bootstrap)) {
//      printf("  P             PHYLIP or XML output format?  %s\n",
//           xml ? "XML" : "PHYLIP");
//      if (xml) {
//        printf("  S             Type of molecular sequences?  ");
//        switch (seq) {
//          case (dna) : printf("DNA\n"); break;
//          case (rna) : printf("RNA\n"); break;
//          case (protein) : printf("Protein\n"); break;
//        }
//      }
//    }
//    if (bootstrap) {
//      if (blocksize > 1)
//      printf("  B      Block size for block-bootstrapping?  %ld\n", blocksize);
//      else
//        printf("  B      Block size for block-bootstrapping?  %ld (regular bootstrap)\n", blocksize);
//    }
//    if (bootstrap || jackknife || permute)
//      printf("  R                     How many replicates?  %ld\n", reps);
//    if (jackknife || bootstrap || permute) {
//      printf("  W              Read weights of characters?  %s\n",
//          (weights ? "Yes" : "No"));
//      if (data == seqs)
//        printf("  C                Read categories of sites?  %s\n",
//          (categories ? "Yes" : "No"));
//      if ((!permute)) {
//        printf("  F     Write out data sets or just weights?  %s\n",
//          (justwts ? "Just weights" : "Data sets"));
//      }
//    }
//    if (data == seqs || data == restsites)
//      printf("  I             Input sequences interleaved?  %s\n",
//             interleaved ? "Yes" : "No, sequential");
//    printf("  0      Terminal type (IBM PC, ANSI, none)?  %s\n",
//           ibmpc ? "IBM PC" : ansi  ? "ANSI" : "(none)");
//    printf("  1       Print out the data at start of run  %s\n",
//           printdata ? "Yes" : "No");
//    if (printdata)
//      printf("  .     Use dot-differencing to display them  %s\n",
//           dotdiff ? "Yes" : "No");
//    printf("  2     Print indications of progress of run  %s\n",
//           progress ? "Yes" : "No");
//    printf("\n  Y to accept these or type the letter for one to change\n");
//#ifdef WIN32
//    phyFillScreenColor();
//#endif
//    scanf("%c%*[^\n]", &ch);
//    getchar();
//    uppercase(&ch);
//    if (ch == 'Y')
//           break;
//    if ((bootstrap && (strchr("ABCDEFJPRWI1.20",ch) != NULL)) ||
//        ((jackknife || permute) && (strchr("ACDEFJPRWI1.20",ch) != NULL)) ||
//        (((!bootstrap) && (!jackknife) && (!permute)) &&
//            ((!xml && (strchr("ADEJPI1.20",ch) != NULL)) ||
//             (xml && (strchr("ADEJPSI1.20",ch) != NULL))))) {
//      switch (ch) {
//        
//      case 'D':
//        if (data == genefreqs)
//          data = seqs;
//        else
//          data = (datatype)((long)data + 1);
//        break;
//        
//      case 'A':
//        all = !all;
//        break;
//        
//      case 'E':
//        enzymes = !enzymes;
//        break;
//        
//      case 'J':
//        if (permute)
//          permute = false;
//        else if (jackknife) {
//            jackknife = false;
//            permute = true;
//        } else if (bootstrap) {
//              bootstrap = false;
//              jackknife = true;
//          } else
//              bootstrap = true;
//        break;
//        
//      case 'P':
//        xml = !xml;
//        break;
//        
//      case 'S':
//        switch (seq) {
//          case (dna): seq = rna; break;
//          case (rna): seq = protein; break;
//          case (protein): seq = dna; break;
//        }
//        break;
//        
//      case 'B':
//        loopcount2 = 0;
//        do {
//          printf("Block size?\n");
//#ifdef WIN32
//          phyFillScreenColor();
//#endif
//          scanf("%ld%*[^\n]", &blocksize);
//          getchar();
//          done1 = (blocksize > 0);
//          if (!done1) {
//            printf("BAD NUMBER: must be positive\n");
//          }
//          countup(&loopcount2, 10);
//        } while (done1 != true);
//        break;
//
//      case 'R':
//        done1 = true;
//        reps0 = reps;
//        loopcount2 = 0;
//        do {
//          printf("Number of replicates?\n");
//#ifdef WIN32
//          phyFillScreenColor();
//#endif
//          scanf("%ld%*[^\n]", &reps);
//          getchar();
//          done1 = (reps > 0);
//          if (!done1) {
//            printf("BAD NUMBER: must be positive\n");
//            reps = reps0;
//          }
//          countup(&loopcount2, 10);
//        } while (done1 != true);
//        break;
//
//      case 'W':
//        weights = !weights;
//        break;
//
//      case 'C':
//        categories = !categories;
//        break;
//
//      case 'F':
//        justwts = !justwts;
//        break;
//
//      case 'I':
//        interleaved = !interleaved;
//        break;
//        
//      case '0':
//        initterminal(&ibmpc, &ansi);
//        break;
//        
//      case '1':
//        printdata = !printdata;
//        break;
//        
//      case '.':
//        dotdiff = !dotdiff;
//        break;
//        
//      case '2':
//        progress = !progress;
//        break;
//      }
//    } else
//      printf("Not a possible option!\n");
//    countup(&loopcount, 100);
//  }
//  if (bootstrap || jackknife || permute)
//    initseed(&inseed, &inseed0, seed);
//  xml = xml && (data == seqs);
//}  /# getoptions #/
*/


void seqboot_inputnumbersfreq(AjPPhyloFreq freq)
{
  /* read numbers of species and of sites */
  long i;

  spp = freq->Size;
  sites = freq->Loci;
  /*fscanf(infile, "%ld%ld", &spp, &sites);*/
  loci = sites;
  maxalleles = 1;
  if (!freq->ContChar) {
    alleles = (long *)Malloc(sites*sizeof(long));
    sites = 0;
    for (i = 0; i < (loci); i++) {
      alleles[i] = freq->Allele[i];
      if (alleles[i] > maxalleles)
         maxalleles = alleles[i];
      sites += alleles[i];
    }
  }
}  /* seqboot_inputnumbers */


void seqboot_inputfactors(AjPPhyloProp fact)
{
  long i, j;
  Char ch, prevch='\0';
  AjPStr str = fact->Str[0];

  j = 0;
  for (i = 0; i < (sites); i++) {
    ch = ajStrChar(str,i);
    if (ch != prevch)
      j++;
    prevch = ch;
    factorr[i] = j;
  }
  factors = true;
}  /* seqboot_inputfactors */


void inputoptions()
{
  /* input the information on the options */
  /*Char ch;*/
  long extranum, weightsum, i;

  factors = false;
  for (i = 1; i <= (sites); i++)
      factorr[i - 1] = i;
  for (i = 0; i < (sites); i++)
    oldweight[i] = 1;
  if (weights)
    inputweightsstr2(phyloweights->Str[0],
		     1, sites, &weightsum, oldweight, &weights, "seqboot");
  extranum = 0;
  if (phylofact)
      seqboot_inputfactors(phylofact);
  if (factors && printdata) {
    for(i = 0; i < sites; i++)
      factor[i] = (char)('0' + (factorr[i]%10));
    printfactors(outfile, sites, factor, " (least significant digit)");
  }
  if (weights && printdata)
    printweights(outfile, 0, sites, oldweight, "Sites");
  for (i = 0; i < (loci); i++)
    how_many[i] = 0;
  for (i = 0; i < (loci); i++)
    where[i] = 0;
  for (i = 1; i <= (sites); i++) {
    how_many[factorr[i - 1] - 1]++;
    if (where[factorr[i - 1] - 1] == 0)
      where[factorr[i - 1] - 1] = i;
  }
  groups = factorr[sites - 1];
  newgroups = 0;
  newsites = 0;
  for (i = 0; i < (groups); i++) {
    if (oldweight[where[i] - 1] > 0) {
      newgroups++;
      newsites += how_many[i];
      newwhere[newgroups - 1] = where[i];
      newhowmany[newgroups - 1] = how_many[i];
    }
  }
}  /* inputoptions */


void seqboot_inputdataseq(AjPSeqset seqset)
{
  /* input the names and sequences for each species */
  long i, j, k, l, m, n;
  /*double x;*/
  Char charstate;
  boolean allread, done;
  AjPStr str;

  nodep = (Char **)Malloc(spp*sizeof(Char *));
  for (i = 0; i < (spp); i++)
      nodep[i] = (Char *)Malloc(sites*sizeof(Char));

  j = nmlngth + (sites + (sites - 1) / 10) / 2 - 5;
  if (j < nmlngth - 1)
    j = nmlngth - 1;
  if (j > 37)
    j = 37;
  if (printdata) {
    fprintf(outfile, "\nBootstrapping algorithm, version %s\n\n\n",VERSION);
    fprintf(outfile, "%3ld species, %3ld  sites\n\n", spp, sites);
    fprintf(outfile, "Name");
    for (i = 1; i <= j; i++)
      putc(' ', outfile);
    fprintf(outfile, "Data\n");
    fprintf(outfile, "----");
    for (i = 1; i <= j; i++)
      putc(' ', outfile);
    fprintf(outfile, "----\n\n");
  }
  allread = false;
  while (!allread) {
    allread = true;
    i = 1;
    while (i <= spp) {
        initnameseq(seqset, i-1);
	str = ajSeqStr(ajSeqsetGetSeq(seqset, i-1));
	j = 0;
	done = false;
	while (!done) {
	    while (j < sites) {
		charstate = ajStrChar(str, j);
		uppercase(&charstate);
		j++;
		if (charstate == '.')
		    charstate = nodep[0][j-1];
	    }
	    if (j == sites)
		done = true;
	}
      i++;
    }
      allread = (i > spp);
  }
  if (!printdata)
    return;
    m = (sites - 1) / 60 + 1;
  for (i = 1; i <= m; i++) {
    for (j = 0; j < (spp); j++) {
      for (k = 0; k < nmlngth; k++)
        putc(nayme[j][k], outfile);
      fprintf(outfile, "   ");
      l = i * 60;
      if (l > sites)
        l = sites;
      n = (i - 1) * 60;
      for (k = n; k < l; k++) {
        if (j + 1 > 1 && nodep[j][k] == nodep[0][k])
            charstate = '.';
          else
            charstate = nodep[j][k];
          putc(charstate, outfile);
          if ((k + 1) % 10 == 0 && (k + 1) % 60 != 0)
            putc(' ', outfile);
      }
      putc('\n', outfile);
    }
    putc('\n', outfile);
  }
  putc('\n', outfile);
}  /* seqboot_inputdata */

void seqboot_inputdatafreq(AjPPhyloFreq freq)
{
  /* input the names and sequences for each species */
  long i, j, k, l, m, n;
  double x;
  ajint ipos;

  nodef = (double **)Malloc(spp*sizeof(double *));
  for (i = 0; i < (spp); i++)
      nodef[i] = (double *)Malloc(sites*sizeof(double));
  j = nmlngth + (sites + (sites - 1) / 10) / 2 - 5;
  if (j < nmlngth - 1)
    j = nmlngth - 1;
  if (j > 37)
    j = 37;
  if (printdata) {
    fprintf(outfile, "\nBootstrapping algorithm, version %s\n\n\n",VERSION);
    fprintf(outfile, "%3ld species, %3ld  loci\n\n", spp, loci);
    fprintf(outfile, "Name");
    for (i = 1; i <= j; i++)
      putc(' ', outfile);
    fprintf(outfile, "Data\n");
    fprintf(outfile, "----");
    for (i = 1; i <= j; i++)
      putc(' ', outfile);
    fprintf(outfile, "----\n\n");
  }
  ipos = 0;
    for (i = 1; i <= (spp); i++)
    {
      initnamefreq(freq, i - 1);
      j = 1;
      while (j <= sites) {
        x = freq->Data[ipos++];
        if ((unsigned)x > 1.0)
	{
          printf("GENE FREQ OUTSIDE [0,1], species%3ld\n", i);
          exxit(-1);
        }
	else
	{
          nodef[i - 1][j - 1] = x;
          j++;
        }
      }
    }

  if (!printdata)
    return;
  m = (sites - 1) / 8 + 1;
  for (i = 1; i <= m; i++) {
    for (j = 0; j < (spp); j++) {
      for (k = 0; k < nmlngth; k++)
        putc(nayme[j][k], outfile);
      fprintf(outfile, "   ");
        l = i * 8;
      if (l > sites)
        l = sites;
        n = (i - 1) * 8;
      for (k = n; k < l; k++) {
          fprintf(outfile, "%8.5f", nodef[j][k]);

      }
      putc('\n', outfile);
    }
    putc('\n', outfile);
  }
  putc('\n', outfile);
}  /* seqboot_inputdatafreq */

void seqboot_inputdatarest(AjPPhyloState rest)
{
  /* input the names and sequences for each species */
  long i, j, k, l, m, n;
  Char charstate;
  boolean allread, done;
  AjPStr str;

  nodep = (Char **)Malloc(spp*sizeof(Char *));
  for (i = 0; i < (spp); i++)
      nodep[i] = (Char *)Malloc(sites*sizeof(Char));

  j = nmlngth + (sites + (sites - 1) / 10) / 2 - 5;
  if (j < nmlngth - 1)
    j = nmlngth - 1;
  if (j > 37)
    j = 37;
  if (printdata) {
    fprintf(outfile, "\nBootstrapping algorithm, version %s\n\n\n",VERSION);
    fprintf(outfile, "%3ld species, %3ld  sites\n\n", spp, sites);
    fprintf(outfile, "Name");
    for (i = 1; i <= j; i++)
      putc(' ', outfile);
    fprintf(outfile, "Data\n");
    fprintf(outfile, "----");
    for (i = 1; i <= j; i++)
      putc(' ', outfile);
    fprintf(outfile, "----\n\n");
  }

  allread = false;
  while (!allread) {
    allread = true;
    i = 1;
    while (i <= spp) {
        initnamestate(rest, i-1);
	str = rest->Str[i-1];
	j = 0;
	done = false;
	while (!done) {
	    while (j < sites) {
		charstate = ajStrChar(str, j);
		uppercase(&charstate);
		j++;
		if (charstate == '.')
		    charstate = nodep[0][j-1];
	    }
	    if (j == sites)
		done = true;
	}
      i++;
    }
      allread = (i > spp);
  }

  if (!printdata)
    return;
    m = (sites - 1) / 60 + 1;
  for (i = 1; i <= m; i++) {
    for (j = 0; j < (spp); j++) {
      for (k = 0; k < nmlngth; k++)
        putc(nayme[j][k], outfile);
      fprintf(outfile, "   ");
      l = i * 60;
      if (l > sites)
        l = sites;
      n = (i - 1) * 60;
      for (k = n; k < l; k++) {
        if (j + 1 > 1 && nodep[j][k] == nodep[0][k])
            charstate = '.';
          else
            charstate = nodep[j][k];
          putc(charstate, outfile);
          if ((k + 1) % 10 == 0 && (k + 1) % 60 != 0)
            putc(' ', outfile);
      }
      putc('\n', outfile);
    }
    putc('\n', outfile);
  }
  putc('\n', outfile);
}  /* seqboot_inputdata */

void seqboot_inputdatastate(AjPPhyloState state)
{
  /* input the names and sequences for each species */
  long i, j, k, l, m, n;
  Char charstate;
  boolean allread, done;
  AjPStr str;

  nodep = (Char **)Malloc(spp*sizeof(Char *));
  for (i = 0; i < (spp); i++)
      nodep[i] = (Char *)Malloc(sites*sizeof(Char));

  j = nmlngth + (sites + (sites - 1) / 10) / 2 - 5;
  if (j < nmlngth - 1)
    j = nmlngth - 1;
  if (j > 37)
    j = 37;
  if (printdata) {
    fprintf(outfile, "\nBootstrapping algorithm, version %s\n\n\n",VERSION);
    fprintf(outfile, "%3ld species, %3ld  sites\n\n", spp, sites);
    fprintf(outfile, "Name");
    for (i = 1; i <= j; i++)
      putc(' ', outfile);
    fprintf(outfile, "Data\n");
    fprintf(outfile, "----");
    for (i = 1; i <= j; i++)
      putc(' ', outfile);
    fprintf(outfile, "----\n\n");
  }

  allread = false;
  while (!allread) {
    allread = true;
    i = 1;
    while (i <= spp) {
        initnamestate(state, i-1);
	str = state->Str[i-1];
	j = 0;
	done = false;
	while (!done) {
	    while (j < sites) {
		charstate = ajStrChar(str, j);
		uppercase(&charstate);
		j++;
		if (charstate == '.')
		    charstate = nodep[0][j-1];
	    }
	    if (j == sites)
		done = true;
	}
      i++;
    }
      allread = (i > spp);
  }

  if (!printdata)
    return;
    m = (sites - 1) / 60 + 1;
  for (i = 1; i <= m; i++) {
    for (j = 0; j < (spp); j++) {
      for (k = 0; k < nmlngth; k++)
        putc(nayme[j][k], outfile);
      fprintf(outfile, "   ");
      l = i * 60;
      if (l > sites)
        l = sites;
      n = (i - 1) * 60;
      for (k = n; k < l; k++) {
        if (j + 1 > 1 && nodep[j][k] == nodep[0][k])
            charstate = '.';
          else
            charstate = nodep[j][k];
          putc(charstate, outfile);
          if ((k + 1) % 10 == 0 && (k + 1) % 60 != 0)
            putc(' ', outfile);
      }
      putc('\n', outfile);
    }
    putc('\n', outfile);
  }
  putc('\n', outfile);
}  /* seqboot_inputdata */

void allocrest()
{
  oldweight = (steptr)Malloc(sites*sizeof(long));
  weight = (steptr)Malloc(sites*sizeof(long));
  where = (steptr)Malloc(loci*sizeof(long));
  how_many = (steptr)Malloc(loci*sizeof(long));
  factor = (Char *)Malloc(sites*sizeof(Char));
  factorr = (steptr)Malloc(sites*sizeof(long));
  newwhere = (steptr)Malloc(loci*sizeof(long));
  newhowmany = (steptr)Malloc(loci*sizeof(long));
  newerwhere = (steptr)Malloc(loci*sizeof(long));
  newerhowmany = (steptr)Malloc(loci*sizeof(long));
  newerfactor = (steptr)Malloc(loci*maxalleles*sizeof(long));
  nayme = (naym *)Malloc(spp*sizeof(naym));
}  /* allocrest */


void doinput(int argc, Char *argv[])
{
  /* reads the input data */
/*  getoptions();*/
  seqboot_inputnumbersfreq(phylofreq);
  allocrest();
/*
  if (weights)
    openfile(&weightfile,WEIGHTFILE,"input weight file",
               "r",argv[0],weightfilename);
*/
  /*if (justwts && !permute)
    openfile(&outweightfile,"outweights","output weight file",
               "w",argv[0],outweightfilename);
  else {
    if (weights)*/
  embossoutweightfile = ajAcdGetOutfile("outweightfile");
      emboss_openfile(embossoutweightfile,&outweightfile,&outweightfilename);
  embossoutfile = ajAcdGetOutfile("outfile");
    emboss_openfile(embossoutfile,&outfile,&outfilename);
  /*}*/
  inputoptions();
  seqboot_inputdatafreq(phylofreq);
}  /* doinput */


void bootweights()
{
  /* sets up weights by resampling data */
  long i, j, k, blocks;
  double p, q, r;

  ws = newgroups;
  for (i = 0; i < (ws); i++)
    weight[i] = 0;
  if (jackknife) {
    if (newgroups & 1) {
      if (randum(seed) < 0.5)
        q = (newgroups - 1.0) / 2;
      else
        q = (newgroups + 1.0) / 2;
    } else
      q = newgroups / 2.0;
    r = newgroups;
    p = q / r;
    ws = 0;
    for (i = 0; i < (newgroups); i++) {
      if (randum(seed) < p) {
        weight[i]++;
        ws++;
        q--;
      }
      r--;
      if (i + 1 < newgroups)
        p = q / r;
    }
  } else if (permute) {
    for (i = 0; i < (newgroups); i++)
      weight[i] = 1;
  } else if (bootstrap) {
    blocks = newgroups / blocksize;
    for (i = 1; i <= (blocks); i++) {
      j = (long)(newgroups * randum(seed)) + 1;
      for (k = 0; k < blocksize; k++) {
        weight[j - 1]++;
        j++;
        if (j > newgroups)
          j = 1;
      }
    }
  } else             /* case of rewriting data */
    for (i = 0; i < (newgroups); i++)
      weight[i] = 1;
  for (i = 0; i < (newgroups); i++)
    newerwhere[i] = 0;
  for (i = 0; i < (newgroups); i++)
    newerhowmany[i] = 0;
  newergroups = 0;
  newersites = 0;
  for (i = 0; i < (newgroups); i++) {
    for (j = 1; j <= (weight[i]); j++) {
      newergroups++;
      for (k = 1; k <= (newhowmany[i]); k++) {
        newersites++;
        newerfactor[newersites - 1] = newergroups;
      }
      newerwhere[newergroups - 1] = newwhere[i];
      newerhowmany[newergroups - 1] = newhowmany[i];
    }
  }
}  /* bootweights */


void sppermute(long n)
{ long i, j, k;
  for (i = 1; i <= (spp - 1); i++) {
    k = (long)((i+1) * randum(seed));
    j = sppord[n - 1][i];
    sppord[n - 1][i] = sppord[n - 1][k];
    sppord[n - 1][k] = j;
  }
}  /* sppermute */


void writedata()
{
  /* write out one set of bootstrapped sequences */
  long i, j, k, l, m, n, n2;
  double x;

  sppord = (long **)Malloc(newergroups*sizeof(long *));
  for (i = 0; i < (newergroups); i++)
    sppord[i] = (long *)Malloc(spp*sizeof(long));
  for (j = 1; j <= spp; j++)
    sppord[0][j - 1] = j;
  for (i = 1; i < newergroups; i++) {
    for (j = 1; j <= (spp); j++)
      sppord[i][j - 1] = sppord[i - 1][j - 1];
  }
  if (!justwts || permute) {
    fprintf(outfile, "%5ld %5ld\n", spp, newergroups);
      for (i = 0; i < (newergroups); i++)
        fprintf(outfile, " %3ld", alleles[factorr[newerwhere[i] - 1] - 1]);
      putc('\n', outfile);
  }
  l = 1;
  m = newergroups;
    if (m > newergroups)
      m = newergroups;
    for (j = 0; j < spp; j++) {
      n = 0;
      if (l == 1) {
        if (!(bootstrap || jackknife || permute) && xml) {
          fprintf(outfile, "   <sequence");
          switch (seq) {
            case (dna): fprintf(outfile, " type=dna"); break;
            case (rna): fprintf(outfile, " type=rna"); break;
            case (protein): fprintf(outfile, " type=protein"); break;
          }
          fprintf(outfile, ">\n");
          fprintf(outfile, "      <name>");
        }
        n2 = nmlngth-1;
        if (!(bootstrap || jackknife || permute) && xml) {
          while (nayme[sppord[0][j] - 1][n2] == ' ')
            n2--;
        }
        for (k = 0; k <= n2; k++)
          putc(nayme[sppord[0][j] - 1][k], outfile);
        if (!(bootstrap || jackknife || permute) && xml)
          fprintf(outfile, "</name>\n      <data>");
      } else {
        if (!(bootstrap || jackknife || permute) && xml) {
          fprintf(outfile, "      ");
        }
        else {
          for (k = 1; k <= nmlngth; k++)
            putc(' ', outfile);
        }
      }
      for (k = l - 1; k < m; k++) {
        if (permute && j + 1 == 1)
          sppermute(newerfactor[n]);
        for (n2 = -1; n2 <= (newerhowmany[k] - 2); n2++) {
          n++;
          if (n > 1 && (n & 7) == 1)
              fprintf(outfile, "\n              ");
          x = nodef[sppord[newerfactor[n - 1] - 1][j] - 1][newerwhere[k] + n2];
            fprintf(outfile, "%8.5f", x);
        }
      }
      if (!(bootstrap || jackknife || permute) && xml) {
        fprintf(outfile, "</data>\n   </sequence>\n");
      }
      putc('\n', outfile);
    }

  for (i = 0; i < (newergroups); i++)
    free(sppord[i]);
  free(sppord);
}  /* writedata */


void writeweights()
{
  /* write out one set of post-bootstrapping weights */
  long k, l, m, n;

  l = 1;
  m = newergroups;
    if (m > newergroups)
      m = newergroups;
    n = 0;
    for (k = l - 1; k < m; k++) {
      if (weight[k] < 10)
        fprintf(outweightfile, "%c", (char)('0'+weight[k]));
      else
        fprintf(outweightfile, "%c", (char)('A'+weight[k]-10));
      n++;
      if (n > 1 && n % 60 == 1) {
        fprintf(outweightfile, "\n");
        if (n % 10 == 0 && n % 60 != 0)
          putc(' ', outweightfile);
      }
    }
    putc('\n', outweightfile);

}  /* writeweights */



void bootwrite()
{
  /* does bootstrapping and writes out data sets */
  long rr, repdiv10;

  if (!(bootstrap || jackknife || permute))
    reps = 1;
  repdiv10 = reps / 10;
  if (repdiv10 < 1)
    repdiv10 = 1;
  if (progress)
    putchar('\n');
  for (rr = 1; rr <= (reps); rr++) {
    bootweights();
    if (!justwts || permute)
      writedata();
    if (justwts && !permute)
      writeweights();
    if (progress && (bootstrap || jackknife || permute)
          && rr % repdiv10 == 0) {
      printf("completed replicate number %4ld\n", rr);
#ifdef WIN32
      phyFillScreenColor();
#endif
    }
  }
  if (progress) {
    if (justwts)
      printf("\nOutput weights written to file \"%s\"\n\n", outweightfilename);
    else
      printf("\nOutput written to file \"%s\"\n\n", outfilename);
  }
}  /* bootwrite */


int main(int argc, Char *argv[])
{  /* Read in sequences or frequencies and bootstrap or jackknife them */
#ifdef MAC
  argc = 1;                /* macsetup("SeqBoot","");                */
  argv[0] = "SeqBoot";
#endif
  init(argc,argv);
  emboss_getoptions("fseqboot",argc,argv);
  /*openfile(&infile,INFILE,"input file","r",argv[0],infilename);*/
  ibmpc = IBMCRT;
  ansi = ANSICRT;
  doinput(argc, argv);
  bootwrite();
  FClose(infile);
  if (weights)
    FClose(weightfile);
  if (justwts && !permute) {
    FClose(outweightfile);
  }
  else
    FClose(outfile);
#ifdef MAC
  fixmacfile(outfilename);
  if (justwts && !permute)
    fixmacfile(outweightfilename);
#endif
  printf("Done.\n\n");
#ifdef WIN32
  phyRestoreConsoleAttributes();
#endif
  ajExit();
  return 0;
}
