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
import org.emboss.jemboss.gui.form.TextFieldSink;
import jalview.AlignFrame;
import jalview.MailProperties;

public class LaunchJalView extends JFrame
{

  public LaunchJalView()
  {
    super("Jalview ");

    final TextFieldSink tfs = new TextFieldSink();

    Box bacross = Box.createHorizontalBox();
    final Box bdown = Box.createVerticalBox();
    bdown.add(Box.createVerticalStrut(5));

    JLabel lname = new JLabel("Multiple Sequence Filename");
    lname.setFont(org.emboss.jemboss.gui.form.SectionPanel.labfont);
    lname.setForeground(Color.black);
    bdown.add(Box.createVerticalStrut(1));
    bacross.add(Box.createHorizontalStrut(1));
    bacross.add(lname);
    bacross.add(Box.createHorizontalGlue());
    bdown.add(bacross);
 
    bdown.add(Box.createVerticalStrut(1));

    bacross = Box.createHorizontalBox();
    bacross.add(Box.createHorizontalStrut(1));
    bacross.add(tfs);
    bacross.add(Box.createHorizontalGlue());
    bdown.add(bacross);

    bdown.add(Box.createVerticalStrut(5));
    
    lname = new JLabel("File Format");
    lname.setFont(org.emboss.jemboss.gui.form.SectionPanel.labfont);
    lname.setForeground(Color.black);
    bacross = Box.createHorizontalBox();
    bacross.add(Box.createHorizontalStrut(1));
    bacross.add(lname);
    bacross.add(Box.createHorizontalGlue());
    bdown.add(bacross);

    bdown.add(Box.createVerticalStrut(1));
    String sformats[] = {"MSF", "FASTA", "CLUSTAL", "PIR"};
    final JComboBox format = new JComboBox(sformats);
    bdown.add(format);
    bdown.add(Box.createVerticalStrut(5));

    final JButton launch = new JButton("LAUNCH");
    bacross = Box.createHorizontalBox();
    bacross.add(Box.createHorizontalStrut(1));
    bacross.add(launch);
    bacross.add(Box.createHorizontalGlue());
    bdown.add(bacross);

    launch.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        new AlignFrame(null,tfs.getText(),"File",(String)format.getSelectedItem());
        setVisible(false);
      }
    });

    JPanel jp = (JPanel)getContentPane();
    jp.setLayout(new BorderLayout());  
    jp.add(bdown, BorderLayout.CENTER);

    pack();
    setLocation( (int)((getWidth())/2),
                 (int)((getHeight())/2) );

    setVisible(true);      
  }

}

