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
import uk.ac.mrc.hgmp.embreo.*;


public class ServerSetup extends JPanel 
{
  
  private MemoryComboBox publicURL;
  private MemoryComboBox privateURL;
  private MemoryComboBox publicName;
  private MemoryComboBox privateName;

  private EmbreoParams mysettings; 

  public ServerSetup(EmbreoParams mysettings)
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

    GridLayout gl = new GridLayout(3,1,10,10);
    
    JPanel jpWest   = new JPanel(gl);
    JPanel jpCenter = new JPanel(gl);
    JPanel jpEast   = new JPanel(gl);

    setLayout(new BorderLayout());

    jpWest.add(new JLabel(""));
    jpCenter.add(new JLabel("Server"));
    jpEast.add(new JLabel("Service Name"));

// public server
    JLabel lab = new JLabel("Public Server");
    jpWest.add(lab);

    publicURL = new MemoryComboBox(PublicServerURL);
    jpCenter.add(publicURL);

    publicName = new MemoryComboBox(PublicServerName);
    jpEast.add(publicName);

//private server
    lab = new JLabel("Private Server");
    jpWest.add(lab);
    privateURL = new MemoryComboBox(PrivateServerURL);

    jpCenter.add(privateURL);    

    privateName = new MemoryComboBox(PrivateServerName);
    jpEast.add(privateName);
 
    add(jpWest, BorderLayout.WEST);
    add(jpCenter, BorderLayout.CENTER);
    add(jpEast, BorderLayout.EAST);

  }

  public EmbreoParams setNewSettings()
  {
    mysettings.setPublicSoapURL((String)publicURL.getSelectedItem());
    mysettings.setPrivateSoapURL((String)privateURL.getSelectedItem());
    mysettings.setPublicSoapService((String)publicName.getSelectedItem());
    mysettings.setPrivateSoapService((String)privateName.getSelectedItem());
    return mysettings;
  }

}

