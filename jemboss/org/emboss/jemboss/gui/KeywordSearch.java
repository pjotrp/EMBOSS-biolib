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
import javax.swing.event.HyperlinkEvent;
import javax.swing.event.HyperlinkListener;
import java.awt.*;
import java.util.Hashtable;
import java.io.IOException;
import java.net.URL;

import org.emboss.jemboss.JembossParams;
import org.emboss.jemboss.programs.*;      // running EMBOSS programs
import org.emboss.jemboss.gui.startup.*;   // finds progs, groups, docs & db's
import org.emboss.jemboss.soap.*;

public class KeywordSearch implements HyperlinkListener
{

  private JTextPane searchHTML;
  private Cursor cbusy = new Cursor(Cursor.WAIT_CURSOR);
  private Cursor cdone = new Cursor(Cursor.DEFAULT_CURSOR);
  private JFrame f;

  public KeywordSearch(JTextField search, JembossParams mysettings,
                       boolean withSoap, JFrame jemFrame, String envp[])
  {
    String searchTxt = search.getText().trim();
//  if(searchTxt.indexOf(" ") > -1)
//  {
//    searchTxt = searchTxt.substring(0,searchTxt.indexOf(" "));
//    int n = JOptionPane.showConfirmDialog(jemFrame,
//                 "Single keyword search.\n"+
//                 "Continue the search using the keyword:\n`"+
//                 searchTxt+"`", "EMBOSS Keyword Search",
//                 JOptionPane.OK_CANCEL_OPTION,
//                 JOptionPane.ERROR_MESSAGE,null);
//     if(n == JOptionPane.CANCEL_OPTION)
//       return;
//  }
                                                                                      
    jemFrame.setCursor(cbusy);
    showSearch(searchTxt,mysettings,withSoap,jemFrame, envp);
    jemFrame.setCursor(cdone);
  }

  private void showSearch(String search, JembossParams mysettings,
                          boolean withSoap, JFrame jemFrame, String envp[])
  {
    String embossBin = mysettings.getEmbossBin();
    String woss = null;
                                                                                              
    if(withSoap)
    {
      try
      {
        String embossCommand = new String("wossname -search "+search+
                                   " -html -prelink "+mysettings.getembURL()+
                                   " -postlink .html -outfile woss.out -auto");
        JembossRun thisrun = new JembossRun(embossCommand,"",
                                 new Hashtable(),mysettings);
        woss = new String((byte[])thisrun.get("woss.out"));
      }
      catch(JembossSoapException jse)
      {
        AuthPopup ap = new AuthPopup(mysettings,null);
        ap.setBottomPanel();
        ap.setSize(380,170);
        ap.pack();
        ap.setVisible(true);
        return;
      }
    }
    else
    {
      String embossCommand[] = { embossBin+"wossname",
                                 "-search", search,
                                 "-html",
                                 "-prelink", mysettings.getembURL(),
                                 "-postlink", ".html",
                                 "-outfile", "stdout",
                                 "-auto" };
 
      RunEmbossApplication2 rea = new RunEmbossApplication2(
                                  embossCommand,envp,null);
      rea.waitFor();
      woss = rea.getProcessStdout();
    }

    f = new JFrame("EMBOSS Keyword Search");
    Dimension d = f.getToolkit().getScreenSize();
    d = new Dimension((int)d.getWidth()/2,(int)d.getHeight()/2);
    f.setSize(d);

    JTabbedPane tab = new JTabbedPane();
    searchHTML = new JTextPane();
    searchHTML.addHyperlinkListener(this);
    searchHTML.setEditable(false);
    searchHTML.setContentType("text/html");
    searchHTML.setText(woss);
    searchHTML.setCaretPosition(0);

    JScrollPane jsp = new JScrollPane(searchHTML);
    jsp.setPreferredSize(d);
    tab.addTab("Search :: "+search,jsp);
    f.getContentPane().add(tab);
    new ResultsMenuBar(f,tab,null,mysettings);
    f.setVisible(true);
  }

  /**
  *
  * Method to handle hyper link events.
  * @param event        hyper link event
  *
  */
  public void hyperlinkUpdate(HyperlinkEvent event)
  {
    if(event.getEventType() == HyperlinkEvent.EventType.ACTIVATED)
    {
      f.setCursor(cbusy);
      try
      {
        new Browser(event.getURL(), event.getDescription());
//      URL url = event.getURL();
//      searchHTML.setPage(url);
      }
      catch(IOException ioe)
      {
        
      }
      f.setCursor(cdone);
    }
//  else if(event.getEventType() == HyperlinkEvent.EventType.ENTERED)
//  else if(event.getEventType() == HyperlinkEvent.EventType.EXITED)
  }
}
