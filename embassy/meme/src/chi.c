/***********************************************************************
*								       *
*	MEME++							       *
*	Copyright 1994, The Regents of the University of California    *
*	Author: Timothy L. Bailey				       *
*								       *
***********************************************************************/
/* from \cite{as72} */

#include "meme.h"

extern double atof();
extern int atoi();
/*extern double sqrt();*/

/* list of alphas in table */
static double alphas[] = { .001, .005, .01, .05, .1, .25, .5, .75,
		.9, .95, .975, .99, .995, 1.0 };

/* chi-square versus nu and Q */
static double x[][31] = { 
	{	/* alpha = .001 */
		0,
		10.828,  13.816,  16.266,  18.467,  20.515,
		22.458,  24.322,  26.125,  27.877,  29.588,
		31.264,  32.909,  34.528,  36.123,  37.697,
		39.252,  40.790,  42.312,  43.820,  45.315,
		46.797,  48.268,  49.728,  51.179,  56.620,
		54.052,  55.476,  56.892,  58.302,  39.703  
	},
	{ 	/* alpha = .005 */	
		0, 
		7.87944, 10.5966, 12.8381, 14.8602, 16.7496,
		18.5476, 20.2777, 21.9550, 23.5893, 25.1882,
		26.7569, 28.2995, 29.8194, 31.3193, 32.8013,
		34.2627, 35.7185, 37.1564, 38.5822, 39.9968,
		41.4010, 42.7956, 44.1813, 45.5585, 46.9278,
		48.2899, 49.6449, 50.9933, 52.3356, 53.6720 
	},
	{	/* alpha = .01 */
		0, 
		6.63490, 9.21034, 11.3449, 13.2767, 15.0863,
		16.8119, 18.4753, 20.0902, 21.6660, 23.2093,
		24.7250, 26.2170, 27.6883, 29.1413, 30.5779,
		31.9999, 33.4087, 34.8053, 36.1908, 37.5662,
		38.9321, 40.2894, 41.6384, 42.9798, 44.3141,
		45.6417, 46.9630, 48.2782, 49.5879, 50.8922 
	},
	{	/* alpha = .05 */
		0, 
		3.84146, 5.99147, 7.81473, 9.48773, 11.0705,
		12.5916, 14.0671, 15.5073, 16.9190, 18.3070,
		19.6751, 21.0261, 22.3621, 23.6848, 24.9958,
		26.2962, 27.5871, 28.8693, 30.1435, 31.4104,
		32.6705, 33.9244, 35.1725, 36.4151, 37.6525,
		38.8852, 40.1133, 41.3372, 42.5569, 43.7729 
	},
	{	/* alpha = .1 */
		0, 
		2.70554, 4.60517, 6.25139, 7.77944, 9.23635,
		10.6446, 12.0170, 13.3616, 14.6837, 15.9871,
		17.2750, 18.5494, 19.8119, 21.0642, 22.3072,
		23.5418, 24.7690, 25.9894, 27.2036, 28.4120,
		29.6151, 30.8133, 32.0069, 33.1963, 34.3816,
		35.5631, 36.7412, 37.9159, 39.0875, 40.2560
	},
	{	/* alpha = .25 */
		0,
		1.32330, 2.77259, 4.10835, 5.38527, 6.62568,
		7.84080, 9.03715, 10.2188, 11.3887, 12.5489,
		13.7007, 14.8454, 15.9839, 17.1170, 18.2451,
		19.3688, 20.4887, 21.6049, 22.7178, 23.8277,
		24.9348, 26.0393, 27.1413, 28.2412, 29.3389,
		30.4345, 31.5284, 32.6205, 33.7109, 34.7998
	},
	{ 	/* alpha = .5 */
		0,
		0.454937, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	},
	{ 	/* alpha = .75 */
		0,
		0.101531, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	},
	{ 	/* alpha = .9 */
		0,
		0.015798, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	},
	{ 	/* alpha = .95 */
		0,
		0.00393214, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	},
	{ 	/* alpha = .975 */
		0,
		0.000982069, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	},
	{ 	/* alpha = .99 */
		0,
		0.000157088, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	},
	{ 	/* alpha = .995 */
		0,
		0.0000392704, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	},
	{ 	/* alpha = 1.0 */
		0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	}
};
	
