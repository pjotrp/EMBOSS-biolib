From beerli@genetics.washington.edu Tue Mar 13 15:47:38 2001
Received: from darwin.genetics.washington.edu (darwin.genetics.washington.edu [128.95.144.42])
        by evolution.genetics.washington.edu (8.9.3/8.9.3) with ESMTP id PAA17484
        for <joe@evolution.genetics.washington.edu>; Tue, 13 Mar 2001 15:47:38 -0800 (PST)
Received: from localhost (beerli@localhost)
        by darwin.genetics.washington.edu (8.9.3/8.9.3) with ESMTP id PAA10268
        for <joe@evolution.genetics.washington.edu>; Tue, 13 Mar 2001 15:47:38 -0800
X-Authentication-Warning: darwin.genetics.washington.edu: beerli owned process doing -bs
Date: Tue, 13 Mar 2001 15:47:38 -0800 (PST)
From: Peter Beerli <beerli@genetics.washington.edu>
To: Felsenstein Joe <joe@evolution.genetics.washington.edu>
Subject: hermite a fini laguerre
Message-ID: <Pine.LNX.4.30.0103131538570.8726-100000@darwin.genetics.washington.edu>
MIME-Version: 1.0
Content-Type: TEXT/PLAIN; charset=US-ASCII
Status: RO

I have two files included below,
and you certainly need to adapt them for phylip.

the hermit stuff contains the word hermite
and its main function is

inthermitcat()


I ripped the initgammacat into a simple driver.
laguerre.c contains a simple test program that
can be compiled using
gcc -DLAGUERRE_TEST laguerre.c -o laguerretest -lm

Peter
-- 

/* laguerre.h */
#ifndef _LAGUERRE_H_
#define _LAGUERRE_H_
/*------------------------------------------------------
 Maximum likelihood estimation
 of migration rate  and effectice population size
 using a Metropolis-Hastings Monte Carlo algorithm
 -------------------------------------------------------
 Laguerre integration  R O U T I N E S

 Peter Beerli 2000, Seattle
 beerli@genetics.washington.edu
 $Id: hermite.c,v 1.1 2003/07/31 17:17:50 rice Exp $
-------------------------------------------------------*/

extern void integrate_laguerre(long categs, double *rate,
                                 double *probcat,
                                 double (*func)(double, helper_fmt *),
                                 helper_fmt *helper, double *result,
                                 double *rmax);
extern void initgammacat (long categs, double alpha, double theta1,
              double *rate, double *probcat);

#endif


/* laguerre.c */
/*
   (mutation) rates following a Gamma distribution
   using orthogonal polynomials for finding rates and
   LOG(probabilities)
   [based on initgammacat of Joe Felsenstein]

   - Generalized Laguerre (routine by Joe Felsenstein 2000)
     defining points for a Gamma distribution with
     shape parameter alpha and location parameter beta=1/alpha
     [mean=1, std = 1/alpha^2]
   - Hermite (approximates a normal and is activated when
     the shape parameter alpha is > 100.)

   Part of Migrate (Lamarc package)
   http://evolution.genetics.washington.edu/lamarc.html

   Peter Beerli, Seattle 2001
   $Id: hermite.c,v 1.1 2003/07/31 17:17:50 rice Exp $
*/
#include "migration.h"
#include "laguerre.h"
#include "tools.h"

#define SQRTPI 1.7724538509055160273
#define SQRT2  1.4142135623730950488


/*this triggers the test  main()
and is called with
gcc -DLAGUERRE_TEST -g laguerre.c -o laguerre -lm*/

#ifdef LAGUERRE_TEST
/* at the end is a test main to help test if the root/weights finding
   is OK*/

/* if machine has lgamma() use it otherwise use lgamma from
   tools.h*/
#undef LGAMMA
#define LGAMMA lgamma


/* for migrate this is defined in tools.h */
double
logfac (long n)
{
  /* log(n!) values were calculated with Mathematica
     with a precision of 30 digits */
  switch (n)
    {
    case 0:
      return 0.;
    case 1:
      return 0.;
    case 2:
      return 0.693147180559945309417232121458;
    case 3:
      return 1.791759469228055000812477358381;
    case 4:
      return 3.1780538303479456196469416013;
    case 5:
      return 4.78749174278204599424770093452;
    case 6:
      return 6.5792512120101009950601782929;
    case 7:
      return 8.52516136106541430016553103635;
    case 8:
      return 10.60460290274525022841722740072;
    case 9:
      return 12.80182748008146961120771787457;
    case 10:
      return 15.10441257307551529522570932925;
    case 11:
      return 17.50230784587388583928765290722;
    case 12:
      return 19.98721449566188614951736238706;
    default:
      return LGAMMA(n + 1.);
    }
}
#endif

/* prototypes */
double hermite(long n, double x);
void root_hermite(long n, double *hroot);
double  halfroot(double (*func)(long m, double x),
                 long n, double startx, double delta);
void hermite_weight(long n, double * hroot, double * weights);
void inithermitcat(long categs, double alpha, double theta1,
                   double *rate, double *probcat);

