/* @source emnu application
**
** Simple menu for EMBOSS applications
**
** @author: Copyright (C) Gary Williams
** 1 Nov 2000 - written - GWW
** @@
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
******************************************************************************/

/*

BUGS
----

27 Oct 2000 - delete_file() doesn't delete directories - this is because
we don't allow directories to be default files.  Should we? It would
complicate things like copy_file. 

30 Oct 2000 - we probably have to update the 'matches' flag after
updating a files menu as there is no guarantee that we are still
matching the current menu item. 

14 Nov 2000 - When using the '-explode' option, one of the program
groups is expanded to be 'HELP'.  Selecting this option simply gives the
help for the Groups Menu.  I don't intend to alter this situation, it is
a feature. 


CHANGES
-------

19 Oct 2000 - changed initialisation from scrollok(stdscr, TRUE) to
scrollok(stdscr, FALSE) so we don't scroll when end of bottom line is
printed to. Also removed cbreak() - raw() does what we want.

19 Oct 2000 - changed wgetch(w) to getch() in yesno() - now the mouse
clicks work OK.  Made this change throughout to get the mouse clicks
passed through. 

24 Oct 2000 - turned off option O_BS_OVERLOAD on forms to prevent
crashing when deleting first character of the field

29 Nov 2001 - minor change to match the new parameters of 
embGrpGetProgGroups

*/

/* the name of this program */
#define PROGRAM_NAME "emnu"

/* the version of this program */
#define PROGRAM_VERSION "1.0.5"


#include "emboss.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>	/* for getenv, system etc. */

/* for directories files and times */
#include <sys/types.h>  /* for opendir etc. */
#include <dirent.h>     /* for readdir */
#include <sys/stat.h>   /* for stat, mkdir */
#include <time.h>
#include <ctype.h>      /* for isalpha etc. */
#include <unistd.h>     /* for chdir, unlink */

/* for curses etc. */
#include <curses.h>
#include <menu.h>
#include <form.h>



/* declare structure used to hold file information */
struct file {
	char *name;
	char *doc;
	int type;
	size_t size;
	time_t lm;
	struct file *next;
};


/* declare structure used to hold a mouse active area and the code we
want returned when we click on that area */
typedef struct SArea {
	int x1;		/* start coords */
	int y1;
	int x2;		/* end coords */
	int y2;
	int ret;	/* return value */
	struct SArea *next;
} Area, *PArea;


/* declare routines */
static void do_form_menu(int type, ITEM *item);
static int form_virtualize(int c);
static void update_dir_menu(int type, MENU **pmenu, int preserve);
static int sortfilename(struct file **s1,struct file **s2);


/* useful definitions */
#define P(string)       printw("%s\n", string)
#ifndef CTRL
#define CTRL(x)         ((x) & 0x1f)
#endif


/* define some key/character names */
#define ESCAPE          CTRL('[')
#define BLANK           ' '     /* this is the background character */
#define TAB		'\t'

/* character names for the menu_ and form_virtualize routines */
enum {QUIT = MAX_COMMAND + 1, MOUSE, RETURN, COMPLETE, MENU_CODE,
HELP, TFM_HELP, SEARCH, NEW_FILES, FILES, CTRL_L, RECALL_FILE,
/* functions for the FILES_MENU and NEW_FILES_MENU */
STORE_FILE, DELETE_FILE, MKDIR, VIEW_FILE, VIEW_SEQ, COPY_FILE, STUFF_FILE,
SORTFILENAME, SORTFILETIME, SORTFILENAMECASE };

/* define some types of menus */
enum {GROUPS_MENU, FILES_MENU, NEW_FILES_MENU, PROGS_MENU, SEARCH_MENU,
ALPHA_MENU};

/* definitions for file types */
enum {UNREADABLE_FILE, TEXT_FILE, DIR_FILE};

/* definitions for file date type */
enum {TIMES_PAST, TIMES_NEW, TIMES_LATEST};




/* static variables */

/* the time that the menu program started - used to decide if a file's
creation time is 'new' or not */
static time_t start_time; 

/* the time that the last shellout() command started - used to decide if
a file was created by the command or not */
static time_t shellout_time;

/* emboss program & group list data */
/* list of groups pointing to lists of programs */
static AjPList glist;
/* alphabetical list of all programs */
static AjPList alpha;
/* results of last search of programs and docs - if not NULL then it has
been assigned to and the list should be deleted before using it again */
static AjPList searchlist = NULL;

/* remembered file name (or directory name)
This is set only by the key comamnd ^S (store filename)
*/
/* the buffer of the remembered file name - initially points to NULL to
show we have not yet stored a file name */
static char *file_store = NULL;

/* the directory of the remembered file name - initially points to NULL to
show we have not yet stored a file name */
static char *file_store_dir = NULL;

/* default_file - the name of the last file we did something with
This is set by doing any of:
	F1 - view file (when on a file in a FILES menu)
	F2 - view sequence file (when on a file in a FILES menu)
	F4 - copy file (when on a file in a FILES menu)
	selecting (RETURN/double click) a file in a FILES menu
*/
/* the buffer of the name of the last file we did something with
 - initially points to NULL to show we have not yet stored a file name */
static char *default_file = NULL;

/* the directory of the name of the last file we did something with
 - initially points to NULL to show we have not yet stored a file name */
static char *default_file_dir = NULL;

/* the above two appended together to make the full default_file path */
static char *default_file_path = NULL;

/* The current routine used to sort files with qsort() */
static int (*SORTFILE)(struct file **s1,struct file **s2) = sortfilename;

/* Flag for want to stuff the comamnd 'more filenames' after every shellout() */
static int WANTMORE = TRUE;


/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/* MOUSE ACTIVE AREA ROUTINES */
/*****************************************************************************/
/*
Add an area to a list to be searched when mouse is clicked

ARGS
	PArea *list - updated list of struct area
	int x1 - x of top left of area
	int y1 - y of top left of area
	int x2 - x of bottom right of area
	int y2 - y of bottom right of area
	int ret - code to return when mouse is clicked in this srea
*/
static void
area_save(PArea *list, int x1, int y1, int x2, int y2, int ret)
{
	PArea new = (PArea)malloc(sizeof(struct SArea));

	new->x1 = x1;
	new->y1 = y1;
	new->x2 = x2;
	new->y2 = y2;
	new->ret = ret;
	new->next = *list;
	*list = new;
}
/*****************************************************************************/
/* 
Print text to window and add place were text is printed to mouse active area
It assumes that the cursor is in the correct place to print already

ARGS
	PArea *list - updated list of struct area
	WINDOW * w - current window to print in
	char * hitext - highlighted text to print 
	char * text - addition text with no highlighting
		    - these make the basic active area (+padding)
	int ret_code - code to return if a mouse is clicked on this text
	int padding - number of extra spaces to left and right to add to area
*/
static void
area_print(PArea *list, WINDOW *w, char *hitext, char *text, int ret_code, int padding)
{
    int winx, winy, curx, cury, x1, y1, x2, y2;
 
/* get the current position of the cursor on the screen */
    getyx(w, cury, curx);
    getbegyx(w, winy, winx);
    y1 = winy+cury;
    x1 = winx+curx-padding;

/* print the text in reverse video */
    wattron(w, A_REVERSE);
    waddstr(w, hitext);
    wattroff(w, A_REVERSE);
    waddstr(w, text);
    wrefresh(w);

/* get the new position of the cursor */
    getyx(w, cury, curx);
    y2 = winy+cury;
    x2 = winx+curx+padding-1;	/* current position of the cursor-1 */

/* save the active area */
    area_save(list, x1, y1, x2, y2, ret_code);

}

/*****************************************************************************/
/* 
Go through an areas list looking for a match to the mouse click area
This assumes we have received a KEY_MOUSE from getch()

ARGS
	PArea list - list of struct area
RETURNS
	the return code if we are in an area, else, -1
*/
static int
area_search(PArea list)
{
#ifdef NCURSES_MOUSE_VERSION
    MEVENT mus;
    PArea area;

    if (getmouse(&mus) == OK) {
        area = list;
        while (area) {
            if (mus.x >= area->x1
	     && mus.x <= area->x2
             && mus.y >= area->y1
             && mus.y <= area->y2) {
		return area->ret;
            }
            area = area->next;
        }
    }
#endif
    return -1;
}

/*****************************************************************************/
/* 
Delete a list of mouse active areas. 
*list is returned as NULL.

ARGS
	PArea list - list of struct area
*/
static void
area_delete(PArea *list)
{
    PArea last;

    while (*list) {
	last = *list;
	*list = (*list)->next;
	free(last);
    }
}

/*****************************************************************************/
/* 
Test if button-2 was clicked

RETURNS
	1 if BUTTON2 was CLICKED, else 0
*/

/*
static int
button2()
{
#ifdef NCURSES_MOUSE_VERSION
    MEVENT mus;

    if (getmouse(&mus) == OK) {
	if (mus.bstate & BUTTON2_CLICKED) return 1;
    }
#endif
    return 0;
}
*/

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/* UTILITY ROUTINES                                                          */
/*****************************************************************************/
static void
Pause(WINDOW *curwin)
{
    wmove(curwin, LINES - 1, 0);
    waddstr(curwin, "Press any key to continue... ");
    wrefresh(curwin);
    (void) getch();
}


/*****************************************************************************/
/* strip leading spaces and TABs from a char* 

ARGS:
	str - char * returned without leading spaces/TABs
*/
static void
stripstartstr(char *str)
{
    char *src=str, *dest=str;
    
    if (*str != ' ' && *str != TAB) return;
    
    while (*src++) {
    	if (*src != ' ' && *src != TAB) {
    	    while ((*dest++ = *src++));
    	    break;
    	}
    }
}
/*****************************************************************************/
/* strip trailing spaces and TABs from a char* 

ARGS:
	str - char * returned without trailing spaces/TABs
*/
static void
stripendstr(char *str)
{
    char *src=str+strlen(str)-1;

    while (src >= str && (*src == ' ' || *src == TAB)) {
        src--;	
    }
    *++src = '\0';


}
/*****************************************************************************/
/* display a message 

ARGS:
	curwin - current window to be redisplayed at end
	title - title to be displayed at top of message window
	message - message to display
*/
static void
message(WINDOW *curwin, char *title, char *message)
{

    WINDOW *w = newwin(10, 70, 5, 5);
    
    box(w, ACS_VLINE, ACS_HLINE);
    mvwaddstr(w, 1, 35 - strlen(title)/2, title);
    mvwaddstr(w, 5, 35 - strlen(message)/2, message);
    mvwaddstr(w, 8, 2, "Press any key to continue... ");
    wrefresh(w);
    getch();    
    werase(w);

    delwin(w);
    touchwin(curwin);
    wrefresh(curwin);
}
/*****************************************************************************/
/* display a message and wait for the user to answer YES or NO - returns 1 or 0 
If using ncurses if allows the user to click on the YES and NO.

ARGS:
	curwin - current window to be redisplayed at end
	title - title to be displayed at top of message window
	message - message to display
*/
static int
yesno(WINDOW *curwin, char *title, char *message)
{

    WINDOW *w = newwin(10, 70, 5, 5);
    int c;
    int result;
    PArea list=NULL;	/* list of mouse active areas */

    box(w, ACS_VLINE, ACS_HLINE);
    mvwaddstr(w, 1, 35 - strlen(title)/2, title);
    mvwaddstr(w, 5, 35 - strlen(message)/2, message);

    wmove(w, 8, 28);
    area_print(&list, w, " Yes ", "", 1, 1);
    waddstr(w, " or ");
    area_print(&list, w, " No ", "", 0, 1);

    while (1) {
        c = getch();
#ifdef NCURSES_MOUSE_VERSION
	if (c == KEY_MOUSE) {
            result = area_search(list);
	    if (result != -1) {
		area_delete(&list);
	    	return result;
	    }
        } else 
#endif
        if (c == 'y' || c == 'Y') {
		result = 1;
		break;
	} else if (c == 'n' || c == 'N') { 
	
		result = 0;
		break;
	}
    }
    werase(w);
    delwin(w);
    touchwin(curwin);
    wrefresh(curwin);
    return result;
}

