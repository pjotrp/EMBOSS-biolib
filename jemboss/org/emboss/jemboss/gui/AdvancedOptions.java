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
import java.io.*;
import javax.swing.*;
import javax.swing.event.*;
import java.awt.event.*;


public class AdvancedOptions extends JPanel
{

  public static JCheckBox prefjni;
  public static JCheckBox prefShadeGUI;
  public static JComboBox jobMgr;
  private String time[] = new String[6];

  public AdvancedOptions()
  {
    super();
    time[0] = "5 s";
    time[1] = "10 s";
    time[2] = "15 s";
    time[3] = "20 s";
    time[4] = "30 s";
    time[5] = "60 s";

    Box bdown =  Box.createVerticalBox();
    Box bleft =  Box.createHorizontalBox();

//shade or remove unused parameters
    prefShadeGUI = new JCheckBox("Shade unused parameters");
    prefShadeGUI.setSelected(true);
    bleft.add(prefShadeGUI);
    bleft.add(Box.createHorizontalGlue());
    bdown.add(bleft);

//use JNI to calculate parameter dependencies
    prefjni = new JCheckBox("Calculate dependencies (JNI)");
    prefjni.setSelected(true);
    bleft =  Box.createHorizontalBox();
    bleft.add(prefjni);
    bleft.add(Box.createHorizontalGlue());
    bdown.add(bleft);

//frequency of job manager updates
    jobMgr = new JComboBox(time);
    jobMgr.setSelectedIndex(2);
    int hgt = (new Double(jobMgr.getPreferredSize().getHeight())).intValue();
    jobMgr.setPreferredSize(new Dimension(100,hgt));
    jobMgr.setMaximumSize(new Dimension(100,hgt));
    bleft =  Box.createHorizontalBox();
    bleft.add(jobMgr);
    JLabel ljobMgr = new JLabel(" Job Manager update frequency");
    ljobMgr.setForeground(Color.black);
    bleft.add(ljobMgr);
    bleft.add(Box.createHorizontalGlue());
    bdown.add(bleft);

    this.add(bdown);
  }

}


