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


/**
*
* JFrame to define the colour of residue if identical or 
* a positive match. 
*
*/
public class PrettyPlotJFrame extends JFrame
                              implements ActionListener
{

  /** field to define min number of identities */
  private TextFieldInt textInt;
  /** colour panel for identical matches */
  private ColourPanel idColour;
  /** colour panel for positive scoring matches */
  private ColourPanel matchColour;
  /** graphic sequence panel */
  private GraphicSequenceCollection gsc;
  /** define if the identities and matches are to be boxed */
  private JCheckBox prettyBox;


  /**
  *
  * @param gsc	graphic sequence panel
  *
  */
  public PrettyPlotJFrame(GraphicSequenceCollection gsc)
  {
    super("Colour Matches");

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
                "Identity Number",
                "Minimum number of identities in a column");

    bacross.add(idLabel);
    bacross.add(Box.createHorizontalGlue());
    bdown.add(bacross);
    bdown.add(Box.createVerticalStrut(4));

// identity colour
    bacross = Box.createHorizontalBox();
    idColour = new ColourPanel("Identity Colour", Color.red);
    bacross.add(idColour);
    idLabel = new LabelTextBox(
                "Identity Colour","");
    bacross.add(idLabel);
    bacross.add(Box.createHorizontalGlue());
    bdown.add(bacross);
    bdown.add(Box.createVerticalStrut(4));

// positive matches colour
    bacross = Box.createHorizontalBox();
    matchColour = new ColourPanel("Match Colour", Color.blue);
    bacross.add(matchColour);
    idLabel = new LabelTextBox(
                "Positive Match Colour",
                "");
    bacross.add(idLabel);
    bacross.add(Box.createHorizontalGlue());
    bdown.add(bacross);
    
// box-in the identical and similar matches
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
  public Color getIDColour()
  {
    return idColour.getColour();
  }


  /**
  *
  * Get the users defined colour to draw positive
  * match residues
  * @return     colour
  *
  */
  public Color getMatchColour()
  {
    return matchColour.getColour();
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
  *  @param ae	the action event generated
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

