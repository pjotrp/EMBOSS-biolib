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
 * RendWindContain.java
 *
 * Created on April 24, 2002, 9:31 AM
 */

package org.emboss.grout;

import javax.swing.*;
import javax.swing.event.*;
import java.awt.event.ComponentListener;

import java.beans.*;
import java.awt.Component;
import java.beans.PropertyChangeSupport;
import java.awt.Canvas;
import java.awt.Frame;

import java.awt.*;
import java.awt.event.*;
import java.util.Vector;

// the ones from rendering window
import javax.media.j3d.Canvas3D;

// the ones from loader
// Standard imports
import java.applet.Applet;
import java.io.*;
import java.awt.*;
import java.awt.event.*;
import javax.media.j3d.*;
import javax.vecmath.*;
import java.net.URL;
import java.net.MalformedURLException;

// Application specific imports
import com.sun.j3d.utils.applet.MainFrame;
import com.sun.j3d.utils.universe.*;
import com.sun.j3d.utils.behaviors.mouse.MouseRotate;
import com.sun.j3d.utils.behaviors.mouse.MouseZoom;
import com.sun.j3d.utils.behaviors.mouse.MouseTranslate;

import com.sun.j3d.loaders.Scene;

//import java.util.regex.Matcher;

/** This object includes all the controls.
 * @author hmorgan2
 */
