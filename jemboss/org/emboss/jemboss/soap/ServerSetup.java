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

package org.emboss.jemboss.soap;

import java.io.*;
import javax.swing.*;
import java.awt.*;
import java.util.*;

import java.awt.event.*;
import org.emboss.jemboss.gui.MemoryComboBox;
import org.emboss.jemboss.JembossParams;


public class ServerSetup extends JPanel 
{
  
  private MemoryComboBox publicURL;
  private MemoryComboBox privateURL;
  private MemoryComboBox publicName;
  private MemoryComboBox privateName;
  private MemoryComboBox proxyName;
  private MemoryComboBox proxyPort;

  private JCheckBox userAuth;
  private JCheckBox useProxy;

  private JembossParams mysettings; 

/**
*
* Setting for public and private settings panel.
* @param JembossParams SOAP parameters
*
*/
  public ServerSetup(JembossParams mysettings)
  {

    this.mysettings = mysettings;

    Vector PublicServerURL = new Vector();
    PublicServerURL.add(mysettings.getPublicSoapURL());

    Vector PrivateServerURL = new Vector();
    PrivateServerURL.add(mysettings.getPrivateSoapURL());

    Vector PublicServerName = new Vector();
    PublicServerName.add(mysettings.getPublicSoapService());

    Vector PrivateServerName = new Vector();
    PrivateServerName.add(mysettings.getPrivateSoapService());

    Vector proxyNameSettings = new Vector();
    proxyNameSettings.add(mysettings.getProxyHost());

    Vector proxyPortSettings = new Vector();
    proxyPortSettings.add(new Integer(mysettings.getProxyPortNum()));

    GridLayout gl = new GridLayout(10,1,6,6);
    
    JPanel jpWest   = new JPanel(gl);
    JPanel jpCenter = new JPanel(gl);

    setLayout(new BorderLayout());

//public server
    JLabel lab = new JLabel("Public Server");
    jpWest.add(lab);
    publicURL = new MemoryComboBox(PublicServerURL);
    jpCenter.add(publicURL);

    lab = new JLabel("Public Service Name");
    jpWest.add(lab);
    publicName = new MemoryComboBox(PublicServerName);
    jpCenter.add(publicName);

//separator
    jpWest.add(new JLabel(""));
    jpCenter.add(new JLabel(""));

//private server
    lab = new JLabel("Private Server");
    jpWest.add(lab);
    privateURL = new MemoryComboBox(PrivateServerURL);
    jpCenter.add(privateURL);    

    lab = new JLabel("Private Service Name ");
    jpWest.add(lab);
    privateName = new MemoryComboBox(PrivateServerName);
    jpCenter.add(privateName);

//separator
    jpWest.add(new JLabel(""));
    jpCenter.add(new JLabel(""));
    
//proxy name
    useProxy = new JCheckBox("Use proxy settings",
                              mysettings.getUseProxy());
    jpWest.add(useProxy);
    jpCenter.add(new JLabel(""));

    lab = new JLabel("Proxy ");
    jpWest.add(lab);
    proxyName = new MemoryComboBox(proxyNameSettings);
    jpCenter.add(proxyName);

    lab = new JLabel("Proxy Port");
    jpWest.add(lab);
    proxyPort = new MemoryComboBox(proxyPortSettings);
    jpCenter.add(proxyPort);

    proxyName.setEnabled(useProxy.isSelected());
    proxyPort.setEnabled(useProxy.isSelected());

    useProxy.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        proxyName.setEnabled(useProxy.isSelected());
        proxyPort.setEnabled(useProxy.isSelected());
      }
    });


//separator
    jpWest.add(new JLabel(""));
    jpCenter.add(new JLabel(""));


    add(jpWest, BorderLayout.WEST);
    add(jpCenter, BorderLayout.CENTER);

//authentication  
    userAuth = new JCheckBox("User authentication required by private server",
                              mysettings.getUseAuth());
    add(userAuth,BorderLayout.SOUTH);
  }

  public JembossParams setNewSettings()
  {
    String settings[] = new String[8];
    settings[0] = 
       new String("server.public="+(String)publicURL.getSelectedItem());
    settings[1] = 
       new String("server.private="+(String)privateURL.getSelectedItem());
    settings[2] = 
       new String("service.public="+(String)publicName.getSelectedItem());
    settings[3] = 
       new String("service.private="+(String)privateName.getSelectedItem());

    if(userAuth.isSelected())
      settings[4] = new String("user.auth=true");
    else
      settings[4] = new String("user.auth=false");

    settings[6] =
         new String("proxy.host="+(String)proxyName.getSelectedItem());
    settings[7] =
         new String("proxy.port="+((Integer)proxyPort.getSelectedItem()).toString());

    if(useProxy.isSelected())
      settings[5] = new String("proxy.use=true");
    else
      settings[5] = new String("proxy.use=false");

    mysettings.updateJembossPropStrings(settings);

    return mysettings;
  }

}

