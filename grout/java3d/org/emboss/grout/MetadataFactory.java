/***************************************************************
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *  @author: Copyright (C) Hugh Morgan
 *
 ***************************************************************/

/*
 * Metadata.java
 *
 * Created on Feb 4th 2003
 */

package org.emboss.grout;

// my latest imports

import org.xml.sax.helpers.DefaultHandler;
import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import javax.xml.parsers.SAXParserFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParser;
import org.xml.sax.InputSource;
// this bit needs 1.4, I have bodged, see below
//import java.util.regex.Matcher;
import java.io.InputStreamReader;
import java.io.Reader;
//import ;

import java.beans.*;
import java.io.StringReader;
import java.util.Enumeration;

import java.io.PrintWriter;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.BufferedReader;
import java.io.FileReader;

import java.util.Vector;
import java.util.Hashtable;
import java.io.File;
import java.net.URL;
import java.net.MalformedURLException;
import java.net.ContentHandlerFactory;
import java.util.EventListener;
/** This generates the meta data for each graphic
 * @author hmorgan2
 */
public class MetadataFactory extends java.lang.Object
{
    
    public static Vector parseStringForMetadata(String input)
    {
        Metadata groutOptions = new Metadata();
        return parseStringForMetadata(input, groutOptions);
    }
    
    public static Vector parseStringForMetadata(String input, Metadata groutOptions)
    {
        Vector returnVector = new Vector();
        
        // Deal with the DTD (a bit dirty)
        String originalHeader = loadFileFromResource
						("resources/originalHeader.txt");
        String newHeader = loadFileFromResource("resources/newHeader.txt");
        input = input.substring(originalHeader.length());
        input = newHeader.concat(input);
        
        //	System.out.println("input af = " + input.substring(0, 200));
        
        // Use an instance of ourselves as the SAX event handler
        MyHandler handler = new MyHandler();
        handler.setMetadatas(returnVector);
        handler.setGroutOptions(groutOptions);
        
        // Use the default (non-validating) parser
        SAXParserFactory factory = SAXParserFactory.newInstance();
        //				factory.setValidating(false);
        //				factory.setNamespaceAware(false);
        try
        {
            // Parse the input
            SAXParser saxParser = factory.newSAXParser();
            //		System.out.println("input = " + input);
            saxParser.parse( new InputSource(new StringReader(input)),
            handler );
            // InputSource inputSource = sortHeaders(new InputSource
            // 													((new StringReader(input))),
            // 													originalHeader, newHeader);
            // saxParser.parse(inputSource,
            // 								handler);
            
        } catch (Throwable t)
        {
            t.printStackTrace();
        }
        
        return returnVector;
    }
    
    /** This deals with the difference in the DOCTYPE line requored for xj3d and JAXP */    
    public static InputSource sortHeaders(InputSource input)
    {
        String originalHeader = loadFileFromResource("resources/originalHeader.txt");
        String newHeader = loadFileFromResource("resources/newHeader.txt");
        return sortHeaders(input, originalHeader, newHeader);
    }
    public static InputSource sortHeaders(InputSource input,
    String originalHeader,
    String newHeader)
    {
        
        String temp1 = new String("getCharacterStream");
        char posOne = temp1.charAt(1);
        String temp2 = String.valueOf(posOne);
        char[] pos2 = new char[1];
        pos2[0] = posOne;
        String temp3 = new String(pos2);
        
        
        Reader reader = input.getCharacterStream();
        // int thisChar = 0;
        char thisChar = 0;
        int inputLength = 0;
        String correctBit = null;
        String tempBuffer = new String();
        String thisCharStr = null;
        try
        {
            while(thisChar != -1)
            {
                thisChar = (char)reader.read();
                ++inputLength;
                thisCharStr = String.valueOf(thisChar);
                //tempBuffer.concat(String.valueOf(thisChar));
                tempBuffer.concat(thisCharStr);
            }
            // System.out.println("tempBuffer = " + tempBuffer);
            
            // reader.reset();
            ((java.io.PushbackReader) reader).unread(tempBuffer.toCharArray());
            
            // System.out.println("inputLength = " + inputLength);
            
            char[] decentBit = new char[inputLength - originalHeader.length() - 1];
            
            for(int i = 0; i < (inputLength - 1); ++i)
            {
                if(i >= originalHeader.length())
                {
                    decentBit[i - originalHeader.length()] = (char)reader.read();
                }
                else
                {
                    reader.read();
                }
            }
            
            // 	reader.read(decentBit, originalHeader.length(),
            // 				(inputLength - originalHeader.length()));
            
            // System.out.println("char[0] = " + decentBit[0]);
            // System.out.println("char[10] = " + decentBit[10]);
            // System.out.println("char[100] = " + decentBit[100]);
            
            correctBit = new String(decentBit);
        }
        catch(java.io.IOException e)
        {
            e.printStackTrace();
        }
        
        // System.out.println("inputLength = " + inputLength);
        // System.out.println("correctBit = " + correctBit);
                                /*
                                System.out.println(" = " + );
                                 */
        
        return new InputSource(new StringReader(correctBit));
    }
    
