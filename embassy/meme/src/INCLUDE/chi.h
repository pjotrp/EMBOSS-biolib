/* chi.h */

#ifndef CHI_H
#define CHI_H

extern double chiq(
  double chisq,			/* chi-square variable */
  double v 			/* degrees of freedom */
);

extern double chi(
  double nu,    /* degrees of freedom */
  double alpha
);

#endif
