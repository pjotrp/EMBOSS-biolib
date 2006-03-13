#include "phylip.h"
#include "seq.h"

/* version 3.6. (c) Copyright 1993-2004 by the University of Washington.
   Written by Joseph Felsenstein, Akiko Fuseki, Sean Lamont, Andrew Keeffe,
   and Doug Buxton.
   Permission is granted to copy and use this program provided no fee is
   charged for it and provided that this copyright notice is not removed. */

typedef enum {
  seqs, morphology, restsites, genefreqs
} datatype;

typedef enum {
  dna, rna, protein
} seqtype;



AjPPhyloState* phylostate = NULL;
AjPPhyloProp phyloweights = NULL;
AjPPhyloProp phyloanc = NULL;
AjPPhyloProp phylomix = NULL;
AjPPhyloProp phylofact = NULL;

#ifndef OLDC
/* function prototypes */
//void   getoptions(void);
void emboss_getoptions(char *pgm, int argc, char *argv[]);

void   seqboot_inputnumbersstate(AjPPhyloState);

void   inputoptions(void);
void   seqboot_inputdatastate(AjPPhyloState);

void   allocrest(void);
void   allocnew(void);
void   doinput(int argc, Char *argv[]);
void   bootweights(void);
void   sppermute(long);
void   charpermute(long, long);
void   writedata(void);
void   writeweights(void);
void   writecategories(void);
void   writeauxdata(steptr, FILE*);
void   writefactors(void);
void   bootwrite(void);
void   seqboot_inputaux(steptr, FILE*);
/* function prototypes */
#endif


FILE *outcatfile, *outweightfile, *outmixfile, *outancfile, *outfactfile;
Char infilename[FNMLNGTH], catfilename[FNMLNGTH], weightfilename[FNMLNGTH], mixfilename[FNMLNGTH], ancfilename[FNMLNGTH], factfilename[FNMLNGTH];


const char* outfilename;
AjPFile embossoutfile;

const char* outweightfilename;
AjPFile embossoutweightfile;

const char* outmixfilename;
AjPFile embossoutmixfile;

const char* outancfilename;
AjPFile embossoutancfile;

const char* outcatfilename;
AjPFile embossoutcatfile;

const char* outfactfilename;
AjPFile embossoutfactfile;

long sites, loci, maxalleles, groups, newsites, newersites,
  newgroups, newergroups, nenzymes, reps, ws, blocksize, categs, maxnewsites;
boolean bootstrap, permute, ild, lockhart, jackknife, regular, xml, nexus,
  weights, categories, factors, enzymes, all, justwts, progress, mixture,
  firstrep, ancvar;
double fracsample;
datatype data;
seqtype seq;
steptr oldweight, where, how_many, newwhere, newhowmany,
  newerwhere, newerhowmany, factorr, newerfactor, mixdata, ancdata;
steptr *charorder;
Char *factor;
long *alleles;
Char **nodep;
double **nodef;
long **sppord;
longer seed;


