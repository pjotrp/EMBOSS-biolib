
#ifndef _INTERFACE_H_
#define _INTERFACE_H_

/* interface.h:  access to interface.c, a 2 window text/graphics
    environment, with a scrolling text window and c-like I/O functions.
    This also sets up some defines for the standard c stuff. */

/* function prototypes */
void   macsetup(char *,char *);
void   queryevent();
void   eventloop();
void    process_window_closure();
int    handleevent();
void   textmode();
void    gfxmode();
pascal void scroll();
int    process_char();
void paint_gfx_window();
void resize_gfx_window(EventRecord ev);
void menu_select(long what);
/*debug void fixmacfile(char *);*/



typedef struct {
        char* fn;
        double* xo;
        double* yo;
        double* scale;
        long nt;
        void* root;

} mpreviewparams;

extern mpreviewparams macpreviewparms;
 
/* function prototypes */


#ifdef __MWERKS__
#define MAC
#endif
#endif
