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
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import javax.swing.tree.*;

import java.awt.event.*;
import java.io.*;
import java.util.*;

import uk.ac.mrc.hgmp.embreo.*;

/**
*
* Shows a list of results from the SOAP server
* and displays individual result sets
*
*/
public class ShowSavedResults
{


// cursors to show when we're at work
  final Cursor cbusy = new Cursor(Cursor.WAIT_CURSOR);
  final Cursor cdone = new Cursor(Cursor.DEFAULT_CURSOR);

  private JFrame f;
  private EmbreoParams settings;

  public ShowSavedResults(final EmbreoParams mysettings, final JFrame f) 
  {

    this.f = f;
    settings =mysettings;

    try
    {
      final EmbreoResList reslist = new EmbreoResList(settings);
      final DefaultListModel datasets = new DefaultListModel();
      final JFrame savedResFrame = new JFrame("Saved Results on Server");
      savedResFrame.setSize(470,300);
      JPanel sp = new JPanel();
      final JTextArea aboutRes = new JTextArea("Select a result set from"
                                   +"\nthose listed and details"
                                   +"\nof that analysis will be"
                                   +"\nshown here. Then you can"
                                   +"\neither delete or view those"
                                   +"\nresults using the buttons below.");

      final JScrollPane aboutScroll = new JScrollPane(aboutRes);
      JScrollPane ss = new JScrollPane(sp);
      Dimension d = new Dimension(270,300);
      ss.setPreferredSize(d);
      ss.setMinimumSize(d);
      ss.setMaximumSize(d);  
      // MenuBar with exit; reload (latter not yet implemented)
      JMenuBar resMenu = new JMenuBar();
      resMenu.setLayout(new FlowLayout(FlowLayout.LEFT,10,1));
      JMenu resFileMenu = new JMenu("File");
      resMenu.add(resFileMenu);

      ImageIcon rfii = new ImageIcon("images/Refresh_button.gif");
      JButton refresh = new JButton(rfii);
      refresh.setMargin(new Insets(0,1,0,1));
      refresh.setToolTipText("Refresh");
      resMenu.add(refresh);
      

//    JMenuItem resFileMenuUpdate = new JMenuItem("Refresh List",KeyEvent.VK_U);
//    resFileMenuUpdate.addActionListener(new ActionListener()
      refresh.addActionListener(new ActionListener()
      {
        public void actionPerformed(ActionEvent e) 
        {
          try
          {
            savedResFrame.setCursor(cbusy);
            EmbreoResList newlist = new EmbreoResList(mysettings);
            savedResFrame.setCursor(cdone);
            if (newlist.getStatus().equals("0")) 
            {
              reslist.updateRes(newlist.hash());
              datasets.removeAllElements();
              StringTokenizer tok = new StringTokenizer((String)reslist.get("list"), "\n");
              while (tok.hasMoreTokens()) 
              {
                String image = tok.nextToken();
                datasets.addElement(image);
              }
            } 
            else 
            {
              EmbreoUtils.warningPopup(savedResFrame,newlist.getStatusMsg());
            }
          } 
          catch (EmbreoAuthException eae) 
          {
            EmbreoAuthPopup ep = new EmbreoAuthPopup(mysettings,savedResFrame);
            EmbreoAuthPrompt epp = new EmbreoAuthPrompt(mysettings);
          }
        }
      });
//    resFileMenu.add(resFileMenuUpdate);
//    resFileMenu.addSeparator();

      JMenuItem resFileMenuExit = new JMenuItem("Close");
      resFileMenuExit.addActionListener(new ActionListener()
      {
        public void actionPerformed(ActionEvent e) 
        {
          savedResFrame.setVisible(false);
        }
      });
      resFileMenu.add(resFileMenuExit);
//    resMenu.add(resFileMenu);
      savedResFrame.setJMenuBar(resMenu);
        
      // this is the list of saved results
        
      StringTokenizer tokenizer = new StringTokenizer((String)reslist.get("list"), "\n");
      while (tokenizer.hasMoreTokens()) 
      {
        String image = tokenizer.nextToken();
        datasets.addElement(image);
      }

      final JList st = new JList(datasets);
      st.addListSelectionListener(new ListSelectionListener()
      {
        public void valueChanged(ListSelectionEvent e) 
        {
          if (e.getValueIsAdjusting())
            return;

          JList theList = (JList)e.getSource();
          if (theList.isSelectionEmpty()) 
          {
            System.out.println("Empty selection");
          } 
          else 
          {
            int index = theList.getSelectedIndex();
            String thisdata = datasets.elementAt(index).toString();
            reslist.setCurrent(thisdata);
	    aboutRes.setText((String)reslist.get(thisdata));
            aboutRes.setCaretPosition(0);
	  }
	}
      });

      st.addMouseListener(new MouseAdapter() 
      {
        public void mouseClicked(MouseEvent e) 
        {
          if (e.getClickCount() == 2) 
          {
            int index = st.locationToIndex(e.getPoint());
            try
            {
              savedResFrame.setCursor(cbusy);
              EmbreoResList thisres = new EmbreoResList(settings, reslist.getCurrent(),
                                                      "show_saved_results");
              new ShowResultSet(thisres.hash());
              savedResFrame.setCursor(cdone);
            } 
            catch (EmbreoAuthException eae) 
            {  
              new EmbreoAuthPopup(mysettings,f);
              EmbreoAuthPrompt pfa = new EmbreoAuthPrompt(settings);
            }
          }
        }
      });

      sp.add(st);
        
      // action buttons
      // display retrieves all the files and shows them in a window
      JPanel resButtonPanel = new JPanel();
      JButton showResButton = new JButton("Display");
      showResButton.addActionListener(new ActionListener()
      {
        public void actionPerformed(ActionEvent e) 
	{
	  if(reslist.getCurrent() != null)
          {
	    try 
	    {
	      savedResFrame.setCursor(cbusy);
	      EmbreoResList thisres = new EmbreoResList(settings, reslist.getCurrent(), 
                                                       "show_saved_results");
              new ShowResultSet(thisres.hash());
	      savedResFrame.setCursor(cdone);
	    } 
            catch (EmbreoAuthException eae)
            {
              new EmbreoAuthPopup(mysettings,f);
              EmbreoAuthPrompt pfa = new EmbreoAuthPrompt(settings);
//            settings = pfa.getSettings();
	    }
	  } 
	  else 
	  {
	    System.out.println("Nothing selected.");
	  }
        }
      });
        
      // delete removes the file on the server
      // and edits the list
      JButton delResButton = new JButton("Delete");
      delResButton.addActionListener(new ActionListener()
      {
        public void actionPerformed(ActionEvent e) 
        {
          if(reslist.getCurrent() != null) 
          {
            try        // ask the server to delete these results
	    {
	      savedResFrame.setCursor(cbusy);
	      EmbreoResList thisres = new EmbreoResList(settings, reslist.getCurrent(), 
                                                       "delete_saved_results");
	      savedResFrame.setCursor(cdone);
	       
	      JOptionPane.showMessageDialog(savedResFrame,"Result set\n"
				    +reslist.getCurrent()+
	  			    "\nhas been successfully deleted");
	      
	      // clean up the list so they can't see it any more
	        
	      reslist.setCurrent(null);
	      aboutRes.setText("");
	      int index = st.getSelectedIndex();
	      datasets.remove(index);
	      st.setSelectedIndex(-1);
	    } 
	    catch (EmbreoAuthException eae) 
	    {
              new EmbreoAuthPopup(mysettings,f);
              EmbreoAuthPrompt pfa = new EmbreoAuthPrompt(settings);
//            settings = pfa.getSettings();
	    }
	  } 
          else 
          {
            System.out.println("Nothing selected.");
	  }
	}
      });
      resButtonPanel.add(delResButton);
      resButtonPanel.add(showResButton);
      savedResFrame.getContentPane().add(ss,BorderLayout.WEST);
      savedResFrame.getContentPane().add(aboutScroll,BorderLayout.CENTER);
      savedResFrame.getContentPane().add(resButtonPanel,BorderLayout.SOUTH);
      savedResFrame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
      savedResFrame.pack();
      savedResFrame.setVisible(true);
    } 
    catch (EmbreoAuthException eae) 
    {
      new EmbreoAuthPopup(mysettings,f);
      EmbreoAuthPrompt pfa = new EmbreoAuthPrompt(settings);
    }
    catch (NullPointerException npe)
    {
      new EmbreoAuthPopup(mysettings,f);
      EmbreoAuthPrompt pfa = new EmbreoAuthPrompt(settings);
    }

  }