    /** Convenience method for printing all the values in a vector */    
    public static void printAll(Vector result)
    {
        for (Enumeration e = result.elements() ; e.hasMoreElements() ;)
        {
            Metadata md = (Metadata) e.nextElement();
            // System.out.println(md + " - " + md.getValues());
        }
    }
    
    
    public static void main(String argv[])
    {
        Vector result = MetadataFactory.parseStringForMetadata
        (loadFile(argv[0]));
        System.out.println("done parsering result.size() = " + result.size());
        MetadataFactory.printAll(result);
        System.exit(0);
    }
    
    
    private static String loadFile(String filename)
    {
        StringBuffer inputReader = new StringBuffer();
        try
        {
            BufferedReader br = new BufferedReader(new FileReader(filename));
            String text;
            while ((text = br.readLine()) != null)
            {
                inputReader = inputReader.append(text).append("\n");
            }
            // Always close a file after opening.
            br.close();
        } catch(java.io.IOException e)
        {
            e.printStackTrace();
        }
        return inputReader.toString();
    }
    
    private static String loadFileFromResource(String name)
    {
        StringBuffer inputReader = new StringBuffer();
        try
        {
            // System.out.println(" = " + );
            BufferedReader br = new BufferedReader
								(new InputStreamReader
								 (MetadataFactory.class.getClassLoader().getResourceAsStream
									(name)));
            //						BufferedReader br = new BufferedReader
            //		(new InputStreamReader(ClassLoader.getSystemClassLoader()
            //													 .getResourceAsStream(name)));
            String text;
            while ((text = br.readLine()) != null)
            {
                inputReader = inputReader.append(text).append("\n");
            }
            // Always close a file after opening.
            br.close();
        } catch(java.io.IOException e)
        {
            e.printStackTrace();
        }
                                /*
                        catch(ClassNotFoundException e)
                        {
                                        e.printStackTrace();
                        }
                                 */
        return inputReader.toString();
    }
    
}

/** Handler for this parser */
class MyHandler extends DefaultHandler
{
    /** true while inside a ProtoInstance tag */    
    boolean inProtoInstance = false;
    /** one graphics metadata */    
    Metadata metadata;
    /** options for all the graphics in this file */    
    Metadata groutOptions;
    Vector metadatas = null;
    
    public MyHandler()
    {
        super();
    }
    
    public void setMetadatas(Vector mds)
    {
        metadatas = mds;
    }
    
    public void setGroutOptions(Metadata go)
    {
        groutOptions = go;
    }
    
    public void startElement(java.lang.String uri,
    java.lang.String localName,
    java.lang.String qName,
    Attributes attributes)
    throws SAXException
    {
        if(qName.equals("ProtoInstance"))
        {
            metadata = new Metadata();
            metadata.setType(qName);
            inProtoInstance = true;
        }
        else if(qName.equals("fieldValue") && inProtoInstance)
        {
            // sort this out properly
            // this is where you will have most of the prossesing
            String value = attributes.getValue("value");
            if(value != null)
            {
                String name = attributes.getValue("name");
                if(// !name.equals("Graph.colour") &&
                !name.equals("Graph.points") &&
                !name.equals("Graph.index"))
                {
                    metadata.setValue(name, value);
                }
            }
        }
        else if(qName.equals("Material") && inProtoInstance)
        {
            // sort this out properly
            // this is where you will have most of the prossesing
            String value = attributes.getValue("diffuseColor");
            if(value != null)
            {
                metadata.setValue("Graph.colour", value);
            }
        }
        if(qName.equals("head"))
        {
        }
        if(qName.equals("meta"))
        {
            String value = attributes.getValue("content");
            if(value != null)
            {
                String name = attributes.getValue("name");
                groutOptions.setValue(name, value);
                // System.out.println("meta getValue = " + value);
            }
        }
    }
    
    public void endElement(java.lang.String uri,
    java.lang.String localName,
    java.lang.String qName)
    throws SAXException
    {
        if(qName.equals("ProtoInstance"))
        {
            metadatas.addElement(metadata);
            inProtoInstance = false;
        }
        
    }
}
