/****************************************************************
*
*  This program is free software; you can redistribute it and/or
*  modify it under the terms of the GNU General Public License
*  as published by the Free Software Foundation; either version 2
*  of the License, or (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*  @author: Copyright (C) Tim Carver
*
***************************************************************/

package org.emboss.jemboss.soap;

import java.io.*;
import java.util.*;

import org.emboss.jemboss.JembossParams;
import javax.swing.JOptionPane;
//AXIS
import org.apache.axis.client.Call;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;
import org.apache.axis.encoding.XMLType;

public class PrivateRequest 
{

  private Hashtable proganswer;
  private String result = "";
  private boolean successful = false;
  private boolean authenticationerror = false;


/**
*
* Make a soap call to a private server, using the default service
* @param mysettings JembossParams defining server parameters
* @param method     String defining which method to call
* @param args       Vector of arguments
* @throws JembossSoapException If authentication fails
*
*/
   public PrivateRequest(JembossParams mysettings, String method, Vector args) 
                throws JembossSoapException 
   {
     this(mysettings, mysettings.getPrivateSoapService(), method, args);
   }

/**
*
* Make a soap call to a private server
* @param mysettings JembossParams defining server parameters
* @param service    String defining which service to call
* @param method     String defining which method to call
*
* @throws JembossSoapException If authentication fails
*/
   public PrivateRequest(JembossParams mysettings, String service, String method)
                throws JembossSoapException
   {
     this(mysettings, service, method, (Vector) null);
   }

/**
*
* Make a soap call to a private server
* @param mysettings JembossParams defining server parameters
* @param service    String defining which service to call
* @param method     String defining which method to call
* @param args       Vector of arguments
*
* @throws JembossSoapException If authentication fails
*/
   public PrivateRequest(JembossParams mysettings, String service, String method,
                         Vector args) throws JembossSoapException 
   {

     try
     {
       String  endpoint = mysettings.getPublicSoapURL()+"/"+service;
       Service serv     = new Service();
       Call    call     = (Call) serv.createCall();
       QName   qn       = new QName(service, method);
       call.setTargetEndpointAddress( new java.net.URL(endpoint) );
       call.setOperationName(new QName(service, method));
       call.setEncodingStyle(org.apache.axis.Constants.URI_SOAP12_ENC);

       int nargs = 0;
       Object params[] = null;
       if(args != null)
       {
         if(mysettings.getUseAuth())
           nargs = args.size()+2;
         else
           nargs = args.size()+1;

         params = new Object[nargs];
         Enumeration e = args.elements();
         for(int i=0;i<args.size();i++)
         {
           Object obj = e.nextElement();
           params[i] = obj;
           if(obj.getClass().equals(String.class))
           {
             call.addParameter("Args"+i, XMLType.XSD_STRING,
                             javax.xml.rpc.ParameterMode.IN);
           }
           else if(obj.getClass().equals(Hashtable.class))
           {
             params[i] = getVector((Hashtable)obj);

             call.addParameter("Args"+i, XMLType.SOAP_VECTOR,
                             javax.xml.rpc.ParameterMode.IN);
           }
           else    // byte[]
           {
             call.addParameter("ByteArray", XMLType.XSD_BASE64,
                               javax.xml.rpc.ParameterMode.IN);
             params[i] = obj;
           }
         
         }
       }

       if(mysettings.getUseAuth() == true)
       {
         if(args == null)
         {
           nargs = 2;
           params = new Object[nargs];
         }
         call.addParameter("Door", XMLType.XSD_STRING,
                           javax.xml.rpc.ParameterMode.IN);
         params[nargs-2] = mysettings.getServiceUserName();
                       
         call.addParameter("Key", XMLType.XSD_BASE64,
                           javax.xml.rpc.ParameterMode.IN);
         params[nargs-1] = mysettings.getServicePasswdByte();
       }
       else       //No authorization reqd, so use user name here
       {          //to create own sand box on server
         if(nargs == 0)
         {
            nargs = 1;
            params = new Object[nargs];
         }

         if(args == null)
           args = new Vector();
         call.addParameter("Door", XMLType.XSD_STRING,
                           javax.xml.rpc.ParameterMode.IN);
         params[nargs-1] = System.getProperty("user.name");
       }

       call.setReturnType(org.apache.axis.Constants.SOAP_VECTOR);
       Vector vans = (Vector)call.invoke( params );

       proganswer = new Hashtable();
       // assumes it's even sized
       int n = vans.size();
       for(int j=0;j<n;j+=2)
       {
         String s = (String)vans.get(j);
         if(s.equals("msg"))
         {
           String msg = (String)vans.get(j+1);
           if(msg.startsWith("Failed Authorisation"))
             throw new JembossSoapException("Authentication Failed");
           else if(msg.startsWith("Error"))
             JOptionPane.showMessageDialog(null, msg, "alert",
                                   JOptionPane.ERROR_MESSAGE);
         }
         proganswer.put(s,vans.get(j+1));
       }
     } 
     catch (Exception e) 
     {
        System.out.println("Exception in PrivateRequest "+
                            e.getMessage ());
        throw new JembossSoapException("  Fault Code   = " );
     }

   }


  private Vector getVector(Hashtable h)
  {
    Vector v = new Vector();
    for(Enumeration e = h.keys() ; e.hasMoreElements() ;)
    {
      String s = (String)e.nextElement();
      v.add(s);
      v.add(h.get(s));
    }

    return v;
  }

/**
*
* @return true if the call succeeded (eventually) or not
*
*/
  public boolean succeeded() 
  {
    return successful;
  }

/**
*
* Gets an element out of the embreo result hash
* @param val The key to look up
* @return Either the element, or an empty String if there is no
* an element that matches the key
*
*/
  public Object getVal(String val) 
  {
    if (proganswer.containsKey(val)) 
      return proganswer.get(val);
    else 
      return "";
  }

/**
*
* @return The result of the soap server call. This is the first element that
* is returned. Only useful for server methods that return a simple value.
*
*/
  public String getResult() 
  {
    return result;
  }

/**
*
* @return Hahtable of results returned by the server call. 
*
*/
  public Hashtable getHash() 
  {
    return proganswer;
  }
}

