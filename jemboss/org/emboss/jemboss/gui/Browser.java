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


import javax.swing.*;
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;
import java.net.*;
import java.io.*;
import java.util.Vector;

public class Browser extends JFrame implements HyperlinkListener, 
                                 ActionListener 
{

  private JIconButton jembossButton;
  private JButton backButton;
  private MemoryComboBox urlField;
  private JEditorPane htmlPane;
  private String initialURL;
  private Vector urlCache; 
  private Cursor cbusy = new Cursor(Cursor.WAIT_CURSOR);
  private Cursor cdone = new Cursor(Cursor.DEFAULT_CURSOR);

  public Browser(String initialURL, String name) throws IOException
  {
    this(initialURL,name,false,"");
  }

  public Browser(String initialURL, String name,  boolean ltext, 
                                 String text) throws IOException
  {

    super(name);
    this.initialURL = initialURL;

    Vector urlCache = new Vector();
    if(!ltext)
      urlCache.add(initialURL);
    else
      urlCache.add("");

    JMenuBar menuBar = new JMenuBar();

    ClassLoader cl = this.getClass().getClassLoader();
    ImageIcon jem = new ImageIcon(cl.getResource("images/Jemboss_logo_small.gif")); 
    jembossButton = new JIconButton(jem);
    jembossButton.addActionListener(this);
    JLabel urlLabel = new JLabel("URL:");
    urlField = new MemoryComboBox(urlCache);
    urlField.addActionListener(this);

    menuBar.add(jembossButton);
    menuBar.add(urlLabel);
    menuBar.add(urlField);
    setJMenuBar(menuBar);

    if(ltext)
    {
      htmlPane = new JEditorPane();
      if( (text.indexOf("<html>") > -1) ||
          (text.indexOf("<HTML>") > -1) )
        htmlPane.setContentType("text/html");
      htmlPane.setText(text);
    }
    else
    {
      try
      {
        htmlPane = new JEditorPane(initialURL);
        htmlPane.setEditable(false);
        htmlPane.addHyperlinkListener(this);
        JScrollPane scrollPane = new JScrollPane(htmlPane);
        getContentPane().add(scrollPane, BorderLayout.CENTER);
      } 
      catch(IOException ioe) 
      {
        setCursor(cdone);
        throw new IOException();
      }
    }

    Dimension screenSize = getToolkit().getScreenSize();
    int width = screenSize.width * 5 / 10;
    int height = screenSize.height * 4 / 10;
    setBounds(width/5, height/4, width, height);
    setVisible(true);
  }

  public void actionPerformed(ActionEvent event) 
  {
    String url;
    setCursor(cbusy);
    if (event.getSource() == urlField) 
      url = (String)urlField.getSelectedItem();
    else if (event.getSource() == jembossButton)
      url = "http://www.hgmp.mrc.ac.uk/Software/EMBOSS/Jemboss/";
    else
      url = initialURL;
    try
    {
      htmlPane.setPage(new URL(url));
      urlField.add(url);
    }
    catch(IOException ioe)
    {
      setCursor(cdone);
      warnUser("Can't follow link to " + url + ": " + ioe);
    }
    setCursor(cdone);
  }

  public void hyperlinkUpdate(HyperlinkEvent event) 
  {
    if (event.getEventType() == HyperlinkEvent.EventType.ACTIVATED)
    {
      setCursor(cbusy);
      try 
      {
        htmlPane.setPage(event.getURL());
        urlField.add(event.getURL().toExternalForm());
      } 
      catch(IOException ioe) 
      {
        setCursor(cdone);
        warnUser("Can't follow link to " 
                 + event.getURL().toExternalForm() + ": " + ioe);
      }
      setCursor(cdone);
    }
  }

  private void warnUser(String message)
  {
    JOptionPane.showMessageDialog(this, message, "Error", 
                                  JOptionPane.ERROR_MESSAGE);
  }


  public class JIconButton extends JButton 
  {
    public JIconButton(ImageIcon ii) 
    {
      super(ii);
      setContentAreaFilled(false);
      setBorderPainted(false);
      setFocusPainted(false);
    }
  }


}