  public ShowSavedResults(final EmbreoParams mysettings, final EmbreoPendingResults epr) throws EmbreoAuthException
  {

    final DefaultListModel datasets = new DefaultListModel();

    //The main gui frame
    final JFrame savedResFrame;
    savedResFrame = new JFrame("Current Sessions Results");

    savedResFrame.setSize(400,300);
    JPanel sp = new JPanel();
    final JTextArea aboutRes = new JTextArea("Select a result set from"
					     +"\nthose listed and details"
					     +"\nof that analysis will be"
					     +"\nshown here. Then you can"
					     +"\neither delete or view those"
					     +"\nresults using the buttons below.");

    final JScrollPane aboutScroll = new JScrollPane(aboutRes);
    JScrollPane ss = new JScrollPane(sp);
    //
    // MenuBar with exit; update
    //
    //JMenuBar resMenu = new JMenuBar();

    JMenuBar resMenu = new JMenuBar();
    resMenu.setLayout(new FlowLayout(FlowLayout.LEFT,10,1));

    JMenu resFileMenu = new JMenu("File");
    resMenu.add(resFileMenu);

    ImageIcon rfii = new ImageIcon("images/Refresh_button.gif");
    JButton refresh = new JButton(rfii);
    refresh.setMargin(new Insets(0,1,0,1));
    refresh.setToolTipText("Refresh");
    resMenu.add(refresh);
//  JMenuItem resFileMenuUpdate = new JMenuItem("Refresh List",KeyEvent.VK_U);
//  resFileMenuUpdate.addActionListener(new ActionListener()
    refresh.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e) 
      {
	savedResFrame.setCursor(cbusy);
	epr.updateStatus();
	savedResFrame.setCursor(cdone);
	datasets.removeAllElements();
	Enumeration enum = epr.descriptionHash().keys();
	while (enum.hasMoreElements()) 
        {
	  String image = (String)enum.nextElement().toString();
	  datasets.addElement(image);
	}
      }
    });
