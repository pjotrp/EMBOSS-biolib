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
import javax.swing.*;
import javax.swing.event.*;
import org.emboss.jemboss.gui.sequenceChooser.*;

import java.awt.event.*;
import java.io.*;

/**
*
* Saves files 
*
*/
public class FileSaving
{

  final Cursor cbusy = new Cursor(Cursor.WAIT_CURSOR);
  final Cursor cdone = new Cursor(Cursor.DEFAULT_CURSOR);
  private String fs = new String(System.getProperty("file.separator"));

  public FileSaving(JTextPane seqText, byte[] pngContent)
  {

     String fileSelected = "";
     String cwd = "";
     JFileChooser fc = new JFileChooser();
     fc.addChoosableFileFilter(new SequenceFilter());
     int returnVal = fc.showSaveDialog(fc);

     if (returnVal == JFileChooser.APPROVE_OPTION)
     {
       File files = fc.getSelectedFile();
       cwd = (fc.getCurrentDirectory()).getAbsolutePath();
       fileSelected = files.getName();

       seqText.setCursor(cbusy);
       File f = null;
       boolean fexist = true;
       int ok = JOptionPane.OK_OPTION;
      
//     save results
       try
       {
         f = new File(cwd + fs + fileSelected);
         if(!f.exists())
           fexist = false;
         else
           ok = JOptionPane.showConfirmDialog(seqText,
                fileSelected + " already exists. Ovewrite?",
                "Overwrite file",
                JOptionPane.YES_NO_OPTION);

         if (ok == JOptionPane.YES_OPTION) 
         {
           f.createNewFile();

           if(pngContent != null)
           {
             FileOutputStream fos = new FileOutputStream(f);
             DataOutputStream ds = new DataOutputStream(fos);
             ds.write(pngContent,0,pngContent.length);
           }
           else
           {
             PrintWriter out = new PrintWriter(new FileWriter(f));
             out.print(seqText.getText());
             out.close();
           }
           if(!fexist)
             org.emboss.jemboss.Jemboss.tree.addObject(fileSelected,cwd);
         }
       }
       catch (IOException ioe)
       {
         System.out.println("Cannot write to file "
                       + cwd + fs + fileSelected);
       }
       seqText.setCursor(cdone);
     }

   }
}

