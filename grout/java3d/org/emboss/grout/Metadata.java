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
/** Metadata for one graphic
 * @author hmorgan2
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
        if(value.substring(0,1).equals("\"") &&
        value.substring(value.length() - 1).equals("\""))
        {
            value = value.substring(1, (value.length() - 1));
        }
        values.put(key, value);
    }
    
    /** Gets all the values
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
