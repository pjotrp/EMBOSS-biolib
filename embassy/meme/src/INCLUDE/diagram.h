/* diagram.h */
 
#ifndef DIAGRAM_H
#  define DIAGRAM_H

#include "macros.h"
#include "logodds.h"

int dptr;
int order[MAXLO];
int space[MAXLO];
int norder;
char diagram[MAXLO];


/*DEXTERN(int, norder, 0);*//* number of motifs for which order is given */
/*EXTERN int order[MAXLO];*//* left-to-right order of motifs */
/*EXTERN int space[MAXLO];*//* distance from end of previous motif to start this */

/* motif block diagram in mast format (see diagram.y):
        diagram         <- mdiagram | sdiagram
        mdiagram        <- motif | motif-diagram
        sdiagram        <- spacer | spacer-mdiagram
        motif           <- [integer]
        spacer          <- integer
*/
/*DEXTERN(char *, diagram, (char *)0);	*/
/*EXTERN int dptr;*/	/* current read position in diagram */



/* prevent compiler complaints */
#if defined(__STDC__) || defined(__cplusplus)
extern int yyparse(void);
#else
extern int yyparse();
#endif


#endif
