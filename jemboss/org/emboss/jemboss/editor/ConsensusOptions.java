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
import java.awt.event.*;
import org.emboss.jemboss.gui.form.TextFieldFloat;
import org.emboss.jemboss.gui.form.TextFieldInt;
import org.emboss.jemboss.gui.form.LabelTextBox;

public class ConsensusOptions extends JFrame
{

  private TextFieldFloat pluralFloat;
  private TextFieldInt idInt;
  private TextFieldFloat caseFloat;

  public ConsensusOptions()
  {
    super("Consensus Options");

    Box bdown = Box.createVerticalBox();
    bdown.add(Box.createVerticalStrut(4));

    Box bacross = Box.createHorizontalBox();
    pluralFloat = new TextFieldFloat();
    pluralFloat.setText("1.0");
    Dimension d = new Dimension(50, 30);
    pluralFloat.setPreferredSize(d);
    pluralFloat.setMaximumSize(d);
    bacross.add(pluralFloat);
    LabelTextBox pluralLabel = new LabelTextBox(
                "Minimum positive match score value for there to be a consensus",
                "Set a cut-off for the number of\n"+
                "positive matches below which\n"+
                "there is no consensus.");
    bacross.add(pluralLabel);
    bacross.add(Box.createHorizontalGlue());
    bdown.add(bacross);

    bacross = Box.createHorizontalBox();
    idInt = new TextFieldInt();
    idInt.setText("1");
    idInt.setPreferredSize(d);
    idInt.setMaximumSize(d);
    bacross.add(idInt);
    LabelTextBox  caselLabel = new LabelTextBox(
                "Minimum number of identitied for there to be a consensus",
                "The required number of identities\n"+
                "at a site for it to give a consensus\n"+
                "at that position. Therefore, if this\n"+
                "is set to the number of sequences in\n"+
                "the alignment only columns of identities\n"+
                "contribute to the consensus.");
    bacross.add(caselLabel);
    bacross.add(Box.createHorizontalGlue());
    bdown.add(bacross);


    bacross = Box.createHorizontalBox();
    caseFloat = new TextFieldFloat();
    caseFloat.setText("1.0");
    caseFloat.setPreferredSize(d);
    caseFloat.setMaximumSize(d);
    bacross.add(caseFloat);
    LabelTextBox  idLabel = new LabelTextBox(
          "Threshold positive match score for setting the consensus to upper-case",
          "Sets the threshold for the positive\n"+
          "matches above which the consensus is\n"+
          "in upper-case and below which the\n"+
          "consensus is in lower-case.");
    bacross.add(idLabel);
    bacross.add(Box.createHorizontalGlue());
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
  * Get the minimum positive match score for a 
  * site to be included in the consensus.
  *
  */ 
  public float getPlurality()
  {
    return Float.parseFloat(pluralFloat.getText());
  }

  /**
  *
  * Get the minimum positive matches for a 
  * site to be made upper case in the consensus.
  *
  */
  public float getCase()
  {
    return Float.parseFloat(caseFloat.getText());
  }

  /**
  *
  * Get the minimum identical matches for a 
  * site to be included in the consensus.
  *
  */
  public int getIdentity()
  {
    return Integer.parseInt(idInt.getText());
  }

}

