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

import java.awt.datatransfer.*;
import javax.swing.tree.*;
import java.io.*;
import java.util.*;

class FileNode extends DefaultMutableTreeNode 
                 implements Transferable, Serializable
{
    private boolean explored = false;
    private String[] childrenNames;
  
    public static DataFlavor FILENODE =
           new DataFlavor(FileNode.class, "Remote file");
    static DataFlavor flavors[] = { FILENODE, DataFlavor.stringFlavor };

    public FileNode(File file, Hashtable openNodeDir)
    { 
      if(file.isDirectory())
        openNodeDir.put(file.getAbsolutePath(),this);
      setUserObject(file); 
    }

    public boolean getAllowsChildren() { return isDirectory(); }
    public boolean isLeaf() { return !isDirectory(); }
    public File getFile() { return (File)getUserObject(); }

    public boolean isExplored() { return explored; }

    public boolean isDirectory() 
    {
      File file = getFile();
      return file.isDirectory();
    }

    public String toString() 
    {
      File file = (File)getUserObject();
      String filename = file.toString();
      int index = filename.lastIndexOf(File.separator);

      return (index != -1 && index != filename.length()-1) ? 
                          filename.substring(index+1) : 
                                            filename;
    }

    public void explore(Hashtable openNodeDir) 
    {
      if(!isDirectory())
        return;

      if(!isExplored()) {
        File file = getFile();

      File[] children;
// filter out dot files
      children = file.listFiles(new FilenameFilter() 
      {
        public boolean accept(File d, String n) 
        {
          return !n.startsWith(".");
        }
      });

// sort into alphabetic order
      java.util.Arrays.sort(children);

      for(int i=0; i < children.length; ++i) 
        add(new FileNode(children[i],openNodeDir));
      explored = true;

      childrenNames = new String[children.length];
      for(int i=0; i < children.length; ++i)
        childrenNames[i] = children[i].getName();
     }
   }

   protected int getAnIndex(String newleaf)
   {
     String newChildren[] = new String[childrenNames.length+1];


     for(int i=0; i < childrenNames.length; ++i)
       newChildren[i] = childrenNames[i];

     newChildren[childrenNames.length] = newleaf;
     java.util.Arrays.sort(newChildren);
       
     int index;
     for(index=0; index < newChildren.length; ++index)
       if(newChildren[index].equals(newleaf))
         break;

     childrenNames = new String[newChildren.length];
     childrenNames[newChildren.length-1] = new String(newleaf);
     for(int i=0; i < childrenNames.length-1; ++i)
       childrenNames[i] = newChildren[i];

     return index;
   }

  
// Transferable
    public DataFlavor[] getTransferDataFlavors()
    {
      return flavors;
    }

    public boolean isDataFlavorSupported(DataFlavor f)
    {
      if(f.equals(FILENODE) || f.equals(DataFlavor.stringFlavor))
        return true;
      return false;
    }

    public Object getTransferData(DataFlavor d)
        throws UnsupportedFlavorException, IOException
    {
      if(d.equals(FILENODE))
        return this;
      else if(d.equals(DataFlavor.stringFlavor))
        return getFile().getAbsolutePath();
      else throw new UnsupportedFlavorException(d);
    }

//Serializable
   private void writeObject(java.io.ObjectOutputStream out) throws IOException
   {
     out.defaultWriteObject();
   }

   private void readObject(java.io.ObjectInputStream in)
     throws IOException, ClassNotFoundException
   {
     in.defaultReadObject();
   }

}
