/* SMG definitions from VMS libraries ****************************************
**
**    An implementation of the UNIX Curses package using the VMS supplied
**    Screen Management tools.
**
****************************************************************************/

#include <smgdef.h>
#include <descrip.h>

/* Variables for SMG macros */

struct dsc$descriptor_s StrDescr, CharDescr;

int __Cols, __Rows,i;
long __x, __y, __t;                 /* Used to save cursor position */
long __row, __col, __h, __r, __c;               /* Used by scroll */
char __chr, __str[256];             /* Used by getstr */
unsigned short __len;

unsigned long Paste, Display, SaveScr, Keyboard;   /* SMG objects */
unsigned long Mode, SaveAttr, Attr;

/*---------------------------------------*/
/* Match "Curses" attributes
*/

#define _NORMAL    SMG$M_NORMAL
#define _BOLD      SMG$M_BOLD   
#define _UNDERLINE SMG$M_UNDERLINE
#define _BLINK     SMG$M_BLINK
#define _REVERSE   SMG$M_REVERSE

/*---------------------------------------*/

#define initscr() {\
	CharDescr.dsc$a_pointer = &__chr;\
	CharDescr.dsc$w_length = 1;\
	CharDescr.dsc$b_class = DSC$K_CLASS_S;\
	CharDescr.dsc$b_dtype = DSC$K_DTYPE_T;\
	StrDescr.dsc$b_class = DSC$K_CLASS_S;\
	StrDescr.dsc$b_dtype = DSC$K_DTYPE_T;\
	SMG$CREATE_PASTEBOARD(&Paste, 0, &__Rows, &__Cols);\
	SMG$CONTROL_MODE(&Paste, 0, &Mode);\
	Mode |= SMG$M_NOTABS;\
	SMG$CONTROL_MODE(&Paste, &Mode);\
	SMG$CREATE_VIRTUAL_DISPLAY(&__Rows, &__Cols, &Display);\
	SMG$PASTE_VIRTUAL_DISPLAY(&Display, &Paste, &1L, &1L);\
	SMG$CREATE_VIRTUAL_KEYBOARD(&Keyboard);\
	SMG$SET_KEYPAD_MODE(&Keyboard, &0L);\
	SMG$HOME_CURSOR(&Display,SMG$C_UPPER_LEFT);\
        SMG$BEGIN_DISPLAY_UPDATE(&Display);\
	Attr=SMG$M_NORMAL;\
	SaveAttr=Attr;}

#define move(y,x)       {__x=(x)+1; __y=(y)+1;\
	                 SMG$SET_CURSOR_ABS(&Display, &__y, &__x);}

#define clrtoeol()      SMG$ERASE_LINE(&Display)
#define addch(c)        {__chr=(c);\
                         SMG$PUT_CHARS(&Display, &CharDescr,0,0,0,&Attr);}

#define addstr(s)       {StrDescr.dsc$a_pointer = (s);\
                         StrDescr.dsc$w_length = strlen(s);\
                         SMG$PUT_CHARS(&Display, &StrDescr,0,0,0,&Attr);}

#define addstrw(s)      {StrDescr.dsc$a_pointer = (s);\
                         StrDescr.dsc$w_length = strlen(s);\
                         SMG$PUT_CHARS_WIDE(&Display, &StrDescr,0,0,0,&Attr);}

#define addstrhw(s)     {StrDescr.dsc$a_pointer = (s);\
                         StrDescr.dsc$w_length = strlen(s);\
                         SMG$PUT_CHARS_HIGHWIDE(&Display, &StrDescr,0,0,0,&Attr);}

#define mvaddstr(y,x,s)   {move(y,x);addstr(s);}
#define mvaddstrw(y,x,s)  {move(y,x);addstrw(s);}
#define mvaddstrhw(y,x,s) {move(y,x);addstrhw(s);}
#define mvaddch(y,x,c)    {move(y,x);addch(c);}

#define flush()          SMG$FLUSH_BUFFER(&Paste);
#define refresh()       {SMG$END_DISPLAY_UPDATE(&Display);\
                         SMG$BEGIN_DISPLAY_UPDATE(&Display);}

#define erase()         SMG$ERASE_DISPLAY(&Display)

#define setattr(a)      if ((a)) Attr|=(a); else Attr = SMG$M_NORMAL
#define clrattr(a)      if ((a)) Attr&=(a); else Attr = SMG$M_NORMAL
#define clear()         LIB$ERASE_PAGE(&1,&1)

#define endwin()        {SMG$ERASE_DISPLAY(&Display);\
                         SMG$HOME_CURSOR(&Display,&SMG$C_LOWER_LEFT);\
                         refresh();\
                         SMG$DELETE_PASTEBOARD(&Paste);}

#define getstr(s)       {SMG$END_DISPLAY_UPDATE(&Display);\
                         StrDescr.dsc$a_pointer = __str;\
                         StrDescr.dsc$w_length = 255;\
                         SMG$READ_STRING(&Keyboard,&StrDescr,\
                          0,&255L,0,0,0,&__len);\
                         __str[__len]='\0'; strcpy(s,__str);\
                         SMG$BEGIN_DISPLAY_UPDATE(&Display);}

#define getkeystroke(i)  SMG$READ_KEYSTROKE(&Keyboard,&i)


/* Additional macros not defined by CURSES but used by SMG */

#define repaint()       SMG$INVALIDATE_DISPLAY(&Display)
#define touch()         repaint()
#define savescreen()    SMG$SAVE_PHYSICAL_SCREEN(&Paste,&SaveScr)
#define restorescreen() SMG$RESTORE_PHYSICAL_SCREEN(&Paste,&SaveScr)
#define setkeypad()     SMG$SET_KEYPAD_MODE(&Keyboard,&1L);
#define resetkeypad()   SMG$SET_KEYPAD_MODE(&Keyboard,&0L);
#define clrtocol(x)     {__t=(x)+1;\
                         SMG$RETURN_CURSOR_POS(&Display,&__y,&__x);\
                         SMG$ERASE_DISPLAY(&Display,&__y,&__x,&__y,&__t);}

#define scroll(row,h,d,c) {__row=(row)+1;__h=(h);__c=(c);\
                           SMG$SCROLL_DISPLAY_AREA(&Display,&__row,&1,\
                           &__h,&__Cols,&d,&__c);}

#define box(row,col,r,c)  {__row=(row)+1;__col=(col)+1;__r=(r)+1;__c=(c)+1;\
                           SMG$DRAW_RECTANGLE(&Display,&__row,&__col,&__r,\
                           &__c,&Attr);}

#define line(row,col,r,c)  {__row=(row)+1;__col=(col)+1;__r=(r)+1;__c=(c)+1;\
                           SMG$DRAW_LINE(&Display,&__row,&__col,&__r,\
                           &__c,&Attr);}

