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
/**
 *
 * @author  hmorgan2
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
	// this bit needs 1.4, I have bodged, see below
	/*
	String normalHeader = loadFile("resources/regex.txt");
	Pattern p = Pattern.compile(normalHeader);
	Matcher m = p.matcher(input);
	input = m.replaceFirst("");
	*/
	String originalHeader = loadFileFromResource("resources/originalHeader.txt");
	String newHeader = loadFileFromResource("resources/newHeader.txt");
	input = input.substring(originalHeader.length());
	//	input = input.concat(newHeader);
	input = newHeader.concat(input);

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
          
	    } catch (Throwable t) 
		{
		    t.printStackTrace();
		}

	return returnVector;
    }

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
						//						BufferedReader br = new BufferedReader(new InputStreamReader(getClass().getClassLoader().getResourceAsStream(name)));
						BufferedReader br = new BufferedReader(new InputStreamReader(ClassLoader.getSystemClassLoader().getResourceAsStream(name)));
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

}

class MyHandler extends DefaultHandler
{
		boolean inProtoInstance = false;
		Metadata metadata;
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
								//								metadata.setName(getNameFromAttributeList(attributes));
								//								// System.out.println("metadata.setType");
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
												if(!name.equals("Graph.colour") &&
													 !name.equals("Graph.points") &&
													 !name.equals("Graph.index"))
														{
																metadata.setValue(name, value);
																//																		// System.out.println("metadata.setValue = " + name + " " + value);
														}
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
								//								// System.out.println("metadatas.addElement(metadata)");
								metadatas.addElement(metadata);
								inProtoInstance = false;
						}
				
		}
}
