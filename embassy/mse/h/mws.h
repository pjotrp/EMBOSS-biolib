/****  MWS.H  **************************************************************
**
**  This is the header file for the routines in the MacWorkStation ToolBox.
**
****************************************************************************/
#if 0
typedef enum { false, true }                      Boolean;
typedef struct { int left, top, right, bottom; }  Rect;
typedef struct { int h, v; }                      Point;
#endif

#define Boolean int
#define true     1
#define false    0

#define EOS    '\0'
#define NULL  0L

#define MALLOC(x)          ((x *) malloc(sizeof(x)))
#define CALLOC(n,x)        ((x *) calloc((n), sizeof(x)))
#define REALLOC(p,n,x)     ((x *) realloc((x *)(p), (n)*sizeof(x)))
#define dispose(x)         {free((x));x=NULL;}

/* Message Classes */

#define AlertEvt      'A'
#define CursorEvt     'C'
#define DialogEvt     'D'
#define FileEvt       'F'
#define GraphicEvt    'G'
#define ListEvt       'L'
#define MenuEvt       'M'
#define ProcessEvt    'P'
#define TextEvt       'T'
#define WindowEvt     'W'
#define ExecEvt       'X'

/* Window Kinds */

#define GRAPHICS      'G'
#define LIST          'L'
#define TEXT          'T' 

/* Window Shapes */

#define DocumentProc   0
#define DBoxProc       1
#define PlainDBoxProc  2
#define AltDBoxProc    3
#define NoGrowDocProc  4
#define ZoomDocProc    8
#define ZoomNoGrow    12
#define RDocProc      16

/*Window Options */

#define ALL            0
#define SAVEABLE       1
#define PRINTABLE      2
#define FONTS          3
#define EDITABLE       4
#define COPYABLE       5 
#define SELECTABLE     6
#define CLOSEABLE      7
#define DISMISS        8
#define ACTIVATE       9
#define DEACTIVATE    10
#define MOVE          11
#define RESIZE        12
#define MIXED         13

/* Standard Text Styles */

#define BOLD          "B"
#define SHADOW        "S"
#define CONDENSE      "C"
#define ITALIC        "I"
#define OUTLINE       "O"
#define PLAIN         "P"
#define UNDERLINE     "U"
#define EXTEND        "E"


/* Text Modes (Graphics) */

#define ERASE         "E"
#define INVERT        "I"
#define OVERLAY       "O"

/* Graphic Operations */

#define FRAME         "F"
#define PAINT         "P"
#define ERASE         "E"
#define INVERT        "I"
#define FILL          "FI"

/* Standard Graphic Patterns */

#define white         "WHITE"
#define ltGray        "LTGRAY"
#define gray          "GRAY"
#define dkGray        "DKGRAY"
#define black         "BLACK"

/* Standard Cursors */

#define arrow         0
#define ibeam         1
#define crosshair     2
#define crossbar      3
#define wristwatch    4

/* Standard Dialog Items */

#define BUTTON        'B'
#define CHECK         'C'
#define EDIT          'E'
#define ICON          'I'
#define LINE          'L'
#define PICT          'P'
#define RADIO         'R'
#define SCROLL        'S'
#define TEXT          'T'
#define USER          'U'

/* List definitions */

#define ASCENDING    'A'
#define DESENDING    'D'


/* Misc. Definitions */

#define T             'T'
#define F             'F'

/* Parameter seperators for Transport ID=2 */
	
#define kMsgBegin     '['
#define kMsgEnd       '|'

struct SFReply {
   char fName[63];   /* File name */
   char creator[5];
   char fileType[5];
    int finderFlags;
    int fileSize;
  };
typedef struct SFReply SFReply;


/*
** Function prototypes for MWS toolbox for VMS C
*/

extern char *GetHostData(int hostID);
extern Boolean MWSAlerts(char *,char *);
extern Boolean MsgBanner(int aStringListID);
extern Boolean PICTBanner(int aPICT,char* line1, char* line2);
extern SFReply *SFGetFile(char *);
extern int MenuEvent(char *,int *,int *, int *);
extern int ModalDialog(int *);
extern int MWSInit();
extern int MWSQuit();
extern int MWSDelay();
extern int MWSGet(char *,int *,char *);
extern int MWSPut(char *,char *);
extern int MWSPutAlias(char *,int,char *);

