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


package org.emboss.jemboss.gui.form;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;

import org.emboss.jemboss.gui.sequenceChooser.*;


public class SetInFileCard
{

  private Box boxFile;

  public SetInFileCard(final JPanel sectionPane, 
                 final int h, String db[], JRadioButton rfile[],
                 FileChooser fileChooser[], String name, final String appName,
                 InputSequenceAttributes inSeqAttr[],
                 CutNPasteTextArea cutnPaste[], boolean fopt)
  {


    boxFile = new Box(BoxLayout.Y_AXIS);
    final CardLayout fileCard = new CardLayout();
    final JPanel pfile = new JPanel(fileCard)
    {
      public Dimension getMinimumSize()
      {
       return getPreferredSize();
      }
      public Dimension getPreferredSize()
      {
       return new Dimension(500, 130);
      }
      public Dimension getMaximumSize()
      {
       return getPreferredSize();
      }
    };
    
    Font labfont = SectionPanel.labfont;
    Color labelColor = SectionPanel.labelColor;

    Box bacross;
    bacross = Box.createHorizontalBox();
    rfile[h]  = new JRadioButton ("file / database entry");
    rfile[h].setFont(labfont);
    JRadioButton rpaste = new JRadioButton ("paste");
    rpaste.setFont(labfont);
    rfile[h].addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        if(((JRadioButton)e.getSource()).isSelected())
          fileCard.show(pfile, "File");
        else
          fileCard.show(pfile, "Paste");
      }
    });
    rfile[h].setSelected(true);

    rpaste.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e) {
        if(((JRadioButton)e.getSource()).isSelected())
          fileCard.show(pfile, "Paste");
        else
          fileCard.show(pfile, "File");
      }
    });
    ButtonGroup group = new ButtonGroup();
    group.add(rfile[h]);
    group.add(rpaste);
    JLabel seqLabel = new JLabel("Enter the sequence as:"); 
    seqLabel.setForeground(labelColor);
    bacross.add(seqLabel);
    bacross.add(Box.createRigidArea(new Dimension(20,0)));
    bacross.add(Box.createHorizontalGlue());
    boxFile.add(bacross);

    bacross = Box.createHorizontalBox();
    bacross.add(rfile[h]);
    bacross.add(new JLabel(" or  "));
    bacross.add(rpaste);
    bacross.add(Box.createHorizontalGlue());
    boxFile.add(bacross);
    boxFile.add(Box.createVerticalStrut(8));

    Box bdown[] = new Box[2];
    for(int k=0; k<2; k++)
      bdown[k] =  Box.createVerticalBox();

//  final JFrame f = new JFrame(appName.toLowerCase() + " - Input Sequence");
    fileChooser[h] = new FileChooser(bdown[0],name);

    cutnPaste[h] = new CutNPasteTextArea(bdown[1],"Sequence Cut and Paste");
//  f.setBackground(Color.white);
//sequence attibute options
    inSeqAttr[h] = new InputSequenceAttributes(db,fileChooser[h]);
    JButton boption = new JButton("Input Sequence Options");
    fileChooser[h].setSize(boption.getPreferredSize());
    fileChooser[h].setForeground(labelColor);

    Box bxleft= new Box(BoxLayout.X_AXIS);
    bxleft.add(boption);
    bxleft.add(Box.createHorizontalGlue());
    bdown[0].add(bxleft);

    final JScrollPane rscroll = inSeqAttr[h].getJScrollPane();
    boption.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        JOptionPane jop = new JOptionPane();
        jop.showMessageDialog(sectionPane,rscroll,
                appName.toLowerCase() + " - Input Sequence",
                JOptionPane.PLAIN_MESSAGE);
//      f.setVisible(true);
      }
    });
    boption = new JButton("Input Sequence Options");
    bxleft= new Box(BoxLayout.X_AXIS);
    bxleft.add(boption);
    bxleft.add(Box.createHorizontalGlue());
    bdown[1].add(bxleft);

    boption.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        JOptionPane jop = new JOptionPane();
        jop.showMessageDialog(sectionPane,rscroll,
                appName.toLowerCase() + " - Input Sequence",
                JOptionPane.PLAIN_MESSAGE);
//      f.setVisible(true);
      }
    });

    pfile.add(bdown[0], "File");
    pfile.add(bdown[1], "Paste");

    bxleft= new Box(BoxLayout.X_AXIS);
    bxleft.add(pfile);
    bxleft.add(Box.createHorizontalGlue());
    boxFile.add(bxleft);

  }


  protected Box getInCard()
  {
    return boxFile;
  }

}