double glaguerre (long m, double b, double x);
void initlaguerrecat(long categs, double alpha, double theta1,
                   double *rate, double *probcat);
void roots_laguerre(long m, double b, double **lgroot);

void initgammacat (long categs, double alpha, double theta1,
                   double *rate, double *probcat);

void integrate_laguerre(long categs, double *rate,
                        double *probcat,
                        double (*func)(double theta, helper_fmt * b),
                        helper_fmt *helper, double *result, double *rmax);


/*------------------------------------------------------
  Generalized Laguerre polynomial computed recursively.
  For use by initgammacat
*/
double
glaguerre (long m, double b, double x)
{
  long i;
  double gln, glnm1, glnp1;        /* L_n, L_(n-1), L_(n+1) */

  if (m == 0)
    return 1.0;
  else
    {
      if (m == 1)
        return 1.0 + b - x;
      else
        {
          gln = 1.0 + b - x;
          glnm1 = 1.0;
          for (i = 2; i <= m; i++)
            {
              glnp1 =
                ((2 * (i - 1) + b + 1.0 - x) * gln - (i - 1 + b) * glnm1) / i;
              glnm1 = gln;
              gln = glnp1;
            }
          return gln;
        }
    }
}                                /* glaguerre */


/* calculates hermite polynomial with degree n and parameter x */
/* seems to be unprecise for n>13 -> root finder does not converge*/
double hermite(long n, double x)
{
 double   h1 = 1.;
 double   h2 =  2. * x;
 double   xx =  2. * x;
 long i;
 for(i = 1; i < n; i++)
   {
     xx = 2. * x * h2 - 2. * (i) * h1;
     h1 = h2;
     h2 = xx;
   }
 return xx;
}

void root_hermite(long n, double *hroot)
{
  long z=0;
  long ii;
  long start;
  if(n % 2 == 0)
    {
      start = n/2;
      z = 1;
    }
  else
    {
      start = n/2 + 1;
      z=2;
      hroot[start-1] = 0.0;
    }
  for(ii=start; ii<n; ii++)
    {
      /* search only upwards*/
      hroot[ii] = halfroot(hermite,n,hroot[ii-1]+EPSILON, 1./n);
      hroot[start - z] = -hroot[ii];
      z++;
    }
}

/*searches from the bound (startx) only in one direction
  (by positive or negative delta, which results in
  other-bound=startx+delta)
  delta should be small.
  (*func) is a function with two arguments
*/
double
halfroot(double (*func)(long m, double x),
         long n, double startx,
         double delta)
{
  double xl;
  double xu;
  double xm;
  double fu;
  double fl;
  double fm = 100000.;
  double gradient;
  boolean down;
  /* decide if we search above or below startx and escapes to trace back
     to the starting point that most often will be
     the root from the previous calculation*/
  if(delta < 0)
    {
      xu = startx;
      xl = xu + delta;
    }
  else
    {
      xl = startx;
      xu = xl + delta;
    }
  delta = fabs(delta);
  fu = (*func)(n, xu);
  fl = (*func)(n, xl);
  gradient = (fl-fu)/(xl-xu);

  while(fabs(fm) > EPSILON)
    {
      /* is root outside of our bracket?*/
      if((fu<0.0 && fl<0.0) || (fu>0.0 && fl > 0.0))
        {
          xu += delta;
          fu = (*func)(n, xu);
          fl = (*func)(n, xl);
          gradient = (fl-fu)/(xl-xu);
          down = gradient < 0 ? TRUE : FALSE;
        }
      else
        {
          xm = xl - fl / gradient;
          fm = (*func)(n, xm);
          if(down)
            {
              if(fm > 0.)
                {
                  xl = xm;
                  fl = fm;
                }
              else
                {
                  xu = xm;
                  fu = fm;
                }
            }
          else
            {
              if(fm > 0.)
                {
                  xu = xm;
                  fu = fm;
                }
              else
                {
                  xl = xm;
                  fl = fm;
                }
            }
          gradient = (fl-fu)/(xl-xu);
        }
    }
  return xm;
}


// calculate the weights for the hermite polynomial
// at the roots
// using formula Abramowitz and Stegun chapter 25.4.46 p.890
void hermite_weight(long n, double * hroot, double * weights)
{
  long i;
  double hr2;
  double nominator = exp(LOG2 * ( n-1.) + logfac(n)) * SQRTPI / (n*n);
  for(i=0;i<n; i++)
    {
      hr2 = hermite(n-1, hroot[i]);
      weights[i] = nominator / (hr2*hr2);
    }
}

/* calculates rates and LOG(probabilities) */
void
inithermitcat(long categs, double alpha, double theta1,
              double *rate, double *probcat)
{
  long i;
  double *hroot;
  double std = SQRT2 * theta1/sqrt(alpha);
  hroot = (double *) calloc(categs+1,sizeof(double));
  root_hermite(categs, hroot);  // calculate roots
  hermite_weight(categs, hroot, probcat);  // set weights
  for(i=0;i<categs;i++)  // set rates
    {
      rate[i] = theta1 + std * hroot[i];
      probcat[i] = log(probcat[i]);
    }
  free(hroot);
}

