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

import org.emboss.jemboss.*;
import org.emboss.jemboss.programs.*;
import org.emboss.jemboss.parser.*;

import java.io.*;
import java.util.*;

/**
*
* Jemboss Authenticated Server for SOAP
*
*/
public class JembossAuthServer
{

  private String fs = new String(System.getProperty("file.separator"));
  private String ps = new String(System.getProperty("path.separator"));
  private String ls = new String(System.getProperty("line.separator"));

//get paths to EMBOSS
  JembossParams jp = new JembossParams();
  String plplot = jp.getPlplot();
  String embossData = jp.getEmbossData();
  String embossBin = jp.getEmbossBin();
  String embossPath = jp.getEmbossPath();
  String acdDirToParse = jp.getAcdDirToParse();

  private String homeDirectory = new String(System.getProperty("user.home") + fs);
  
  private String username = new String(System.getProperty("user.name") + fs);
  private String tmproot = new String("/tmp/SOAP/emboss/" + username );
  private File tmprootDir = new File(tmproot);

  private String[] envp = 
  {                                         //embossBin first in path-
    "PATH=" + embossBin + ps + embossPath,  //JNI speedup
    "PLPLOT_LIB=" + plplot,
    "EMBOSS_DATA=" + embossData,
    "HOME=" + homeDirectory
  };
 
  private static Hashtable acdStore;


  public String name()
  {
    return "The EMBOSS Application Suite";
  }

  public String version()
  {
    String embossCommand = new String(embossBin + "embossversion");
    RunEmbossApplication rea = new RunEmbossApplication(embossCommand,envp,null);
    rea.isProcessStdout();
    return rea.getProcessStdout();
  }


  public String appversion()
  {
    String embossCommand = new String(embossBin + "embossversion");
    RunEmbossApplication rea = new RunEmbossApplication(embossCommand,envp,null);
    rea.isProcessStdout();
    return rea.getProcessStdout();
  }


  public String about()
  {
    return "Jemboss is an interface to the EMBOSS suite of programs.";
  }

  public String helpurl()
  {
    return "http://www.uk.embnet.org/Software/EMBOSS/";
  }

  public String abouturl()
  {
    return "http://www.uk.embnet.org/Software/EMBOSS/overview.html";
  }

  public String docurl()
  {
     return "http://www.uk.embnet.org/Software/EMBOSS/general.html";
  }