/****************************************************************************************************************
** Alert Director Commands
*/

#define  BeepSpeaker(duration)                printf("%cA001%d;%c\n", kMsgBegin, duration, kMsgEnd)
#define  StopAlert(message)                   MWSAlerts("A002",(message))
#define  NoteAlert(message)                   MWSAlerts("A003",(message))
#define  CautionAlert(message)                MWSAlerts("A004",(message))


/****************************************************************************************************************
** Cursor Director Commands
*/

#define  AutoTrack()                          printf("%cC001%c\n", kMsgBegin, kMsgEnd)
#define  SetCursor(cursID)                    printf("%cC002%d%c\n",kMsgBegin,(cursID),kMsgEnd)
#define  ShowCursor()                         printf("%cC003%c\n", kMsgBegin, kMsgEnd)
#define  HideCursor()                         printf("%cC004%c\n", kMsgBegin, kMsgEnd)
#define  ObscureCursor()                      printf("%cC005%c\n", kMsgBegin, kMsgEnd)


/****************************************************************************************************************/
/* Dialog Director Commands
*/

#define NewDialog(Alias,m,title,shape,v,l,t,r,b)  printf("%cD001%d;%c;\"%s\";%d;%c;%d,%d,%d,%d%c\n", kMsgBegin,(Alias),(m),(title),(shape),(v),(l),(t),(r),(b),kMsgEnd) 
Boolean GetDialog(int,char);
Boolean GetRSRCDialog(int dlogAlias,char visible,int rsrcID);
Boolean SaveDialog(int);
#define RemoveDialog(Alias)            printf("%cD004%d%c\n",kMsgBegin,(Alias),kMsgEnd)
#define TossDialog(Alias)              printf("%cD005%d%c\n",kMsgBegin,(Alias),kMsgEnd)
#define SelectDialog(Alias)            printf("%cD006%d%c\n",kMsgBegin,(Alias),kMsgEnd)
#define ShowDialog(Alias)              printf("%cD007%d%c\n",kMsgBegin,(Alias),kMsgEnd)
#define HideDialog(Alias)              printf("%cD008%d%c\n",kMsgBegin,(Alias),kMsgEnd)

#define AddButton(Alias,parm)     printf("%cD009%d;B;%s%c\n",kMsgBegin,(Alias),(parm),kMsgEnd)
#define AddCheckBox(Alias,parm)   printf("%cD009%d;C;%s%c\n",kMsgBegin,(Alias),(parm),kMsgEnd)
#define AddRadio(Alias,parm)      printf("%cD009%d;R;%s%c\n",kMsgBegin,(Alias),(parm),kMsgEnd)
#define AddEditText(Alias,parm)   printf("%cD009%d;E;%s%c\n",kMsgBegin,(Alias),(parm),kMsgEnd)
#define AddStaticText(Alias,parm) printf("%cD009%d;T;%s%c\n",kMsgBegin,(Alias),(parm),kMsgEnd)
#define AddIcon(Alias,parm)       printf("%cD009%d;I;%s%c\n",kMsgBegin,(Alias),(parm),kMsgEnd)
#define AddLine(Alias,parm)       printf("%cD009%d;L;%s%c\n",kMsgBegin,(Alias),(parm),kMsgEnd)
#define AddPICT(Alias,parm)       printf("%cD009%d;P;%s%c\n",kMsgBegin,(Alias),(parm),kMsgEnd)
#define AddPopUp(Alias,parm)      printf("%cD009%d;M;%s%c\n",kMsgBegin,(Alias),(parm),kMsgEnd)
#define AddScroll(Alias,parm)     printf("%cD009%d;S;%s%c\n",kMsgBegin,(Alias),(parm),kMsgEnd)
#define AddUserItem(Alias,parm)   printf("%cD009%d;U;%s%c\n",kMsgBegin,(Alias),(parm),kMsgEnd)

