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
 * IntervalMenu.java
 *
 */

package org.emboss.grout;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.Vector;

/** This is a menu to set the axis marks interval */
public class IntervalMenu extends javax.swing.JFrame
{
    private Vector targets = new Vector();
    
    private AxisMarks xMarks;
    private AxisMarks yMarks;
    
    private JTextField xFirstTick;
    private JTextField yFirstTick;
    private JTextField xInterval;
    private JTextField yInterval;
    private JButton setButton;
    private JButton autoButton;
    
    private	JMenuItem setMenuItem;
    private	JMenuItem autoMenuItem;
    
    public IntervalMenu(AxisMarks x, AxisMarks y)
    {
        super("Tick Marks");
        xMarks = x;
        yMarks = y;
        
        getContentPane().setLayout(new GridLayout(0,1));
        
        if(x != null)
        {
            String firstTickString = String.valueOf(x.getFirstTick());
            // System.out.println(" = " + );
            if(firstTickString.endsWith(".0"))
            {
                firstTickString = firstTickString.substring
                (0, (firstTickString.length() - 2));
            }
            String intervalString = String.valueOf(x.getInterval());
            if(intervalString.endsWith(".0"))
            {
                intervalString = intervalString.substring
                (0, (intervalString.length() - 2));
            }
            
            getContentPane().add(new JLabel(" X Axis First Tick"));
            xFirstTick = new JTextField(firstTickString);
            xFirstTick.addActionListener(new MenuListener());
            getContentPane().add(xFirstTick);
            getContentPane().add(new JLabel(" X Axis Interval"));
            xInterval = new JTextField(intervalString);
            xInterval.addActionListener(new MenuListener());
            getContentPane().add(xInterval);
        }
        if(y != null)
        {
            String firstTickString = String.valueOf(y.getFirstTick());
            // System.out.println(" = " + );
            if(firstTickString.endsWith(".0"))
            {
                firstTickString = firstTickString.substring
                (0, (firstTickString.length() - 2));
            }
            String intervalString = String.valueOf(y.getInterval());
            if(intervalString.endsWith(".0"))
            {
                intervalString = intervalString.substring
                (0, (intervalString.length() - 2));
            }
            
            getContentPane().add(new JLabel(" Y Axis First Tick"));
            yFirstTick = new JTextField(firstTickString);
            yFirstTick.addActionListener(new MenuListener());
            getContentPane().add(yFirstTick);
            getContentPane().add(new JLabel(" Y Axis Interval"));
            yInterval = new JTextField(intervalString);
            yInterval.addActionListener(new MenuListener());
            getContentPane().add(yInterval);
        }
        commonConstrution();
    }
    
    public IntervalMenu(AxisMarks marks)
    {
        super("Tick Marks");
        xMarks = marks;
        
        getContentPane().setLayout(new GridLayout(0,1));
        
        String firstTickString = String.valueOf(marks.getFirstTick());
        // System.out.println(" = " + );
        if(firstTickString.endsWith(".0"))
        {
            firstTickString = firstTickString.substring
            (0, (firstTickString.length() - 2));
        }
        String intervalString = String.valueOf(marks.getInterval());
        if(intervalString.endsWith(".0"))
        {
            intervalString = intervalString.substring
            (0, (intervalString.length() - 2));
        }
        
        getContentPane().add(new JLabel("First Tick"));
        xFirstTick = new JTextField(firstTickString);
        xFirstTick.addActionListener(new MenuListener());
        getContentPane().add(xFirstTick);
        getContentPane().add(new JLabel("Interval"));
        xInterval = new JTextField(intervalString);
        xInterval.addActionListener(new MenuListener());
        getContentPane().add(xInterval);
        commonConstrution();
    }
    
