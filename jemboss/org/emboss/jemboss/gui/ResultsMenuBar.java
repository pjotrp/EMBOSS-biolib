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

import java.util.Hashtable;
import java.awt.Cursor;
import java.awt.Dimension;
import javax.swing.*;
import javax.swing.event.*;
import java.awt.event.*;
import java.io.*;

import org.emboss.jemboss.gui.sequenceChooser.*;
import org.emboss.jemboss.gui.filetree.*;
import org.emboss.jemboss.gui.AdvancedOptions;

/**
*
* Sets up a results menu bar with save and close
* 
*
*/
public class ResultsMenuBar extends JMenuBar
{

  private JMenuItem fileMenuShowres;

/**
*
* Sets up a results menu bar with save and close
* @param JFrame frame containing the results
*
*/
  public ResultsMenuBar(final JFrame frame)
  {
    add(Box.createRigidArea(new Dimension(5,24)));

    JMenu fileMenu = new JMenu("File");
    fileMenu.setMnemonic(KeyEvent.VK_F);
    fileMenuShowres = new JMenuItem("Save...");

    fileMenu.add(fileMenuShowres);

    JMenuItem resFileMenuExit = new JMenuItem("Close");
    resFileMenuExit.setAccelerator(KeyStroke.getKeyStroke(
                    KeyEvent.VK_C, ActionEvent.CTRL_MASK));
    
    resFileMenuExit.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        frame.setVisible(false);
      }
    });
    fileMenu.add(resFileMenuExit);
    add(fileMenu);

    frame.setJMenuBar(this);
  }


/**
*
* Adds action listener to save contents of a JTextPane. This
* allows editing of the area to be saved.
* @param JFrame frame containing the results
* @param JTextPane text area to add listener to
*
*/
  public ResultsMenuBar(final JFrame frame, final FileEditorDisplay fed)
  {
    this(frame);

    fileMenuShowres.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        FileSaving fsave = new FileSaving(fed, fed.getPNGContent());

        if(fsave.writeOK())
        {
          String fileSelected = fsave.getFileName();
          String pathSelected = fsave.getPath();
          try
          {
            org.emboss.jemboss.Jemboss.tree.addObject(fileSelected,pathSelected,null);
          }
          catch(NullPointerException npe){}
          DragTree ltree = org.emboss.jemboss.gui.SetUpMenuBar.getLocalDragTree();
          if(ltree!=null)
            ltree.addObject(fileSelected,pathSelected,null);
        }
      }
    });

   
    ButtonGroup group = new ButtonGroup();
    JMenu optionsMenu   = new JMenu("Options");
    JRadioButtonMenuItem optionsMenuText = new JRadioButtonMenuItem("Text");
    optionsMenu.add(optionsMenuText);
    optionsMenuText.setSelected(true);
    group.add(optionsMenuText);
    JRadioButtonMenuItem optionsMenuSeq = new JRadioButtonMenuItem("Sequence");
    optionsMenu.add(optionsMenuSeq);
    group.add(optionsMenuSeq);

    add(optionsMenu);

    optionsMenuSeq.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        if(((JRadioButtonMenuItem)e.getSource()).isSelected())
        {
          String text = fed.getText();
          fed.setText("");
          fed.setText(text,"sequence");
          fed.setCaretPosition(0);
        }
      }
    });

   optionsMenuText.addActionListener(new ActionListener()
   {
     public void actionPerformed(ActionEvent e)
     {
       if(((JRadioButtonMenuItem)e.getSource()).isSelected())
       {
         String text = fed.getText();
         fed.setText("");
         fed.setText(text,"regular");
         fed.setCaretPosition(0);
       }
     }
   });


 }


/**
*
* Adds action listener to save contents of contents of a
* tabbed pane. Allows saving to files of text and png files.
* @param JFrame frame containing the results
* @param JTabbedPane tab pane containing results
* @param Hashtable containing results
*
*/
  public ResultsMenuBar(final JFrame frame, final JTabbedPane rtb,
                        final Hashtable hashOut, final Hashtable hashIn)
  {

    this(frame);
    
    fileMenuShowres.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {

        String fileSelected = "";
        String cwd = "";

        SecurityManager sm = System.getSecurityManager();
        System.setSecurityManager(null);
        JFileChooser fc = new JFileChooser(AdvancedOptions.cwd);
        System.setSecurityManager(sm);

        fc.addChoosableFileFilter(new SequenceFilter());
        int returnVal = fc.showSaveDialog(fc);

        if (returnVal == JFileChooser.APPROVE_OPTION)
        {
          File files = fc.getSelectedFile();
          cwd = (fc.getCurrentDirectory()).getAbsolutePath();
          fileSelected = files.getName();

          frame.setCursor(new Cursor(Cursor.WAIT_CURSOR));
//        save results
          String tabTitle = rtb.getTitleAt(rtb.getSelectedIndex());

          if(hashOut.containsKey(tabTitle))
            fileSave(cwd,fileSelected,tabTitle,hashOut);
          else if(hashIn != null)
          {
            if(hashIn.containsKey(tabTitle))
              fileSave(cwd,fileSelected,tabTitle,hashIn);
          }

          frame.setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
        }
      }
    });
  }

  private void fileSave(String cwd, String fileSelected, 
                        String tabTitle, Hashtable h)
  {
    String fs = new String(System.getProperty("file.separator"));
    FileSave fsave = new FileSave(new File(cwd + fs + fileSelected));
    if(fsave.doWrite())
      fsave.fileSaving(h.get(tabTitle));
    if(!fsave.fileExists())
    {
      org.emboss.jemboss.Jemboss.tree.addObject(fileSelected,cwd,null);
      DragTree ltree = SetUpMenuBar.getLocalDragTree();
      if(ltree!=null)
        ltree.addObject(fileSelected,cwd,null);
    }
  }

  public JMenuBar getJMenuBar()
  {
    return this;
  }
  
}