public class ControlContainer extends java.awt.Panel implements
java.io.Serializable, ViewPositionModelListener, GroutPanelListener,
ComponentListener
{
    
    private static final String PROP_SAMPLE_PROPERTY = "SampleProperty";
    /** Not used */    
    private Vector targets = new Vector();
    
    private JSlider scaleYFactor, transformZFactor, scaleXFactor;
    private MyScrollBar transformYFactor, transformXFactor;
    private JCheckBox lockScaleFactors;
    private JButton reset;
    private boolean scaleFactorsLocked = false;
    
    /** This panel has the Y axis */
    private JPanel centrePanel;
    
    /** This panel has the x axis */
    private JPanel bottomPanel;
    
    /** This has the lock and reset buttons */    
    private JPanel   lockButtonPanel;
    
    private double xMin, xMax, yMin, yMax;
    
    /** Creates new RendWindContain */
    public ControlContainer()
    {
        super(new BorderLayout());
        addSliders();
    }
    
    private void addSliders()
    {
        
        transformXFactor = new MyScrollBar(JScrollBar.HORIZONTAL, 0, 10, 0, 100);
        transformXFactor.addMyScrollBarListener(new ScrollBarListener());
        transformXFactor.setBorder(BorderFactory.createEmptyBorder(0,0,0,0));
        transformXFactor.setPreferredSize(new Dimension(440, 17));
        
        transformYFactor = new MyScrollBar(JScrollBar.VERTICAL, 0, 10, 0, 100);
        transformYFactor.addMyScrollBarListener(new ScrollBarListener());
        transformYFactor.setBorder(BorderFactory.createEmptyBorder(0,0,0,0));
        transformYFactor.setPreferredSize(new Dimension(17, 475));
        
        
        transformZFactor = new JSlider(JSlider.VERTICAL, 0, 1, 1);
        transformZFactor.addChangeListener(new SliderListener());
        transformZFactor.setMajorTickSpacing(300);
        transformZFactor.setMinorTickSpacing(50);
        transformZFactor.setPaintTicks(true);
        transformZFactor.setPaintLabels(false);
        transformZFactor.setBorder(BorderFactory.createEmptyBorder(0,0,0,0));
        
        scaleXFactor = new JSlider(JSlider.HORIZONTAL, 10, 150, 10);
        scaleXFactor.addChangeListener(new SliderListener());
        scaleXFactor.setMajorTickSpacing(30);
        scaleXFactor.setMinorTickSpacing(10);
        scaleXFactor.setPaintTicks(true);
        scaleXFactor.setPaintLabels(false);
        scaleXFactor.setBorder(BorderFactory.createEmptyBorder(0,0,0,0));
        
        scaleYFactor = new JSlider(JSlider.VERTICAL, 10, 150, 10);
        scaleYFactor.addChangeListener(new SliderListener());
        scaleYFactor.setMajorTickSpacing(30);
        scaleYFactor.setMinorTickSpacing(10);
        scaleYFactor.setPaintTicks(true);
        scaleYFactor.setPaintLabels(false);
        scaleYFactor.setBorder(BorderFactory.createEmptyBorder(0,0,0,0));
        
        lockButtonPanel = new JPanel(new BorderLayout());
        
        lockScaleFactors = new JCheckBox("Lock");
        lockScaleFactors.addActionListener(new CheckBoxListener());
        lockButtonPanel.add(lockScaleFactors, BorderLayout.NORTH);
        
        reset = new JButton("Reset");
        reset.addActionListener(new ButtonListener());
        lockButtonPanel.add(reset, BorderLayout.SOUTH);
        
        //        JPanel centrePanel = new JPanel(new FlowLayout());
        //        centrePanel = new JPanel(new GridLayout(1,3));
        centrePanel = new JPanel(new BorderLayout());
        centrePanel.setBackground(Color.blue);
        centrePanel.add(transformYFactor, BorderLayout.CENTER);
        centrePanel.add(scaleYFactor, BorderLayout.WEST);
        //        centrePanel.add(transformZFactor, BorderLayout.EAST);
        //        centrePanel.add(lockScaleFactors, BorderLayout.EAST);
        //		useButton();
        
        // this is a test bit
        //			test.setBackground(Color.BLACK);
        //			test.setForeground(Color.BLACK);
        //			test.setPreferredSize(new Dimension(440, 1));
        //			test.setMaximumSize(new Dimension(440, 1));
        
        bottomPanel = new JPanel(new BorderLayout());
        bottomPanel.add(scaleXFactor, BorderLayout.SOUTH);
        bottomPanel.add(transformXFactor, BorderLayout.NORTH);
        //			bottomPanel.add(test, BorderLayout.CENTER);
        //        bottomPanel.add(, BorderLayout.EAST);
        
        //        add(transformYFactor, BorderLayout.CENTER);
        //        add(centrePanel, BorderLayout.CENTER);
        //        add(bottomPanel, BorderLayout.SOUTH);
        //        add(transformZFactor, BorderLayout.WEST);
        
        // Listen to the transformXAxis, so you can keep them the same size
        transformXFactor.addComponentListener(this);
    }
    
    public JPanel getCentrePanel()
    {
        return centrePanel;
    }
    public JPanel getBottomPanel()
    {
        return bottomPanel;
    }
    public JPanel getLockButtonPanel()
    {
        return lockButtonPanel;
    }
    
    /** Not used */    
    private void useSlider()
    {
        // // System.out.println("useSlider");
        //		centrePanel.remove(lockScaleFactors);
        centrePanel.add(transformZFactor, BorderLayout.EAST);
        //		lockButtonPanel.remove(lockScaleFactors);
        centrePanel.revalidate();
    }
    
    /** Not used */    
    private void useButton()
    {
        // // System.out.println("useButton");
        //		centrePanel.remove(transformZFactor);
        //		lockButtonPanel.add(lockScaleFactors);
        centrePanel.remove(transformZFactor);
        centrePanel.revalidate();
        //		lockButtonPanel.revalidate();
    }
    
    public synchronized void addControlContainerListener(ControlContainerListener listener)
    {
        targets.addElement(listener);
    }
    
    public synchronized void removeControlContainerListener(ControlContainerListener listener)
    {
        targets.removeElement(listener);
    }
    
    protected void notifyTargets(String propertyName, Object oldValue, 
																 Object newValue)
    {
        Vector l;
        ControlContainerEvent s = new ControlContainerEvent((Object) this, 
																														propertyName, 
																														oldValue, 
																														newValue);
        synchronized(this)
        {
            l = (Vector) targets.clone();
        }
        for (int i = 0; i < l.size(); i++)
        {
            ControlContainerListener sl = (ControlContainerListener) l.elementAt(i);
            sl.controlContainerChanged(s);
        }
    }
    
    /* what is this doing? hugh */
    public void componentResized(ComponentEvent e)
    {
        if(e.getSource() == transformXFactor)
        {
            int x = ((int)( transformXFactor.getHeight() / 1.35));
        }
    }
    
    public void componentMoved(ComponentEvent e)
    {
    }
    public void componentShown(ComponentEvent e)
    {
    }
    public void componentHidden(ComponentEvent e)
    {
    }
    
    public void viewPositionModelChanged(ViewPositionModelEvent e)
    {
        String whatChanged = e.getPropertyName();
        ViewPositionModel model = (ViewPositionModel) e.getSource();
        if(whatChanged.equals("parser"))
        {
            
            //      model.printValues();
            
            transformXFactor.setModel(model.getBottomX(), model.getDisplaySizeX(),
            model.getMinX(), model.getMaxX());
            transformYFactor.setModel((model.getMaxY() -  model.getTopY() +
            model.getMinY()), model.getDisplaySizeY(),
            model.getMinY(), model.getMaxY());
            
            
            transformZFactor.setModel(new DefaultBoundedRangeModel(((int) model.getOverallScale()), 0, 0, ((int) model.getOverallScale())));
        }
        if(whatChanged.equals("reset"))
        {
            // // System.out.println("viewPositionModel reset in controls");
            
            //      model.printValues();
            
            transformXFactor.setModel(model.getBottomX(), model.getDisplaySizeX(),
            model.getMinX(), model.getMaxX());
            transformYFactor.setModel((model.getMaxY() -  model.getTopY() +
            model.getMinY()), model.getDisplaySizeY(),
            model.getMinY(), model.getMaxY());
            
            scaleXFactor.setValue(10);
            scaleYFactor.setValue(10);
            
        }
        if(whatChanged.equals("scaleXFactor") || (whatChanged.equals("scaleYFactor") && model.getScaleFactorsLocked()) || whatChanged.equals("fitViewToWindow"))
        {
            //				System.out.println("Controls scaleXFactor");
            //				model.printValues();
            //DefaultBoundedRangeModel(int value, int extent, int min, int max)
            //min <= value <= value+extent <= max
            transformXFactor.setModel(model.getBottomX(), model.getDisplaySizeX(),
            model.getMinX(), model.getMaxX());
            // System.out.println("Setting values transformXFactor");
        }
        if(whatChanged.equals("scaleYFactor") || (whatChanged.equals("scaleXFactor") && model.getScaleFactorsLocked()) || whatChanged.equals("fitViewToWindow"))
        {
            transformYFactor.setModel((model.getMaxY() -  model.getTopY() +
            model.getMinY()), model.getDisplaySizeY(),
            model.getMinY(), model.getMaxY());
        }
        if(whatChanged.equals("transformZFactor"))
        {
            // // System.out.println("transformZFactor changed in Controls");
            transformXFactor.setModel(model.getBottomX(), model.getDisplaySizeX(),
            model.getMinX(), model.getMaxX());
            transformYFactor.setModel((model.getMaxY() -  model.getTopY() +
            model.getMinY()), model.getDisplaySizeY(),
            model.getMinY(), model.getMaxY());
        }
    }
    
    public void groutPanelChanged(GroutPanelEvent e)
    {
        GroutPanel source = (GroutPanel)(e.getSource());
        String whatChanged = e.getPropertyName();
        //				System.out.println("groutPanelChanged by " + whatChanged + " in = " + this);
        if(whatChanged.equals("useButton"))
        {
            useButton();
        } else if(whatChanged.equals("useSlider"))
        {
            useSlider();
        } else if(whatChanged.equals("FitViewToWindow"))
        {
            transformXFactor.setValue(0);
            transformXFactor.setExtent(transformXFactor.getMaximum()
            - transformXFactor.getMinimum());
            transformYFactor.setValue(transformYFactor.getMaximum());
            transformYFactor.setExtent(transformYFactor.getMaximum()
            - transformYFactor.getMinimum());
        }
    }
    
    /** Handles the events from sliders */    
    class SliderListener implements ChangeListener
    {
        public void stateChanged(ChangeEvent e)
        {
            JSlider source = (JSlider)e.getSource();
            // // System.out.println("Loaded JSlider moved = " + source);
            //                        int fps = (int)source.getValue();
            if(source.equals(transformXFactor))
            {
                notifyTargets("transformXFactor", (Object) new Double(0),  
															(Object) new Double((double)source.getValue()));
            }
            if(source.equals(transformYFactor))
            {
                notifyTargets("transformYFactor", new Double(0),  
															new Double((double)source.getMaximum() - 
																				 (double)source.getValue()));
            }
            if(source.equals(transformZFactor))
            {
                notifyTargets("transformZFactor", new Double(0),  
															new Double((double)source.getValue()));
            }
            if(source.equals(scaleXFactor))
            {
                notifyTargets("scaleXFactor", new Double(0),  
															new Double(((double)source.getValue()/10)));
            }
            if(source.equals(scaleYFactor))
            {
                notifyTargets("scaleYFactor", new Double(0),  
															new Double(((double)source.getValue())/10));
            }
        }
    }
    
    /** Required for serialisable */    
    private void writeObject(java.io.ObjectOutputStream out)
    throws IOException
    {
        
        out.defaultWriteObject();
    }
    
    /** Required for serialisable */    
    private void readObject(java.io.ObjectInputStream in)
    throws IOException, ClassNotFoundException
    {
        // (new Exception()).printStackTrace();
        in.defaultReadObject();
    }
    
    /** HAndles the events from scroll bars */    
    class ScrollBarListener implements MyScrollBarListener, Serializable
    {
        
        public ScrollBarListener()
        {
            // System.out.println("ScrollBarListener = " + this);
            // (new Exception()).printStackTrace();
            
        }
        
        /** Required for serialisable */        
        private void writeObject(java.io.ObjectOutputStream out)
        throws IOException
        {
            // (new Exception()).printStackTrace();
            // System.out.println("ScrollBarListener = " + this);
            out.defaultWriteObject();
        }
        
        /** Required for serialisable */        
        private void readObject(java.io.ObjectInputStream in)
        throws IOException, ClassNotFoundException
        {
            // (new Exception()).printStackTrace();
            in.defaultReadObject();
        }
        
        public void myScrollBarChanged(MyScrollBarEvent e)
        {
            MyScrollBar source = (MyScrollBar)e.getSource();
            //                        int fps = (int)source.getValue();
            if(source.equals(transformXFactor))
            {
                notifyTargets("transformXFactor", (Object) new Double(0),  (Object) new Double((double)source.getValue()));
            }
            if(source.equals(transformYFactor))
            {
                notifyTargets("transformYFactor", (Object) new Double(0),
                (Object) new Double(MyScrollBar.convertValueToRequiredNumber(source)));
            }
        }
    }
    
    /** Handles the events from the buttons */    
    class ButtonListener implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
            JButton source = (JButton)e.getSource();
            if(source.equals(reset))
            {
                notifyTargets("reset", (Object) new Boolean(false),  (Object) new Boolean(true));
            }
        }
    }
    
    /** HAndles the events from check box */    
    class CheckBoxListener implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
            JCheckBox source = (JCheckBox)e.getSource();
            if(source.equals(lockScaleFactors))
            {
                scaleFactorsLocked = !scaleFactorsLocked;
                notifyTargets("lockScaleFactors", (Object) new Boolean(!scaleFactorsLocked),  (Object) new Boolean(scaleFactorsLocked));
                //				LockUnlockScaleFactors(scaleFactorsLocked);
            }
        }
    }
}
