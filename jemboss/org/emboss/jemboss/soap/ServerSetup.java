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

import org.emboss.jemboss.gui.form.TextFieldPaste;
import uk.ac.mrc.hgmp.embreo.*;


public class ServerSetup extends JPanel
{
  
  private TextFieldPaste publicURL;
  private TextFieldPaste privateURL;
  private TextFieldPaste publicName;
  private TextFieldPaste privateName;
  private EmbreoParams mysettings; 

  public ServerSetup(EmbreoParams mysettings)
  {
    this.mysettings = mysettings;

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

    publicURL = new TextFieldPaste();
    publicURL.setText(mysettings.getPublicSoapURL());
    jpCenter.add(publicURL);

    publicName = new TextFieldPaste();
    publicName.setText(mysettings.getPublicSoapService());
    jpEast.add(publicName);

//private server
    lab = new JLabel("Private Server");
    jpWest.add(lab);

    privateURL = new TextFieldPaste();
    privateURL.setText(mysettings.getPrivateSoapURL());
    jpCenter.add(privateURL);
     
    privateName = new TextFieldPaste();
    privateName.setText(mysettings.getPrivateSoapService());
    jpEast.add(privateName);
 
    add(jpWest, BorderLayout.WEST);
    add(jpCenter, BorderLayout.CENTER);
    add(jpEast, BorderLayout.EAST);

  }

  public EmbreoParams setNewSettings()
  {
    mysettings.setPublicSoapURL(publicURL.getText());
    mysettings.setPrivateSoapURL(privateURL.getText());
    mysettings.setPublicSoapService(publicName.getText());
    mysettings.setPrivateSoapService(privateName.getText());
    return mysettings;
  }


}

