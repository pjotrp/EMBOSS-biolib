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


package org.emboss.jemboss.gui;

import java.awt.datatransfer.*;
import java.io.*;

/**
*
* Content of each row in the DragJTable
*
*/
public class SequenceData implements Transferable, Serializable
{
  public String s_name;       //seq location
  public String s_beg;        //seq start
  public String s_end;        //seq end
  public Boolean s_default;   //use as the default
  public Boolean s_remote;    //file on remote file system
  public Boolean s_listFile;  //sequence list file


  public static DataFlavor SEQUENCEDATA =
           new DataFlavor(SequenceData.class, "Sequence data");
  static DataFlavor flavors[] = { SEQUENCEDATA };

  public SequenceData()
  {
    s_name = new String();
    s_beg = new String();
    s_end = new String();
    s_default = new Boolean(false);
    s_remote = new Boolean(false);
    s_listFile = new Boolean(false);
  }

  public SequenceData(String name, String beg, String end,
                      Boolean lis, Boolean def, Boolean remote)
  {
    s_name = name;
    s_beg = beg;
    s_end = end;
    s_default = def;
    s_listFile = lis;
    s_remote = remote;
  }

  public String getSequenceName()
  {
    return s_name;
  }

// Transferable
  public DataFlavor[] getTransferDataFlavors()
  {
    return flavors;
  }

  public boolean isDataFlavorSupported(DataFlavor f)
  {
    if(f.equals(SEQUENCEDATA))
      return true;
    return false;
  }

  public Object getTransferData(DataFlavor d)
      throws UnsupportedFlavorException, IOException
  {
    if(d.equals(SEQUENCEDATA))
      return this;
    else throw new UnsupportedFlavorException(d);
  }

// Serializable
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

