/****  MWSEvtMgr.h  *********************************************************
**
**   Public interface for the MWS Event Manager.
**
******************************************************************************
*****************************************************************************/
/*-----------------------------------------------------------------------
** Function Prototypes for Event Manager
**
**   MWSSetup    - Install a procedure to be invloked at startup.  Maybe
**                 NULL but usually isn't
**   MWSRun      - Start the event dispatcher until "doneFlag" is set
**                 false by the Process Director.
**   MWSClobber  - Runs when MWS shutdowns, it will run the user specified
**                 routine last.  This procedure may be NULL and usually is.
**   MWSUpdate   - Invokes the update function bound to the window at create
**                 time.  This message comes from the Host application not MWS.
**
**   MWSWindow   - Associate procedures with window events. Does not
**                 draw the window.
**   MWSRmveWind - Removes a window handler from the Event Manager. This
**                 not related to removing a window but may be associated
**                 with it.
**
**   MWSDialog   - Associate a procedure with modeless dialog item pressed
**                 events. 
**   MWSRmveDlog - Removes a dialog handler from the Event Manager.
**
**
**   MWSMenu     - Associates a procedure with a menu.
**   MWSRmveMenu - Removes a menu handler from the Event Manager. 
*/
#include "Boolean.h"


void MWSSetup(ProcPtr pStartup);
void MWSRun(void);
void MWSClobber(ProcPtr pShutdown);
void MWSUpdate(int window);

Boolean MWSWindow( int  theWind,
                  void *data, 
               ProcPtr  pDblClk,
               ProcPtr  pUpdate,
               ProcPtr  pClose,
               ProcPtr  pActivate,
               ProcPtr  pDeactivate,
               ProcPtr  pMove,
               ProcPtr  pResize);

void MWSRmveWind(int theWind);

Boolean MWSDialog( int  theDialog,
                  void *data, 
               ProcPtr  pItemPressed);

void MWSRmveDlog(int theDialog);


Boolean MWSMenu(int theMBar,
                int theID,
            ProcPtr pSelect);

void MWSRmveMenu(int theMBAR, 
                 int theID);