void emboss_getoptions(char *pgm, int argc, char *argv[])
{
  AjPStr test = NULL; 
  AjPStr typeofseq = NULL;
  AjPStr justweights = NULL;
  AjBool rewrite = false;

  long inseed, inseed0;
 
  data = morphology;
  seq = dna;
  bootstrap = false;
  jackknife = false;
  permute = false;
  ild = false;
  lockhart = false;
  blocksize = 1;
  regular = true;
  fracsample = 1.0;
  all = false;
  reps = 100;
  weights = false;
  mixture = false;
  ancvar = false;
  categories = false;
  justwts = false;
  printdata = false;
  dotdiff = true;
  progress = true;
  interleaved = true;
  xml = false;
  nexus = false;
  factors = false;
  enzymes = false;

   embInitP (pgm, argc, argv, "PHYLIPNEW");

    phylostate = ajAcdGetDiscretestates("infile");
 
    phylofact = ajAcdGetProperties("factorfile");
    if(phylofact) {
      factors = true;
      embossoutfactfile = ajAcdGetOutfile("outfactfile");
      emboss_openfile(embossoutfactfile, &outfactfile, &outfactfilename);
    }


    
    test = ajAcdGetListI("test", 1);
    
    if(ajStrMatchC(test, "b")) {
      bootstrap = true;
      regular = ajAcdGetToggle("regular");
      if(regular) fracsample = 1.0;
      else {
        fracsample = ajAcdGetFloat("fracsample");
        fracsample = fracsample/100.0;
      }      
      blocksize = ajAcdGetInt("blocksize");
    }
    else if(ajStrMatchC(test, "j")) {
      jackknife = true;
      regular = ajAcdGetToggle("regular");
      if(regular) fracsample = 0.5;
      else {
        fracsample = ajAcdGetFloat("fracsample");
        fracsample = fracsample/100.0;
      }
    }
    else if(ajStrMatchC(test, "c")) permute = true;
    else if(ajStrMatchC(test, "o")) ild = true;
    else if(ajStrMatchC(test, "s")) lockhart = true;
    else if(ajStrMatchC(test, "r")) rewrite = true;

    


    if(rewrite) {
      if (data == morphology) {
        typeofseq = ajAcdGetListI("morphseqtype", 1);
        if(ajStrMatchC(typeofseq, "d")) seq = dna;
        else if(ajStrMatchC(typeofseq, "r")) seq = rna;
        else if(ajStrMatchC(typeofseq, "p")) seq = protein;	  
      }
    }
    else{
      reps = ajAcdGetInt("reps");
      inseed = ajAcdGetInt("seed");
      emboss_initseed(inseed, &inseed0, seed);

      if(jackknife || bootstrap || permute) {
        phyloweights = ajAcdGetProperties("weights");
        if(phyloweights) weights = true;
      
        phyloanc = ajAcdGetProperties("ancfile");
        if(phyloanc) {
          ancvar = true;
          embossoutancfile = ajAcdGetOutfile("outancfile");
          emboss_openfile(embossoutancfile, &outancfile, &outancfilename);
        }

        phylomix = ajAcdGetProperties("mixfile");
        if(phylomix) {
          mixture = true;
          embossoutmixfile = ajAcdGetOutfile("outmixfile");
          emboss_openfile(embossoutmixfile, &outmixfile, &outmixfilename);
        }
 
        if(!permute) {
          justweights = ajAcdGetListI("justweights", 1); 
          if(ajStrMatchC(justweights, "j")) justwts = true;
        }

      }
    }

    printdata = ajAcdGetBool("printdata");
    if(printdata) dotdiff = ajAcdGetBool("dotdiff");
    progress = ajAcdGetBool("progress");


    embossoutfile = ajAcdGetOutfile("outfile");   
    emboss_openfile(embossoutfile, &outfile, &outfilename);

    printf("\n bootstrap: %s",(bootstrap ? "true" : "false"));
    printf("\njackknife: %s",(jackknife ? "true" : "false"));
    printf("\n permute: %s",(permute ? "true" : "false"));
    printf("\n lockhart: %s",(lockhart ? "true" : "false"));
    printf("\n ild: %s",(ild ? "true" : "false"));
    printf("\n justwts: %s \n",(justwts ? "true" : "false"));

     

}  /* emboss_getoptions */




void seqboot_inputnumbersstate(AjPPhyloState state)
{
  /* read numbers of species and of sites */

  spp = state->Size;
  sites = state->Len;
  loci = sites;
  maxalleles = 1;
}  /* seqboot_inputnumberstate */


void seqboot_inputfactors(AjPPhyloProp fact)
{
  long i, j;
  Char ch, prevch;
  AjPStr str;

  prevch = ' ';
  str = fact->Str[0];

  j = 0;
  for (i = 0; i < (sites); i++) {
    ch = ajStrGetCharPos(str,i);
    if (ch != prevch)
      j++;
    prevch = ch;
    factorr[i] = j;
  }

}  /* seqboot_inputfactors */


