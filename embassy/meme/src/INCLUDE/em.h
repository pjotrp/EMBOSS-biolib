#ifndef EM_H
#  define EM_H


extern void em(
  MODEL *model,			/* the model */
  DATASET *dataset,		/* the dataset */
  PRIORS *priors,		/* the priors */
  int maxiter,			/* maximum number of iterations */
  double distance		/* stopping criterion */
);
extern void m_step(
  MODEL *model,			/* the model */
  DATASET *dataset,		/* the dataset */
  PRIORS *priors,		/* the priors */
  int iter			/* iteration number */
);
extern void mcm_m_step(
  MODEL *model,			/* the model */
  DATASET *dataset,		/* the dataset */
  PRIORS *priors,		/* the priors */
  int iter			/* iteration number */
);
extern double e_step(
  MODEL *model,                 /* the model */
  DATASET *dataset              /* the dataset */
);
extern double mcm_e_step(
  MODEL *model,			/* the model */
  DATASET *dataset		/* the dataset */
);
extern double tcm_e_step(
  MODEL *model,			/* the model */
  DATASET *dataset		/* the dataset */
);
extern double zoops_e_step(
  MODEL *model,			/* the model */
  DATASET *dataset		/* the dataset */
);
extern double like_e_step(
  MODEL *model,                 /* the model */
  DATASET *dataset              /* the dataset */
);
extern double smooth(
  int w,				/* width to smooth over */
  MODEL *model,				/* the model */
  DATASET *dataset			/* the dataset */
);
extern void convert_theta_to_log(
  MODEL *model,				/* the model */
  int alength				/* length of alphabet */
);
#endif
/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/INCLUDE/em.h,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