/* normal distribution percentage points */
/* x_p values for Q(x_p) =  .001 .005, .01, .05 .1 .25 */
static double xp[] = {3.0902, 2.57583, 2.32635, 1.64485, 1.28155, 0.67449};

/* correction factors for computing X^2_{p,\nu} */
static double h60[] = {0.005004, 0.00209, 0.0012, -0.000297, -.000656, -.0004};

/* 
	erfc function 

	Page 221 in Numerical Recipes in C
	Second Edition
*/
static double my_erfc(double x)
{
  double t, z, ans;

  z = fabs(x);
  t = 1.0/(1.0 + 0.5 * z);
  ans = t * 
   exp(-z * z - 1.26551223 + 
   t * (1.00002368 +
   t * (0.37409196 +
   t * (0.09678418 +
   t * (-0.18628806 +
   t * (0.27886807 +
   t * (-1.13520398 +
   t * (1.48851587 +
   t * (-0.82215223 +
   t * (0.17087277) ) ) ) ) ) ) ) ) ); 
               
   return (x >= 0.0 ? ans : 2.0 - ans); 
}
#ifdef standalone 
main(argc, argv)
  int argc;
  char *argv[];
{
  double nu;
  double alpha;
  double x;
  if (argc < 3) {
    printf("Usage: chi <nu> <alpha>\n");
    printf("  prints X^2 such that Q(X | nu) = alpha\n");
    return;
  }
  nu = atof(argv[1]);
  alpha = atof(argv[2]);
  x = chi(nu, alpha);
  printf("%5.0f %f %f\n", nu, x, x/(2 * log(2.0)));
}
#endif
#ifdef chiq_standalone
void main(argc, argv)
  int argc;
  char *argv[];
{
  double x, nu, q;
  double e, m;
  if (argc < 3) {
    printf("Usage: chiq <x> <nu> \n");
    printf("  prints Q(X^2 | nu)\n");
    return;
  }
  x = atof(argv[1]);
  nu = atof(argv[2]);
  q = chiq(x, nu);
  exp10_logx(q, m, e);
  printf("Q(%g | %g) = %5.3fe%+03.0f\n", x, nu, m, e);
  printf("Q(%g | %g) = %10.3e\n", x, nu, exp10(q));
}
#endif

/* chi
  return X^2 such that Q(X^2 | nu) = alpha
  Uses improved cubic approximation for nu >= 30.
  \cite{as72}
*/
extern double chi(
  double nu,    /* degrees of freedom */
  double alpha	/* significance level */
)
{
  double q;
  int ii;	/* 0 = .001, 1 = .005, 2 = .01, 3 = .05, 4 = .1, 5 = .25*/
  for (ii=0; alphas[ii] < 1.0; ii++) if (alpha <= alphas[ii]) break; 

  if (nu >= 1 && nu < 30) {	/* use linear interpolation if nu < 30 */
    int x1 = (int) nu;
    if (ii ==0) return x[0][x1];	/* punt of alpha too small */
    q = x[ii][x1] + 
      (x[ii][x1] - x[ii-1][x1])*(alpha-alphas[ii])/(alphas[ii]-alphas[ii-1]);
  } else { 
/*  use improved cube approx if nu >= 30 */
    double hnu = (60/nu)*h60[ii];
    q = 1 - (2/(9*nu)) + ((xp[ii] - hnu) * sqrt(2/(9*nu)));
    q = nu * q * q * q;
  }
  return q;
}
#if defined(crayc90) || defined(crayt3e)
  #define cbrt(x) pow((x),0.333333333333)
#endif

#define sqrt2 1.41421
#define log_Q(x) (x < 28) ? log10(.5*my_erfc(x/sqrt2)) : -pow(x-5.4, 1.65)
/* chiq
	returns log10(Q(X^2 | nu))
*/
extern double chiq(
  double chisq,			/* chi-square variable */
  double v 			/* degrees of freedom */
)
{
  double tnv = 2.0/(9.0 * v);
  double x2 = (cbrt(chisq/v) - (1 - tnv)) / sqrt(tnv);
  return log_Q(x2);
}
/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/chi.c,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
