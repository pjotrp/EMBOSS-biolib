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

import java.awt.*;
import javax.swing.*;
import org.apache.regexp.*;
import java.io.*;
import java.util.Hashtable;
import java.util.Enumeration;

import org.emboss.jemboss.gui.filetree.FileEditorDisplay;
import org.emboss.jemboss.JembossParams;

/**
*
* Displays JTabbedPane of the contents of the Hashtable
*
*/
public class ShowResultSet extends JFrame
{

  /**
  * 
  * @param reslist 	result list
  * @param project	the result data to display
  * @param mysettings	jemboss properties
  *
  */
  public ShowResultSet(Hashtable reslist, String project, 
                                JembossParams mysettings)
  {
    this(reslist,null,project,mysettings);
  }

  /**
  *
  * @param reslist      result list
  * @param inputFiles 	input files
  *
  */
  public ShowResultSet(Hashtable reslist, Hashtable inputFiles)
  {
    this(reslist,inputFiles,null,null);
  }

  /**
  *
  * @param reslist      result list
  * @param inputFiles   input files
  * @param mysettings   jemboss properties
  *
  */
  public ShowResultSet(Hashtable reslist, Hashtable inputFiles,
                       JembossParams mysettings)
  {
    this(reslist,inputFiles,null,mysettings);
  }

  /**
  *  
  * @param reslist	result list 
  * @param inputFiles	the input data to display
  * @param project      the result data to display
  * @param mysettings   jemboss properties
  *
  */
  public ShowResultSet(Hashtable reslist, Hashtable inputFiles, 
                       String project, JembossParams mysettings)
  {
    super("Saved Results on the Server");
    JTabbedPane rtp = new JTabbedPane();

    setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);

    ScrollPanel s1;
    JScrollPane r1;

    String stabs[] = addHashContentsToTab(reslist,rtp);
    if(inputFiles != null)
      addHashContentsToTab(inputFiles,rtp);

// now load png files into pane
    for(int i=0; i<stabs.length;i++)
    {
      ImageIcon i1 = new ImageIcon((byte [])reslist.get(stabs[i]));
      ImageIconJPanel iiPanel = new ImageIconJPanel(i1);
      r1 = new JScrollPane(iiPanel);
      r1.getViewport().setBackground(Color.white);
      if(stabs[i] != null)
      {
        rtp.add(r1,i);
        rtp.setTitleAt(i,stabs[i]);
      }
    }

    String cmd = "cmd";
    if(reslist.containsKey(cmd))
    {
      FileEditorDisplay fed = new FileEditorDisplay(cmd,
                                         reslist.get(cmd));
      fed.setCaretPosition(0);
      r1 = new JScrollPane(fed);
      rtp.add(cmd,r1);
    }

    new ResultsMenuBar(this,rtp,reslist,inputFiles,project,mysettings);

    setSize(640,480);
    getContentPane().add(rtp,BorderLayout.CENTER);
    setVisible(true);
  }

  /**
  *
  * Add the contents of a hash table to the tabbed pane
  * @param h	hash table
  * @param rtp	tabbed pane
  * @return	array of names of PNG tabs
  *
  */
  private String[] addHashContentsToTab(Hashtable h,JTabbedPane rtp)
  {

    ScrollPanel s1;
    JScrollPane r1;

    String cmd = "cmd";
    Enumeration enum = h.keys();
    String stabs[] = new String[h.size()];
    int ntabs = 0;

    while (enum.hasMoreElements())
    {
      String thiskey = (String)enum.nextElement().toString();
      if(!thiskey.equals(cmd))
      {
        if (thiskey.endsWith("png") || thiskey.endsWith("html"))
        {
          int index = findInt(thiskey);
          if(index>0)
          {
            stabs[index-1] = new String(thiskey);
            ntabs++;
          }
          else
          {
            ImageIcon i1 = new ImageIcon((byte [])h.get(thiskey));
            ImageIconJPanel iiPanel = new ImageIconJPanel(i1);
            r1 = new JScrollPane(iiPanel);
            r1.getViewport().setBackground(Color.white);
            rtp.add(thiskey,r1);
          }
        }
        else
        {
          FileEditorDisplay fed = new FileEditorDisplay(thiskey,
                                                     h.get(thiskey));
          fed.setCaretPosition(0);
          r1 = new JScrollPane(fed);
          rtp.add(thiskey,r1);
        }
      }
    }

    String pngtabs[] = new String[ntabs];
    for(int i=0;i<ntabs;i++)
      pngtabs[i] = new String(stabs[i]);
    
    return pngtabs;
  }

  /**
  *
  * Find the number in a string expression
  * @param exp	string expression
  * @return 	number in a string expression or -1 
  *		if none found
  *
  */
  private int findInt(String exp)
  {

    RECompiler rec = new RECompiler();
    try
    {
      REProgram  rep = rec.compile("^(.*?)([:digit:]+)");
      RE regexp = new RE(rep);
      if(regexp.match(exp))
      {
        int ia = (new Integer(regexp.getParen(2))).intValue();
        return ia;
      }
    }
    catch (RESyntaxException rese)
    {
      System.out.println("RESyntaxException ");
    }

    return -1;
  }

}