void
roots_laguerre(long m, double b, double **lgroot)
{
  /* For use by initgammacat.
     Get roots of m-th Generalized Laguerre polynomial, given roots
     of (m-1)-th, these are to be stored in lgroot[m][] */
  long i;
  double upper, lower, x, y;
  boolean dwn=FALSE;
  double tmp;
  /* is function declining in this interval? */
  if (m == 1)
    {
      lgroot[1][1] = 1.0 + b;
    }
  else
    {
      dwn = TRUE;
      for (i = 1; i <= m; i++)
        {
          if (i < m)
            {
              if (i == 1)
                lower = 0.0;
              else
                lower = lgroot[m - 1][i - 1];
              upper = lgroot[m - 1][i];
            }
          else
            {                        /* i == m, must search above */
              lower = lgroot[m - 1][i - 1];
              x = lgroot[m - 1][m - 1];
              do
                {
                  x = 2.0 * x;
                  y = glaguerre (m, b, x);
                }
              while ((dwn && (y > 0.0)) || ((!dwn) && (y < 0.0)));
              upper = x;
            }
          while (upper - lower > 0.000000001)
            {
              x = (upper + lower) / 2.0;
              if ((tmp=glaguerre (m, b, x)) > 0.0)
                {
                  if (dwn)
                    lower = x;
                  else
                    upper = x;
                }
              else
                {
                  if (dwn)
                    upper = x;
                  else
                    lower = x;
                }
            }
          lgroot[m][i] = (lower + upper) / 2.0;
          dwn = !dwn;                /* switch for next one */
        }
    }
}                                /* root_laguerre */


void
initgammacat (long categs, double alpha, double theta1,
              double *rate, double *probcat)
{
/* calculate rates and probabilities to approximate Gamma distribution
   of rates with "categs" categories and shape parameter "alpha" using
   rates and weights from Generalized Laguerre quadrature */

  if(alpha>=100.)
    {
      inithermitcat(categs, alpha, theta1, rate, probcat);
    }
  else
    {
      initlaguerrecat(categs, alpha, theta1, rate, probcat);
    }
}

void
initlaguerrecat(long categs, double alpha, double theta1, double *rate,
                double *probcat)
{
  long i;
  double  **lgroot;                /* roots of GLaguerre polynomials */
  double f, x, xi, y;

  lgroot = (double **) calloc(categs+1,sizeof(double*));
  lgroot[0] = (double *) calloc((categs+1)*(categs+1),sizeof(double));
  for(i=1;i<categs+1;i++)
    {
      lgroot[i] = lgroot[0] + i * (categs+1);
    }
  lgroot[1][1] = 1.0 + alpha;
  for (i = 2; i <= categs; i++)
    roots_laguerre (i, alpha, lgroot);        /* get roots for L^(a)_n */
  /* here get weights */
  /* Gamma weights are
     (1+a)(1+a/2) ... (1+a/n)*x_i/((n+1)^2 [L_{n+1}^a(x_i)]^2)  */
  f = 1;
  for (i = 1; i <= categs; i++)
    f *= (1.0 + alpha / i);
  for (i = 1; i <= categs; i++)
    {
      xi = lgroot[categs][i];
          y = glaguerre (categs + 1, alpha, xi);
          x = f * xi / ((categs + 1) * (categs + 1) * y * y);
          rate[i - 1] = xi / (1.0 + alpha);
          probcat[i - 1] = x;
    }
  for(i=0;i<categs;i++)
    {
      probcat[i] = log(probcat[i]);
      rate[i] *= theta1;
    }
  free(lgroot[0]);
  free(lgroot);
}                                /* initgammacat */


void integrate_laguerre(long categs, double *rate,
                        double *probcat,
                        double (*func)(double theta, helper_fmt * b),
                        helper_fmt *helper, double *result, double *rmax)
{
  double sum=0.;
  long i;
  double *temp;
  int *stemp;
  *rmax = -DBL_MAX;

  temp = (double *) calloc(categs, sizeof(double));
  stemp = (int *) calloc(categs, sizeof(int));
  for(i=0;i<categs;i++)
    {
      temp[i] = (*func)(rate[i], (void *)helper);
      stemp[i] = helper->sign;
      if(temp[i] > *rmax)
        *rmax = temp[i];
    }
  for(i=0;i<categs;i++)
    {
      sum += ((double) stemp[i]) * probcat[i] * exp(temp[i] - *rmax);
    }
  free(temp);
  free(stemp);
  *result =  sum;
}


/* initgammacat test function*/
#ifdef LAGUERRE_TEST
int main()
{
  long categs=10;
  double alpha;
  double theta1;
  double *rate;
  double *probcat;
  long i;
  printf("Enter alpha, theta1,  and categs\n");
  fscanf(stdin,"%lf%lf%li",&alpha,&theta1,&categs);
  rate = (double *) calloc(categs+1,sizeof(double));
  probcat = (double *) calloc(categs+1,sizeof(double));
  initgammacat(categs, alpha, theta1, rate,probcat);
  for(i=0;i<categs;i++)
    {
      printf("%20.20f %20.20f\n",rate[i],probcat[i]);
    }
  return 0;
}
#endif