  public Hashtable servicedesc()
  {
     Hashtable desc = new Hashtable();
     desc.put("name",name());
     desc.put("version",version());
     desc.put("appversion",appversion());
     desc.put("about",about());
     desc.put("helpurl",helpurl());
     desc.put("abouturl",abouturl());
     desc.put("docurl",docurl());
     return desc;
  }


/**
*
* Retrieves the ACD file of an application.
* @param  application name
* @return Vector of containing the ACD string
*
*/
  public Vector show_acd(String appName)
  {

    Vector acd = new Vector();
    String acdText = new String("");
    String line;
    String acdToParse = new String(acdDirToParse + appName + ".acd");

    try
    {
      BufferedReader in = new BufferedReader(new FileReader(acdToParse));
      while((line = in.readLine()) != null )
      {
        if(!line.startsWith("#") && !line.equals(""))
        { 
          line = line.trim();
          line = line.replace('}',')');
          acdText = acdText.concat(line + "\n");
        }
      }
    }
    catch (IOException e)
    {
      System.out.println("Cannot open EMBOSS acd file " + acdToParse);
    }

    acd.add("status");
    acd.add("0");
    acd.add("acd");
    acd.add(acdText);

    return acd;
  }

/**
*
* Returns the output of the EMBOSS utility wossname
* @return wossname output
*
*/
  public Vector getWossname()
  {
    Vector wossOut = new Vector();
    String embossCommand = new String(embossBin + "wossname -colon -auto");
    RunEmbossApplication rea = new RunEmbossApplication(embossCommand,envp,null);
    wossOut.add("status");
    wossOut.add("0");
    rea.isProcessStdout();
    wossOut.add("wossname");
    wossOut.add(rea.getProcessStdout());

    return wossOut;
  }


/**
*
* Returns the help for an application as given by 'tfm'
* @param String application name
* @return help 
*
*/
  public Vector show_help(String applName)
  {
    String command = embossBin.concat("tfm " + applName + " -nomore");
    RunEmbossApplication rea = new RunEmbossApplication(command,envp,null);
    String helptext = "";
    if(rea.isProcessStdout())
      helptext = rea.getProcessStdout();
    else
      helptext = "No help available for this application.";

    Vector vans = new Vector();
    vans.add("helptext");
    vans.add(helptext);

    return vans;
  }


/**
*
* Uses JNI to calculate sequence attributes using EMBOSS library call. 
* @param sequence filename or database entry
* @return sequence length, weight & type (protein/nucleotide)
*
*/
  public Vector call_ajax(String fileContent, String seqtype)
  {
    boolean afile = false;
    String fn = null;
    File tf = null;

    Vector vans = new Vector();

    // create temporary file
    if( ((fileContent.indexOf(":") < 0) || 
         (fileContent.indexOf("\n") > 0) ) &&
       !((new File(fileContent)).exists()) ) 
    {
      afile = true;
      try
      {
        tf = File.createTempFile("tmp",".jembosstmp", tmprootDir);
        PrintWriter out = new PrintWriter(new FileWriter(tf));
        out.println(fileContent);
        out.close();
    
        fn = new String(tf.getCanonicalPath());
      }
      catch (IOException ioe) 
      {
        System.out.println("IOException : STATUS NOT OK");
        vans.add("status");
        vans.add("1");
        return vans;
      }
    }
    else
    {
      fn = fileContent;     //looks like db entry or local file name
    }

    boolean ok = false;
    Ajax aj = null;

    if( ((new File(fn)).exists()) ||    //call ajax if sequence file
         (fn.indexOf(":") > 0) )        //or db
    {
      try
      {
        aj = new Ajax();
        if(seqtype.startsWith("seqset"))
          ok = aj.seqsetType(fn);
        else
          ok = aj.seqType(fn);
      }
      catch (Exception e)
      {
        System.out.println("AjaxException : STATUS NOT OK");
        vans.add("status");
        vans.add("1");
        return vans;
      }
    }

    if(ok)
    {
      System.out.println("STATUS OK");
      vans.add("length");
      vans.add(new Integer(aj.length));
      vans.add("protein");
      vans.add(new Boolean(aj.protein));
      vans.add("weight");
      vans.add(new Float(aj.weight));
      vans.add("status");
      vans.add("0");
    }
    else
    {
      System.out.println("STATUS NOT OK");
      vans.add("status");
      vans.add("1");
    }

    if(afile)
      tf.delete();

    return vans;
  }


/**
*
* Returns the databases held on the server
* @return output from 'showdb'
*
*/
  public Vector show_db()
  {
    Vector showdbOut = new Vector();
    String embossCommand = new String(embossBin + "showdb -auto");
    RunEmbossApplication rea = new RunEmbossApplication(embossCommand,envp,null);
    showdbOut.add("status");
    showdbOut.add("0");
    rea.isProcessStdout();
    showdbOut.add("showdb");
    showdbOut.add(rea.getProcessStdout());
     
    return showdbOut;
  }


/**
*
* Private Authenticated Server
* 
* Run an EMBOSS application
* @param command line to run
* @param unused 
* @param Hashtable of input files
* @return output files from application run
*
*/
  public Vector run_prog(String embossCommand, String options, Hashtable inFiles,
                         String userName, String passwd)
  {

    tmproot = tmproot.concat(userName+fs);
   
    Ajax aj = new Ajax();

    Vector result = new Vector();

    if(!verifyUser(aj,userName,passwd,result))
      return result;
 
    // remember the original uid & uid
    int sgid = aj.getgid();
    int suid = aj.getuid();

    // change to user id   
    if(!changeUser(aj,result,userName,aj.uid,aj.gid))
      return result;

    System.out.println("Running runProg now.... " + tmproot);

    //disallow pathnames and multiple command constructions
    if((embossCommand.indexOf("/") > -1) || (embossCommand.indexOf(";") > -1) ||
       (embossCommand.indexOf("/") > -1) )
    {
       result.add("msg");
       result.add("ERROR: Disallowed command syntax "+embossCommand);
       result.add("status");
       result.add("1");
       return result;
    }

    //trap anything that is trying to write to stdout
    int stdIndex = embossCommand.indexOf(" stdout ");
    if(stdIndex > -1)
    {
      String startCmd = embossCommand.substring(0,stdIndex+7);
      String endCmd = embossCommand.substring(stdIndex+8);
      embossCommand = startCmd.concat("file ");
      embossCommand = embossCommand.concat(endCmd);
    }

    Enumeration enum = inFiles.keys();
    String appl   = embossCommand.substring(0,embossCommand.indexOf(" "));
    String rest   = embossCommand.substring(embossCommand.indexOf(" "));
    embossCommand = embossBin.concat(embossCommand);
    String msg = new String("");

    boolean ok;
    
    if(!tmprootDir.isDirectory())
    {
      ok = tmprootDir.mkdirs();
      if(ok)
        System.out.println("Created directory "+tmproot);
      else
        System.out.println("Failed to create directory "+tmproot);
    }

    Random rnd = new Random();
    String dat = new Date().toString();

//get a unique project name 
    String project = new String(tmproot + appl + "_" +
         dat.replace(' ','_') + "_" + rnd.nextInt(99999));

    File projectDir = new File(project);
    ok = projectDir.mkdirs();

//create description file
    File desc = new File(new String(project + fs + ".desc"));

    String descript = "";
    try
    {
      PrintWriter dout = new PrintWriter(new FileWriter(desc));

      descript = descript.concat("EMBOSS run details"+ls+ls+
                                 "Application: "+appl+ls+rest+
                                 "Started at "+dat+ls+ls+"Input files:");
      dout.println("EMBOSS run details" + ls + ls);
      dout.println("Application: " + appl + ls + rest);
      dout.println("Started at " + dat + ls + ls + "Input files:");
  
      while (enum.hasMoreElements())
      {
        String thiskey = (String)enum.nextElement().toString();
        String filec = (String)inFiles.get(thiskey);
        File f = new File(project + fs + thiskey);
        descript = descript.concat(project+fs+thiskey);
        dout.println(project + fs + thiskey);
        try
        {
          f.createNewFile();
          PrintWriter out = new PrintWriter(new FileWriter(f));
          out.println(filec);
          out.close();
        }
        catch (IOException ioe) 
        {
          msg = new String("Error making description file");
        }
      }
      
      dout.close();
    }
    catch (IOException ioe) {} 
 

    aj.setegid(sgid);
    aj.seteuid(suid);
  
    String environ ="";
    for(int i=0;i<envp.length;i++)
       environ = environ.concat(envp[i]+" ");

    result.add("cmd");
    result.add(appl + " " + rest);
    result.add("status");
    result.add("0");

    if(options.toLowerCase().indexOf("interactive") > -1)
    {
      boolean lfork = aj.fork(embossCommand,environ,project,
                                             aj.uid,aj.gid);

      System.out.println("STDOUT \n"+aj.outStd);
      System.out.println("STDERR \n"+aj.errStd);

      result.add("msg");
      if(!lfork)
        result.add("Fork process in run_prog failed on server");
      else
        result.add("");
      createFinishedFile(project);
    }
    else      //batch or background
    {
      JembossAuthThread jt = new JembossAuthThread(aj,embossCommand,environ,
                                           project);
      jt.start();
      result.add("msg");
      result.add("");
      result.add("job_submitted");
      result.add("Job " + projectDir.getName() + "submitted.");
      result.add("jobid");
      result.add(projectDir.getName());
      result.add("description");
      result.add(descript+ls+"Application pending"+ls);
    }

//get the output files
    result = loadFilesContent(projectDir,project,result);
    result = loadPNGContent(projectDir,project,result);

    return result;
  }

/**
*
* Creates a file named "finished" in the project directory,
* that contains a time stamp.
* @param String project directory name
*
*/ 
  private void createFinishedFile(String project)
  {
    File finished = new File(new String(project + fs + ".finished"));
    try
    {
      PrintWriter fout = new PrintWriter(new FileWriter(finished));
      fout.println((new Date()).toString());
      fout.close();
    }
    catch (IOException ioe) {}
  }


/**
*
* Private Server
*
* Returns the results for a saved project.
* @param project/directory name
* @param unused
* @return saved results files
*
*/
  public Vector show_saved_results(String project, String cl,
                              String userName, String passwd)
  {

    Ajax aj = new Ajax();
    Vector ssr = new Vector();
    if(!verifyUser(aj,userName,passwd,ssr))
      return ssr;

// remember the original uid & uid
    int sgid = aj.getgid();
    int suid = aj.getuid();

// change to user id
    if(!changeUser(aj,ssr,userName,aj.uid,aj.gid))
      return ssr;

    tmproot = tmproot.concat(userName+fs); 
    project = tmproot.concat(project);
    File projectDir = new File(project);
    ssr = loadFilesContent(projectDir,project,ssr);
    ssr = loadPNGContent(projectDir,project,ssr);
        
    ssr.add("status");
    ssr.add("0");

    ssr.add("msg");
    ssr.add("OK");

    aj.setegid(sgid);
    aj.seteuid(suid);


    return ssr;
  }


/**
*  
* Private server
*
* Deletes a projects saved results.
* @param project/directory name
* @param unused
* @return message
*
*/
  public Vector delete_saved_results(String project, String cl,
                                String userName, String passwd)
  {

    Vector dsr = new Vector();
    Ajax aj = new Ajax();
    if(!verifyUser(aj,userName,passwd,dsr))
      return dsr;

// remember the original uid & uid
    int sgid = aj.getgid();
    int suid = aj.getuid();

// change to user id
    if(!changeUser(aj,dsr,userName,aj.uid,aj.gid))
      return dsr;

    tmproot = tmproot.concat(userName+fs);
    project = tmproot.concat(project);
    File projectDir = new File(project);
    File resFiles[] = projectDir.listFiles();

    for(int i=0;i<resFiles.length;i++)
      resFiles[i].delete();
      
    projectDir.delete();

    dsr.add("status");
    dsr.add("0");
    dsr.add("msg");
    dsr.add("Results deleted successfully.");

    aj.setegid(sgid);
    aj.seteuid(suid);

    return dsr;
  }


/**
*
* Private Server
*
* List of the saved results on the server.
* @return list of the saved results.
*
*/
  public Vector list_saved_results(String userName, String passwd)
  {
    Ajax aj = new Ajax();
    Vector lsr = new Vector();
    if(!verifyUser(aj,userName,passwd,lsr))
      return lsr;

// remember the original uid & uid
    int sgid = aj.getgid();
    int suid = aj.getuid();

// change to user id
    if(!changeUser(aj,lsr,userName,aj.uid,aj.gid))
      return lsr;

    tmproot = tmproot.concat(userName+fs);
    tmprootDir = new File(tmproot);
   
    lsr.add("status");
    lsr.add("0");
    lsr.add("msg");
    lsr.add("OK");


    String resFiles[] = tmprootDir.list(new FilenameFilter()
    {
      public boolean accept(File cwd, String name)
      {
        return !name.startsWith(".");
      };
    });
    
    String list = "";
    for(int i=0;i<resFiles.length;i++)
    {
      String line = new String("");
      String fc = new String("");
     
      try
      {
        BufferedReader in = new BufferedReader(new FileReader(tmproot + fs + 
                                               resFiles[i] + fs + ".desc"));
        while((line = in.readLine()) != null)
          fc = fc.concat(line + "\n");

        in.close();
        lsr.add(resFiles[i]);
        lsr.add(fc);
        list = list.concat(resFiles[i] + "\n");
      }
      catch (IOException ioe)
      {
        System.out.println("IOException in list_saved_results "+tmproot +fs +
                                               resFiles[i] +fs + ".desc");
      }
    }

    lsr.add("list");
    lsr.add(list);

    aj.setegid(sgid);
    aj.seteuid(suid);

    return lsr;
  }

/**
*
* Reads in files from EMBOSS output
*
*/
  private Vector loadFilesContent(File projectDir, String project, Vector result)
  {

    String outFiles[] = projectDir.list(new FilenameFilter()
    {
      public boolean accept(File cwd, String name)
      {
        return (!name.startsWith(".") && !name.endsWith(".png"));
      };
    });

    for(int i=0;i<outFiles.length;i++)
    {
      String line = new String("");
      String fc = new String("");
      String key = new String(outFiles[i]);
      try
      {
        BufferedReader in = new BufferedReader(new FileReader(project + fs + outFiles[i]));
        while((line = in.readLine()) != null)
          fc = fc.concat(line + "\n");
      }
      catch (IOException ioe){}
      if(!fc.equals(""))
      {
        result.add(key);
        result.add(fc);
      }
    }
    return result;

  }


/**
*
* Reads in png files from EMBOSS output
*
*/
  private Vector loadPNGContent(File projectDir, String project, Vector result)
  {

    String pngFiles[] = projectDir.list(new FilenameFilter()
    {
      public boolean accept(File cwd, String name)
      {
        return name.endsWith(".png");
      };
    });

    for(int i=0;i<pngFiles.length;i++)
    {
      String line = new String("");
      String key = new String(pngFiles[i]);
      DataInputStream dis;
      FileInputStream fis;
      int nby = 0;
      byte data[] = new byte[1];
      try
      {
        fis = new FileInputStream(project + fs + pngFiles[i]);
        dis = new DataInputStream(fis);
        while(true)
        {
          dis.readByte();
          nby++;
        }
      }
      catch (EOFException eof){}
      catch (IOException ioe){}

      if(nby >0)
      {
        try
        {
          data = new byte[nby];
          fis = new FileInputStream(project + fs + pngFiles[i]);
          dis = new DataInputStream(fis);
          nby=0;
          while(true)
          {
            data[nby]=dis.readByte();
            nby++;
          }
        }
        catch (EOFException eof){}
        catch (IOException ioe){}

        result.add(key);
        result.add(data);
      }
    }
    return result;
  }
 
/**
*
* Used to provide information on the batch/background
* processes.
*
*/
  public Vector update_result_status(String prog, String opt,
                        Hashtable resToQuery,String userName,
                        String passwd)
  {
    Ajax aj = new Ajax();
    Vector vans = new Vector();
    if(!verifyUser(aj,userName,passwd,vans))
      return vans;

    // remember the original uid & uid
    int sgid = aj.getgid();
    int suid = aj.getuid();

    // change to user id
    if(!changeUser(aj,vans,userName,aj.uid,aj.gid))
      return vans;

    tmproot = tmproot.concat(userName+fs);

    Enumeration enum = resToQuery.keys();
    while (enum.hasMoreElements())
    {
      String thiskey = (String)enum.nextElement().toString();
      String thiselm = (String)resToQuery.get(thiskey);
//    System.out.println("KEY : "+thiskey+" ELEMENT: "+thiselm);
      File f = new File(tmproot+fs+thiskey+fs+".finished");
      if(f.exists())
      {
        vans.add(thiskey);
        vans.add("complete");
        String fc = "";
        try
        {
          String line;
          BufferedReader in = new BufferedReader(new FileReader(tmproot+
                                                fs+thiskey+fs+".desc"));
          while((line = in.readLine()) != null)
            fc = fc.concat(line + "\n");
        }
        catch (IOException ioe)
        {
          fc = "Error in reading information file";
        }
        vans.add(thiskey+"-description");
        vans.add(fc);
      }
      else
      {
        vans.add(thiskey);
        vans.add("pending");
      }
    }

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

