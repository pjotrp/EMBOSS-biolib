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
*  @author: Copyright (C) Tim Carver
*
********************************************************************/

package org.emboss.jemboss.gui.filetree;

import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.tree.*;
import java.io.*;
import java.util.*;

import uk.ac.mrc.hgmp.embreo.*;
import uk.ac.mrc.hgmp.embreo.filemgr.*;


public class RemoteFileNode extends DefaultMutableTreeNode 
                    implements Transferable, Serializable
{
    private boolean explored = false;
    private boolean isDir = false;
    private EmbreoFileList parentList;
    private String[] childrenNames;
    private String fullname;
    private Vector children;
    private String rootdir;   
    private EmbreoParams mysettings;
    private EmbreoFileRoots froots;

    private String fs = new String(System.getProperty("file.separator"));

    public static DataFlavor REMOTEFILENODE = 
           new DataFlavor(RemoteFileNode.class, "Remote file");
    static DataFlavor flavors[] = { REMOTEFILENODE, DataFlavor.stringFlavor };


    public RemoteFileNode(EmbreoParams mysettings, EmbreoFileRoots froots,
                    String file, EmbreoFileList parentList, String parent)
    { 
      this.mysettings = mysettings;
      this.froots = froots;
      this.parentList = parentList;
      rootdir = froots.getCurrentRoot();

      if(file.equals(" "))
        isDir = true;

      if(parentList != null)
      {
        fullname = parent + fs + file;
        if(parentList.isDirectory(file))
          isDir = true;
      }
      else
        fullname = ".";

      setUserObject(file); 
    }
   
    public boolean getAllowsChildren() { return isDir; }
    public boolean isLeaf() { return !isDir; }
    public boolean isDirectory() { return isDir; }
//  public String getFile() { return (String)getUserObject(); }
    public String getRootDir() { return rootdir; }
    public String getFullName() { return fullname; }
    public boolean isExplored() { return explored; }
    public String getServerName() 
    { 
      String prefix = (String)froots.getRoots().get(froots.getCurrentRoot());
      return prefix + fs + fullname;
    }

    public void explore() 
    {
      if(!isDir)
        return;

      if(!explored)
      {
        try
        {
//        System.out.println(froots.getCurrentRoot() + " :: " + fullname);
          EmbreoFileList efl = new EmbreoFileList(mysettings,
                                   froots.getCurrentRoot(),fullname);
          children = efl.fileVector();
          for(int i=0;i<children.size();i++)
            add(new RemoteFileNode(mysettings,froots,(String)children.get(i),
                                   efl,fullname));
        }
        catch(EmbreoAuthException eae) {}
      }
      explored = true;
    }

// Transferable
    public DataFlavor[] getTransferDataFlavors()
    {
      return flavors;
    }
 
    public boolean isDataFlavorSupported(DataFlavor f)
    {
      if(f.equals(REMOTEFILENODE) || f.equals(DataFlavor.stringFlavor))
        return true;
      return false;
    }

    public Object getTransferData(DataFlavor d) 
        throws UnsupportedFlavorException, IOException
    {
      if(d.equals(REMOTEFILENODE))
        return this;
      else if(d.equals(DataFlavor.stringFlavor))
        return getServerName();
      else throw new UnsupportedFlavorException(d);
    } 

// Serializable    

}