/*****************************************************************************/
/* form to prompt for input

ARGS:
	curwin - current window to be redisplayed at end
	title - title to be displayed at top of message window
	msg - message to display
RETURNS
	malloc'd entered string - should be free'd after use
*/
static char *
prompt(WINDOW *curwin, char *title, char *msg)
{

    WINDOW *w = newwin(6, 70, 5, 5);
    FORM *form;
    FIELD *f[2];
    WINDOW *s;
    int finished = FALSE;
    int chr, c;
    int res;
    char *buffer = NULL;

    box(w, ACS_VLINE, ACS_HLINE);
    mvwaddstr(w, 1, 35 - strlen(title)/2, title);
    mvwaddstr(w, 3, 2, msg);
    mvwaddstr(w, 4, 2, ">");	/* prompt for field at line 4, col 2 */
    wrefresh(w);

/* describe the form */
    f[0] = new_field(1, 60, 0, 0, 0, 0);
    f[1] = (FIELD *) 0;
    form = new_form(f);
    set_form_win(form, w);

/* position form subwindow of 1 line height, window width-5 at window
line 4, col 4 */
    s = derwin(w, 1, 65, 4, 4);
    set_form_sub(form, s);
    keypad(w, TRUE);

/* turn off the overloaded behaviour of REQ_DEL_PREV (delete previous
character - this appends the current line to the end of the previous
field by default - we have no previous field that is editable, so it
crashes!) */
    form_opts_off(form, O_BS_OVERLOAD);

    if (post_form(form) != E_OK) wrefresh(w);
    wrefresh(form_win(form));

/* edit field */
    while (!finished) {
/* use the input character to drive the form field */
        chr = getch();
        c = form_virtualize(chr);
        res = form_driver(form, c);
        wrefresh(w);
        switch (res) {
/****************************/
/* IN-LINE EDITING COMMANDS */
/****************************/
	case E_OK:
	    break;
	    
/****************************/
/* UNKNOWN COMMANDS         */
/****************************/
        case E_UNKNOWN_COMMAND:
	    if (c == RETURN) {
                form_driver(form, REQ_VALIDATION);	/* force the form field buffer to be updated */
	        buffer = (char *)malloc(strlen(field_buffer(f[0], 0))+1);
	        strcpy(buffer, field_buffer(f[0], 0));

/* remove start and trailing spaces */
	        stripstartstr(buffer);
	        stripendstr(buffer);
		finished = TRUE;
	    }
	    break;
/*****************************/
/* REQUEST DENIED, ERROR etc */
/*****************************/
        default:
            beep();
            break;
        }
    }

/* end form */
    unpost_form(form);	/* erase form from window on screen */
    werase(w);		/* erase window on screen */
    wrefresh(w);	
    delwin(s);		/* delete subwindow and window */
    delwin(w);
    touchwin(curwin);
    wrefresh(curwin);

    return buffer;
}
/*****************************************************************************/
/* Run the command displaying the results on the screen.
Check for 'cd' commands and do a chdir() instead! 

A new, blank window is created to display the commands against,
the original terminal characteristics are recreated,
the cursor is placed at the top of the screen (using system("clear") - ugly!)
the command is run using system
if status != 0 then a prompt is displayed - wait for key press
the new window is deleted,
and the curses display is recreated.

ARGS:
	curwin - current window to refresh after system call;
	cmd - command to execute
*/
static void
shellout(WINDOW *curwin, char *cmd)
{
    int result;

    WINDOW *w;
    char *home = getenv("HOME");

/* strip leading spaces from cmd */
    stripstartstr(cmd);

/* strip trailing spaces from cmd */
    stripendstr(cmd);

/* make a blank window to write program output against */
    w = newwin(LINES, COLS, 0, 0);
    wclear(w);
    wrefresh(w);

/* restore normal terminal characteristics, run command, prompt for key press */
    def_prog_mode();
    endwin();
    system("clear");
    printf("%% %s\n", cmd);fflush(stdout);
    if (!strncmp(cmd, "cd ", 3)) {
        stripstartstr(&cmd[3]);	/* make sure there are no more spaces before dir name */
    	result = chdir(&cmd[3]);
    } else if (!strncmp(cmd, "cd", 2)) {
    	result = chdir(home);
/* STOP USER RUNNING THE MENU RECURSIVELY - implemented 9 Feb 2001 - GWW */
    } else if (!strcmp(cmd, PROGRAM_NAME)) {
    	printf("You are trying to run '%s' from within '%s'\n", PROGRAM_NAME, PROGRAM_NAME);
    	printf("This can get confusing, so it is not allowed.\n\n");
    	result = 0;

    } else {

/* get the time so we can note files created by this command */
/* backdate the time by 5 seconds in case the file-server has a slightly
different time to the user's machine - this is a problem that can be
quite bad and there is not a good workaround (maybe touch a temporary
file - see what its creation time is and adjust our times accordingly?)
*/
        shellout_time = time(NULL)-5;

        result = system(cmd);
    }
    if (result != 0) {
    	printf("WARNING An error occurred when this command was run.\n");
    }
    printf("Press RETURN to continue ");
    fflush(stdout);
    getch();
    
/* delete blank window */
    delwin(w);
    
/* completely redraw the old window and restore curses */
    touchwin(curwin);
    wrefresh(curwin);
    
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/* FILES MENU ROUTINES                                                       */
/*****************************************************************************/

/*
Returns DIR_FILE if the file is a directory
Returns TEXT_FILE if the file is not a directory
Returns UNREADABLE_FILE if an error occurs (not a valid filename, etc.)

*/
static int
is_directory(char *path)
{
   struct stat finfo;
	  
   if (stat(path, &finfo) == -1)
       return UNREADABLE_FILE; /* if error */
	       
   return(S_ISDIR(finfo.st_mode) ? DIR_FILE : TEXT_FILE);
}

/*****************************************************************************/
/*
Returns the size of a file

*/
static void
file_size(struct stat finfo, char *size)
{

  size[0] = '\0';

  if (finfo.st_size == -1) {
      size[0] = '\0';
  } else if (!finfo.st_size) {
    sprintf(size, "   0  b");
  } else if(finfo.st_size < 1024) {
    sprintf(size, "%4d  b", (int)finfo.st_size);
  } else if (finfo.st_size < 1048576) {
    sprintf(size, "%4d Kb", (int)finfo.st_size / 1024);
  } else {
    sprintf(size, "%4d Mb", (int)finfo.st_size / 1048576);
  }

  return;

}

/*****************************************************************************/
/* routine used for sorting file entries by filename */

static int 
sortfilename(struct file **s1,struct file **s2)
{           
    return(strcmp((*s1)->name,(*s2)->name));
}

/*****************************************************************************/
/* routine used for sorting file entries by filename (case independently) */

static int 
sortfilenamecase(struct file **s1,struct file **s2)
{           
    return(strcasecmp((*s1)->name,(*s2)->name));
}

/*****************************************************************************/
/* routine used for sorting file entries by last modified time */

static int 
sortfiletime(struct file **s1,struct file **s2)
{           
    return((int)(*s1)->lm - (int)(*s2)->lm);
}

/*****************************************************************************/

/* a name of a file input and the details of the file are stored in the
output struct file  NULL is returned if we are not interested in this file 
for example, we only want new files, or its name starts with a '.'

ARGS
	char *name - name of file
	int new - TIMES_PAST displays all files
		- TIMES_NEW - displays files created since menu program started
		- TIMES_LATEST - displays files created in last shellout()
	int required_type - OR'd set of file-types. Only the file types
		matching this will be returned. Values are one or more of:
		- TEXT_FILE
		- DIR_FILE
RETURNS
	struct file * - pointer to structure containing information on the file

*/
        
static struct file *
make_dir_entry(char *name, int new, int required_type)
{
    struct file *p;
    struct stat finfo;
    char tmp[100];
    struct tm *ts;
    int type;

/* if file isn't readable, return NULL */
    if (stat(name, &finfo) == -1)
        return(NULL);

/* ignore any filename starting with a dot '.' */
    if((name[0] == '.'))
        return(NULL);
  
/* ignore old files if want new ones */
    if (new == TIMES_NEW && (difftime(finfo.st_mtime, start_time) < 0.0))
        return(NULL);
    if (new == TIMES_LATEST && (difftime(finfo.st_mtime, shellout_time) < 0.0))
        return(NULL);
    
/* ignore file types we don't want */
    type = TEXT_FILE;
    if (S_ISDIR(finfo.st_mode))
    	type = DIR_FILE;
    if (!(required_type & type))
    	return(NULL); 

/* make the structure */
    if(!(p=(struct file *)malloc(sizeof(struct file))))
        exit(1);
    if(!(p->name=(char *)malloc(strlen(name) + 1)))
        exit(1);
    strcpy(p->name, name);
     
/* file exists, put info into struct file */
    p->lm = finfo.st_mtime;
    p->size = -1;
    p->type = type;
 
/* prepare the description */
    p->doc = (char *)malloc(100);

/* is it is a directory */
    if (S_ISDIR(finfo.st_mode)) {
        strcpy(p->doc, "Dir ");
        p->size = -1;

/* normal file */
     } else {
         p->size = finfo.st_size;
         strcpy(p->doc, "    ");
     }

/* do time */
    tmp[0] = '\0';
    ts = localtime(&finfo.st_mtime);
/* 100 = size of char * time */
    strftime(tmp, 100, "%d %b %Y %H:%M ", ts);
    strcat(p->doc, tmp);

/* size */
    file_size(finfo, tmp);
    strcat(p->doc, tmp);

    return(p);
}
    

/*****************************************************************************/
/*
Create a list of struct file

ARGS
	int new - TIMES_PAST displays all files
		- TIMES_NEW - displays files created since menu program started
		- TIMES_LATEST - displays files created in last shellout()
	int *no_of_files - returned number of files in list
	int required_type - OR'd set of file-types. Only the file types
		matching this will be returned. Values are one or more of:
		- TEXT_FILE
		- DIR_FILE
	
RETURNS
	a pointer to the head of a struct file list
	or
	-1 if the directory is not searchable
*/
static struct file *
new_file_list(int new, int *no_of_files, int required_type)
{

    DIR *d;
    struct dirent *dstruct;
    struct file *head,*p;

    *no_of_files = 0;

/* get the files in current directory */
    if (!(d=opendir("."))) {
  	return (struct file *)-1;
    }

    head=NULL;
/* read through directory using names to get details which are stored in
a struct file list
*/
    while ((dstruct=readdir(d))) {
        if ((p = make_dir_entry(dstruct->d_name, new, required_type))) {
            p->next = head;
            head = p;
            (*no_of_files)++;
        }
    }
    closedir(d);

    return head;

}
/*****************************************************************************/
/*
Delete a list of struct file

ARGS
	struct file* head - pointer to first node of list to delete
*/
static void
delete_file_list(struct file* head)
{

    struct file *p,*q;

    q = head;
    while(q) {
         p=q->next;
         free(q);
         q=p;
    }           
}
/*****************************************************************************/
/*
Sets up the ITEM array for a menu of files.

ARGS
	int new - TIMES_PAST displays all files
		- TIMES_NEW - displays files created since menu program started
		- TIMES_LATEST - displays files created in last shellout()
	WINDOW *w - window to display error message in
*/
static ITEM **
populate_file_menu(int new, WINDOW *w)
{

    ITEM **items;
    ITEM **ip;
    int n;
    int no_of_files=0;
    int x;
    
    struct file *head,*p;
    struct file **ar;

    int got_a_long_name = 0;

/*
 * Since we don't know how many dir. entries there are, put them into a
 * linked list and then arrayificate them so qsort can use them.
 */
    head = new_file_list(new, &no_of_files, DIR_FILE|TEXT_FILE);
    
    if (head == (struct file *)-1) {
        message(w, "WARNING", "You do not have permission to look at that directory");
        items = (ITEM **)malloc(sizeof(ITEM *)*5);
        ip = items;
        *ip++ = new_item("EXIT", "");
        *ip++ = new_item("HELP", "");
        *ip++ = new_item("TO HOME", "");
        *ip++ = new_item("PARENT DIRECTORY", "");
        *ip = (ITEM *) 0;
        return items;
    }


/* arrayificate */
    if (!(ar=(struct file **) malloc(no_of_files*sizeof(struct file *)))) {
        printf("Error - too many files to list.\n");
        exit(0);
    }
    p=head;
    x=0;
    while(p) {
/* quick check to see if we have any ridiculously long file names */
        if (strlen(p->name)+31 > COLS) got_a_long_name = 1;
/* continue with the arrayification */
        ar[x++]=p;
        p = p->next;
    }
    
/* sort the array of filenames usnig the current type of sorting routine */
    qsort((void *)ar, no_of_files, sizeof(struct file *), (int (*)(const void *,const void *))SORTFILE); 
    

/* add on space for 5 extra options plus null at end */
    items = (ITEM **)malloc(sizeof(ITEM *)*(no_of_files+6));
    ip = items;
    *ip++ = new_item("EXIT", "");
    *ip++ = new_item("HELP", "");
    *ip++ = new_item("TO HOME", "");
    *ip++ = new_item("PARENT DIRECTORY", "");
    *ip++ = new_item(" ", "");

    for (n = 0; n < no_of_files; n++, ip++) {
        if (got_a_long_name)
            *ip = new_item(ar[n]->name, "");
        else
            *ip = new_item(ar[n]->name, ar[n]->doc);
    }
    *ip = (ITEM *) 0;  

/* tidy up 
(don't delete the elements of array as they are still pointed to by items[]) */
    free(ar);

/* free up struct file list */
    delete_file_list(head);

    return items;
}
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/* FIELD & FORMS ROUTINES                                                    */
/*****************************************************************************/
static FIELD *
make_label(int frow, int fcol, char *label)
{
    FIELD *f = new_field(1, strlen(label), frow, fcol, 0, 0);

    if (f) {
        set_field_buffer(f, 0, label);
        field_opts_off(f, O_ACTIVE);	/* can't move into this field */
    }
    return (f);
}

/*****************************************************************************/
static FIELD *
make_field(int frow, int fcol, int rows, int cols)
{
    FIELD *f = new_field(rows, cols, frow, fcol, 0, 0);

    if (f) {
        set_field_userptr(f, (void *) 0);
        field_opts_off(f, O_STATIC);	/* field can grow in size */
    }
    return (f);
}

/*****************************************************************************/
static int
form_virtualize(int c)
{

/* control codes used in the menu driver, should be converted to
'MENU_CODE' so that the form driver doesn't swallow them up */
    static const struct {
        int code;
        int result;
    } lookup[] = {
        { CTRL('A'),	 REQ_BEG_FIELD	}, /* start of field */
        { CTRL('B'),	 REQ_LEFT_CHAR	}, /* <- char */
        { CTRL('C'),	 REQ_CLR_FIELD	}, /* clear field */
        { CTRL('D'),	 MENU_CODE	}, /* page down */
        { CTRL('E'),	 REQ_END_FIELD	}, /* end of field */
        { CTRL('F'),	 REQ_RIGHT_CHAR	}, /* -> char */
        { CTRL('G'),	 STUFF_FILE	}, /* Stuff default file into field */
        { CTRL('H'),	 REQ_DEL_PREV	}, /* Delete character */
        { CTRL('I'),	 COMPLETE	}, /* complete field if it matches menu item */
        { CTRL('K'),	 REQ_CLR_EOF	}, /* kill field to end */
	{ CTRL('L'),	 CTRL_L		}, /* redisplay */
        { CTRL('M'),	 RETURN		}, /* RETURN */
        { CTRL('N'),	 NEW_FILES	}, /* New Files menu */
        { CTRL('R'),	 RECALL_FILE	}, /* recall stored file */
        { CTRL('S'),	 STORE_FILE	}, /* store file name */
        { CTRL('T'),	 FILES		}, /* All files menu */
        { CTRL('U'),	 MENU_CODE	}, /* page up */
        { CTRL('W'),	 REQ_NEXT_WORD	}, /* next word in field */
        { CTRL('X'),	 QUIT		}, /* EXIT */
        { CTRL('?'),	 TFM_HELP	}, /* ^? - tfm help */
        { '?',		 HELP		}, /* ? - help */
        { KEY_SLEFT,	 REQ_LEFT_CHAR	}, /* <- char */
        { KEY_SRIGHT,	 REQ_RIGHT_CHAR	}, /* -> char */
        { KEY_BACKSPACE, REQ_DEL_PREV	}, /* BACKSPACE */
        { KEY_DC,	 REQ_DEL_PREV	}, /* DELETE */
        { KEY_F(1),	 VIEW_FILE	}, /* F1 - view file */
        { KEY_F(2),	 VIEW_SEQ	}, /* F2 - view file as sequence */
        { KEY_F(3),	 MKDIR		}, /* F3 - make new directory */
        { KEY_F(4),	 COPY_FILE	}, /* F4 - copy file */
        { KEY_F(5),	 DELETE_FILE	}, /* F5 - delete file */
        { KEY_F(7),	 SORTFILENAMECASE},/* F7 - sort by filename (case independently) */
        { KEY_F(8),	 SORTFILENAME	}, /* F8 - sort by filename */
        { KEY_F(9),	 SORTFILETIME	}, /* F9 - sort by file create time */
    };

    unsigned n;

    for (n = 0; n < sizeof(lookup) / sizeof(lookup[0]); n++) {
        if (lookup[n].code == c) {
            c = lookup[n].result;
            break;
        }
    }

    return c;
}

/*****************************************************************************/
/* displays the help for a form in the two lines at the bottom of the screen

ARGS
	int type - menu type
	WINDOW *w - current window
	PArea *list - updated list of active mouse areas
*/
static void
display_help(int type, WINDOW *w, PArea *list)
{

/* delete any old mouse active areas in list */
    area_delete(list);

/* clear help lines */
    wmove(w, LINES-1, 0);
    wclrtoeol(w);
    wmove(w, LINES-2, 0);
    wclrtoeol(w);

    switch (type) {

    	case GROUPS_MENU:

            area_print(list, w, "^X", " Exit ", CTRL('X'), 0);
            area_print(list, w, "^T", " All Files ", CTRL('T'), 0);
	    area_print(list, w, "^A", " StartLine ", CTRL('A'), 0);
            area_print(list, w, "^B", " Back    ", CTRL('B'), 0);
            area_print(list, w, "^C", " Clear  ", CTRL('C'), 0);
            area_print(list, w, "^U", " Menu Up ", CTRL('U'), 0);
            area_print(list, w, "^P", " Page Up", CTRL('P'), 0);

            wmove(w, LINES-1, 0);

	    waddstr(w, " ");
	    area_print(list, w, "?", " Help ", '?', 0);
            area_print(list, w, "^N", " New Files ", CTRL('N'), 0);
            area_print(list, w, "^E", " End Line  ", CTRL('E'), 0);
            area_print(list, w, "^F", " Forward ", CTRL('F'), 0);
            area_print(list, w, "^K", " ClrEnd ", CTRL('K'), 0);
            area_print(list, w, "^D", " Menu Dn ", CTRL('D'), 0);
	    area_print(list, w, "^Z", " Page Dn", CTRL('Z'), 0);

	    break;
	    
    	case FILES_MENU:
            area_print(list, w, "^X", " Exit ", CTRL('X'), 0);
	    area_print(list, w, "F1", " ViewFile ", KEY_F(1), 0);
	    area_print(list, w, "F3", " New Dir  ", KEY_F(3), 0);
	    area_print(list, w, "^G", " GetFile ", CTRL('G'), 0);
	    area_print(list, w, "^S", " StoreFile ", CTRL('S'), 0);
	    area_print(list, w, "F9", " TimeSort ", KEY_F(9), 0);
	    area_print(list, w, "^N", " NewFile", CTRL('N'), 0);
	    
            wmove(w, LINES-1, 0);

	    waddstr(w, " ");
	    area_print(list, w, "?", " Help ", '?', 0);
	    area_print(list, w, "F2", " View Seq ", KEY_F(1), 0);
	    area_print(list, w, "F4", " CopyFile ", KEY_F(4), 0);
	    area_print(list, w, "F5", " DelFile ", KEY_F(5), 0);
	    area_print(list, w, "^R", " RecalFile ", CTRL('R'), 0);
	    area_print(list, w, "F8", " AlphaSort", KEY_F(8), 0);


	    break;
    	case NEW_FILES_MENU:

            area_print(list, w, "^X", " Exit ", CTRL('X'), 0);
	    area_print(list, w, "F1", " ViewFile ", KEY_F(1), 0);
	    area_print(list, w, "F3", " New Dir  ", KEY_F(3), 0);
	    area_print(list, w, "^G", " GetFile ", CTRL('G'), 0);
	    area_print(list, w, "^S", " StoreFile ", CTRL('S'), 0);
	    area_print(list, w, "F9", " TimeSort ", KEY_F(9), 0);
	    area_print(list, w, "^T", " AllFile", CTRL('T'), 0);
	    
            wmove(w, LINES-1, 0);

	    waddstr(w, " ");
	    area_print(list, w, "?", " Help ", '?', 0);
	    area_print(list, w, "F2", " View Seq ", KEY_F(1), 0);
	    area_print(list, w, "F4", " CopyFile ", KEY_F(4), 0);
	    area_print(list, w, "F5", " DelFile ", KEY_F(5), 0);
	    area_print(list, w, "^R", " RecalFile ", CTRL('R'), 0);
	    area_print(list, w, "F8", " AlphaSort", KEY_F(8), 0);

	    break;

    	case PROGS_MENU:
    	case ALPHA_MENU:
    	case SEARCH_MENU:

            area_print(list, w, "^X", " Exit ", CTRL('X'), 0);
            area_print(list, w, "^T", " All Files ", CTRL('T'), 0);
	    area_print(list, w, "^G", " Get File  ", CTRL('G'), 0);
	    area_print(list, w, "^J", " Complete  ", TAB, 0);
            area_print(list, w, "^C", " Clear  ", CTRL('C'), 0);
            area_print(list, w, "^U", " Menu Up ", CTRL('U'), 0);
            area_print(list, w, "^P", " Page Up", CTRL('P'), 0);


            wmove(w, LINES-1, 0);

	    waddstr(w, " ");
	    area_print(list, w, "?", " Help ", '?', 0);
            area_print(list, w, "^N", " New Files ", CTRL('N'), 0);
	    area_print(list, w, "^R", " RecalFile ", CTRL('R'), 0);
            area_print(list, w, "^E", " End Line  ", CTRL('E'), 0);
            area_print(list, w, "^K", " ClrEnd ", CTRL('K'), 0);
            area_print(list, w, "^D", " Menu Dn ", CTRL('D'), 0);
	    area_print(list, w, "^Z", " Page Dn", CTRL('Z'), 0);




	    break;
    }
    wrefresh(w);
}


/*****************************************************************************/
/* sets up the form, returns form object handles */
static void
start_form(int type, WINDOW *w, FORM **pf)
{
    FORM *form;
    FIELD *f[3];
    unsigned n=0;

/* describe the form */
    f[n++] = make_label(0, 0, "> ");
    f[n++] = make_field(0, 2, 1, 70);
    f[n++] = (FIELD *) 0;

    form = new_form(f);

    set_form_win(form, w);
/* position form subwindow of 1 line height, full screen width at 3 lines up from the bottom */
    set_form_sub(form, derwin(w, 1, COLS, LINES-3, 0));
    keypad(w, TRUE);

/* turn off the overloaded behaviour of REQ_DEL_PREV (delete previous
character - this appends the current line to the end of the previous
field by default - we have no previous field that is editable, so it
crashes!) */
    form_opts_off(form, O_BS_OVERLOAD);


    if (post_form(form) != E_OK) wrefresh(w);
    wrefresh(form_win(form));

/* return object handles */
    *pf = form;
}

/*****************************************************************************/
static void
end_form(FORM *form)
{
    WINDOW *w = form_win(form);
    WINDOW *s = form_sub(form);

    unpost_form(form);	/* erase form from window on screen */
    werase(w);		/* erase window on screen */
    wrefresh(w);	
    delwin(s);		/* delete subwindow and window */
/* don't do a  free_form(form); here, despite what the NCURSES guide says - you get a Bus error */
     
}


/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/* MENU ROUTINES                                                             */
/*****************************************************************************/
static int
menu_virtualize(int c)
{
    static const struct {
        int code;
        int result;
    } lookup[] = {
        { CTRL('P'),	 REQ_SCR_UPAGE	},
        { CTRL('Z'),	 REQ_SCR_DPAGE	},
        { KEY_NPAGE,	 REQ_SCR_UPAGE	},
        { KEY_PPAGE,	 REQ_SCR_DPAGE	},
        { KEY_DOWN,	 REQ_NEXT_ITEM	},
        { KEY_UP,	 REQ_PREV_ITEM	},
        { CTRL('D'),	 REQ_NEXT_ITEM	},
        { CTRL('U'),	 REQ_PREV_ITEM	},
	{ KEY_EXIT,	 QUIT		},
	{ KEY_HOME, 	 REQ_FIRST_ITEM	},
	{ KEY_END,	 REQ_LAST_ITEM	},
        { KEY_LEFT,	 REQ_LEFT_ITEM	},
        { KEY_RIGHT,	 REQ_RIGHT_ITEM	}
    };

    unsigned n;

    for (n = 0; n < sizeof(lookup) / sizeof(lookup[0]); n++) {
        if (lookup[n].code == c) {
            c = lookup[n].result;
            break;
        }
    }

    return c;

}   
/*****************************************************************************/
/*
Populate the menus with the program name and descriptions from a list.

We need to truncate the descriptions of any EMBOSS programs where
length(program-name) + length(description) + 2 > COLS
*/
static void
populate_program_menus(AjPList plist, ITEM **ip)
{
	
    EmbPGroupTop gl;    /* Group/Program list node pointer */
    AjIList giter;	/* iterator for groups/programs lists */
    int namelen=0;	/* longest name string */
    char *buffer;

/* iterate through the programs list to get the longest name */
    giter = ajListIterRead(plist);
    while ((gl = ajListIterNext(giter)) != NULL) {
        if (ajStrGetLen(gl->name) > namelen) namelen = ajStrGetLen(gl->name);
    }
    ajListIterFree(&giter);

/* iterate through the programs list populating the items */
    giter = ajListIterRead(plist);
    while ((gl = ajListIterNext(giter)) != NULL) {
    	if (ajStrGetLen(gl->doc) + namelen + 2 > COLS) {
/* doc is longer than max length - copy string to buffer then truncate */
    	    buffer = (char *) malloc(ajStrGetLen(gl->doc)+1);
    	    strcpy(buffer, ajStrGetPtr(gl->doc));
    	    buffer[COLS-namelen-2] = '\0';
            *ip++ = new_item(ajStrGetPtr(gl->name), buffer);
        } else {
/* simply use the char* of the documentation string */
            *ip++ = new_item(ajStrGetPtr(gl->name), ajStrGetPtr(gl->doc));
        }
    }
    ajListIterFree(&giter);

/* +++ debug to see if a blank line at the end of a menu is destructive */
/* *ip++ = new_item(" ", ""); */
    *ip = (ITEM *) 0;  
}
/*****************************************************************************/
/*
Set up the items of a menu depnding on the type of menu:
GROUPS_MENU, FILES_MENU, NEW_FILES_MENU, PROGS_MENU, SEARCH_MENU, ALPHA_MENU

The 'item' argument is passed over from the previous menu and may be
used to obtain the usrptr of item name or other useful information for
populating a submenu - it may be NULL. 

*/
ITEM **
populate_menu(int type, WINDOW *w, ITEM *item)
{
    ITEM **items;
    ITEM **ip;
    char *name;	/* points to name of program to search for */
    AjIList giter;	/* iterator for groups/programs lists */
    EmbPGroupTop gl;		/* Group/Program list node pointer */
    AjPList plist;	/* list of programs */
    AjPStr searchstr = NULL;


    switch (type) {
    case GROUPS_MENU:
/* add on space for 7 extra options plus null at end */
	ajDebug("starting malloc() for %d items (+8)\n", ajListLength(glist));
        items = (ITEM **)malloc(sizeof(ITEM *)*(ajListLength(glist)+8));
        ip = items;
        *ip++ = new_item("EXIT", "");
        *ip++ = new_item("HELP", "");
        *ip++ = new_item("ALL FILES", "");
        *ip++ = new_item("NEW FILES", "");
        *ip = new_item("ALPHABETIC LISTING OF ALL PROGRAMS", "");
/* set item usrptr to the programs list for this group */	
        ajListFirst(alpha, (void**) &gl);	/* get list of all programs */
        set_item_userptr(*ip, (void *)gl->progs);
	ip++;

        *ip++ = new_item("SEARCH FOR PROGRAMS", "");
        *ip++ = new_item(" ", "");
/* iterate through the groups list */
	ajDebug("starting iteration through groups list\n");
        giter = ajListIterRead(glist);
        while ((gl = ajListIterNext(giter)) != NULL) {
	    ajDebug("Next list node: %S\n", gl->name);
            *ip = new_item(ajStrGetPtr(gl->name), "");
/* set item usrptr to the programs list for this group */
            set_item_userptr(*ip, (void *)gl->progs);
            ip++;
        }
        ajListIterFree(&giter);
        *ip = (ITEM *) 0;  
    	break;

    case FILES_MENU:
	items = populate_file_menu(FALSE, w);
	break;
	
    case NEW_FILES_MENU:
	items = populate_file_menu(TRUE, w);
	break;
	
    case PROGS_MENU:
/* add on space for 3 extra options plus null at end */
        items = (ITEM **)malloc(sizeof(ITEM *)*(ajListLength((AjPList)item_userptr(item))+4));
        ip = items;
        *ip++ = new_item("EXIT", "");
        *ip++ = new_item("HELP", "");
        *ip++ = new_item(" ", "");
	plist = (AjPList)item_userptr(item);
	populate_program_menus(plist, ip);
	break;
	
    case SEARCH_MENU:
/* get thing to search for */
        name = prompt(w, "SEARCH FOR PROGRAMS", 
			"Search in name or documentation of programs for:");
/* if 'searchlist' is not NULL then it has been assigned to before and
we should delete the old list */
	if (searchlist != NULL) {
		(void) embGrpGroupsListDel(&searchlist);
	}
/* do search */
	ajStrAssignC(&searchstr, name);
	searchlist = ajListNew();
	(void) embGrpKeySearchProgs (searchlist, alpha, searchstr, ajFalse);

/* tidy up */
	ajStrDel(&searchstr);
	free(name);

/* get list of found programs */
        ajListFirst(searchlist, (void**)&gl);
        plist = gl->progs;

/* add on space for 3 extra options plus null at end */
        items = (ITEM **)malloc(sizeof(ITEM *)*(ajListLength(plist)+4));
        ip = items;
        *ip++ = new_item("EXIT", "");
        *ip++ = new_item("HELP", "");
/* if nothing found then inform user and don't add to *ip */
	if (ajListLength(plist)) {
            *ip++ = new_item(" ", "");
	    populate_program_menus(plist, ip);
	} else {
	    message(w, "SORRY", "No programs were found");	
            *ip = (ITEM *) 0;  
	}
	break;
	
    case ALPHA_MENU:
/* add on space for 3 extra options plus null at end */
        items = (ITEM **)malloc(sizeof(ITEM *)*(ajListLength((AjPList)item_userptr(item))+4));
        ip = items;
        *ip++ = new_item("EXIT", "");
        *ip++ = new_item("HELP", "");
        *ip++ = new_item(" ", "");
	plist = (AjPList)item_userptr(item);
	populate_program_menus(plist, ip);
	break;
	
    default:
	printf("No such menu type\n");
	exit(1);
    }

    ajDebug("returning from populate_menu()\n");
    return items;
}
/*****************************************************************************/
/*
The 'item' argument is passed over from the previous menu and may be 
used to obtain the usrptr of item name or other useful information for
populating a submenu - it may be NULL.
*/
static void
start_menu(int type, WINDOW *w, MENU **menu, ITEM *item)
{   
    MENU *m;
    ITEM **items;

#define HEIGHT LINES-4

    ajDebug("starting populate_menu\n");
    items = populate_menu(type, w, item);

/* make the menu with some nice options */
    ajDebug("starting new_menu\n");
    m = new_menu(items);
    ajDebug("starting set_menu_mark()\n");    
    set_menu_mark(m, ">");	/* nice pointer for the menu mark string */
/* wrap around to the other end of the menu */
    ajDebug("starting menu_opts_off()\n");    
    menu_opts_off(m, O_NONCYCLIC);
    ajDebug("after menu_opts_off()\n");    
/* if displaying the menu in two columns, go to the end before starting
the second column */
    ajDebug("starting menu_opts_off()\n");    
/* 5 Dec 2000 - we seem to have a crash here when using the curses library 
    menu_opts_off(m, O_ROWMAJOR);
*/    
    ajDebug("after menu_opts_off()\n");    

    ajDebug("starting set_menu_format with HEIGHT=%d\n", HEIGHT);
/* if this is a GROUPS_MENU, make it two columns, else 1 column */
    set_menu_format(m, HEIGHT, (type==GROUPS_MENU) ? 2 : 1);	
    ajDebug("starting set_menu_win\n");
    set_menu_win(m, w);
    ajDebug("starting keypad\n");
    keypad(w, TRUE);
/* position menu 1 line down from top of screen */
    ajDebug("starting set_menu_sub\n");
    set_menu_sub(m, derwin(w, HEIGHT, COLS, 1, 0));

    ajDebug("starting post_menu\n");
    post_menu(m);
    ajDebug("starting wrefresh\n");
    wrefresh(w);
    

/* return menu */
    *menu = m;   
  
}        
/*****************************************************************************/
static void
end_menu(MENU *m)
{
    ITEM **items = menu_items(m);
    ITEM **ip;

    WINDOW *w = menu_win(m);
    WINDOW *s = menu_sub(m);

    unpost_menu(m);
    werase(s);		/* erase subwindow on screen */
    wrefresh(w);	
    delwin(s);		/* delete subwindow and window */
    free_menu(m);

/* free the items */
    for (ip = items; *ip; ip++) {
        free_item(*ip);
    }
    free(items);

}
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/* ROUTINES TO DO SPECIALISED CONTROL KEY FUNCTIONS                          */
/*****************************************************************************/

/* Routine to implement storage of a file in the current menu item. 

Also stores the current directory in case we are in another dir when te
file name is recalled and have to use the full path. 
*/
static void
store_file(int type, MENU *menu)
{
    
    if (type == FILES_MENU || type == NEW_FILES_MENU) {

/* store current menu item name in file_store */
        if (file_store != NULL) free(file_store);	/* free any previous file name string */
        file_store = (char *)malloc(strlen(item_name(current_item(menu)))+1);
        strcpy(file_store, item_name(current_item(menu)));
/* store current directory in file_store_dir */
        if (file_store_dir != NULL) free(file_store_dir);	/* free any previous directory name string */
        file_store_dir = getcwd(NULL, 1000);

    }
}
/*****************************************************************************/
/* Routine to implement recall of a file in the current menu item. 

If the recalled file is from another directory than the current one,
then the full path is stuffed into the form field, else just the
filename is used. 

ARGS:
	int type - menu type
	FORM *form - form with field to be updated
	int *pmatches - updated flag for whether field matches menu item
*/
static void
recall_file(int type, FORM *form, int *pmatches)
{
    int i;
    char *dir;
    
/* recall file from file_store */
    if (file_store) {	
        form_driver(form, ' ');	/* space */
/* if we are not still in file_store_dir then prepend file_store_dir */
        dir = getcwd(NULL, 1000);	/* this directory */
        if (file_store_dir && strcmp(file_store_dir, dir)) {
            for (i=0; file_store_dir[i]; i++) form_driver(form, file_store_dir[i]);
            form_driver(form, '/');	/* add '/' to the directory path */
        }
        free(dir);
/* insert filename into form field */
        for (i=0; file_store[i]; i++) form_driver(form, file_store[i]);
        form_driver(form, ' ');	/* space */
        form_driver(form, REQ_VALIDATION);	/* force the form field buffer to be displayed on the screen */
        *pmatches = FALSE;	/* form field no longer matches menu item */
    }

}
/*****************************************************************************/
/* tests to see if we should use the current menu item or the default
(last used) file name.
Updates the default file name to the current FILES menu item is this is
a valid file name.

If we are not in the original directory of the default file, the path to
this file is prepended. 

ARGS:
	int type - menu type
	MENU **pmenu - pointer to menu (not updated)	
RETURNS:
	path to last file looked at (or NULL if no such valid file)
*/
static char *
get_default_file(int type, MENU **pmenu)
{
MENU *menu = *pmenu;
char *name;
	
/* are we in the NEW_FILES or FILES menus? */
    if (type == NEW_FILES_MENU || type == FILES_MENU) {
/* is the current menu item a valid file? 
- cast it to (char *) because in ncurses item_name() returns (const char *) */
	name = (char *)item_name(current_item(menu));
	if (is_directory(name) == TEXT_FILE) {
/* yes - stuff the current menu item in the default file store */	
            if (default_file != NULL) free(default_file);	/* free any previous file name string */
            default_file = (char *)malloc(strlen(name)+1);
            strcpy(default_file, name);
/* store current directory in default_file_dir */
            if (default_file_dir != NULL) free(default_file_dir);	/* free any previous directory name string */
            default_file_dir = getcwd(NULL, 1000);
/* make path in case we are not in original directory */
	    if (default_file_path != NULL) free(default_file_path);
	    default_file_path = (char *)malloc(strlen(default_file_dir)+
	    				strlen(default_file)+
	    				2);
	    strcpy(default_file_path, default_file_dir);
	    strcat(default_file_path, "/");
	    strcat(default_file_path, default_file);				
	}
    }

    if (default_file == NULL) {
    	return NULL;
    } else if (!strcmp(getcwd(NULL, 1000), default_file_dir)) {
    	return default_file;
    } else {
    	return default_file_path;
    }
}
/*****************************************************************************/
/* Routine to display the current menu item (if we are in a FILES menu)
else we display the default file we last did something with.

ARGS:
	int type - menu type
	WINDOW *w - current window to redisplay at end of shellout
	MENU **pmenu - pointer to menu (not updated)	
*/
static void
view_file(int type, WINDOW *w, MENU **pmenu)
{

char *file;
char *cmd;

/* get default file */
	file = get_default_file(type, pmenu);
	
/* shellout to 'more filename' */
	if (file != NULL) {
            cmd = (char *)malloc(strlen(file)+6);
            strcpy(cmd, "more ");
            strcat(cmd, file);
	    shellout(w, cmd);
	    free(cmd);
	} else {
	    message(w, "NO FILE!", "There is no valid file to display.");
	}

}
/*****************************************************************************/
/* Routine to display the current menu item (if we are in a FILES menu)
as a sequence 
else we display the default file we last did something with.

ARGS:
	int type - menu type
	WINDOW *w - current window to redisplay at end of shellout
	MENU **pmenu - pointer to menu (not updated)	
*/
static void
view_seq(int type, WINDOW *w, MENU **pmenu)
{

char *file;
char *cmd;

/* get default file */
	file = get_default_file(type, pmenu);
	
/* get the width of the screen and get appropriate values for margin/width
to give to showseq */

/* shellout to 'showseq -filter filename|more' */
	if (file != NULL) {
            cmd = (char *)malloc(strlen(file)+22);
            strcpy(cmd, "showseq -filter ");
            strcat(cmd, file);
            strcat(cmd, "|more");
	    shellout(w, cmd);
	    free(cmd);
	} else {
	    message(w, "NO FILE!", "There is no valid file to display.");
	}

}
/*****************************************************************************/
/* Routine to create a subdirectory
If sucessful cd into the new directory.
If we are displaying a files menu, update it.

ARGS:
	int type - menu type
	WINDOW *w - current window to redisplay at end of shellout
	MENU **pmenu - pointer to menu (updated)
*/

static void
make_dir(int type, WINDOW *w, MENU **pmenu)
{
char *name = prompt(w, "MAKE NEW DIRECTORY", 
			"Enter the name of the new directory to create");
char *unwanted = " \t/!@#$%^&*()+|~=\\`;':\",<>[]{}?";
int got_unwanted=FALSE;
char *ptr;

/* convert unusual characters to underscores */
    while((ptr = strpbrk(name, unwanted)) != NULL) {
    	got_unwanted = TRUE;
    	*ptr = '_';
    }

/* if we changed the name, inform the user */
    if (got_unwanted) {
        message(w, "STRANGE CHARACTERS IN NAME - CHANGED TO:", name);
    }

/* make the directory (Read, write, execute by owner) and cd into it */
    if (mkdir(name, S_IRWXU)) {
/* error */    	
        message(w, "ERROR", "You can't make this directory - sorry.");

    } else {
	if (chdir(name)) {
	    message(w, "WARNING", "You do not have permission to go into the directory.");
	} else {
	    message(w, "SUCCESS", "You are now in the new directory.");
	}
    }

/* redisplay files menu */
    if (type == FILES_MENU || type == NEW_FILES_MENU) {
        update_dir_menu(type, pmenu, FALSE);
    }

    free(name);
}
/*****************************************************************************/
/* Routine to copy the default file

ARGS:
	int type - menu type
	WINDOW *w - current window to redisplay at end of shellout
	MENU **pmenu - pointer to menu (updated)
*/

static void
copy_file(int type, WINDOW *w, MENU **pmenu)
{
char *name;
char *unwanted = " \t/!@#$%^&*()+|~=\\`;':\",<>[]{}?";
int got_unwanted=FALSE;
char *ptr;
char *file;
char *cmd;
char *title;

/* get default file */
    file = get_default_file(type, pmenu);
    if (file == NULL) {
        message(w, "NO FILE!", "There is no valid file to copy.");
	return;
    }	

/* get name of copy */
    title = (char *)malloc(strlen(file) + 14);
    strcpy(title, "COPY FILE: '");
    strcat(title, file);
    strcat(title, "'");
    name = prompt(w, title, 
			"Enter the name of the new file:");

/* convert unusual characters to underscores */
    while((ptr = strpbrk(name, unwanted)) != NULL) {
    	got_unwanted = TRUE;
    	*ptr = '_';
    }

/* if we changed the name, inform the user */
    if (got_unwanted) {
        message(w, "STRANGE CHARACTERS IN NAME - CHANGED TO:", name);
    }

/* run command 'cp file name' */
    cmd = (char *)malloc(strlen(file)+strlen(name)+5);
    strcpy(cmd, "cp ");
    strcat(cmd, file);
    strcat(cmd, " ");
    strcat(cmd, name);
    if (system(cmd)) {
        message(w, "ERROR", "You can't copy that file.");
    } else {

/* redisplay files menu */
        if (type == FILES_MENU || type == NEW_FILES_MENU) {
            update_dir_menu(type, pmenu, TRUE);
        }
    }
    
    free(cmd);
    free(name);
    free(title);
}
/*****************************************************************************/
/* Routine to delete the default file

ARGS:
	int type - menu type
	WINDOW *w - current window to redisplay at end of shellout
	MENU **pmenu - pointer to menu (updated)
*/

static void
delete_file(int type, WINDOW *w, MENU **pmenu)
{
char *file;
char *title;

/* get default file */
    file = get_default_file(type, pmenu);
    if (file == NULL) {
        message(w, "NO FILE!", "There is no valid file to delete.");
	return;
    }	

/* check that this is correct */
    if (is_directory(file) == DIR_FILE) {
        title = (char *)malloc(strlen(file) + 21);
        strcpy(title, "DELETE DIRECTORY: '");
        strcat(title, file);
        strcat(title, "'");
        if (yesno(w, title, "Do you wish to delete this directory?")) {
    	    if (rmdir(file)) {
                message(w, "ERROR",
                	"Not deleted. Maybe there are still files in it?");
	        return;
    	    }
        }
    } else {
        title = (char *)malloc(strlen(file) + 16);
        strcpy(title, "DELETE FILE: '");
        strcat(title, file);
        strcat(title, "'");
        if (yesno(w, title, "Do you wish to delete this file?")) {
    	    if (unlink(file)) {
                message(w, "ERROR",
                	"Not deleted. Maybe you don't own it?");
	        return;
    	    }    	
        }
    }

/* no default file now! */
    free(default_file);
    free(default_file_dir);
    free(default_file_path);
    default_file = default_file_dir = default_file_path = NULL;

/* redisplay files menu */
    if (type == FILES_MENU || type == NEW_FILES_MENU) {
        update_dir_menu(type, pmenu, TRUE);
    }

    free(title);
}
/*****************************************************************************/
/* Routine to stuff the default filename into the form field

This is almost the same as recall_file() except that we are using the
'default' filename (saved by pressing RETURN on a file in the files
menus, or by using the F1-5 function keys. 

We are not using the filename saved by ^S. 

If the recalled file is from another directory than the current one,
then the full path is stuffed into the form field, else just the
filename is used. 

ARGS:
	int type - menu type
	FORM *form - form with field to be updated
	int *pmatches - updated flag for whether field matches menu item
*/

static void
stuff_file(int type, FORM *form, int *pmatches)
{

    int i;
    char *dir;
    
/* recall file from default_file */
    if (default_file) {
        form_driver(form, ' ');	/* space */
/* if we are not still in file_store_dir then use default_file_path */
        dir = getcwd(NULL, 1000);	/* this directory */
        if (default_file_dir && strcmp(default_file_dir, dir)) {
            for (i=0; default_file_path[i]; i++) form_driver(form, default_file_path[i]);
        } else {
/* insert filename into form field */
            for (i=0; default_file[i]; i++) form_driver(form, default_file[i]);
            form_driver(form, ' ');	/* space */
            form_driver(form, REQ_VALIDATION);	/* force the form field buffer to be displayed on the screen */
        }
        *pmatches = FALSE;	/* form field no longer matches menu item */
    }

}
/*****************************************************************************/
/* detailed help on a program - shellout to run 'tfm' on the program


*/
static void
show_tfm_help(int type, WINDOW *curwin, MENU *menu, FORM *form, int matches) 
{

    char *cmd;	/* used to hold help comamnds during shellout() */
    char *buffer; /* points to form field buffer */
    char *ptr;	/* temporary pointer to command string */
    int i;

/* if we have a match of form field with current menu item, then complete
what has been typed in and run 'tfm' on the buffer contents */
    if (matches) {	
/* copy menu item to form field */
        set_field_buffer(current_field(form), 0, item_name(current_item(menu))); /* copy menu item to form field */
        form_driver(form, REQ_END_FIELD);   /* cursor at end of line */
    }

/* use the form field contents with the EMBOSS program 'tfm' */
/* copy field contents to buffer */            
    buffer = field_buffer(current_field(form), 0);
    cmd = (char *)malloc(strlen(buffer)+5);
    strcpy(cmd, buffer);
/* strip off starting spaces */
    stripstartstr(cmd);
/* end at the first space - we now only have the programname with no args */
    if ((ptr = strpbrk(cmd, " \t")) != NULL) {
        *ptr = '\0';
    }
    if (*cmd) {	/* check we have some program name in cmd */
/* shift up by 4 places */
        for (i=strlen(cmd); i>=0; i--) {
            cmd[i+4] = cmd[i];
        }
/* stuff the command 'tfm ' in front */
        cmd[0] = 't';
        cmd[1] = 'f';
        cmd[2] = 'm';
        cmd[3] = ' ';
        shellout(curwin, cmd);
    }
    free(cmd);
}

/*****************************************************************************/
/* standard help text for the menus
In menu:
	GROUPS_MENU - tell them the basics
	PROGS/SEARCH/ALPHA_MENUS - tell them how to use the programs
		- but if we are on a menu item that is a program
		shellout to 'program -help'
	(NEW)_FILES_MENUS - tell them how to use the files commands
*/
static void
show_help(int type, WINDOW *curwin, MENU *menu, FORM *form, int matches) 
{
	
    WINDOW *w;
    char *cmd;	/* used to hold help comamnds during shellout() */
    char *buffer; /* points to form field buffer */
    char *ptr;	/* temporary pointer to command string */

/*
If we have no match between the command field and the menu, 
run 'command-field -help'
(expert user typed in the name of the program and wants to check on the args)
*/
    if (!matches) {
/* copy field contents to buffer */            
        buffer = field_buffer(current_field(form), 0);
        cmd = (char *)malloc(strlen(buffer)+12);
        strcpy(cmd, buffer);
/* strip off starting spaces */
        stripstartstr(cmd);
/* end at the first space - we now only have the programname with no args */
        if ((ptr = strpbrk(cmd, " \t")) != NULL) {
            *ptr = '\0';
	}
	if (*cmd) {	/* check we have some program name in cmd */
            strcat(cmd, " -help|more");
	    shellout(curwin, cmd);
	}
	free(cmd);
	return;
    }



/* so we have a match between the form field and the current menu item */
/* make a blank window to write help output against */
    w = newwin(LINES, COLS, 0, 0);
    wclear(w);
    wrefresh(w);

/* write some help */
    switch (type) {
    case GROUPS_MENU:
	wmove(w, 0, COLS/2-6);
	waddstr(w, "MAIN MENU HELP\n");
    	waddstr(w, "This menu system aims to make it easy to run EMBOSS programs and manage files.\n\n");

    	waddstr(w, "You now have a menu of EMBOSS program groups. You can select any program group\n");
    	waddstr(w, "and select the programs. Move to an option and press RETURN to select it.\n");
    	waddstr(w, "You can move to menu options by typing the first few letters of the option name\n");
    	waddstr(w, "or by using the cursor (arrow) keys");
#ifdef NCURSES_MOUSE_VERSION
	waddstr(w, " or possibly by clicking with the mouse");
#endif
	waddstr(w, ".\n");
	waddstr(w, "You can also type any normal UNIX command and run it.\n\n");

	waddstr(w, "Before the program group options, are the following special options:\n");
	waddstr(w, "EXIT - exit the program\n");
	waddstr(w, "HELP - this page of help\n");
	waddstr(w, "ALL FILES - display files in this directory, view them, change directory, etc.\n");
	waddstr(w, "NEW FILES - display the files that have been created since ");
	waddstr(w, PROGRAM_NAME);
	waddstr(w, " started running\n");
	waddstr(w, "ALPHABETIC - listing of all EMBOSS programs\n");
	waddstr(w, "SEARCH - search for EMBOSS programs by name or description\n\n");
	
	waddstr(w, "The following control keys edit the command line:\n");
	waddstr(w, "^F forward, ^B back, ^A start, ^E end, ^C delete line, ^K delete to end of line\n\n");
	
	waddstr(w, "The following keys get other things:\n");
	waddstr(w, "F1,F2,F3,F5 -view, view sequence, copy and delete the last file looked at\n");
	waddstr(w, "^X exit, ^N/^T file menus, ^R recall stored file, TAB complete name of option\n");

/* display and wait for user to read help */
        wrefresh(w);
        Pause(w);

    	break;
    	
    case PROGS_MENU:
    case SEARCH_MENU:
    case ALPHA_MENU:
        if (item_index(current_item(menu)) > 2) { /* 3rd item or greater */
/*
If we have a program name as the current menu item, run 'tfm' on it
(unsure user has moved to that menu item and wants detailed help displayed)
*/
            show_tfm_help(type, w, menu, form, matches);
	} else {
/*
If we have a non-program menu item, display a page of help text
(unsure user wants to see what you can do in this menu)
*/
	    wmove(w, 0, COLS/2-8);
	    waddstr(w, "PROGRAMS MENU HELP\n");
    	    waddstr(w, "This menu aims to make it easy to run the EMBOSS programs.\n\n");
	    	
            waddstr(w, "You now have a menu of EMBOSS programs. You can select any program.\n");
            waddstr(w, "Move to an option and press RETURN to select it.\n");
            waddstr(w, "You can move to menu options by typing the first few letters of the option name\n");
            waddstr(w, "or by using the cursor (arrow) keys");
#ifdef NCURSES_MOUSE_VERSION
            waddstr(w, " or possibly by clicking with the mouse");
#endif
            waddstr(w, ".\n");
	    waddstr(w, "You can also type any normal UNIX command and run it.\n\n");

	    waddstr(w, "Before the program group options, are the following special options:\n");
	    waddstr(w, "EXIT - exit the program\n");
	    waddstr(w, "HELP - this page of help\n\n");
	
	    waddstr(w, "The following control keys edit the command line:\n");
	    waddstr(w, "^F forward, ^B back, ^A start, ^E end, ^C delete line, ^K delete to end of line\n\n");
	
	    waddstr(w, "The following keys get other things:\n");
	    waddstr(w, "F1,F2,F3,F5 -view, view sequence, copy and delete the last file looked at\n");
	    waddstr(w, "^X exit, ^N/^T file menus, ^R recall stored file, ? see detailed help on program\n");

/* display and wait for user to read help */
	    wrefresh(w);
            Pause(w);

	}
    	break;

    case FILES_MENU:
	wmove(w, 0, COLS/2-8);
	waddstr(w, "ALL FILES MENU HELP\n");
    	waddstr(w, "This menu aims to make it easy to view and manage the files output by programs.\n");
    	waddstr(w, "You now have a menu of file names in the current directory.\n");
    	waddstr(w, "You can select any file by moving to its menu option\n");
    	waddstr(w, "You can move to menu options by typing the first few letters of the file name\n");
    	waddstr(w, "or by using the cursor (arrow) keys");
#ifdef NCURSES_MOUSE_VERSION
	waddstr(w, " or possibly by clicking with the mouse");
#endif
	waddstr(w, ".\n");
	waddstr(w, "Press RETURN to view the file (or if it is a directory, to go into it).\n");
	waddstr(w, "You can also type any normal UNIX command and run it.\n\n");

	waddstr(w, "Before the file name options, are the following special options:\n");
	waddstr(w, "EXIT - exit back to the previous menu screen\n");
	waddstr(w, "HELP - this page of help\n");
	waddstr(w, "TO HOME - go back to your 'Home' directory\n");
	waddstr(w, "PARENT DIRECTORY - go to the directory above this one\n\n");
	
	waddstr(w, "The current file becomes the default file for use by:\n");
	waddstr(w, "F1 view file, F2 display as sequence, F3 copy file, F5 delete file\n\n");

	waddstr(w, "The following keys do other things:\n");
	waddstr(w, "F4 makes a new directory, F8 sort files by name, F9 sort files by time\n");
	waddstr(w, "^X exit, ^N new files menu, ^S store current file name, ^R recall stored file\n");

/* display and wait for user to read help */
        wrefresh(w);
        Pause(w);

    	break;

    case NEW_FILES_MENU:
	wmove(w, 0, COLS/2-8);
	waddstr(w, "NEW FILES MENU HELP\n");
    	waddstr(w, "This menu aims to make it easy to view and manage the files output by programs.\n");
    	waddstr(w, "It shows the files that have been created since '");
    	waddstr(w, PROGRAM_NAME);
    	waddstr(w, "' started.\n\n");
    	
    	waddstr(w, "You now have a menu of file names in the current directory.\n");
    	waddstr(w, "You can select any file by moving to its menu option\n");
    	waddstr(w, "You can move to menu options by typing the first few letters of the file name\n");
    	waddstr(w, "or by using the cursor (arrow) keys");
#ifdef NCURSES_MOUSE_VERSION
	waddstr(w, " or possibly by clicking with the mouse");
#endif
	waddstr(w, ".\n");
	waddstr(w, "Press RETURN to view the file (or if it is a directory, to go into it).\n");
	waddstr(w, "You can also type any normal UNIX command and run it.\n\n");

	waddstr(w, "Before the file name options, are the following special options:\n");
	waddstr(w, "EXIT - exit back to the previous menu screen\n");
	waddstr(w, "HELP - this page of help\n");
	waddstr(w, "TO HOME - go back to your 'Home' directory\n");
	waddstr(w, "PARENT DIRECTORY - go to the directory above this one\n\n");
	
	waddstr(w, "The current file becomes the default file for use by:\n");
	waddstr(w, "F1 view file, F2 display as sequence, F3 copy file, F5 delete file\n\n");

	waddstr(w, "The following keys do other things:\n");
	waddstr(w, "F4 makes a new directory, F8 sort files by name, F9 sort files by time\n");
	waddstr(w, "^X exit, ^T all files menu, ^S store current file name, ^R recall stored file\n");

/* display and wait for user to read help */
        wrefresh(w);
        Pause(w);

    	break;

    }

/* delete help window */
    delwin(w);
    
/* completely redraw the old window */
    touchwin(curwin);
    wrefresh(curwin);

}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/* COMBINED MENU & FORMS ROUTINES                                            */
/*****************************************************************************/

/* display title at first line of menu/form window */
static void
title_line(int type, WINDOW *w)
{
	
    char *cwd;
    char *p;

/* clear first line */
    wmove(w, 0, 0);
    wclrtoeol(w);
    
    switch (type) {
    case GROUPS_MENU:
        mvwaddstr(w, 0, COLS/2 - 5, "GROUPS MENU");	
    	break;
    	
    case FILES_MENU:
        cwd = getcwd(NULL, 1000);
/* quick hack to shorten long directory names */
	if (strlen(cwd) > COLS/2) {
	    p = cwd+strlen(cwd)-(COLS/2);
	} else {
	    p = cwd;
	}
        mvwaddstr(w, 0, COLS/2 - strlen(p)/2 - 4, "FILES: ");
        mvwaddstr(w, 0, COLS/2 - strlen(p)/2 + 3, p);
        free(cwd);
	break;

    case NEW_FILES_MENU:
        cwd = getcwd(NULL, 1000);
/* quick hack to shorten long directory names */
	if (strlen(cwd) > COLS/2) {
	    p = cwd+strlen(cwd)-(COLS/2);
	} else {
	    p = cwd;
	}
        mvwaddstr(w, 0, COLS/2 - strlen(p)/2 - 6, "NEW FILES: ");
        mvwaddstr(w, 0, COLS/2 - strlen(p)/2 + 5, p);
        free(cwd);
	break;
	
    case PROGS_MENU:
        mvwaddstr(w, 0, COLS/2 - 6, "PROGRAMS MENU");	
	break;
	
    case SEARCH_MENU:
        mvwaddstr(w, 0, COLS/2 - 5, "SEARCH MENU");	
	break;
	
    case ALPHA_MENU:
        mvwaddstr(w, 0, COLS/2 - 16, "ALPHABETIC LIST OF PROGRAMS MENU");	
	break;
	
    default:
	printf("No such menu type\n");
	exit(1);
    }

          
}
/*****************************************************************************/
/* remove trailing spaces */
static void
trimstr(char *buffer)
{
    int i;
    for (i = strlen(buffer)-1; i >= -1 && buffer[i] == ' '; i--);
    buffer[i+1] = '\0';
}
/*****************************************************************************/

/* if the form field has a special command, convert to its code,
*/

static int
command_virtualise(FORM *form)
{
    char *buffer;
    int c = 0;
    

    static const struct {
        char * pattern;
        int result;
    } lookup[] = {
        { "EXIT",			 QUIT		},
        { "HELP",			 HELP		},
        { "SEARCH FOR PROGRAMS",	 SEARCH		},
        { "ALL FILES",			 FILES		},
        { "NEW FILES",			 NEW_FILES	},
        { "", 				 BLANK		},
    };

    unsigned n;

/* copy field contents to buffer */
    buffer = (char *)malloc(strlen(field_buffer(current_field(form), 0))+1);
    strcpy(buffer, field_buffer(current_field(form), 0));

/* remove trailing spaces */
    trimstr(buffer);
	
/* match string in buffer to various command strings and convert to
control codes */

    for (n = 0; n < sizeof(lookup) / sizeof(lookup[0]); n++) {
        if (!strcasecmp(lookup[n].pattern, buffer)) {
            c = lookup[n].result;
            break;
        }
    }
    free(buffer);

/* if no match, return 0 */
    if (n >= sizeof(lookup) / sizeof(lookup[0])) c = 0;

    return c;

}
/*****************************************************************************/
/* start a submenu using this item of the current menu.
The 'type' argument is the type of new sub-menu we wish to start running.

The 'item' argument is passed over from the previous menu and may be 
used to obtain the usrptr of item name or other useful information for
populating a submenu - it may be NULL.

(Note - the switch() statement here is completely redundant - it is left
over from the debugging stage.)

*/

static void
submenu(int type, ITEM *item, WINDOW *curwin)
{

    switch (type) {
    case GROUPS_MENU:
/* I don't think we will ever get a submenu of this type, but put it
here for completeness
*/
        do_form_menu(type, item);
    	break;
    	
    case FILES_MENU:
        do_form_menu(type, item);
	break;
	
    case NEW_FILES_MENU:
        do_form_menu(type, item);
	break;
	
    case PROGS_MENU:
        do_form_menu(type, item);
	break;
	
    case SEARCH_MENU:
        do_form_menu(type, item);
	break;
	
    case ALPHA_MENU:
        do_form_menu(type, item);
	break;
	
    default:
	printf("No such menu type\n");
	exit(1);
    }

/* completely redraw the old window */
    touchwin(curwin);
    wrefresh(curwin);

}
/*****************************************************************************/

/* Whenever a command has been run when we are displaying a FILES_MENU or
NEW_FILE_MENU that could change directory or create or delete files, we
want to update the menu display to show these files.

The old menu structure is destroyed and the new menu structure holding
the new file listing is returned in its place. 

An attempt is made to preserve the current item by noting which item
number we are on in the old menu and then moving down to the n'th item
in the new menu - this is done if he argument 'preserve' is TRUE.  This
feature should not be used if the order of the files is being changed,
for example we are changing from a NEW_FILES_MENU to a FILE_MENU or if
we are using a different sort routine. 

Args:
	type - menu type
	pmenu - updated pointer to old and new menus
	preserve - TRUE if we want to try to preserve the current item number
*/


static void
update_dir_menu(int type, MENU **pmenu, int preserve) {

    WINDOW *w = menu_win(*pmenu);
    int item_number = item_index(current_item(*pmenu)); /* the current item */
    int i;

    if (type == FILES_MENU || type == NEW_FILES_MENU) {
        end_menu(*pmenu);
        title_line(type, w);
        start_menu(type, w, pmenu, NULL);

/* go down n items */
	if (preserve) {
            for (i=0; i<item_number; i++) {
                menu_driver(*pmenu, REQ_NEXT_ITEM);
            }
        }

    }
}

/*****************************************************************************/
/* RETURN has been pressed - do various things with the current menu
selection, depending on which menu we are in, eg go into a subdirectory,
display a file, go into a submenu.  Changing directory causes the menu
to be deleted and recreated - menu is therefore returned in the pointer to
MENU* */

static void
select_action(int type, MENU **pmenu)
{
    MENU *menu = *pmenu;
    WINDOW *w = menu_win(menu);
    char *home = getenv("HOME");
    char *parent = "..";
    char *dir;

    switch (type) {
        case GROUPS_MENU:
            submenu(PROGS_MENU, current_item(menu), w);
            break;
        case FILES_MENU:
        case NEW_FILES_MENU:
/* deal with special names */
            if (!strcmp(item_name(current_item(menu)), "TO HOME")) {
            	dir = home;
            } else if (!strcmp(item_name(current_item(menu)), "PARENT DIRECTORY")) {
            	dir = parent;
            } else {

/* ncurses complains that item_name() returns (const char*) if it is not
cast as (char*) */
            	dir = (char *)item_name(current_item(menu));
            }
            
/* change directory */            
            if (is_directory(dir) == DIR_FILE) {
		if (!chdir(dir)) {
/* replace menu of old directory listing with menu of new directory */
		    update_dir_menu(type, pmenu, FALSE);

		} else {
		    message(w, "WARNING", "You do not have permission to go into that directory.");
		}
            } else {
/* run the command 'more filename' */
            	dir = (char *)malloc(strlen(item_name(current_item(menu)))+6);
            	strcpy(dir, "more ");
            	strcat(dir, item_name(current_item(menu)));
		shellout(w, dir);
		free(dir);
/* now store this file as the default for viewing files with the F1 key etc. 
'dir' is not used here except to get the returned value */
		dir = get_default_file(type, pmenu);
            }
            break;
        case PROGS_MENU:
        case SEARCH_MENU:
        case ALPHA_MENU:
/* execute the menu item (EMBOSS program name) as a shell command */
/* ncurses complains that item_name() returns (const char*) if it is not
cast as (char*) */
            shellout(w, (char *)item_name(current_item(menu)));

            break;
        default:
            printf("No such menu type\n");
            exit(1);
    }
}

/*****************************************************************************/
/* Routine to check to see if the last shellout() command created any files.
This checks to see if the global variable WANTMORE is TRUE and we have files.
If so, it stuffs 'more <filenames>' into the form field 
     sets the 'matches' flag to FALSE and returns 1,
else it leaves the form field alone and returns 0

ARGS
	FORM *form - field's form
	int *pmatches - pointer to flag showing there is a match between menu
		item and form field (updated)
*/

static int
stuff_shellout_files(FORM *form, int *pmatches)
{	
    int no_of_files;
    struct file *head;
    struct file *q;
    int i;

    if (!WANTMORE) return 0;

/* have we got any newly created TEXT files? (We don't want to do 'more dir') */
    head = new_file_list(TIMES_LATEST, &no_of_files, TEXT_FILE);

/* if we have some files produced by the command
   stuff 'more filenames' into form field */
    if (no_of_files) {
	    form_driver(form, REQ_CLR_FIELD);
            form_driver(form, 'm');
            form_driver(form, 'o');
            form_driver(form, 'r');
            form_driver(form, 'e');
            form_driver(form, ' ');	/* space */
	    q = head;
	    while(q) {
                for (i=0; q->name[i]; i++) form_driver(form, q->name[i]);
                form_driver(form, ' ');	/* space */
	        q=q->next;
	    }           
	    form_driver(form, REQ_BEG_FIELD);	/* cursor to start of field to make it easy to remove this command */
            form_driver(form, REQ_VALIDATION);	/* force the form field buffer to be displayed on the screen */

/* set flag to show menu item doesn't match form field */
	*pmatches = FALSE;

/* tidy up */
	delete_file_list(head);

	return 1;
	
    } else {
    	return 0;
    }
}
/*****************************************************************************/

/* This routine takes a control character that is an unknown command for
the menu or form drivers and executes various actions that have been
assigned to the character.

Args:
	int c - command character to be interpreted
	int *pmatches - pointer to flag for menu matches form field - may be updated
	int *ptype - pointer to type of current menu - may be updated
	FORM *form - form with field 
	MENU **pmenu - pointer to menu - may be updated
        PArea *list - updated list of mouse active areas
Returns:
	0 - if command character was recognised
	1 - not recognised, (may be a menu driver character)
	QUIT - QUIT/EXIT command has been given - want to quit from command loop
*/

static int
process_unknown_command(int c, int *pmatches, int *ptype, FORM *form, MENU **pmenu, PArea *list)
{

    WINDOW *w = form_win(form);
    int cv;
    int matches = *pmatches;
    int type = *ptype;

/* deal with RETURN specially - we may need to convert the form field
command into a control code character */

    if (c == RETURN) {
/* if we have a match, complete the form field to match the current item */
	if (matches) {
	    set_field_buffer(current_field(form), 0, item_name(current_item(*pmenu))); /* copy menu item to form field */
	    form_driver(form, REQ_END_FIELD); /* cursor at end of line */
	
/* if the form field has a special command eg EXIT, HELP, FILES, SEARCH,
convert to its code, and continue processing the control code later on
*/

	    if ((cv = command_virtualise(form))) {
	        c = cv;	/* if command code is non-zero, use it */
	    } else {

/* the form field was not a special command - it was a normal menu item
so do various things with the current menu selection, depending on which
menu we are in eg. go into a subdirectory, display a file, go into a
submenu - the menu may be updated if we are in a FILES_MENU or
NEW_FILES_MENU */

	        select_action(type, pmenu);

/* if we created some new files, we want to stuff 'more filename(s)'
into the command buffer so we can easily display them */
	        if (!((type == PROGS_MENU || 
	          type == SEARCH_MENU || 
	          type == ALPHA_MENU) &&
	          stuff_shellout_files(form, pmatches))) {


/* the menu may have altered if we were in a file menu and we selected a
directory - copy menu item to form field */
                    set_field_buffer(current_field(form), 0, item_name(current_item(*pmenu))); /* copy menu item to form field */
                    form_driver(form, REQ_END_FIELD);   /* cursor at end of line */
                    *pmatches = TRUE; 	/* form field matches menu item */
	            return 0;	/* command recognised - no further action required */
	        }
	    }

        } else {

/* if no match then the user must have typed in a shell command - shell
out (but check to see if we have 'cd') */
            shellout(w, field_buffer(current_field(form), 0));

/* Executing the command may have created or deleted some files.
Replace FILES menu of old directory listing with menu of new directory */
            update_dir_menu(type, pmenu, TRUE);

/* if we created some new files, we want to stuff 'more filename(s)'
into the command buffer so we can easily display them */
	    stuff_shellout_files(form, pmatches);


/* tidy up and return */
            pos_form_cursor(form);	/* reposition cursor on form */
            wrefresh(w);
	    return 0;	/* command recognised - no further action required */
	}
    }





/* now deal with the control code characters */
    switch (c) {
    	
    case COMPLETE:
/* TAB -complete the form field to match the current item (if matches so far) */
	if (matches) {
/* copy menu item to form field */
            set_field_buffer(current_field(form), 0, item_name(current_item(*pmenu))); /* copy menu item to form field */
            form_driver(form, REQ_END_FIELD);   /* cursor at end of line */
	    wrefresh(w);
	}
	return 0;

/* now process character codes that do things */

    case QUIT:
/* EXIT - exit from loop */
        return QUIT;

    case BLANK:
/* BLANK - ignore! */
	return 0;
	
    case HELP:
/* HELP - display help page or 'prog -help' depending on menu type and continue */
	show_help(type, w, *pmenu, form, matches);
        wrefresh(w);
        return 0;

    case TFM_HELP:
/* TFM_HELP - display help page and continue */
	show_tfm_help(type, w, *pmenu, form, matches);
	wrefresh(w);
	return 0;

    case CTRL_L:
/* CTRL_L - redisplay screen */
	redrawwin(w);
	return 0;

    case STORE_FILE:
/* store file */
	store_file(type, *pmenu);
	return 0;

    case RECALL_FILE:
/* recall file */
	recall_file(type, form, pmatches);
	matches = *pmatches;
	wrefresh(w);
	return 0;

    case FILES:
/* FILES - if already in a FILES_MENU ignore this */
	if (type != FILES_MENU && type != NEW_FILES_MENU) {
	    submenu(FILES_MENU, NULL, w);
	} else if (type == NEW_FILES_MENU) {
/* change type from NEW_FILES_MENU to FILES_MENU */
	    type = *ptype = FILES_MENU;
	    display_help(type, w, list);
	    update_dir_menu(type, pmenu, FALSE);
	}
	return 0;

		
    case NEW_FILES:
/* NEW FILES - if already in a NEW_FILES_MENU ignore this */
	if (type != FILES_MENU && type != NEW_FILES_MENU) {
	    submenu(NEW_FILES_MENU, NULL, w);
	} else if (type == FILES_MENU) {
/* change type from FILES_MENU to NEW_FILES_MENU */
	    type = *ptype = NEW_FILES_MENU;
	    display_help(type, w, list);
	    update_dir_menu(type, pmenu, FALSE);
	}
	return 0;

    case SEARCH:
/* SEARCH - get search words, search and display submenu */
        submenu(SEARCH_MENU, NULL, w);
        return 0;

    case VIEW_FILE:
/* view the default file */
	view_file(type, w, pmenu);
	wrefresh(w);
	return 0;
	    	
    case VIEW_SEQ:
/* view the default file as a sequence */
	view_seq(type, w, pmenu);
	wrefresh(w);
	return 0;
	    	
    case MKDIR:
/* make a new directory */
	make_dir(type, w, pmenu);
	wrefresh(w);
	return 0;
	    	
    case COPY_FILE:
/* copy the default file */
	copy_file(type, w, pmenu);
	wrefresh(w);
	return 0;

    case DELETE_FILE:
/* delete the default file */
	delete_file(type, w, pmenu);
	wrefresh(w);
	return 0;
	    	
    case STUFF_FILE:
/* copy the default file into the form field */
	stuff_file(type, form, pmatches);
	matches = *pmatches;
	wrefresh(w);
	return 0;
	    	
    case SORTFILENAME:
/* change the way we sort files */
	SORTFILE = sortfilename;
/* if not already in a *FILES_MENU dispaly the files */
	if (type != FILES_MENU && type != NEW_FILES_MENU) {
	    submenu(FILES_MENU, NULL, w);
	} else {
/* redisplay files */
	    update_dir_menu(type, pmenu, FALSE);
	}
	return 0;
	
    case SORTFILETIME:
/* change the way we sort files */
	SORTFILE = sortfiletime;
/* if not already in a *FILES_MENU dispaly the files */
	if (type != FILES_MENU && type != NEW_FILES_MENU) {
	    submenu(FILES_MENU, NULL, w);
	} else {
/* redisplay files */
	    update_dir_menu(type, pmenu, FALSE);
	}
	return 0;

    case SORTFILENAMECASE:
/* change the way we sort files */
	SORTFILE = sortfilenamecase;
/* if not already in a *FILES_MENU dispaly the files */
	if (type != FILES_MENU && type != NEW_FILES_MENU) {
	    submenu(FILES_MENU, NULL, w);
	} else {
/* redisplay files */
	    update_dir_menu(type, pmenu, FALSE);
	}
	return 0;

    }


/* we didn't recognise this command character - maybe it is a
menu-driver character? */
    return 1;
}






/*****************************************************************************/

/* This integrates entering text on the command-line form field and
navigating around the menu.  Deals with pressing RETURN to select menu
items or run a command if the form field does not match the current item
in the menu, with exeution of control keys to perform other specialised
actions like starting searches or displaying files in a sub-menu. 

The menu argument may be changed by the action of entering
subdirectories when display a menu of files */

static void
run_form_menu(int type, FORM *form, MENU **pmenu)
{

    int finished = 0, c;
    int result;
    char *buffer;	/* used to clean up field buffer contents */
    int res;
    int chr;
    int matches;
    WINDOW *w = menu_win(*pmenu);
    int command_result;
    PArea list=NULL;	/* list of mouse active areas */
#ifdef NCURSES_MOUSE_VERSION
    int i;
#endif

/* debug */
/*FILE *F = fopen("debug.file", "w");*/

    ajDebug("running display_help()\n");
/* display help and set up the mouse active areas */
    display_help(type, w, &list);

    ajDebug("running set_field_buffer()\n");
/* initial positioning of cursor on form field - put first menu item
(usually EXIT) in the form field and move the cursor to the start of the
field so that the first keypress of a printable character will clear the
field */
    set_field_buffer(current_field(form), 0, /* copy menu item to form field */
		     item_name(current_item(*pmenu)));
    form_driver(form, REQ_BEG_FIELD);
    pos_form_cursor(form);	/* reposition cursor on form */
    wrefresh(w);
    matches = TRUE;

    while (!finished) {

/* use the input character to drive the form field */
        chr = getch();
/* debug */
/*fprintf(F, "next key chr=%d\n", chr); fflush(F);*/

#ifdef NCURSES_MOUSE_VERSION
/* if we have a mouse event, we may wish to convert it to a control code
character.  Search through the list of active areas.  If we get a hit,
change 'c' to the control code for that area, else pass 'c' through
unchanged.  */
        if (chr == KEY_MOUSE) {
            if ((i = area_search(list)) != -1) {
                chr = i;
            }
        }
#endif
	   
/* convert the input character into form driver code */
        c = form_virtualize(chr);
        res = form_driver(form, c);
        wrefresh(w);
        switch (res) {

/****************************/
/* IN-LINE EDITING COMMANDS */
/****************************/
        case E_OK:

/* move menu to the item matching the form field's text */
/* we need to force curses to update the field's internal buffer, so we
have a seemingly spurious field-validation call here which does a call to the
internal n/curses routine 'Window_To_Buffer()'
*/
	    form_driver(form, REQ_VALIDATION);

/* copy field contents to buffer */
	    buffer = (char *)malloc(strlen(field_buffer(current_field(form), 0))+1);
	    strcpy(buffer, field_buffer(current_field(form), 0));

/* remove trailing spaces */
	    trimstr(buffer);
/* change menu to point at the first item whose name matches the buffer contents */
	    result = set_menu_pattern(*pmenu, buffer);
	    free(buffer);
	    pos_form_cursor(form);	/* reposition cursor on form */
	    wrefresh(w);

/* if we have a match of buffer in the menu (null buffer matches current
menu item) */
            if (result == E_OK) {
/* set flag for have a menu command */
		matches = TRUE;            
          
            } else if (result == E_NO_MATCH) {
/* set flag for have a non-menu command */            	
		if (matches) beep();	/* beep if we start to not match anything in the menu */
		matches = FALSE;

            } else {
/* error of some sort */
            	exit(1);
            }
            break;

/****************************/
/* UNKNOWN COMMANDS         */
/****************************/
        case E_UNKNOWN_COMMAND:

/* do our special-action key-codes */
	    command_result = process_unknown_command(c, &matches, &type, form, pmenu, &list);

	    if (command_result == QUIT) {
	    	finished = TRUE;
	    } else if (command_result) {	/* command character not recognised */

/* the command code wasn't one we are using for special purposes - it is
probably a menu-driver code */

/* virtualise the original input character to get menu driver codes */
	        c = menu_virtualize(chr);

/* perform the menu code to navigate the menu */
		result = menu_driver(*pmenu, c);
	        pos_form_cursor(form);	/* reposition cursor on form */
		if (result == E_OK) {

/* copy result of menu navigation to the form field buffer (if not blank) */
		    if (*item_name(current_item(*pmenu)) != '\0') {
                        set_field_buffer(current_field(form), 0, item_name(current_item(*pmenu))); /* copy menu item to form field */
                        form_driver(form, REQ_BEG_FIELD);   /* cursor at start of line */
/* set flag for have a menu command */
			matches = TRUE;            
		    }

#ifdef NCURSES_MOUSE_VERSION
		} else if (result == E_UNKNOWN_COMMAND && chr == KEY_MOUSE) {
			
/* We can only get here if we double click with the mouse on a menu item
to run that item.  So we pretend that RETURN has been pressed and
process the character again.  */
 		    chr = RETURN;
		    command_result = process_unknown_command(chr, &matches, &type, form, pmenu, &list);
		    if (command_result == QUIT) {
	    	    finished = TRUE;
		}
#endif
		    
		} else {
/* not even a menu command! - usually quietly ignore it, beep while debugging */
/*		    beep(); */
		}
		wrefresh(w);
	    }
            break;
            
/*****************************/
/* REQUEST DENIED, ERROR etc */
/*****************************/
        default:
            beep();
            break;
        }
    }

/*debug*/
/*fclose(F);*/
}

/*****************************************************************************/

/*
Routine to set up, run and delete a combined form and menu sytem.

The 'item' argument is passed over from the previous menu and may be
used to obtain the usrptr of item name or other useful information for
populating a submenu - it may be NULL as it is only used to populate
PROGS_MENU & ALPHA_MENU. 
*/

static void
do_form_menu(int type, ITEM *item)
{
    FORM *form;  
    MENU *menu;  
    WINDOW *w;

    w = newwin(LINES, COLS, 0, 0);
    title_line(type, w);
    ajDebug("starting form\n");
    start_form(type, w, &form);
    ajDebug("starting menu\n");
    start_menu(type, w, &menu, item);

    ajDebug("about to run_form_menu()\n");
    run_form_menu(type, form, &menu);

    ajDebug("ending form\n");
    end_form(form);
    end_menu(menu);
    delwin(w);
}
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/* MAIN                                                                      */
/*****************************************************************************/
int main (int argc, char * argv[], char **env) {
	
    AjBool explode;

/* get the time so we can note files created after the menu started */
/* backdate the start time by a 10 minutes so we can capture recent files */
    start_time = time(NULL)-600;

/* read in the EMBOSS acd files to build the programs groups data structure */
    embInitP("emnu",argc,argv,"EMNU");

/* display name and version */
    printf("%s version: %s\nEMBOSS Simple Menu\n\nStarting...", 
  	PROGRAM_NAME, PROGRAM_VERSION);

/* get the args */
    WANTMORE = (int)ajAcdGetBool("morefiles");
    explode = ajAcdGetBool("explode");

/* get the groups and program information - both EMBOSS and EMBASSY */
    glist = ajListNew();
    alpha  = ajListNew();
/* don't want to put colons in group names,
**  don't want to ignore applications that don't work in GUIs 
**  (emnu is not a GUI!)
*/
    embGrpGetProgGroups (glist, alpha, env, ajTrue, ajTrue, NULL, explode, 
    	ajFalse, ajFalse);


/* we must initialize the curses data structure only once */
    initscr();

    noecho();
    scrollok(stdscr, FALSE);	/* don't scroll at last position of screen */
    idlok(stdscr, TRUE);
    keypad(stdscr, TRUE); 
    raw();	/* like cbreak(), but it passes ^Z and ^C uninterrupted */
    nonl();	/* lets us read ^M's */
    intrflush(stdscr, FALSE);	/* don't flush buffer when BREAK pressed */

    bkgdset(BLANK);
    clear();
    refresh();
   
#ifdef NCURSES_MOUSE_VERSION
    mousemask(ALL_MOUSE_EVENTS, (mmask_t *) 0);
/* change mouse interval for double-clicks from the default of 1/5
second to 1/2 second to make double-clicks easier on slow links */
    mouseinterval(500);
#endif

/* loop until we really wish to exit */
    while (1) {    
        do_form_menu(GROUPS_MENU, NULL); 
        if (yesno(stdscr, "EXIT MENU", "Do you wish to exit?"))
            break;
    }

    clear();
    refresh();
    def_prog_mode();
    endwin();

/* tidy up */
    embGrpGroupsListDel(&glist);
    embGrpGroupsListDel(&alpha);

    ajExit();
    return 0;


}
/*****************************************************************************/