char *GetIText(int,int,char *);
#define SetIText(Alias,item,text)   printf("%cD011%d;%d;\"%s\"%c\n",kMsgBegin,(Alias),(item),(text),kMsgEnd)
#define EnableDlogItem(Alias,item)  printf("%cD012%d;%d;T%c\n",kMsgBegin,(Alias),(item),kMsgEnd)
#define DisableDlogItem(Alias,item) printf("%cD012%d;%d;F%c\n",kMsgBegin,(Alias),(item),kMsgEnd)
int GetCtlValue(int,int);
#define SetCtlValue(Alias,item,value)     printf("%cD014%d;%d;%d%c\n",kMsgBegin,(Alias),(item),(value),kMsgEnd)
#define AddEntry(Alias,item,text)            printf("%cD016%d;%d;%s%c\n",kMsgBegin,(Alias),(item),(text),kMsgEnd)
#define DeleteEntry(Alias,item,entry)        printf("%cD017%d;%d;%d%c\n",kMsgBegin,(Alias),(item),(entry),kMsgEnd)
#define DeleteAllEntries(Alias,item)         printf("%cD017%d;%d;0%c\n",kMsgBegin,(Alias),(item),kMsgEnd)
#define SelectEntry(Alias,item,entry) printf("%cD018%d;%d;T;%d%c\n",kMsgBegin,(Alias),(item),(entry),kMsgEnd) 
#define DeselectEntry(Alias,item,entry) printf("%cD018%d;%d;F;%d%c\n",kMsgBegin,(Alias),(item),(entry),kMsgEnd) 
char *GetSelectedEntry(int,int,char *);
char *GetEntryName(int,int,int,char *);
int EnumEntries(int,int,int *);


/****************************************************************************************************************
** File Director Commands
*/

#if 0
#define CreateFile
#define DeleteFile
#define RenameFile
#define OpenDataFork
#define OpenResourceFork
#define CloseFork
#define EZReadFork
#define ReadFork
#define EZWriteFork
#define WriteForkCommence
#define WriteForkMessage
#define WriteForkComplete
#define GetForkSize
#define SetForkSize
#define GetForkMark
#define SetForkMark
#define SelectFile
#define SelectNewFile
#define GetFileInfo
#define SetFileInfo
#define GetVolumeInfo
#define ListCurrentDirectory
#define ReadForkCancel
#define GetFileStatus
#define FileResultReport
#define SetCurrentDirectory
#endif


/*****************************************************************************************************************
** Graphics Director Commands
*/

#define DeleteAllGraphics(Alias)               printf("%cG001%d%c\n",kMsgBegin,(Alias),kMsgEnd)
#define DeleteGraphicElement(Alias,element)    printf("%cG001%d;%d%c\n",kMsgBegin,(Alias),(element),kMsgEnd)
#define DeleteLastElement(Alias)               printf("%cG003%d%c\n",kMsgBegin,(Alias),kMsgEnd)
#define PenNormal(Alias)                    printf("%cG004%d%c\n",kMsgBegin,(Alias),kMsgEnd)
#define PenSize(Alias,w,h)                  printf("%cG005%d;%d;%d%c\n",kMsgBegin,(Alias),(w),(h),kMsgEnd)
#define PenMode(Alias,not,mode)             printf("%cG006%d;%s;%s%c\n",kMsgBegin,(Alias),(not),(mode),kMsgEnd)
#define PenPat(Alias,pattern)              printf("%cG007%d;%s%c\n",kMsgBegin,(Alias),(pattern),kMsgEnd)
#define MoveTo(Alias,h,v)              printf("%cG008%d;%d;%d%c\n",kMsgBegin,(Alias),(h),(v),kMsgEnd)
#define Move(Alias,dh,dv)         printf("%cG009%d;%d;%d%c\n",kMsgBegin,(Alias),(dh),(dv),kMsgEnd)
void Line(int alias,int dh,int dv);
#define LineTo(Alias,h,v)                 printf("%cG011%d;%d;%d%c\n",kMsgBegin,(Alias),(h),(v),kMsgEnd)
   /* PlotLine(Alias,points...); */
