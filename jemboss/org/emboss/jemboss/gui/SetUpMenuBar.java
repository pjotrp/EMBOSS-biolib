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


import org.emboss.jemboss.*;
import uk.ac.mrc.hgmp.embreo.*;
import uk.ac.mrc.hgmp.embreo.filemgr.*;

import java.net.URL;

import java.awt.*;
import java.io.*;
import javax.swing.*;
import javax.swing.event.*;
import java.awt.event.*;

/**
*
* Sets the top menu bar for Jemboss main window
*
*/
public class SetUpMenuBar
{

// cursors to show when we're at work
  final Cursor cbusy = new Cursor(Cursor.WAIT_CURSOR);
  final Cursor cdone = new Cursor(Cursor.DEFAULT_CURSOR);
  public static JCheckBox prefjni;
  public static JCheckBox prefShadeGUI;


  public SetUpMenuBar(final EmbreoParams mysettings, final JFrame f,
                      final String envp[], final String cwd,
                      final boolean withSoap)
  {


    JMenuBar menuPanel = new JMenuBar();
    new BoxLayout(menuPanel,BoxLayout.X_AXIS);
    menuPanel.add(Box.createRigidArea(new Dimension(5,30))); 

    JMenu fileMenu = new JMenu("File");
    fileMenu.setMnemonic(KeyEvent.VK_F);
    JMenuItem fileMenuShowres = new JMenuItem("Show Saved Results");
    fileMenuShowres.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e) 
      {
	f.setCursor(cbusy);
        new ShowSavedResults(mysettings,f);
	f.setCursor(cdone);
      }
    });
    fileMenu.add(fileMenuShowres);

    JMenuItem showRemoteFile = new JMenuItem("Show Remote Files");
    showRemoteFile.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        f.setCursor(cbusy);
        try
        {
          JFrame fres = new JFrame();
          fres.getContentPane().add(new EmbreoFileTreePanel(mysettings));
          fres.setSize(400,400);
          fres.pack();
          fres.setVisible(true);
        }
        catch (Exception expf) 
        {
          f.setCursor(cdone); 
          EmbreoAuthPopup ep = new EmbreoAuthPopup(mysettings,f);
          EmbreoAuthPrompt epp = new EmbreoAuthPrompt(mysettings);
        }
        f.setCursor(cdone);
      }
    });
    fileMenu.add(showRemoteFile);
    fileMenu.addSeparator();

    JMenuItem fileMenuExit = new JMenuItem("Exit");
    fileMenuExit.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e) 
      {
        deleteTmp(new File(cwd), ".jembosstmp");
        System.exit(0);
      }
    });
    fileMenu.add(fileMenuExit);
    menuPanel.add(fileMenu);

    JMenu prefsMenu = new JMenu("Preferences");
    prefsMenu.setMnemonic(KeyEvent.VK_P);

    prefShadeGUI = new JCheckBox("Shade unused parameters");
    prefsMenu.add(prefShadeGUI);
    prefShadeGUI.setSelected(true);

    prefjni = new JCheckBox("Calculate dependencies (JNI)");
    prefsMenu.add(prefjni);
    prefjni.setSelected(true);
    prefsMenu.addSeparator();
    
    JMenuItem showEnvironment = new JMenuItem("Show Environment");
    showEnvironment.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e) 
      {
        if(withSoap)
          JOptionPane.showMessageDialog(f,
           "Public Server: " + mysettings.getPublicSoapURL() +
           "\nPublic Server Name: " + mysettings.getPublicSoapService() +
           "\nPrivate Server: " + mysettings.getPrivateSoapURL() +
           "\nPrivate Server Name: " + mysettings.getPrivateSoapService());
        else
          JOptionPane.showMessageDialog(f, 
              envp[0] + "\n" + envp[1] + "\n" +
              envp[2] + "\n" + envp[3] + "\n");
      }
    });
    prefsMenu.add(showEnvironment);
    menuPanel.add(Box.createHorizontalStrut(5));
    menuPanel.add(prefsMenu);

    JMenu helpMenu = new JMenu("Help");
    helpMenu.setMnemonic(KeyEvent.VK_H);

    JMenuItem helpMenuAboutJemboss = new JMenuItem("About Jemboss");
    helpMenuAboutJemboss.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e) 
      {
        ClassLoader cl = this.getClass().getClassLoader();
        try
        {
          URL inURL = cl.getResource("resources/readme.txt");
                                      
          JTextPane textURL = new JTextPane();

          JPanel pscroll = new JPanel(new BorderLayout());
          JScrollPane rscroll = new JScrollPane(pscroll);
          textURL.setPage(inURL);
          textURL.setEditable(false);
          pscroll.add(textURL);
          JOptionPane jop = new JOptionPane();
          rscroll.setPreferredSize(new Dimension(560,400));
          rscroll.setMinimumSize(new Dimension(560,400));
          rscroll.setMaximumSize(new Dimension(560,400));

          jop.showMessageDialog(f,rscroll,"Jemboss Help",
                              JOptionPane.PLAIN_MESSAGE);
        } 
        catch (Exception ex)
        {
          System.out.println("Didn't find resources/" +
                             "readme.txt");
        }
      }
    });
    helpMenu.add(helpMenuAboutJemboss);

    JMenuItem helpMenuAbout = new JMenuItem("Version");
    helpMenuAbout.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        JOptionPane.showMessageDialog(f,"Developers Version, written" +
                 " by Tim Carver and Peter Tribble at the HGMP-RC, UK");
      }
    });
    helpMenu.add(helpMenuAbout);

    menuPanel.add(Box.createHorizontalStrut((int)(Jemboss.jdim.getWidth()-195)));

    menuPanel.add(helpMenu);
//  menuPanel.add(Box.createHorizontalStrut(5));
    menuPanel.add(Box.createHorizontalGlue());

    f.setJMenuBar(menuPanel);

   }

/**
*
*  Delete temporary files
*
*/
  public void deleteTmp(File cwd, final String suffix) 
  {

    String tmpFiles[] = cwd.list(new FilenameFilter()
    {
      public boolean accept(File cwd, String name)
      {
        return name.endsWith(suffix);
      };
    });

    for(int h =0;h<tmpFiles.length;h++)
    {
      File tf = new File(tmpFiles[h]);
      tf.delete();
    }
  }


}

