#include "ajax.h"
#include "meme.h"
#define MAXS	200
static BOOLEAN first_time = TRUE;

#define LogAddLog(x, y) ((x) > (y) ? LogAddLog1((x),(y)) : LogAddLog1((y),(x)))
#define LogAddLog1(x,y) ((x)-(y) > BITS ? (x) : (x) + log(1+exp((y)-(x))))
	
/* ---------------------------------------------------------------------- */
PriorLib *alloc_PriorLib(int l, int Alpha)
/*
        L       Number of distributions
        Alpha   Number of alphabet characters
*/
{
        PriorLib        *temp;
	int		i;

        temp = (PriorLib *)malloc(sizeof(PriorLib));
        temp->L = l;
        temp->AlphaChar = Alpha;

        temp->Mix = (Real *)malloc(sizeof(Real)*l);
        temp->B = (Real *)malloc(sizeof(Real)*l);
        temp->FullUpdate = (int *)malloc(sizeof(int)*l);
        temp->QUpdate = (int *)malloc(sizeof(int)*l);

        temp->StructID = (char **)malloc(sizeof(char *)*l);
        temp->Comment = (char **)malloc(sizeof(char *)*l);
        temp->Distr = (Real **)malloc(sizeof(Real *)*l);
        for (i=0; i<l; i++) {
           temp->Distr[i] = (Real *)malloc(sizeof(Real)*(Alpha+1));
           temp->StructID[i] = (char *)malloc(sizeof(char)*MAXS);
           temp->Comment[i] = (char *)malloc(sizeof(char)*MAXS);
        } /* endfor */

        return(temp);
}

/* ---------------------------------------------------------------------- */
PriorLib *read_PriorLib(char *plib_name, double desired_beta)
/*
	plib_name	name of prior library file
	desired_beta	>0, scale \beta_{i,j} so
				\sum_{i=0}^L \lambda_i \sum_{j=1}^20 \beta_{i,j}
			has this value
			=, don't scale prior
			< 0, just get alphabet
*/
{
        int             i,j, line=0;
        int             Alpha, l;
        PriorLib        *temp;
        char            input[MAXS], foo[MAXS], alphabet[MAXALPH+1], 
			checkstr[81], *token;
        float x;
  	FILE *fp;
	AjPFile inf=NULL;
	
/* tlb */
/*	fp = fopen(plib_name, "r");
	if (!fp) {
	  printf("Can't find prior library %s\n", plib_name);
	  exit(1);
	}*/
	ajFileDataNewC(plib_name,&inf);
	if(!inf)
	  ajFatal("Can't find prior library %s\n", plib_name);
	fp = ajFileFp(inf);
	    

	token = "Alphabet="; line++;
        fscanf(fp,"%s %s\n", checkstr, alphabet);
        if (strcmp(checkstr, token)) {
	  printf("Line %d of prior library file \n %s \n"
	    "should start with \"%s\" "
	    "but it starts with \"%s\".\n", line, plib_name, token, checkstr);
	  exit(1);
	}
	Alpha = strlen(alphabet);

	token = "NumDistr="; line++;
        fscanf(fp,"%s %d\n", checkstr, &l);
        if (strcmp(checkstr, token)) {
	  printf("Line %d of prior library file \n %s \n"
	    "should start with \"%s\" "
	    "but it starts with \"%s\"\n.", line, plib_name, token, checkstr);
	  exit(1);
	}

        temp = alloc_PriorLib(l,Alpha);

	if (Alpha > MAXALPH) {
	  printf("Alphabet size specified in prior library %s too big.\n"
		 "Change MAXALPH in user.h and remake meme.\n", plib_name);
	  exit(1);
	}
        strcpy(temp->alphabet, alphabet);
        temp->AlphaChar = Alpha;
        temp->L = l;

	if (desired_beta < 0) return(temp);

        for (i=0; i < temp->L; i++)
        {

        /* Get rid of number= */
        fscanf(fp,"%*s %*s\n");

        /* Mixture */
        fscanf(fp,"%*s");
        fscanf(fp,"%f\n", &x);
        temp->Mix[i] = (Real)x;

        /* B (strength) */
        fscanf(fp,"%*s");
        fscanf(fp,"%f\n", &x);
        temp->B[i] = (Real)x;

        /* Alpha */
        temp->Distr[i][0] = temp->B[i];
        fscanf(fp,"%*s");
        for (j=1; j <= temp->AlphaChar; j++) {
                fscanf(fp,"%g", &x);
                temp->Distr[i][j] = (Real)(x * temp->B[i]);
        }

        /* FullUpdate */
        fscanf(fp,"%*s");
        fscanf(fp,"%d\n", &(temp->FullUpdate[i]));

        /* QUpdate */
        fscanf(fp,"%*s");
        fscanf(fp,"%d\n", &(temp->QUpdate[i]));

        /* StructID */
        fgets(input, MAXS, fp);
        sscanf(input,"%s",foo);
	input[strlen(input)-1] = '\0';
        strcpy( (temp->StructID[i]), (input + strlen(foo)) );

        /* Comments */
        fgets(input, MAXS, fp);
        sscanf(input,"%s",foo);
        strcpy( (temp->Comment[i]), (input + strlen(foo)) );
        }

  /* tlb; scale beta to desired value */
  if (desired_beta > 0) {
    int i, j;
    double beta = 0;
    double scale;
    for (i=0; i<temp->L; i++) {
      beta += temp->Mix[i] * temp->B[i];
    }
    /*printf("beta = %10.6f\n", beta);*/
    scale = desired_beta/beta;
    for (i=0; i<temp->L; i++) {
      for (j=0; j<=temp->AlphaChar; j++) {
        temp->Distr[i][j] *= scale;
      }
    }
  }
/*	fclose(fp);*/
	ajFileClose(&inf);

        return(temp);
}

