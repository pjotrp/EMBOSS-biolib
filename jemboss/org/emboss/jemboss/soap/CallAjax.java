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
********************************************************************/

package org.emboss.jemboss.soap;

import java.io.*;
import java.util.*;

import uk.ac.mrc.hgmp.embreo.*;

import java.net.*;
import org.w3c.dom.*;
import org.xml.sax.*;
import javax.xml.parsers.*;
import javax.mail.*;
import org.apache.soap.util.xml.*;
import org.apache.soap.*;
import org.apache.soap.encoding.*;
import org.apache.soap.encoding.soapenc.*;
import org.apache.soap.rpc.*;
import org.apache.soap.transport.http.SOAPHTTPConnection;

public class CallAjax {

  private String statusmsg;
  private String status;
  private int length;
  private float weight;
  private boolean protein;

   public CallAjax(String fileContent, String seqtype, EmbreoParams mysettings) throws EmbreoAuthException {

     URL proglisturl;
     try {
       proglisturl = new URL(mysettings.getPublicSoapURL());
     } catch (Exception e) { 
       System.err.println("While Initialising URL Caught Exception (" +
			  e.getMessage ());
       return;
     }
     int i;

     System.out.println("Settings URL: "+mysettings.getPrivateSoapURL());

     SOAPHTTPConnection proglistconn = new SOAPHTTPConnection();
     //
     // if proxy, set the proxy values
     //
     if (mysettings.getUseProxy() == true) {
       if (mysettings.getProxyHost() != null) {
	 proglistconn.setProxyHost(mysettings.getProxyHost());
	 proglistconn.setProxyPort(mysettings.getProxyPortNum());
       }
     }
     // if authenticated, add the authentication headers
     // must set them to the right values first
     //
     if (mysettings.getUseAuth() == true) {
       if (mysettings.getServiceUserName() != null) {
	 if (mysettings.getServicePasswd() != null) {
	   proglistconn.setUserName(mysettings.getServiceUserName());
	   proglistconn.setPassword(mysettings.getServicePasswd());
	 }
       }
     }
     Call proglistcall = new Call();
     proglistcall.setSOAPTransport(proglistconn);
     proglistcall.setTargetObjectURI(mysettings.getPublicSoapService());
     proglistcall.setMethodName("call_ajax");
     proglistcall.setEncodingStyleURI(Constants.NS_URI_SOAP_ENC);

     Vector params = new Vector();
     params.addElement(new Parameter("fileContent", String.class,
                                    fileContent, null));
     params.addElement(new Parameter("seqtype", String.class,
                                     seqtype, null));
     proglistcall.setParams(params);

     Response proglistresp;
     try 
     {
       proglistresp = proglistcall.invoke(proglisturl,null);
     } 
     catch(SOAPException e) 
     {
       System.err.println("Caught SOAPException (" +
			  e.getFaultCode () + "): " +
			  e.getMessage ());
       Hashtable efaulth = proglistconn.getHeaders();
       if(efaulth.containsKey("WWW-Authenticate")) 
       {
	 System.err.println("Auth header found!");
	 throw new EmbreoAuthException("Authentication Failed");
       }
       return;
     }

    //Check response
     if (!proglistresp.generatedFault()) 
     {
       Parameter progret = proglistresp.getReturnValue();
       Object progvalue = progret.getValue();
      
       Vector vans = (Vector)progvalue;
       int n = vans.size();
       for(int j=0;j<n;j+=2)
       {
        if(vans.get(j).equals("length"))
          length = ((Integer)(vans.get(j+1))).intValue();
        else if(vans.get(j).equals("weight"))
          weight = ((Float)(vans.get(j+1))).floatValue();
        else if(vans.get(j).equals("protein"))
           protein = ((Boolean)(vans.get(j+1))).booleanValue();
        else if(vans.get(j).equals("status"))
          status = (String)(vans.get(j+1));
       }
       System.out.println("length/weight/protein " + length + " " + weight );
      
     } 
     else 
     {
       Fault fault = proglistresp.getFault();
       System.err.println("Generated fault: ");
       System.out.println("  Fault Code   = " + fault.getFaultCode());
       System.out.println("  Fault String = " + fault.getFaultString());
     }


   }


  public String getStatus() 
  {
    return status;
  }
  
  public String getStatusMsg() 
  {
    return statusmsg;
  }

  public int getLength()
  {
    return length;
  }
  
  public float getWeight()
  {
    return weight;
  }

  public boolean isProtein()
  {
    return protein;
  }

}
