/********************************************************************
*
*  This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Library General Public
*  License as published by the Free Software Foundation; either
*  version 2 of the License, or (at your option) any later version.
*
*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Library General Public License for more details.
*
*  You should have received a copy of the GNU Library General Public
*  License along with this library; if not, write to the
*  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
*  Boston, MA  02111-1307, USA.
********************************************************************/

package org.emboss.jemboss.soap;

import uk.ac.mrc.hgmp.embreo.*;
import org.emboss.jemboss.gui.*;

import javax.swing.*;

public class PendingResults extends EmbreoPendingResults
{

  EmbreoParams mysettings;

  public PendingResults(EmbreoParams mysettings)
  {
    super(mysettings);
    this.mysettings = mysettings;
  }

  public void showPendingResults(JFrame f) 
  {
    if ((numCompleted() == 0) && (numRunning() == 0)) 
    {
      JOptionPane.showMessageDialog(f,"You can only view pending results\n"
				    + "if any background jobs have been\n"
				    + "submitted in the current session.");
    } 
    else 
    {
      try
      {
//	ResListView epv = new ResListView(mysettings, this);
        new ShowSavedResults(mysettings, this);
      } 
      catch (EmbreoAuthException eae) 
      {
	EmbreoAuthPopup ep = new EmbreoAuthPopup(mysettings,f);
	EmbreoAuthPrompt epp = new EmbreoAuthPrompt(mysettings);
      }
    }
  } 

}