//  resFileMenu.add(resFileMenuUpdate);
//  resFileMenu.addSeparator();

    JMenuItem resFileMenuExit = new JMenuItem("Close",KeyEvent.VK_C);
    resFileMenuExit.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e) 
      {
	savedResFrame.dispose();
      }
    });
    resFileMenu.add(resFileMenuExit);
    savedResFrame.setJMenuBar(resMenu);
    //
    // set up the results list in the gui
    //
    Enumeration enum = epr.descriptionHash().keys();
    while (enum.hasMoreElements()) 
    {
      String image = (String)enum.nextElement().toString();
      datasets.addElement(image);
    }
    final JList st = new JList(datasets);
    st.addListSelectionListener(new ListSelectionListener()
    {
      public void valueChanged(ListSelectionEvent e) 
      {
	if (e.getValueIsAdjusting())
	  return;
	
	JList theList = (JList)e.getSource();
	if (theList.isSelectionEmpty()) 
        {
	  if (mysettings.getDebug()) {
	    System.out.println("EmbreoResListView: Empty selection");
	  }
	} else {
	  int index = theList.getSelectedIndex();
	  String thisdata = datasets.elementAt(index).toString();
	  epr.setCurrent(thisdata);
	  aboutRes.setText((String)epr.descriptionHash().get(thisdata));
      	  aboutRes.setCaretPosition(0);
	  aboutRes.setEditable(false);
	}
      }
    });


    st.addMouseListener(new MouseAdapter() {
      public void mouseClicked(MouseEvent e) {
	if (e.getClickCount() == 2) 
        {
	  int index = st.locationToIndex(e.getPoint());
	  try {
	    savedResFrame.setCursor(cbusy);
	    EmbreoResList thisres = new EmbreoResList(mysettings, epr.getCurrent(), "show_saved_results");
	    savedResFrame.setCursor(cdone);
	    if (thisres.getStatus().equals("0")) 
            {
              new ShowResultSet(thisres.hash());
//	      EmbreoResView ev = new EmbreoResView(epr.getCurrent(), thisres.hash(), mysettings);
	    } else {
	      EmbreoUtils.errorPopup(savedResFrame,thisres.getStatusMsg());
	    }
	  } catch (EmbreoAuthException eae) {
	    EmbreoAuthPopup ep = new EmbreoAuthPopup(mysettings,savedResFrame);
	    EmbreoAuthPrompt epp = new EmbreoAuthPrompt(mysettings);
	  }
	}
      }
    });
    sp.add(st);
    //
    // action buttons
    //
    // display retrieves all the files and shows them in a window
    //
    JPanel resButtonPanel = new JPanel();
    JButton showResButton = new JButton("Display");
    showResButton.addActionListener(new ActionListener(){
      public void actionPerformed(ActionEvent e) {
	if(epr.getCurrent() != null) {
	  try {
	    savedResFrame.setCursor(cbusy);
	    EmbreoResList thisres = new EmbreoResList(mysettings, epr.getCurrent(), "show_saved_results");
	    savedResFrame.setCursor(cdone);
	    if (thisres.getStatus().equals("0")) {
              new ShowResultSet(thisres.hash());
//	      EmbreoResView ev = new EmbreoResView(epr.getCurrent(), thisres.hash(), mysettings);
	    } else {
	      EmbreoUtils.errorPopup(savedResFrame,thisres.getStatusMsg());
	    }
	  } catch (EmbreoAuthException eae) {
            savedResFrame.setCursor(cdone);
	    EmbreoAuthPopup ep = new EmbreoAuthPopup(mysettings,savedResFrame);
	    EmbreoAuthPrompt epp = new EmbreoAuthPrompt(mysettings);
	  }
	}
      }
    });
    //
    // delete removes the file on the server
    // and edits the list
    //
    JButton delResButton = new JButton("Delete");
    delResButton.addActionListener(new ActionListener(){
      public void actionPerformed(ActionEvent e) {
	if(epr.getCurrent() != null) {
	  try {
	    savedResFrame.setCursor(cbusy);
	    //
	    // ask the server to delete these results
	    //
	    EmbreoResList thisres = new EmbreoResList(mysettings, epr.getCurrent(), "delete_saved_results");
	    savedResFrame.setCursor(cdone);
	    //
	    // be nice to the user and tell them we've wasted their results
	    //
	    JOptionPane.showMessageDialog(savedResFrame,"Result set\n"
                         +epr.getCurrent()+
			 "\nhas been successfully deleted");
	    epr.setCurrent(null);
	    //
	    // clean up the list so they can't see it any more
	    //
	    aboutRes.setText("");
	    int index = st.getSelectedIndex();
	    datasets.remove(index);
	    st.setSelectedIndex(-1);
	  } catch (EmbreoAuthException eae) {
	    // this shouldn't happen
	    EmbreoAuthPopup ep = new EmbreoAuthPopup(mysettings,savedResFrame);
	    EmbreoAuthPrompt epp = new EmbreoAuthPrompt(mysettings);
	  }
	}
      }
    });
    resButtonPanel.add(delResButton);
    resButtonPanel.add(showResButton);
    savedResFrame.getContentPane().add(ss,BorderLayout.WEST);
    savedResFrame.getContentPane().add(aboutScroll,BorderLayout.CENTER);
    savedResFrame.getContentPane().add(resButtonPanel,BorderLayout.SOUTH);
    savedResFrame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
    savedResFrame.pack();
    savedResFrame.setVisible(true);
  }

}

