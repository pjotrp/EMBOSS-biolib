/***************************************************************
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*  @author: Copyright (C) Hugh Morgan
*
***************************************************************/

/*
 * sequenceView.java
 *
 * Created on June 25, 2002, 11:25 AM
 */

package org.emboss.grout;

import java.awt.*;
import java.awt.event.*;
import java.applet.*;
import javax.swing.*;
import java.util.*;

import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.io.FileReader;

/**
 *
 * @author  hmorgan2
 */
public class SequenceView extends JEditorPane implements 
																									ViewPositionModelListener
{
  public static final int HORIZONTAL = 0;
  public static final int VERTICAL = 1;
  
  private String sequence = new String();
  private int orientation = HORIZONTAL;
  
  public SequenceView()
  {
    setPreferredSize(new Dimension(100, 15));
		//    setBackground(Color.black);
		//    setForeground(Color.white);
    setBackground(Color.white);
    setForeground(Color.black);
  }
  
  public void setSequence(String seq)
  {
    sequence = seq;
  }
  
  public void setOrientation(int o)
  {
    orientation = o;
    if(orientation == HORIZONTAL)
    {
      setPreferredSize(new Dimension(100, 15));
      removeLineBreaks();
    } else if(orientation == VERTICAL)
    {
      setPreferredSize(new Dimension(15, 100));
      insertLineBreaks();
    }
  }
  
  private void insertLineBreaks()
  {
    String newLine = new String("\n");
/*		Pattern p = Pattern.compile("([a-zA-Z])");
                Matcher m = p.matcher(sequence); */
    char[] newSequence = new char[(sequence.length() * 2)];
    for(int i = 0; i < sequence.length(); ++i)
    {
      newSequence[2 * i] = sequence.charAt(i);
      newSequence[2 * i + 1] = newLine.charAt(0);
    }
    sequence = new String(newSequence);
  }
  
  private void removeLineBreaks()
  {
/*		Pattern p = Pattern.compile("([a-zA-Z])");
                Matcher m = p.matcher(sequence); */
    char[] newSequence = new char[sequence.length()];
    
    
  }
  
  
  public void setSequenceFile(String filename)
  {
    sequence = loadFile(filename);
    if(orientation == VERTICAL)
    {
      insertLineBreaks();
    }
    //		// // System.out.println("sequence = " + sequence);
  }
  
  private String loadFile(String filename)
  {
    StringBuffer inputReader = new StringBuffer();
    //    		String sequence = null;
    try
    {
      ClassLoader cl = this.getClass().getClassLoader();
      BufferedReader br = new BufferedReader
					(new InputStreamReader(cl.getResourceAsStream(filename)));
      String text;
      while ((text = br.readLine()) != null)
      {
        inputReader = inputReader.append(text).append("\n");
      }
      // Always close a file after opening.
      br.close();
    } catch(java.io.IOException e)
    {
      e.printStackTrace();
    }
    return inputReader.toString();
    
  }
  
  public void viewPositionModelChanged(ViewPositionModelEvent e)
  {
    if(orientation == HORIZONTAL)
    {
      int pos = (int) ((ViewPositionModel) e.getSource()).getPosX();
      setText(sequence.substring((pos - 26), (pos + 26)));
    } else if(orientation == VERTICAL)
    {
      int pos = (int) ((ViewPositionModel) e.getSource()).getPosY();
      if(Math.IEEEremainder(pos, 2) == 0)
      {
        setText(sequence.substring((pos - 22), (pos + 24)));
      } else
      {
        setText(sequence.substring((pos - 23), (pos + 23)));
      }
      
    }
	}

}
