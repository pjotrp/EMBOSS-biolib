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

import uk.ac.mrc.hgmp.embreo.EmbreoParams;

//PCT SOAP add
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

public class GetWossname 
{

  private String statusmsg;
  private String status;
   private String dbText;

   public GetWossname(EmbreoParams mysettings)
   {
     URL proglisturl;
     try {
       proglisturl = new URL(mysettings.getPublicSoapURL());
     } catch (Exception e) { 
       System.err.println("While Initialising URL Caught Exception (" +
			  e.getMessage ());
       return;
     }
     int i;

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
     //
     // end proxy handling
     //
     Call proglistcall = new Call();
     proglistcall.setSOAPTransport(proglistconn);
     proglistcall.setTargetObjectURI(mysettings.getPublicSoapService());
     proglistcall.setMethodName("getWossname");
     proglistcall.setEncodingStyleURI(Constants.NS_URI_SOAP_ENC);

     Response proglistresp;
     try {
       proglistresp = proglistcall.invoke(proglisturl,null);
     } catch(SOAPException e) {
       System.err.println("Caught SOAPException (" +
			  e.getFaultCode () + "): " +
			  e.getMessage ());
       return;
     }
    //Check response
    if (!proglistresp.generatedFault()) {
      Parameter progret = proglistresp.getReturnValue();
      Object progvalue = progret.getValue();
      
      //
      // should at this point check the application status we get
      // back
      //

      Vector progans = proglistresp.getParams();
      Parameter progansp;
      dbText = "";
      String tstr;
      if (progans != null) {
        progansp = (Parameter) progans.get(0);
        int progians = progans.size();
        tstr = (String)progvalue;
        if (tstr.equalsIgnoreCase("status"))
	  {status=progansp.getValue().toString();}
        if (tstr.equalsIgnoreCase("msg"))
	  {statusmsg=progansp.getValue().toString();}
        if (tstr.equalsIgnoreCase("showdb"))
	  {dbText=progansp.getValue().toString();}
        for (int j=1; j<progians;j++) {
	  Parameter progansk = (Parameter) progans.get(j);
	  tstr = progansk.getValue().toString();
	  j++;
	  Parameter progansv = (Parameter) progans.get(j);
	  if (tstr.equalsIgnoreCase("status"))
	    {status=progansv.getValue().toString();}
	  if (tstr.equalsIgnoreCase("msg"))
	    {statusmsg=progansv.getValue().toString();}
	  if (tstr.equalsIgnoreCase("showdb"))
	    {dbText=progansv.getValue().toString();}
        }
      }
      else
      {
        Vector vans = (Vector)progvalue;
        int n = vans.size();
        for(int j=0;j<n;j+=2)
        {
          if(((String)vans.get(j)).equalsIgnoreCase("wossname"))
            dbText=(String)vans.get(j+1);
          else if(((String)vans.get(j)).equalsIgnoreCase("msg"))
            statusmsg=(String)vans.get(j+1);
          else if(((String)vans.get(j)).equalsIgnoreCase("status"))
            status=(String)vans.get(j+1);
        }
      }


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

   public String getDBText() 
   {
     return dbText;
   }

}