void inputoptions()
{
  /* input the information on the options */
  long weightsum, maxfactsize, i, j, k, l, m;

  if (data == genefreqs) {
    k = 0;
    l = 0;
    for (i = 0; i < (loci); i++) {
      m = alleles[i];
      k++;
      for (j = 1; j <= m; j++) {
        l++;
        factorr[l - 1] = k;
      }
    }
  } else {
    for (i = 1; i <= (sites); i++)
      factorr[i - 1] = i;
  }
  if(factors){
    seqboot_inputfactors(phylofact);
  }
  for (i = 0; i < (sites); i++)
    oldweight[i] = 1;
  if (weights)
    inputweightsstr2(phyloweights->Str[0],0, sites, &weightsum, oldweight, &weights, "seqboot");
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
  maxfactsize = 0;
  for(i = 0 ; i < loci ; i++){
    if(how_many[i] > maxfactsize){
      maxfactsize = how_many[i];
    }
  }
  maxnewsites = groups * maxfactsize;
  allocnew();
  for (i = 0; i < (groups); i++) {
    if (oldweight[where[i] - 1] > 0) {
      newgroups++;
      newsites += how_many[i];
      newwhere[newgroups - 1] = where[i];
      newhowmany[newgroups - 1] = how_many[i];
    }
  }
}  /* inputoptions */




 void seqboot_inputdatastate(AjPPhyloState state)
{
  /* input the names and sequences for each species */
  long i, j, k, l, m, n;
  Char charstate;
  AjPStr str;
  boolean allread, done;

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
    if (bootstrap)  {
      if (blocksize > 1) {
        if (regular)      
      fprintf(outfile, "Block-bootstrap with block size %ld\n\n", blocksize);
        else
          fprintf(outfile, "Partial (%2.0f%%) block-bootstrap with block size %ld\n\n",
                  100*fracsample, blocksize);
      } else {
        if (regular)
          fprintf(outfile, "Bootstrap\n\n");
	else 
          fprintf(outfile, "Partial (%2.0f%%) bootstrap\n\n", 100*fracsample);
      }
    } else {
      if (jackknife) {
        if (regular)
          fprintf(outfile, "Delete-half Jackknife\n\n");
        else
    fprintf(outfile, "Delete-%2.0f%% Jackknife\n\n", 100*(1.0-fracsample));
      } else {
        if (permute) {
          fprintf(outfile, "Species order permuted separately for each");
          if (data == morphology)
            fprintf(outfile, " character\n\n");
          if (data == restsites)
            fprintf(outfile, " site\n\n");
        }
        else {
          if (ild) {
            if (data == morphology)
              fprintf(outfile, "Character");
            if (data == restsites)
              fprintf(outfile, "Site");
            fprintf(outfile, " order permuted\n\n");
	  } else {
            if (lockhart)
              if (data == morphology)
                fprintf(outfile, "Character");
              if (data == restsites)
                fprintf(outfile, "Site");
         fprintf(outfile, " order permuted separately for each species\n\n");
          }
        }
      }
    }
      fprintf(outfile, "%3ld species, ", spp);
      if (data == seqs)
        fprintf(outfile, "%3ld  sites\n\n", sites);
        else if (data == morphology)
          fprintf(outfile, "%3ld  characters\n\n", sites);
          else if (data == restsites)
            fprintf(outfile, "%3ld  sites\n\n", sites);

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
          charstate = ajStrGetCharPos(str, j);
          uppercase(&charstate);
          j++;
          if (charstate == '.')
            charstate = nodep[0][j-1];
          nodep[i-1][j-1] = charstate;
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
    for (j = 0; j < spp; j++) {
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
}  /* seqboot_inputdatastate */


void allocrest()
{ /* allocate memory for bookkeeping arrays */

  oldweight = (steptr)Malloc(sites*sizeof(long));
  weight = (steptr)Malloc(sites*sizeof(long));
  if (categories)
    category = (steptr)Malloc(sites*sizeof(long));
  if (mixture)
    mixdata = (steptr)Malloc(sites*sizeof(long));
  if (ancvar)
    ancdata = (steptr)Malloc(sites*sizeof(long));
  where = (steptr)Malloc(loci*sizeof(long));
  how_many = (steptr)Malloc(loci*sizeof(long));
  factor = (Char *)Malloc(sites*sizeof(Char));
  factorr = (steptr)Malloc(sites*sizeof(long));
  nayme = (naym *)Malloc(spp*sizeof(naym));
}  /* allocrest */

void allocnew(void)
{ /* allocate memory for arrays that depend on the lenght of the 
     output sequence*/
  long i;

  newwhere = (steptr)Malloc(loci*sizeof(long));
  newhowmany = (steptr)Malloc(loci*sizeof(long));
  newerwhere = (steptr)Malloc(loci*sizeof(long));
  newerhowmany = (steptr)Malloc(loci*sizeof(long));
  newerfactor = (steptr)Malloc(maxnewsites*maxalleles*sizeof(long));
  charorder = (steptr *)Malloc(spp*sizeof(steptr));
  for (i = 0; i < spp; i++)
    charorder[i] = (steptr)Malloc(maxnewsites*sizeof(long));
}

void doinput(int argc, Char *argv[])
{ /* reads the input data */
  seqboot_inputnumbersstate(phylostate[0]);
  allocrest();
  inputoptions();
  seqboot_inputdatastate(phylostate[0]);


}  /* doinput */


void bootweights()
{ /* sets up weights by resampling data */
  long i, j, k, blocks;
  double p, q, r;

  ws = newgroups;
  for (i = 0; i < (ws); i++)
    weight[i] = 0;
  if (jackknife) {
    if (fabs(newgroups*fracsample - (long)(newgroups*fracsample+0.5))
       > 0.00001) {
      if (randum(seed)
          < (newgroups*fracsample - (long)(newgroups*fracsample))
            /((long)(newgroups*fracsample+1.0)-(long)(newgroups*fracsample)))
        q = (long)(newgroups*fracsample)+1;
      else
        q = (long)(newgroups*fracsample);
    } else
      q = (long)(newgroups*fracsample+0.5);
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
    blocks = fracsample * newgroups / blocksize;
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
{ /* permute the species order as given in array sppord */
  long i, j, k;

  for (i = 1; i <= (spp - 1); i++) {
    k = (long)((i+1) * randum(seed));
    j = sppord[n - 1][i];
    sppord[n - 1][i] = sppord[n - 1][k];
    sppord[n - 1][k] = j;
  }
}  /* sppermute */


void charpermute(long m, long n)
{ /* permute the n+1 characters of species m+1 */
  long i, j, k;

  for (i = 1; i <= (n - 1); i++) {
    k = (long)((i+1) * randum(seed));
    j = charorder[m][i];
    charorder[m][i] = charorder[m][k];
    charorder[m][k] = j;
  }
} /* charpermute */


void writedata()
{
  /* write out one set of bootstrapped sequences */
  long i, j, k, l, m, n, n2=0;
  double x;
  Char charstate;

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
    if (data == restsites && enzymes)
      fprintf(outfile, "%5ld %5ld% 4ld\n", spp, newergroups, nenzymes);
    else if (data == genefreqs)
      fprintf(outfile, "%5ld %5ld\n", spp, newergroups);
    else {
      if ((data == seqs)
          && !(bootstrap || jackknife || permute || ild || lockhart) && xml)
        fprintf(outfile, "<alignment>\n");
      else 
        if (!(bootstrap || jackknife || permute || ild || lockhart) && nexus) {
          fprintf(outfile, "#NEXUS\n");
          fprintf(outfile, "BEGIN DATA\n");
          fprintf(outfile, "  DIMENSIONS NTAX=%ld NCHAR=%ld;\n",
                    spp, newersites);
          fprintf(outfile, "  FORMAT");
          fprintf(outfile, " interleave");
          fprintf(outfile, " DATATYPE=");
          if (data == seqs) {
            switch (seq) { 
              case (dna): fprintf(outfile, "DNA missing=N gap=-"); break;
              case (rna): fprintf(outfile, "RNA missing=N gap=-"); break;
              case (protein):
                fprintf(outfile, "protein missing=? gap=-");
                break;
              }
          }
          if (data == morphology)
            fprintf(outfile, "STANDARD");
          fprintf(outfile, ";\n  MATRIX\n");
          }
        else fprintf(outfile, "%5ld %5ld\n", spp, newersites);
    }
    if (data == genefreqs) {
      for (i = 0; i < (newergroups); i++)
        fprintf(outfile, " %3ld", alleles[factorr[newerwhere[i] - 1] - 1]);
      putc('\n', outfile);
    }
  }
  l = 1;
  if ((!(bootstrap || jackknife || permute || ild || lockhart | nexus))
       && ((data == seqs) || (data == restsites))) {
    interleaved = !interleaved;
    if (!(bootstrap || jackknife || permute || ild || lockhart) && xml)
      interleaved = false;
  }
  if (interleaved)
    m = 60;
  else
    m = newergroups;
  do {
    if (m > newergroups)
      m = newergroups;
    for (j = 0; j < spp; j++) {
      n = 0;
      if ((l == 1) || (interleaved && nexus)) {
        if (!(bootstrap || jackknife || permute || ild || lockhart) && xml) {
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
        if (!(bootstrap || jackknife || permute || ild || lockhart)
            && (xml || nexus)) {
          while (nayme[j][n2] == ' ')
            n2--;
        }
        if (nexus)
          fprintf(outfile, "  ");
        for (k = 0; k <= n2; k++)
          if (nexus && (nayme[j][k] == ' ') && (k < n2))
            putc('_', outfile);
          else
            putc(nayme[j][k], outfile);
        if (!(bootstrap || jackknife || permute || ild || lockhart) && xml)
          fprintf(outfile, "</name>\n      <data>");
      } else {
        if (!(bootstrap || jackknife || permute || ild || lockhart) && xml) {
          fprintf(outfile, "      ");
        }
        else {
          for (k = 1; k <= nmlngth; k++)
            putc(' ', outfile);
        }
      }
      if (nexus)
        for (k = 0; k < nmlngth+1-n2; k++)
          fprintf(outfile, " "); 
      for (k = l - 1; k < m; k++) {
        if (permute && j + 1 == 1)
          sppermute(newerfactor[n]);    /* we can assume chars not permuted */
        for (n2 = -1; n2 <= (newerhowmany[k] - 2); n2++) {
          n++;
          if (data == genefreqs) {
            if (n > 1 && (n & 7) == 1)
              fprintf(outfile, "\n              ");
            x = nodef[sppord[newerfactor[charorder[j][n - 1]] - 1][j] - 1]
                    [newerwhere[charorder[j][k]] + n2];
            fprintf(outfile, "%8.5f", x);
          } else {
            if (!(bootstrap || jackknife || permute || ild || lockhart) && xml
                && (n > 1) && (n % 60 == 1))
              fprintf(outfile, "\n            ");
            else if (!nexus && !interleaved && (n > 1) && (n % 60 == 1))
                fprintf(outfile, "\n           ");
            charstate = nodep[sppord[newerfactor[charorder[j][n - 1]] - 1]
                             [j] - 1][newerwhere[charorder[j][k]] + n2];
            putc(charstate, outfile);
            if (n % 10 == 0 && n % 60 != 0)
              putc(' ', outfile);
          }
        }
      }
      if (!(bootstrap || jackknife || permute || ild || lockhart ) && xml) {
        fprintf(outfile, "</data>\n   </sequence>\n");
      }
      putc('\n', outfile);
    }
    if (interleaved) {
      if ((m <= newersites) && (newersites > 60))
        putc('\n', outfile);
      l += 60;
      m += 60;
    }
  } while (interleaved && l <= newersites);
  if ((data == seqs) &&
      (!(bootstrap || jackknife || permute || ild || lockhart) && xml))
    fprintf(outfile, "</alignment>\n");
  if (!(bootstrap || jackknife || permute || ild || lockhart) && nexus)
    fprintf(outfile, "  ;\nEND;\n");
  for (i = 0; i < (newergroups); i++)
    free(sppord[i]);
  free(sppord);
}  /* writedata */


void writeweights()
{ /* write out one set of post-bootstrapping weights */
  long j, k, l, m, n, o;

  j = 0;
  l = 1;
  if (interleaved)
    m = 60;
  else
    m = sites;
  do {
    if(m > sites)
      m = sites;
    n = 0;
    for (k = l - 1; k < m; k++) {
      for(o = 0 ; o < how_many[k] ; o++){
        if(oldweight[k]==0){
          fprintf(outweightfile, "0");
          j++;
        }
        else{
          if (weight[k-j] < 10)
            fprintf(outweightfile, "%c", (char)('0'+weight[k-j]));
          else
            fprintf(outweightfile, "%c", (char)('A'+weight[k-j]-10));
          n++;
          if (!interleaved && n > 1 && n % 60 == 1) {
            fprintf(outweightfile, "\n");
            if (n % 10 == 0 && n % 60 != 0)
              putc(' ', outweightfile);
          }
        }
      }
    }
    putc('\n', outweightfile);
    if (interleaved) {
      l += 60;
      m += 60;
    }
  } while (interleaved && l <= sites);
}  /* writeweights */


void writecategories()
{
  /* write out categories for the bootstrapped sequences */
  long k, l, m, n, n2;
  Char charstate;
  if(justwts){
    if (interleaved)
      m = 60;
    else
      m = sites;
    l=1;
    do {
      if(m > sites)
        m = sites;
      n=0;
      for(k=l-1 ; k < m ; k++){
        n++;
        if (!interleaved && n > 1 && n % 60 == 1)
          fprintf(outcatfile, "\n ");
        charstate =  '0' + category[k];
        putc(charstate, outcatfile);
      }
      if (interleaved) {
        l += 60;
        m += 60;
      }
    }while(interleaved && l <= sites);
    fprintf(outcatfile, "\n");
    return;
  }

  l = 1;
  if (interleaved)
    m = 60;
  else
    m = newergroups;
  do {
    if (m > newergroups)
      m = newergroups;
    n = 0;
    for (k = l - 1; k < m; k++) {
      for (n2 = -1; n2 <= (newerhowmany[k] - 2); n2++) {
        n++;
        if (!interleaved && n > 1 && n % 60 == 1)
          fprintf(outcatfile, "\n ");
        charstate = '0' + category[newerwhere[k] + n2];
        putc(charstate, outcatfile);
        if (n % 10 == 0 && n % 60 != 0)
          putc(' ', outcatfile);
      }
    }
    if (interleaved) {
      l += 60;
      m += 60;
    }
  } while (interleaved && l <= newersites);
  fprintf(outcatfile, "\n");
}  /* writecategories */


void writeauxdata(steptr auxdata, FILE *outauxfile)
{
  /* write out auxiliary option data (mixtures, ancestors, ect) to
     appropriate file.  Samples parralel to data, or just gives one
     output entry if justwts is true */
  long k, l, m, n, n2;
  Char charstate;

  /* if we just output weights (justwts), and this is first set
     just output the data unsampled */
  if(justwts){
    if(firstrep){
      if (interleaved)
        m = 60;
      else
        m = sites;
      l=1;
      do {
        if(m > sites)
          m = sites;
        n = 0;
        for(k=l-1 ; k < m ; k++){
        n++;
        if (!interleaved && n > 1 && n % 60 == 1)
          fprintf(outauxfile, "\n ");
          charstate = auxdata[k];
          putc(charstate, outauxfile);
        }
        if (interleaved) {
          l += 60;
          m += 60;
        }
      }while(interleaved && l <= sites);
      fprintf(outauxfile, "\n");
    }
    return;
  }

  l = 1;
  if (interleaved)
    m = 60;
  else
    m = newergroups;
  do {
    if (m > newergroups)
      m = newergroups;
    n = 0;
    for (k = l - 1; k < m; k++) {
      for (n2 = -1; n2 <= (newerhowmany[k] - 2); n2++) {
        n++;
        if (!interleaved && n > 1 && n % 60 == 1)
          fprintf(outauxfile, "\n ");
        charstate = auxdata[newerwhere[k] + n2];
        putc(charstate, outauxfile);
        if (n % 10 == 0 && n % 60 != 0)
          putc(' ', outauxfile);
      }
    }
    if (interleaved) {
      l += 60;
      m += 60;
    }
  } while (interleaved && l <= newersites);
  fprintf(outauxfile, "\n");
}  /* writeauxdata */

void writefactors(void)
{
  long k, l, m, n, prevfact, writesites;
  char symbol;
  steptr wfactor;

  if(!justwts || firstrep){
    if(justwts){
      writesites = sites;
      wfactor = factorr;
    } else {
      writesites = newersites;
      wfactor = newerfactor;
    }
    prevfact = wfactor[0];
    symbol = '+';
    if (interleaved)
      m = 60;
    else
      m = writesites;
    l=1;
    do {
      if(m > writesites)
        m = writesites;
      n = 0;
      for(k=l-1 ; k < m ; k++){
        n++;
        if (!interleaved && n > 1 && n % 60 == 1)
          fprintf(outfactfile, "\n ");
        if(prevfact != wfactor[k]){
          symbol = (symbol == '+') ? '-' : '+';
          prevfact = wfactor[k];
        }
        putc(symbol, outfactfile);
        if (n % 10 == 0 && n % 60 != 0)
          putc(' ', outfactfile);
      }
      if (interleaved) {
        l += 60;
        m += 60;
      }
    }while(interleaved && l <= writesites);
    fprintf(outfactfile, "\n");
  }
} /* writefactors */


void bootwrite()
{ /* does bootstrapping and writes out data sets */
  long i, j, rr, repdiv10;

  if (!(bootstrap || jackknife || permute || ild || lockhart))
    reps = 1;
  repdiv10 = reps / 10;
  if (repdiv10 < 1)
    repdiv10 = 1;
  if (progress)
    putchar('\n');
  for (rr = 1; rr <= (reps); rr++) {
    for (i = 0; i < spp; i++)
      for (j = 0; j < maxnewsites; j++)
        charorder[i][j] = j;
    if(rr==1)
      firstrep = true;
    else
      firstrep = false;
    if (ild) {
      charpermute(0, maxnewsites);
      for (i = 1; i < spp; i++)
        for (j = 0; j < maxnewsites; j++)
          charorder[i][j] = charorder[0][j];
    }
    if (lockhart)
      for (i = 0; i < spp; i++)
        charpermute(i, maxnewsites);
    bootweights();
    if (!justwts || permute || ild || lockhart)
      writedata();
    if (justwts && !(permute || ild || lockhart))
      writeweights();
    if (categories)
      writecategories();
    if (factors)
      writefactors();
    if (mixture)
      writeauxdata(mixdata, outmixfile);
    if (ancvar)
      writeauxdata(ancdata, outancfile);
    if (progress && (bootstrap || jackknife || permute || ild || lockhart)
          && ((reps < 10) || rr % repdiv10 == 0)) {
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
  emboss_getoptions("fdiscboot", argc, argv);
  ibmpc = IBMCRT;
  ansi = ANSICRT;
  doinput(argc, argv);
  bootwrite();
  FClose(infile);
  if (weights)
    FClose(weightfile);
  if (categories) {
    FClose(catfile);
    FClose(outcatfile);
  }
  if(mixture)
    FClose(outmixfile);
  if(ancvar)
    FClose(outancfile);
  if (justwts && !permute) {
    FClose(outweightfile);
  }
  else
    FClose(outfile);
#ifdef MAC
  fixmacfile(outfilename);
  if (justwts && !permute)
    fixmacfile(outweightfilename);
  if (categories)
    fixmacfile(outcatfilename);
  if (mixture)
    fixmacfile(outmixfilename);
#endif
  printf("Done.\n\n");
#ifdef WIN32
  phyRestoreConsoleAttributes();
#endif
  return 0;
}
