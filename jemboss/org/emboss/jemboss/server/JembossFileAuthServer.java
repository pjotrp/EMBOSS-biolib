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

package org.emboss.jemboss.server;

import org.emboss.jemboss.programs.*;
import org.emboss.jemboss.parser.*;

import java.io.*;
import java.util.*;

public class JembossFileAuthServer
{


  public Vector embreo_roots(String userName, String passwd)
  {

    Ajax aj = new Ajax();
    Vector vans = new Vector();
    if(!verifyUser(aj,userName,passwd,vans))
      return vans;

    String userHomeDir = aj.home+"/";

    vans.add("status");
    vans.add("0");
    vans.add("msg");
    vans.add("");

    vans.add("default-root");
    vans.add("HOME");

    vans.add("HOME");
    vans.add(userHomeDir);
 
    System.out.println(userHomeDir);
//  vans.add("SCRATCH");
//  vans.add("/m3/users/tim/soap");
  
    return vans;
  }


/**
*
* Given the alias a user root alias e.g. "HOME" return
* the directory this represents
*
* @param root alias (e.g "HOME")
* @return directory path
*
*/
  private String getRoot(String s, String userName, String passwd)
  {
    String rt = null;
    Vector userRoots = embreo_roots(userName,passwd);
 
    for(int i=0; i<userRoots.size();i+=2)
    {
      String root = (String)userRoots.get(i);
      if(root.equalsIgnoreCase(s))
        return (String)userRoots.get(i+1);
    }

    return rt;
  }


  public Vector directory_shortls(String options, String dirname,
                           String userName, String passwd)
  {

    System.out.println("directory_shortls\n");
    Vector vans = new Vector();
    Ajax aj = new Ajax();
    if(!verifyUser(aj,userName,passwd,vans))
      return vans;

    // remember the original uid & uid
    int sgid = aj.getgid();
    int suid = aj.getuid();

    int split = options.indexOf("=")+1;

    File dir = new File(getRoot(options.substring(split),userName,passwd)
                        + "/" + dirname);

    // change to user id
    if(!changeUser(aj,vans,userName,aj.uid,aj.gid))
      return vans;

// filter out dot files
    File files[] = dir.listFiles(new FilenameFilter()
    {
      public boolean accept(File d, String n)
      {
        return !n.startsWith(".");
      }
    });

    String listAll = "";
    String listDir = "";

    for(int i=0;i<files.length;i++)
    {
      if(files[i].isDirectory())
        listDir = listDir.concat(files[i].getName() + "\n");
      listAll = listAll.concat(files[i].getName() + "\n");
    }

    vans.add("status");
    vans.add("0");
    vans.add("msg");
    vans.add("");
    vans.add("list");
    vans.add(listAll);
    vans.add("dirlist");
    vans.add(listDir);

    aj.setegid(sgid);
    aj.seteuid(suid);

    return vans;
  }

  public Vector get_file(String options, String filename,
                           String userName, String passwd)
  {

    Vector vans = new Vector();
    Ajax aj = new Ajax();
    if(!verifyUser(aj,userName,passwd,vans))
      return vans;

    // remember the original uid & uid
    int sgid = aj.getgid();
    int suid = aj.getuid();

    int split = options.indexOf("=")+1;    
    File dir = new File(getRoot(options.substring(split),userName,passwd)
                         + "/" + filename);

    // change to user id
    if(!changeUser(aj,vans,userName,aj.uid,aj.gid))
      return vans;

    String line = new String("");
    String fc = new String("");
    try
    {
      BufferedReader in = new BufferedReader(new FileReader(dir));
      while((line = in.readLine()) != null)
        fc = fc.concat(line + "\n");
    }
    catch (IOException ioe){}
    vans.add("contents");
    vans.add(fc);
    vans.add("status");
    vans.add("0");
    vans.add("msg");
    vans.add("");

    aj.setegid(sgid);
    aj.seteuid(suid);

    return vans;
  } 


/**
*
* @param option determines the root directory to put the file
* @param filename name of the file to put
* @param filedata file contents
*
*/
  public Vector put_file(String options, String filename, byte[] filedata,
                           String userName, String passwd)
  {
    Vector vans = new Vector();
    Ajax aj = new Ajax();
    if(!verifyUser(aj,userName,passwd,vans))
      return vans;

    // remember the original uid & uid
    int sgid = aj.getgid();
    int suid = aj.getuid();

    int split = options.indexOf("=")+1;
    File f = new File(getRoot(options.substring(split),userName,passwd) 
                              + "/" + filename);

    // change to user id
    if(!changeUser(aj,vans,userName,aj.uid,aj.gid))
      return vans;

    try
    {
      FileOutputStream out = new FileOutputStream(f);
      out.write(filedata);
      out.close();
      vans.add("status");
      vans.add("0");
//    System.out.println("WRITTEN TO " + getRoot(options.substring(split)) +
//                       "/" + filename);
    }
    catch(IOException ioe)
    {
      vans.add("status");
      vans.add("1");
    }

    vans.add("msg");
    vans.add("");
 
    aj.setegid(sgid);
    aj.seteuid(suid);
  
    return vans;
  }
 
  private boolean verifyUser(Ajax aj, String userName,
                            String passwd, Vector res)
  {

    if(!aj.userInfo(userName,passwd))
    {
      System.out.println("Failed Authorisation "+userName);
      res.add("msg");
      res.add("Failed Authorisation "+userName);
      res.add("status");
      res.add("1");
      return false;
    }
    return true;
  }

  private boolean changeUser(Ajax aj, Vector res, String userName,
                             int uid, int gid)
  {

    int resgid = aj.setegid(gid);
    int resuid = aj.seteuid(uid);

    if(resgid != 0 || resuid != 0)
    {
      if(resgid != 0)
        System.out.println("Failed Authorisation change group "+userName);
      if(resuid != 0)
        System.out.println("Failed Authorisation change to user "+userName);
      res.add("msg");
      res.add("Failed Authorisation "+userName);
      res.add("status");
      res.add("1");
      return false;
    }
    return true;
  }

}