    private void commonConstrution()
    {
        //				JPanel buttons = new JPanel(new BorderLayout());
        JPanel buttons = new JPanel();
        setButton = new JButton(" Set ");
        setButton.addActionListener(new MenuListener());
        buttons.add(setButton);
        
        autoButton = new JButton("Auto");
        autoButton.addActionListener(new MenuListener());
        buttons.add(autoButton);
        
        JButton closeButton = new JButton("Close");
        closeButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                dispose();
            }
        });
        buttons.add(closeButton);
        getContentPane().add(buttons);
        
        addWindowListener(new java.awt.event.WindowAdapter()
        {
            public void windowClosing(java.awt.event.WindowEvent evt)
            {
                dispose();
            }
        });
        
        JMenuBar menuBar = new JMenuBar();
        
        JMenu submenu = new JMenu("Ticks");
        
        JMenuItem closeMenuItem = new JMenuItem("Close");
        closeMenuItem.setAccelerator(KeyStroke.getKeyStroke
        (KeyEvent.VK_E, ActionEvent.CTRL_MASK));
        closeMenuItem.getAccessibleContext().setAccessibleDescription("close");
        closeMenuItem.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                dispose();
            }
        });
        menuBar.add(closeMenuItem);
        
        setMenuItem = new JMenuItem("Set");
        setMenuItem.getAccessibleContext().setAccessibleDescription("set");
        setMenuItem.addActionListener(new MenuListener());
        submenu.add(setMenuItem);
        
        autoMenuItem = new JMenuItem("Auto");
        autoMenuItem.getAccessibleContext().setAccessibleDescription("auto");
        autoMenuItem.addActionListener(new MenuListener());
        submenu.add(autoMenuItem);
        menuBar.add(submenu);
        
        setJMenuBar(menuBar);
        
        pack();
    }
    
    protected void notifyTargets(String propertyName, Object oldValue, 
																 Object newValue)
    {
        System.out.println("xMarks = " + xMarks);
        System.out.println("yMarks = " + yMarks);
        if(xMarks != null)
        {
            xMarks.changeFont(propertyName, null);
        }
        if(yMarks != null)
        {
            yMarks.changeFont(propertyName, null);
        }
        
        System.out.println("notifyTargets in IntervalMenu = " + propertyName);
        
        Vector l;
        IntervalMenuEvent s = new IntervalMenuEvent((Object) this, 
																										propertyName, oldValue, 
																										newValue);
        synchronized(this)
        {
            l = (Vector) targets.clone();
        }
        for (int i = 0; i < l.size(); i++)
        {
            IntervalMenuListener sl = (IntervalMenuListener) l.elementAt(i);
            sl.groutPanelChanged(s);
            // System.out.println("Event in IntervalMenu to = " + sl);
        }
    }
    
    public static void main(String args[])
    {
        IntervalMenu intervalMenu = new IntervalMenu(null, null);
        intervalMenu.show();
    }
    
    class MenuListener implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
            // // System.out.println("ActionEvent in Grout Panel");
            Object source = e.getSource();
            if((source == xFirstTick) || (source == xInterval)
            || (((source == setButton) || (source == setMenuItem)) &&
            xMarks != null))
            {
                try
                {
                    Double.parseDouble(xFirstTick.getText());
                    Double.parseDouble(xInterval.getText());
                    String propertyName =
                    "FontAxisMarksSetInterval" + xFirstTick.getText()
                    + "interval=" + xInterval.getText();
                    xMarks.changeFont(propertyName, null);
                }
                catch(NumberFormatException ex)
                {
                }
            }
            if((source == yFirstTick) || (source == yInterval)
            || (((source == setButton) || (source == setMenuItem)) &&
            (yMarks != null)))
            {
                try
                {
                    Double.parseDouble(yFirstTick.getText());
                    Double.parseDouble(yInterval.getText());
                    String propertyName =
                    "FontAxisMarksSetInterval" + yFirstTick.getText()
                    + "interval=" + yInterval.getText();
                    yMarks.changeFont(propertyName, null);
                }
                catch(NumberFormatException ex)
                {
                }
            }
            if((source == autoButton) || (source == autoMenuItem))
            {
                if(xMarks != null)
                    xMarks.changeFont("FontAxisMarksSetIntervalDefualt", null);
                if(yMarks != null)
                    yMarks.changeFont("FontAxisMarksSetIntervalDefualt", null);
            }
        }
    }
}
