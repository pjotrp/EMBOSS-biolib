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
*  @author: Copyright (C) Tim Carver
*
***************************************************************/

package org.emboss.jemboss.editor;

import javax.swing.*;
import java.awt.Dimension;
import java.awt.Color;
import java.awt.event.*;

import org.emboss.jemboss.gui.form.TextFieldInt;
import org.emboss.jemboss.gui.form.LabelTextBox;

public class PrettyPlotJFrame extends JFrame
                              implements ActionListener
{

  private TextFieldInt textInt;
  private ColourPanel colPane;
  private GraphicSequenceCollection gsc;
  private JCheckBox prettyBox;

  public PrettyPlotJFrame(GraphicSequenceCollection gsc)
  {
    super("Parameters");

    this.gsc = gsc;
    Box bdown = Box.createVerticalBox();
    bdown.add(Box.createVerticalStrut(4));

    Box bacross = Box.createHorizontalBox();
    textInt = new TextFieldInt();
    textInt.setText(Integer.toString(gsc.getNumberSequences()));
    Dimension d = new Dimension(50, 30);
    textInt.setPreferredSize(d);
    bacross.add(textInt);
    LabelTextBox idLabel = new LabelTextBox(
                "Identity Number","");

    bacross.add(idLabel);
    bacross.add(Box.createHorizontalGlue());
    bdown.add(bacross);
 
    bacross = Box.createHorizontalBox();
    colPane = new ColourPanel("Identity Colour", Color.red);
    bacross.add(colPane);
    idLabel = new LabelTextBox(
                "Identity Colour","");
    bacross.add(idLabel);
    bacross.add(Box.createHorizontalGlue());
    bdown.add(bacross);

    bacross = Box.createHorizontalBox();
    prettyBox = new JCheckBox("Box");
    prettyBox.setSelected(true);
    bacross.add(prettyBox);
    bacross.add(Box.createHorizontalGlue());
    bdown.add(bacross);

    bacross = Box.createHorizontalBox();
    JButton draw = new JButton("Set");
    draw.addActionListener(this);
    draw.setActionCommand("SET");
    bacross.add(draw);
    bdown.add(bacross);
    
// set up a menu bar
    JMenuBar menuBar = new JMenuBar();

// File menu
    JMenu fileMenu = new JMenu("File");
    fileMenu.setMnemonic(KeyEvent.VK_F);

// exit
    fileMenu.add(new JSeparator());
    JMenuItem fileMenuExit = new JMenuItem("Close");
    fileMenuExit.setAccelerator(KeyStroke.getKeyStroke(
              KeyEvent.VK_E, ActionEvent.CTRL_MASK));

    fileMenuExit.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        setVisible(false);
      }
    });
    fileMenu.add(fileMenuExit);
    menuBar.add(fileMenu);
    setJMenuBar(menuBar);

    getContentPane().add(bdown);
    pack();
  }


  /**
  *
  * Get the users defined identity limit
  * @param	number of identical residues
  *
  */
  public int getMinimumIdentity(int nseqs)
  {
    return Integer.parseInt(textInt.getText());
  }


  /**
  *
  * Get the users defined colour to draw identical
  * residues
  * @return 	colour
  *
  */
  public Color getColour()
  {
    return colPane.getColour();
  }


  /**
  *
  * Determine if boxes are to be drawn aroun the 
  * identical matches
  * @param	draw boxes if true
  *
  */
  public boolean isPrettyBox()
  {
    return prettyBox.isSelected();
  }

 
  /**
  *
  *  @param ae          the action event generated
  *
  */
  public void actionPerformed(ActionEvent ae)
  {
    if(ae.getActionCommand().equals("SET"))
    {
      gsc.setPrettyPlot(true,this);
      gsc.setDrawBoxes(false);
      gsc.setDrawColor(false);
    }
  }

}

