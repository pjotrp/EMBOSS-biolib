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
*
*  based on EmbreoViewPanel
*
*  @author: Copyright (C) Tim Carver
*
********************************************************************/

package org.emboss.jemboss.gui.filetree;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.io.File;

import org.emboss.jemboss.soap.JembossSoapException;
import org.emboss.jemboss.soap.AuthPopup;
import org.emboss.jemboss.JembossParams;

public class LocalAndRemoteFileTreeFrame extends JFrame
{

/**
*
* @param mysettings JembossParams with settings information
*/
  public LocalAndRemoteFileTreeFrame(final JembossParams mysettings) 
  {
    super();
    try
    {  
      RemoteFileTreePanel rtree = new RemoteFileTreePanel(mysettings);
      DragTree ltree = new DragTree(new File(System.getProperty("user.home")), 
                                                             this, mysettings);
      JScrollPane scrollTree = new JScrollPane(ltree);   
      Dimension d = rtree.getPreferredSize();
      scrollTree.setPreferredSize(d);
      JSplitPane treePane = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT,
                                           scrollTree,rtree);
      getContentPane().add(treePane);
      pack();
      setVisible(true);
    }
    catch(JembossSoapException jse)
    {
      new AuthPopup(mysettings,this);
    }
  }

}

