#ifdef __cplusplus
extern "C"
{
#endif

#ifndef embiep_h
#define embiep_h

#define EMBIEPSIZE 28			/* Usual alpha plus two array */
#define EMBIEPCYSTEINE 2	      /* Array index for C */
#define EMBIEPLYSINE 10		      /* Array index for K */
#define EMBIEPAMINO 26			/* Amino rerminus array index  */
#define EMBIEPCARBOXYL 27		/* Carboxyl terminus array index */



/*
** Prototype definitions
*/

void   embIepCalcK (double *K);
void   embIepCompC (const char *s, ajint amino,
		    ajint sscount, ajint modlysine,
		    ajint *c);
void   embIepCompS (const AjPStr str, ajint amino,
		    ajint sscount, ajint modlysine,
		    ajint *c);
double embIepGetCharge (const ajint *c, const double *pro, double *total);
void   embIepGetProto (const double *K, const ajint *c,
		       ajint *op, double H, double *pro);
AjBool embIepIepC (const char *s, ajint amino,
		   ajint sscount, ajint modlysine,
		   double *iep, AjBool termini);
AjBool embIepIepS (const AjPStr str, ajint amino,
		   ajint sscount, ajint modlysine,
		   double *iep, AjBool termini);
double embIepPhFromHconc (double H);
double embIepPhToHconc (double pH);
double embIepPhConverge (const ajint *c, const double *K,
			 ajint *op, double *pro);
void   embIepPkRead (void);
double embIepPkToK (double pK);
double embIepPkFromK (double K);

void   __deprecated embIepComp (const char *s, ajint amino, ajint *c);
AjBool __deprecated embIepIEP (const char *s, ajint amino,
			       double *iep, AjBool termini);
/*
** End of prototype definitions
*/

#endif

#ifdef __cplusplus
}
#endif
