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
import org.emboss.jemboss.JembossParams;

/**
*
* Jemboss web browser
*
*/
public class Browser extends JFrame implements HyperlinkListener, 
                                 ActionListener 
{

  /** URL cache combo field */
  private MemoryComboBox urlField;
  /** HTML pane   */
  private JEditorPane htmlPane;
  /** initial URL */
  private String initialURL;
  /** busy cursor */
  private Cursor cbusy = new Cursor(Cursor.WAIT_CURSOR);
  /** done cursor */
  private Cursor cdone = new Cursor(Cursor.DEFAULT_CURSOR);

  /**
  *
  * @param initialURL	initial URL
  * @param name		browser frame title
  * @param mysettings	jemboss settings
  *
  */
  public Browser(String initialURL, String name, 
                 JembossParams mysettings) throws IOException
  {
    this(initialURL,name,false,"",mysettings);
  }

  /**
  *
  * @param initialURL   initial URL
  * @param name         browser frame title
  * @param ltext	true if html as string past to web browser 
  * @param text		html as string
  * @param mysettings   jemboss settings
  *
  */
  public Browser(String initialURL, String name,  boolean ltext, 
                 String text, JembossParams mysettings) throws IOException
  {
    super(name);
    this.initialURL = initialURL;

    if(mysettings.isBrowserProxy())
    {
      System.setProperty("proxyHost",mysettings.getBrowserProxyHost());
      System.setProperty("proxyPort", Integer.toString(
                          mysettings.getBrowserProxyPort()));
    }

    if(ltext)        
    {
      htmlPane = new JEditorPane();
      if( (text.indexOf("<html>") > -1) ||
          (text.indexOf("<HTML>") > -1) )
        htmlPane.setContentType("text/html");
      htmlPane.setText(text);
      htmlPane.addHyperlinkListener(this);
      setBrowserSize();
      Vector urlCache = new Vector();
      urlCache.add(name+".html");
      setUpJMenuBar(urlCache);
      addToScrollPane();
      setVisible(true);
    }
    else
    {
      URL pageURL = new URL(initialURL);
      setURL(pageURL,initialURL);
    }
  }


  /**
  *
  * @param urlName	URL to display
  * @param initialURL   initial URL
  *
  */
  public Browser(URL urlName, String initialURL) throws IOException
  {
    super(initialURL);
    this.initialURL = initialURL;
    setURL(urlName,initialURL);
  }


  /**
  *
  * Set the URL in the browser
  * @param url		URL to display
  * @param name 	URL name
  *
  */
  public void setURL(URL url, String name)
  {
    try
    {
      htmlPane = new JEditorPane(url);
      htmlPane.addHyperlinkListener(this);

      Vector urlCache = new Vector();
      urlCache.add(url);
      setBrowserSize();
      setUpJMenuBar(urlCache);
      setTitle(name);
      addToScrollPane();
      setVisible(true);
    }
    catch(IOException ioe)
    {
      JOptionPane.showMessageDialog(null,
                              "Cannot Load URL\n"+name,
                              "Error", JOptionPane.ERROR_MESSAGE);
    }
  }


  /**
  *
  * Method to create the frames menu and tool bar.
  * @param urlCache	URL cache
  *
  */
  private void setUpJMenuBar(Vector urlCache)
  {
    JMenuBar menuBar = new JMenuBar();
    JToolBar toolBarURL  = new JToolBar();
    JToolBar toolBarIcon = new JToolBar();

    JMenu fileMenu = new JMenu("File");
    fileMenu.setMnemonic(KeyEvent.VK_F);
    menuBar.add(fileMenu);

    // back
    JMenuItem backMenu = new JMenuItem("Back");
    backMenu.setAccelerator(KeyStroke.getKeyStroke(
              KeyEvent.VK_B, ActionEvent.CTRL_MASK));
    backMenu.setActionCommand("BACK");
    backMenu.addActionListener(this);
    fileMenu.add(backMenu);

    JMenuItem fwdMenu = new JMenuItem("Forward");
    fwdMenu.setAccelerator(KeyStroke.getKeyStroke(
              KeyEvent.VK_F, ActionEvent.CTRL_MASK));
    fwdMenu.setActionCommand("FWD");
    fwdMenu.addActionListener(this);
    fileMenu.add(fwdMenu);

    // close
    fileMenu.addSeparator();
    JMenuItem closeMenu = new JMenuItem("Close");
    closeMenu.setAccelerator(KeyStroke.getKeyStroke(
              KeyEvent.VK_E, ActionEvent.CTRL_MASK));

    closeMenu.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        setVisible(false);
      }
    });
    fileMenu.add(closeMenu);

    // jemboss logo button
    ClassLoader cl = this.getClass().getClassLoader();
    ImageIcon jem = new ImageIcon(cl.getResource("images/Jemboss_logo_small.gif"));
    JIconButton jembossButton = new JIconButton(jem);
    jembossButton.addActionListener(this);
    jembossButton.setActionCommand("JEMBOSS");

    // url field
    JLabel urlLabel = new JLabel("URL:");
    urlField = new MemoryComboBox(urlCache);
    
    int urlFieldHeight = (int)urlField.getPreferredSize().getHeight();
    urlField.addActionListener(this);

    toolBarIcon.add(jembossButton);
    toolBarURL.add(urlLabel);
    toolBarURL.add(urlField);

    setJMenuBar(menuBar);
    getContentPane().add(toolBarURL, BorderLayout.NORTH);
    getContentPane().add(toolBarIcon, BorderLayout.SOUTH);

    int urlFieldWidth  = (int)toolBarURL.getPreferredSize().getWidth(); 
    Dimension d = new Dimension(urlFieldWidth,urlFieldHeight);
    urlField.setMaximumSize(d);

    int iconBarWidth  = (int)toolBarIcon.getPreferredSize().getWidth();
    int iconBarHeight = jem.getIconHeight();
    d = new Dimension(iconBarWidth,iconBarHeight);
    toolBarIcon.setPreferredSize(d);

  }


  /**
  *
  * Set the Jemboss web browser size
  *
  */
  private void setBrowserSize()
  {
    Dimension screenSize = getToolkit().getScreenSize();
    int width  = screenSize.width * 5 / 10;
    int height = screenSize.height * 6 / 10;
    setBounds(width/5, height/6, width, height);
  }


  /**
  *
  * Add the html pane to a scrollpane and set the
  * size of the html pane
  *
  */
  private void addToScrollPane()
  {
    htmlPane.setEditable(false);
    htmlPane.setCaretPosition(0);
    JScrollPane scrollPane = new JScrollPane(htmlPane);
   
    // ensures html wraps properly
    htmlPane.setPreferredSize(getPreferredSize());
    getContentPane().add(scrollPane, BorderLayout.CENTER);
  }


  /**
  *
  * Override actionPerformed
  * @param event 	action event
  *
  */
  public void actionPerformed(ActionEvent event) 
  {
    URL url = null;
    setCursor(cbusy);
    if (event.getSource() == urlField) 
    {
      Object select = urlField.getSelectedItem();
      if(select instanceof String)
      {
        try
        {
          url = new URL((String)select);
        }
        catch(MalformedURLException me){}
      }
      else
        url = (URL)select;
    }
    else if (event.getActionCommand().equals("JEMBOSS"))
    {
      try
      {
        url = new URL("http://www.rfcgr.mrc.ac.uk/Software/EMBOSS/Jemboss/");
      }
      catch(MalformedURLException me){}
    }
    else if (event.getActionCommand().equals("BACK"))
    {
      int index = urlField.getIndexOf(urlField.getSelectedItem())-1;
      if(index > -1 && index < urlField.getItemCount())
        url = urlField.getURLAt(index);
    }
    else if (event.getActionCommand().equals("FWD"))
    {
      int index = urlField.getIndexOf(urlField.getSelectedItem())+1;
      if(index > -1 && index < urlField.getItemCount())
        url = urlField.getURLAt(index);
    }

    try
    {
      htmlPane.setPage(url);
      
      if(!urlField.isItem(url))
        urlField.add(url);
      else
        urlField.setSelectedItem(url);
    }
    catch(IOException ioe)
    {
      setCursor(cdone);
      warnUser("Can't follow link to " + url );
    }
    setCursor(cdone);
  }


  /**
  *
  * Method to handle hyper link events.
  * @param event	hyper link event
  *
  */
  public void hyperlinkUpdate(HyperlinkEvent event) 
  {
    if (event.getEventType() == HyperlinkEvent.EventType.ACTIVATED)
    {
      setCursor(cbusy);
      try 
      {
        htmlPane.setPage(event.getURL());
        urlField.add(event.getURL());
//      urlField.add(event.getURL().toExternalForm());
      } 
      catch(IOException ioe) 
      {
        setCursor(cdone);
        warnUser("Can't follow link to " +  
                  event.getURL().toExternalForm() );
      }
      
      setCursor(cdone);
    }
  }


  /**
  *
  * Display a warning message
  * @param message	message to display
  *
  */
  private void warnUser(String message)
  {
    JOptionPane.showMessageDialog(this, message, "Warning", 
                                  JOptionPane.ERROR_MESSAGE);
  }


  /**
  *
  * Jemboss icon button
  *
  */
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


  public static void main(String arg[])
  {
    ClassLoader cl = ClassLoader.getSystemClassLoader();
    try
    {
      URL inURL = cl.getResource("resources/seqList.html");
      new Browser(inURL,"resources/seqList.html");
    }
    catch (MalformedURLException mex)
    {
      System.out.println("Didn't find resources/seqList.html");
    }
    catch (IOException iex)
    {
      System.out.println("Didn't find resources/seqList.html");
    }
  }


}