#define SetTextNormal(Alias)                   printf("%cG013%d%c\n",kMsgBegin,(Alias),kMsgEnd)
#define TextFont(Alias,font)                    printf("%cG014%d;%s%c\n",kMsgBegin,(Alias),(font),kMsgEnd)
#define TextSize(Alias,size)                    printf("%cG015%d;%d%c\n",kMsgBegin,(Alias),(size),kMsgEnd)
#define TextStyle(Alias,style)                  printf("%cG016%d;%s%c\n",kMsgBegin,(Alias),(style),kMsgEnd)
#define TextMode(Alias,mode)                printf("%cG017%d;%s%c\n",kMsgBegin,(Alias),(mode),kMsgEnd)
#define DrawString(Alias,text)                   printf("%cG018%d;\"%s\"%c\n",kMsgBegin,(Alias),(text),kMsgEnd)
#define FrameRect(Alias,l,t,r,b)        printf("%cG019%d;FRAME;%d,%d,%d,%d%c\n",kMsgBegin,(Alias),(l),(t),(r),(b),kMsgEnd)
#define PaintRect(Alias,l,t,r,b)        printf("%cG019%d;PAINT;%d,%d,%d,%d%c\n",kMsgBegin,(Alias),(l),(t),(r),(b),kMsgEnd)
#define EraseRect(Alias,l,t,r,b)        printf("%cG019%d;ERASE;%d,%d,%d,%d%c\n",kMsgBegin,(Alias),(l),(t),(r),(b),kMsgEnd)
#define InvertRect(Alias,l,t,r,b)        printf("%cG019%d;INVERT;%d,%d,%d,%d%c\n",kMsgBegin,(Alias),(l),(t),(r),(b),kMsgEnd)
#define FillRect(Alias,l,t,r,b,pattern)        printf("%cG019%d;FILL;%d,%d,%d,%d%c\n",kMsgBegin,(Alias),(l),(t),(r),(b),(pattern),kMsgEnd)
#define DrawRoundRectangle(Alias,op,l,t,r,b,w,h) printf("%cG019%d;%s;%d,%d,%d,%d;%d;%d%c\n",kMsgBegin,(Alias),(op),(l),(t),(r),(b),(w),(h),kMsgEnd)
#define DrawOval(Alias,op,l,t,r,b)             printf("%cG021%d;%s;%d,%d,%d,%d%c\n",kMsgBegin,(Alias),(op),(l),(t),(r),(b),kMsgEnd)
#define DrawArc(Alias,op,l,t,r,b,s,a)          printf("%cG022%d;%s;%d,%d,%d,%d;%d;%d%c\n",kMsgBegin,(Alias),(op),(l),(t),(r),(b),(s),(a),kMsgEnd)
   /* DrawPolygon(Alias,points...); */
#define DrawPICT(Alias,h,v,pictID)          printf("%cG024%d;%d,%d;%d%c\n",kMsgBegin,(Alias),(h),(v),(pictID),kMsgEnd)
#define DrawIcon(Alias,h,v,iconID)             printf("%cG025%d;%d,%d;%d%c\n",kMsgBegin,(Alias),(h),(v),(iconID),kMsgEnd)

#if 0
#define GetPictureSize(wndoAlias)
#define ReadPicture
#define WritePictureCommence
#define WritePictureData
#define WritePictureComplete
#define CancelPictureTransfer
#endif

#define SetBkgPattern(Alias,pattern)            printf("%cG032%d;%s%c\n",kMsgBegin,(Alias),(pattern),kMsgEnd)
#define SetForeColor(Alias,color)               printf("%cG033%d;%s%c\n",kMsgBegin,(Alias),(color),kMsgEnd)
#define SetBkgCol(Alias,color)                  printf("%cG034%d;%s%c\n",kMsgBegin,(Alias),(color),kMsgEnd)


/****************************************************************************************************************
** List Director Commands
*/

