/*
 * Metadata.java
 *
 * Created on Feb 4th 2003
 */

package org.emboss.grout;

// my latest imports

//import ;

import java.beans.*;
import java.io.StringReader;
import java.util.Enumeration;

import java.io.PrintWriter;
import java.io.BufferedWriter;
import java.io.FileWriter;

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
public class Metadata extends java.lang.Object implements java.io.Serializable
{

		Hashtable values = new Hashtable(); // The key / value pairs
		String type; // The graphic type

		/**
		 * Default constructor
		 * 
		 */
		public Metadata()
		{
				
		}

		/**
		 * Sets a value
		 * 
     * @param key the key string
     * @param value the value string
		 */
		public void setValue(String key, String value)
		{
				// System.out.println("value = |" + value + "|");
				value = value.substring(1, (value.length() - 1));
				values.put(key, value);
		}

		/**
		 * Gets all the values
		 * 
     * @param key the key string
     * @return the value Hashtable
		 */
		public Hashtable getValues()
		{
				return (Hashtable) values;
		}
		
		/**
		 * Gets a value
		 * 
     * @param key the key string
     * @return the value string, null if the key is not in it
		 */
		public String getValue(String key)
		{
				return (String) values.get(key);
		}
		
		/**
		 * Gets number of values
		 * 
     * @return the number of values
		 */
		public Enumeration keys()
		{
				return values.keys();
		}
		
		/**
		 * Sets the type
		 * 
     * @param tp the type
		 */
		public void setType(String tp)
		{
				type = tp;
				return;
		}

		/**
		 * Gets the type
		 * 
     * @return the type
		 */
		public String getType()
		{
				return type;
		}

}