/* ---------------------------------------------------------------------- */
extern void mixture_regularizer(
  double *freq, 		/* obs freq */
  PriorLib *Lib,		/* priors */ 
  double *reg			/* pseudo-counts */
)
{
	Real	f[MAXALPH+1], sum, tmp;
	int 	i,j;
	/*Real	logpajgy();*/

	/* Put frequencies into array with f[0] = sum f_i */
	sum=0.0;
	for (i=0; i< Lib->AlphaChar; i++)
	{
		sum += freq[i];
		f[i+1]=freq[i];
	}
	f[0]=sum;

	/* Calculate probs */
	logpajgy(f, Lib, 0, 1);

	/* Calculate new regularizer */
	for (i=0; i< Lib->AlphaChar; i++)
	{
		reg[i]=0.0;
		for (j=0; j< Lib->L; j++)
		{
			tmp = (exp(logpajgy(f, Lib, j, 0)))*
			      ((Lib->Distr[j])[i+1]); /* skip A0 */
			reg[i] += tmp; 
		}
	}
}

/* ---------------------------------------------------------------------- */

/* This function computes log(p(a^j|y)) used in the calculation of theta. 
   It is defined to be

\log(\frac{q_j p(y given \alpha^j)}{\sum_k q_k p(y given \alpha^k})

*/

Real	logpajgy(
		Real	 *y,		/* observed frequencies */
		PriorLib *Library, 	/* Library of priors */
		int	 j, 		/* j'th prior to examine */
	   	int	 Calc		/* if ==1 calculate probs */
		)
{
	int i;
	RealPrec tmp;
	static RealPrec	logprob[MAXS], logdenom;/* Holders for probabilities */

	/* Calculate log probs if not already done */
	if (Calc)
	{
		tmp = log(Library->Mix[0]) + logpygaj(y,Library->Distr[0],
			Library->AlphaChar);
		logdenom = tmp;
		logprob[0] = tmp;

		/* Do remaining terms */
		for (i=1; i < Library->L; i++)
		{
			tmp = (log(Library->Mix[i]) + 
			  logpygaj(y, Library->Distr[i], Library->AlphaChar));
		
			logdenom = LogAddLog(logdenom, tmp);
			logprob[i] = tmp;
		}
	}
	return(logprob[j] - logdenom);
}

/* ---------------------------------------------------------------------- */

/* This function computes log(p(y|a^j)) used in the calculation of theta. 
   It is defined to be

\log(\frac{\Gamma(n+1)\Gamma(\alpha_0)}{\Gamma(n+\alpha_0)}
\prod_{i=1}^{20}\frac{\Gamma(y_i+\alpha_i)}{\Gamma(y_i+1)\Gamma(\alpha_i)})

*/

#define MAXX 10
#define DELTA .001
static double lgam(double x);
static double lgam_array[(int) 10000 + 2];

RealPrec	logpygaj(
			Real	*y,	/* observed frequencies */
			Real	 *a,	/* distribution parameters */
			int	AlphLength	/* length of alphabet */
			)
{
	int		i;
	RealPrec	temp;

  /* set up array of values of loggamma to save time */
  if (first_time) {
    double x = 0;
    for (i=1; i<=MAXX/DELTA + 1; i++) { 
      x += DELTA;
      lgam_array[i] = loggamma(x);
    }
    first_time = FALSE;
  }
	
	temp=0.0;

	temp+= lgam(y[0]+1.0);
	temp+= lgam(a[0]);
	temp+= -lgam(y[0]+a[0]);

	for (i=1; i<=AlphLength; i++)
	{
		temp+= lgam(y[i]+a[i]);
		temp+= -lgam(y[i]+1.0);
		temp+= -lgam(a[i]);
	}
	
	return(temp);

}


static double lgam(double x)
{
  if (x >= MAXX || x < DELTA)
    return(loggamma(x));
  else {
    int i = (int) (x/DELTA);
    return(lgam_array[i] + (lgam_array[i+1] - lgam_array[i])/2);
  }
}

/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/prior.c,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