#define DeleteRecord(wndo,record)                printf("%cL001%d;%d%c\n",kMsgBegin,(wndo),(record),kMsgEnd)
#define DeleteAllRecords(wndo)                   printf("%cL001%d;0;%c\n",kMsgBegin,(wndo),kMsgEnd)
#define SelectRecord(wndo,record)                printf("%cL002%d;T;%d%c\n",kMsgBegin,(wndo),(record),kMsgEnd)
#define SelectAllRecords(wndo)                   printf("%cL002%d;T;0%c\n",kMsgBegin,(wndo),kMsgEnd)
#define DeselectRecord(wndo,record)              printf("%cL002%d;F;%d%c\n",kMsgBegin,(wndo),(record),kMsgEnd)
#define DeselectAllRecords(wndo)                 printf("%cL002%d;F;0%c\n",kMsgBegin,(wndo),kMsgEnd)
int GetRecordCount(int);
Boolean GetRecord(int,int *,char *);
int GetSelectedRecords(int,int *);
char *ReadRecord(int,int,char *);
char *ReadSelectedRecord(int,char *);
#define AddRecord(wndo,data)                  printf("%cL009%d;%s%c\n",kMsgBegin,(wndo),(data),kMsgEnd)
#define ChangeRecord(wndo,record,data)        printf("%cL013%d;%d;%s%c\n",kMsgBegin,(wndo),(record),(data),kMsgEnd)
#define ChangeField(wndo,record,field,data)   printf("%cL014%d;%d;%d;%s%c\n",kMsgBegin,(wndo),(record),(field),(data),kMsgEnd)
#define SetListOption(wndo,set,option)        printf("%cL015%d;%c;%d%c\n",kMsgBegin,(wndo),(set),(option),kMsgEnd)
#define SetListHeadings(wndo,titles)          printf("%cL016%d;%s%c\n",kMsgBegin,(wndo),(titles),kMsgEnd)
#define SetListFont(wndo,fontname)            printf("%cL017%d;%s%c\n",kMsgBegin,(wndo),(fontname),kMsgEnd)
#define SetListFontSize(wndo,point)           printf("%cL018%d;%d%c\n",kMsgBegin,(wndo),(point),kMsgEnd)
#define SetTabStop(wndo,tab,pixel)           printf("%cL019%d;%d;%d%c\n",kMsgBegin,(wndo),(tab),(pixel),kMsgEnd)
#define SortRecords(wndo,field,order)         printf("%cL022%d;%d,%c\n",kMsgBegin,(wndo),(field),(order),kMsgEnd)


/****************************************************************************************************************
** Menu Director Commands
*/

#define NewMBar(Alias)                          printf("%cM001%d%c\n",kMsgBegin,(Alias),kMsgEnd)
#define TossMBar(Alias)                         printf("%cM002%d%c\n",kMsgBegin,(Alias),kMsgEnd)
#define InstallMBar(Alias)                      printf("%cM003%d%c\n",kMsgBegin,(Alias),kMsgEnd)
#define AddStdMenu(mbar,items)                  printf("%cM004%d;%s%c\n",kMsgBegin,(mbar),(items),kMsgEnd)
#define AddMenu(mbar,menu,title,items)          printf("%cM004%d;%s,%d,%s%c\n",kMsgBegin,(mbar),(title),(menu),(items),kMsgEnd)
#define GetMenu(mBarAlias,rsrcID)               printf("%cM004%d;RSRC,%d%c\n",kMsgBegin,(mBarAlias),(rsrcID),kMsgEnd)
#define DeleteMenu(Alias)                       printf("%cM005%d%c\n",kMsgBegin,(Alias),kMsgEnd)
#define AppendMenu(Alias,menu,items)            printf("%cM006%d;%d;\"%s\"%c\n",kMsgBegin,(Alias),(menu),(items),kMsgEnd)
#define ChangeItem(Alias,menu,item,new)         printf("%cM007%d;%d;%d;\"%s\"%c\n",kMsgBegin,(Alias),(menu),(item),(new),kMsgEnd)
#define EnableItem(Alias,menu,item)             printf("%cM008%d;%d;%d%c\n",kMsgBegin,(Alias),(menu),(item),kMsgEnd)
#define DisableItem(Alias,menu,item)            printf("%cM009%d;%d;%d%c\n",kMsgBegin,(Alias),(menu),(item),kMsgEnd)
#define CheckItem(Alias,menu,item,checked)      printf("%cM010%d;%d;%d;%c%c\n",kMsgBegin,(Alias),(menu),(item),(checked),kMsgEnd)
#define OnlyCheckItem(Alias,menu,item)          printf("%cM011%d;%d;%d%c\n",kMsgBegin,(Alias),(menu),(item),kMsgEnd)


/****************************************************************************************************************
** Process Director Commands
*/

#define HostOnline()                            printf("%cP001%c\n",kMsgBegin,kMsgEnd)
#define HostOffline()                           printf("%cP002%c\n",kMsgBegin,kMsgEnd)
#define QuitMWS()                               printf("%cP003%c\n",kMsgBegin,kMsgEnd)

int  Synchronize(int synch);
char* GetHostData(int hostID);
char* GetMWSVersion(void);

#if 0
#define SystemVersion()
#define FreeMemory()
#endif

