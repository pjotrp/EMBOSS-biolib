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
import org.apache.soap.rpc.Parameter;

import java.io.*;
import java.util.*;

/**
*
* HGMP Jemboss Server for SOAP
*
*/
public class JembossServer
{


  private String fs = new String(System.getProperty("file.separator"));
  private String ps = new String(System.getProperty("path.separator"));

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

  private String[] envp = {
    "PATH=" + embossPath,
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

    acd.addElement(new Parameter("status",String.class,"status",null));
    acd.addElement(new Parameter("status",String.class,"0",null));
    acd.addElement(new Parameter("acd",String.class,"acd",null));
    acd.addElement(new Parameter("acd",String.class,acdText,null));

//  acd.add("status");
//  acd.add("0");
//  acd.add("acd");
//  acd.add(acdText);

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
      catch (IOException ioe) {}
    }
    else
    {
      fn = fileContent;     //looks like db entry or local file name
    }

    Ajax aj = new Ajax();
//  boolean ok = aj.seqType(fn);

    boolean ok;
    if(seqtype.startsWith("seqset"))
      ok = aj.seqsetType(fn);
    else
     ok = aj.seqType(fn);
   
    Vector vans = new Vector();

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
* Run an EMBOSS application
* @param command line to run
* @param unused
* @param String of input files and names
* @return output files from application run
*
*/
  public Vector run_prog(String embossCommand, String options, String[] inFiles)
  {
    System.out.println("Running runProg now with Strings[].... " + tmproot);
    Vector result = new Vector();

    Hashtable inF = new Hashtable();
    for(int i=0; i<inFiles.length;i+=2)
      inF.put(inFiles[i],inFiles[i+1]);

    result = run_prog(embossCommand,options,inF);

    return result;
  }

/**
*
* Run an EMBOSS application
* @param command line to run
* @param unused 
* @param Hashtable of input files
* @return output files from application run
*
*/
  public Vector run_prog(String embossCommand, String options, Hashtable inFiles)
  {

    System.out.println("Running runProg now.... " + tmproot);
    Enumeration enum = inFiles.keys();
    String appl   = embossCommand.substring(0,embossCommand.indexOf(" "));
    String rest   = embossCommand.substring(embossCommand.indexOf(" "));
    embossCommand = embossBin.concat(embossCommand);
    String msg = new String("");
    Vector result = new Vector();

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
    try
    {
      PrintWriter dout = new PrintWriter(new FileWriter(desc));  
      dout.println("EMBOSS run details\n\n");
      dout.println("Application: " + appl + "\n" + rest);
      dout.println("Started at " + dat + "\n\nInput files:");
  
      while (enum.hasMoreElements())
      {
        String thiskey = (String)enum.nextElement().toString();
        String filec = (String)inFiles.get(thiskey);
        File f = new File(project + fs + thiskey);
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
 
    RunEmbossApplication rea = new RunEmbossApplication(embossCommand,envp,new File(project));
    
    result.add("cmd");
    result.add(appl + " " + rest);
    result.add("msg");
    result.add(msg);
    result.add("status");
    result.add("0");
    if(rea.isProcessStdout())
    {
      result.add("stdout");
      result.add(rea.getProcessStdout());
    }
    System.out.println("JEMBOSSSERVER running " + appl);
    
//get the output files
    result = loadFilesContent(projectDir,project,result);
    result = loadPNGContent(projectDir,project,result);

    return result;
  }

 
/**
*
* Returns the results for a saved project.
* @param project/directory name
* @param unused
* @return saved results files
*
*/
  public Vector show_saved_results(String project, String cl)
  {
    Vector ssr = new Vector();
   
    project = tmproot.concat(project);
    File projectDir = new File(project);
    ssr = loadFilesContent(projectDir,project,ssr);
    ssr = loadPNGContent(projectDir,project,ssr);
        
    ssr.add("status");
    ssr.add("0");

    ssr.add("msg");
    ssr.add("OK");

    return ssr;
  }


/**
*
* Deletes a projects saved results.
* @param project/directory name
* @param unused
* @return message
*
*/
  public Vector delete_saved_results(String project, String cl)
  {
    Vector dsr = new Vector();

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
    return dsr;
  }


/**
*
* List of the saved results on the server.
* @return list of the saved results.
*
*/
  public Vector list_saved_results()
  {
    Vector lsr = new Vector();
   
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
        lsr.add(resFiles[i]);
        lsr.add(fc);
        list = list.concat(resFiles[i] + "\n");
      }
      catch (IOException ioe)
      {
        System.out.println("IOException in list_saved_results " +fs +
                                          resFiles[i] + fs + ".desc");
      }
    }

    lsr.add("list");
    lsr.add(list);

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
 
  public Vector embreo_roots()
  {
    Vector vans = new Vector();
    vans.add("status");
    vans.add("0");
    vans.add("msg");
    vans.add("");
    vans.add("default-root");
    vans.add("HOME");
    vans.add("HOME");
    vans.add("/m3/users/tim");    

    return vans;
  }


  public Vector directory_shortls(String options, String dirname)
  {
    Vector vans = new Vector();
    File dir = new File("/m3/users/tim/");


    File files[] = dir.listFiles();
    
    System.out.println("HERE IN directory_shortls");
    String listFiles = "";
    String listDir = "";

    for(int i=0;i<files.length;i++)
    {
      if(files[i].isDirectory())
        listDir = listDir.concat(files[i].getName() + "\n");
      else
        listFiles = listFiles.concat(files[i].getName() + "\n");
    }

    vans.add("status");
    vans.add("0");
    vans.add("msg");
    vans.add("");
    vans.add("list");
    vans.add(listFiles);
    vans.add("dirlist");
    vans.add(listDir);

    return vans;
  }

 
}

