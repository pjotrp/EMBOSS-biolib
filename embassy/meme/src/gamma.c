/***********************************************************************
*                                                                      *
*       MEME++                                                         *
*       Copyright 1994, The Regents of the University of California    *
*       Author: Timothy L. Bailey                                      *
*                                                                      *
***********************************************************************/

#include <meme.h>

#ifdef gamma_stand_alone
extern int main(
  int argc,
  char **argv
)
{
  double x = atof(argv[1]);
  double nu = atof(argv[2]);
  printf("CHIQ(%f|%f) = %e\n", x, nu, GAMMAQ(nu/2,x/2));
  return 0;
}
#endif

/**********************************************************************/
/*
	gser

	Returns the incomplete gamma function P(a,x) evaluated by its
	continued fraction representation.  
	Valid when x < a+1

	From Numerical Recipes.
*/
/**********************************************************************/
double gser(
  double a, 
  double x
)  
{
  double ap = a;
  double sum = 1/a;
  double del = sum;
  double gln = loggamma(a);
  int n;

  if (x < 0 || a <= 0) {
    printf("gser: invalid arguments\n");
    return 1.0;
  }
  if (x == 0) return 1.0;

  for (n=1; n<=ITMAX; n++) {
    ap += 1;
    del *= x/ap;
    sum += del;
    if (fabs(del) < fabs(sum)*EPS) break;
  }
  if (n > ITMAX) printf("gser: a too large, ITMAX too small\n");

  return (sum * exp(-x + a*log(x) - gln));
}

/**********************************************************************/
/*
	gcf

	Returns the incomplete gamma function Q(a,x) evaluated by its
	continued fraction representation.  
	Valid when x > a+1

	From Numerical Recipes.
*/
/**********************************************************************/
double gcf(
  double a, 
  double x
)  
{
  double gln = loggamma(a);
  double gold = 0;	/* This is previous value, tested against for converg.*/
  double a0 = 1;		
  double a1 = x;	/* As and Bs of eq. 5.2.4 for eval using cont. frac. */
  double b0 = 0;
  double b1 = 1;
  double fac = 1;	/* normalization factor to prevent overflows */
  double g = 1;
  double an, ana, anf;
  int n;

  if (x < 0 || a <= 0) {
    printf("gcf: invalid arguments\n");
    return 0.0;
  }

  for (n=1; n<=ITMAX; n++) {
    an = n;
    ana = an - a;
    a0 = (a1 + a0*ana) * fac;	/* one step of recurrence (5.2.5) */
    b0 = (b1 + b0*ana) * fac;
    anf = an * fac;
    a1 = x*a0 + anf*a1;		/* next step of recurrence (5.2.5) */
    b1 = x*b0 + anf*b1;
    if (a1 != 0.0) {		/* renormalize? */
      fac = 1.0/a1;		/* yes; set fac so it happens */
      g = b1 * fac;		/* new value of answer */
      if (fabs((g-gold)/g) < EPS) break;		/* converged?   exit if yes */
      gold = g;
    }
  }
  if (n > ITMAX) printf("gcf: a too large, ITMAX too small\n");
  return (exp(-x + a * log(x) - gln) * g);
}

/**********************************************************************/
/*
	Returns the logarithm of the gamma function 
*/
/**********************************************************************/
double loggamma(
  double x
) 
{ 
  int i; 
  double xx, s; 
  static double c[]={
    76.18009172947146,
    -86.50532032941677, 
    24.01409824083091,
    -1.231739572450155, 
    0.1208650973866179e-2,
    -0.5395239384953e-5
  }; 
 
  xx = x;
  s = 1.000000000190015;
  for (i=0; i<=5; i++) s += c[i]/++xx; 
  return ((x+0.5) * log(x+5.5)) - (x+5.5) + log(2.5066282746310005*s/x); 
}

/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/gamma.c,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