#define BeginWait(withWindow,gaugeMax,message)  printf("%cP009%c;%d;\"%s\"%c\n%cC0024%c\n",kMsgBegin,(withWindow),(gaugeMax),(message),kMsgEnd,kMsgBegin,kMsgEnd)
#define EndWait()                               printf("%cP010%c\n%cC001%c\n",kMsgBegin,kMsgEnd,kMsgBegin,kMsgEnd)
#define UpdateWaitMessage(message)              printf("%cP011\"%s\"%c\n",kMsgBegin,(message),kMsgEnd)
#define UpdateWaitGauge(value)                  printf("%cP012%d%c\n",kMsgBegin,(value),kMsgEnd)


/****************************************************************************************************************
** Text Director Commands
*/

#define DeleteAllText(Alias)                    printf("%cT001%d%c\n",kMsgBegin,(Alias),kMsgEnd)
Boolean TextChanged(int);
#define SetTextFont(Alias,fontname)             printf("%cT003%d;%s%c\n",kMsgBegin,(Alias),(fontname),kMsgEnd)
#define SetTextSize(Alias,fontsize)             printf("%cT004%d;%d%c\n",kMsgBegin,(Alias),(fontsize),kMsgEnd)
#define SetTextStyle(Alias,style)               printf("%cT005%d;%s%c\n",kMsgBegin,(Alias),(style),kMsgEnd)
#define SetTextJusify(Alias,justify)            printf("%cT006%d;%s%c\n",kMsgBegin,(Alias),(justify),kMsgEnd)
#define SetWordWrap(Alias,wrap)                 printf("%cT007%d;%c%c\n",kMsgBegin,(Alias),(wrap),kMsgEnd)
int GetTextSize(int);
#define EZWriteText(Alias,string)               printf("%cT010%d;%s\r%c\n",kMsgBegin,(Alias),(string),kMsgEnd)

#if 0
Boolean WriteText(int,char,char *,char *);
Boolean ReadText(int,char,char *);
#endif

/****************************************************************************************************************
** Window Director Commands
*/

#define NewWindow(Alias,k,title,shape,close,v,l,t,r,b)     printf("%cW001%d;%c;\"%s\";%d;%c;%c;%d,%d,%d,%d;%c\n",kMsgBegin,(Alias),(k),(title),(shape),(close),(v),(l),(t),(r),(b),kMsgEnd)
#define TossWindow(Alias)                      printf("%cW002%d%c\n",kMsgBegin,(Alias),kMsgEnd)
#define SelectWindow(Alias)                    printf("%cW003%d%c\n",kMsgBegin,(Alias),kMsgEnd)
#define ShowWindow(Alias)                      printf("%cW004%d%c\n",kMsgBegin,(Alias),kMsgEnd)
#define HideWindow(Alias)                      printf("%cW005%d%c\n",kMsgBegin,(Alias),kMsgEnd)

int FrontWindow(void);
char WindowKind(int wndoAlias);
char* GetWTitle(int wndoAlias, char* wndoTitle);

#define SetWindowTitle(Alias,title)            printf("%cW007%d;\"%s\"%c\n",kMsgBegin,(Alias),(title),kMsgEnd)
#define SetWindowMenuBar(Alias,mbar)           printf("%cW008%d;%d%c\n",kMsgBegin,(Alias),(mbar),kMsgEnd)
#define SetWindowCursor(Alias,cursID)          printf("%cW009%d;%d%c\n",kMsgBegin,(Alias),(cursID),kMsgEnd)
#define SetWindowOption(Alias,set,option)      printf("%cW010%d;%c;%d;%c\n",kMsgBegin,(Alias),(set),(option),kMsgEnd)
#define SetDefWindowOption(set,option)         printf("%cW011%c;%d%c\n",kMsgBegin,(set),(option),kMsgEnd)
#define PrintWindow(Alias)                     printf("%cW012%d%c\n",kMsgBegin,(Alias),kMsgEnd)
#define PrintWindowDialog(Alias)               printf("%cW013%d%c\n",kMsgBegin,(Alias),kMsgEnd)


/****************************************************************************************************************
** Exec Commands
*/

/*      LoadExec()                             subroutine */
#define UnloadExec(Alias)                      printf("%cX002%d%c\n",kMsgBegin,(Alias),kMsgEnd)
#define InvokeExec(Alias)                      printf("%cX003%d%c\n",kMsgBegin,(Alias),kMsgEnd)
#define CallExec(Alias,data)                   printf("%cX004%d;%s%c\n",kMsgBegin,(Alias),(data),kMsgEnd)

/* End of MWS.h */
