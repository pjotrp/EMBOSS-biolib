/**
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*
*  @author: Copyright (C) Tim Carver
*
*  Based on Embreo's splash screen
*/

package org.emboss.jemboss.gui.startup;

import org.emboss.jemboss.*;
import uk.ac.mrc.hgmp.embreo.*;
import javax.swing.*;
import java.awt.event.*;
import java.awt.*;

public class Splash 
{

  private int iprogress = 0;
  private int iprogressmax;
  private EmbreoParams mysettings;

  private boolean exitOnDone = false;
  private boolean allDone = false;

  JProgressBar progressBar;
  JLabel progressLabel;
  final JFrame splashf;

/**
* Provide a startup monitor
*
* @param mysettings Configuration parameters
* @param iprogressmax The number of startup calls, to determine the
*                     scaling of the progress bar. The doneSomething
*                     method will move the bar, so this should be
*                     set to the expected number of doneSomething calls.
*/
  public Splash(final EmbreoParams mysettings, int iprogressmax)
  {

    this.mysettings = mysettings;
    this.iprogressmax = iprogressmax;

    String appName = mysettings.getAppName();

    splashf = new JFrame("");
    JPanel splashp = new JPanel(new BorderLayout());

    JPanel logoPanel = new JPanel(new BorderLayout());
    JembossLogo jlo = new JembossLogo(20,38,35);
    Dimension d = new Dimension(380,52);
    jlo.setPreferredSize(d); 
    jlo.setMinimumSize(d);
    jlo.setMaximumSize(d);

    logoPanel.add(jlo);
    splashp.add(logoPanel, BorderLayout.NORTH);

    //if required, a login prompt
    if (mysettings.getUseAuth()) 
    {
      JPanel promptPanel = new JPanel(new BorderLayout());
      JPanel loginPanel = new JPanel();
      if (mysettings.getUseX11()) 
	loginPanel.setLayout(new GridLayout(3,2));
      else 
	loginPanel.setLayout(new GridLayout(2,2));
      
      final JTextField ufield = new JTextField(16);
      if (mysettings.getServiceUserName() != null) 
	ufield.setText(mysettings.getServiceUserName());
      
      final JPasswordField pfield = new JPasswordField(16);
      final JTextField xfield = new JTextField(16);
      JLabel ulab = new JLabel(" Username:", SwingConstants.LEFT);
      JLabel plab = new JLabel(" Password:", SwingConstants.LEFT);
      JLabel xlab = new JLabel("X Display:", SwingConstants.LEFT);
      //add labels etc
      loginPanel.add(ulab);
      loginPanel.add(ufield);
      loginPanel.add(plab);
      loginPanel.add(pfield);
      if (mysettings.getUseX11()) 
      {
	loginPanel.add(xlab);
	loginPanel.add(xfield);
      }
      promptPanel.add(loginPanel, BorderLayout.CENTER);
      // buttons across the bottom
      JPanel buttonPanel = new JPanel();
      buttonPanel.setLayout(new FlowLayout(FlowLayout.RIGHT));
      JButton cancelButton = new JButton("Cancel");
      JButton exitButton = new JButton("Exit");
      JButton okButton = new JButton("OK");
      cancelButton.addActionListener(new ActionListener() 
      {
	public void actionPerformed(ActionEvent e) 
        {
	  splashf.setVisible(false);
	}
      });
      exitButton.addActionListener(new ActionListener() 
      {
	public void actionPerformed(ActionEvent e) 
        {
	  System.exit(0);
	}
      });
      okButton.addActionListener(new ActionListener()
      {
	public void actionPerformed(ActionEvent e)
        {
	  mysettings.setServiceUserName(ufield.getText());
	  mysettings.setServicePasswd(pfield.getText());
	  if (mysettings.getUseX11()) 
          {
	    mysettings.setX11display(xfield.getText());
	  }
	  exitOnDone = true;
	  /*
	   * There is a potential for coding errors here. What if we don't
	   * get all the startup code done? So, just to be on the safe
	   * side, we don't wait for all the background stuff to finish.
	   */
	  splashf.setVisible(false);
	}
      });
      /*
       * the cancel button isn't currently used; it's the same as
       * the OK button. But if the OK button actually did login
       * validation then we should enable this button again.
       *
      buttonPanel.add(cancelButton);
      */
      buttonPanel.add(exitButton);
      buttonPanel.add(okButton);
      promptPanel.add(buttonPanel, BorderLayout.SOUTH);
      splashp.add(promptPanel);
    } else {
      exitOnDone = true;
    }
    //if required, a progress meter
    if (iprogressmax > 0)
    {
      JPanel progressPanel = new JPanel();
      progressPanel.setLayout(new FlowLayout(FlowLayout.LEFT));
      progressBar = new JProgressBar(0,iprogressmax);
      progressBar.setValue(0);
      progressLabel = new JLabel("Starting up.");
      progressPanel.add(progressBar);
      progressPanel.add(progressLabel);
      splashp.add(progressPanel,BorderLayout.SOUTH);
    }

    // add a border to the main pane to make is stand out
    splashp.setBorder(BorderFactory.createMatteBorder(2,2,2,2,Color.black));

    splashf.getContentPane().add(splashp);
    splashf.setSize(380,200);

    // all added, display the frame
    splashf.setDefaultCloseOperation(WindowConstants.HIDE_ON_CLOSE);
    splashf.setVisible(true);

  }

/**
* Update the progress bar and label
*/
  public void doneSomething(String s) 
  {
    if (iprogressmax > 0) 
    {
      if (iprogress < iprogressmax) 
      {
	iprogress++;
	progressBar.setValue(iprogress);
      }
      progressLabel.setText(s);
      if (iprogress == iprogressmax) 
      {
	progressLabel.setText("Startup complete.");
	allDone = true;
	if (exitOnDone) 
	  splashf.setVisible(false);
      }
    }
  }

/**
* Finish the progress bar and label
*/
  public void doneEverything(String s) 
  {
    if (iprogressmax > 0) 
    {
      progressBar.setValue(iprogressmax);
      progressLabel.setText(s);
      if (exitOnDone) 
	splashf.setVisible(false);
    }
  }

/**
* Called if something went wrong in startup.
*/
  public void startupFailed() 
  {
    // hide the splash so it doesn't obscure the dialogs
    //splashf.setVisible(false);
    EmbreoUtils.errorPopup(null, "Embreo startup failed. Please check\n"
			   +"your server and proxy settings");
    mysettings.editFrame();
    Object[] options = {"Try again", "Exit"};
    int itry = JOptionPane.showOptionDialog(null,
				 "Would you like to start over?",
				 "Restart?",
				 JOptionPane.YES_NO_OPTION,
				 JOptionPane.QUESTION_MESSAGE,
				 null,
				 options,
				 options[0]);
    if (itry == JOptionPane.YES_OPTION) {
      //splashf.setVisible(true);
      return;
    } else {
      System.exit(1);
    }
  }

  public JFrame getSplashFrame()
  {
    return splashf;
  }

}
