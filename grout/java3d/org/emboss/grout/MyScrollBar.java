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

package org.emboss.grout;

import javax.swing.*;

import javax.swing.DefaultBoundedRangeModel;
import javax.swing.BoundedRangeModel;
import java.awt.event.AdjustmentListener;
import java.awt.event.AdjustmentEvent;
import java.awt.Adjustable;
import java.util.Vector;
import java.awt.Dimension;
import java.awt.BorderLayout;
//import ;
//import ;



/** This scroll bar handles values other that int's */
public class MyScrollBar extends JPanel implements java.io.Serializable
{
    
    JScrollBar bar;
    double value;
    double extent;
    double min;
    double max;
    
    private Vector targets = new Vector();
    
    private static int DIVIDE_BY = 10000;
    public static int VERTICAL = JScrollBar.VERTICAL;
    public  static int HORIZONTAL = JScrollBar.HORIZONTAL;
    
    MyScrollBar(int orientation, int value, int extent, int min, int max)
    {
        super(new BorderLayout());
        bar = new JScrollBar(orientation, value * DIVIDE_BY,
        extent * DIVIDE_BY,
        min * DIVIDE_BY, max * DIVIDE_BY);
        bar.setUnitIncrement(DIVIDE_BY);
        bar.addAdjustmentListener(new ScrollBarListener());
        add(bar, BorderLayout.CENTER);
        setBorder(BorderFactory.createEmptyBorder(0,0,0,0));
        //				setPreferredSize(new Dimension(440, 25));
        //				bar.setPreferredSize(new Dimension(440, 25));
        //				pack();
        setValues();
    }
    
    MyScrollBar()
    {
        super(new BorderLayout());
        bar = new JScrollBar();
        bar.setUnitIncrement(DIVIDE_BY);
        bar.addAdjustmentListener(new ScrollBarListener());
        add(bar, BorderLayout.CENTER);
        setBorder(BorderFactory.createEmptyBorder(0,0,0,0));
        //				pack();
        //				setPreferredSize(new Dimension(440, 25));
        //				bar.setPreferredSize(new Dimension(440, 25));
        setValues();
    }
    
    public synchronized void addMyScrollBarListener(MyScrollBarListener
    listener)
    {
        targets.addElement(listener);
    }
    
    public synchronized void removeMyScrollBarListener(MyScrollBarListener
    listener)
    {
        targets.removeElement(listener);
    }
    
    protected void notifyTargets(String propertyName, Object oldValue,
    Object newValue)
    {
        Vector l;
        MyScrollBarEvent s = new MyScrollBarEvent((Object) this,
        propertyName, oldValue,
        newValue);
        synchronized(this)
        {
            l = (Vector) targets.clone();
        }
        for (int i = 0; i < l.size(); i++)
        {
            MyScrollBarListener sl = (MyScrollBarListener) l.elementAt(i);
            sl.myScrollBarChanged(s);
        }
    }
    
    public double getValue()
    {
        //				printValues();
        return value;
    }
    public double getExtent()
    {
        //				printValues();
        return extent;
    }
    public double getMinimum()
    {
        //				printValues();
        return min;
    }
    public double getMaximum()
    {
        //				printValues();
        return max;
    }
    
    public void printValues()
    {
        System.out.println("value = " + value + " extent = " + extent +
													 " min = " + min + " max = " + max + " bar.value = "
													 + bar.getValue() + " bar.getExtent = " + 
													 bar.getVisibleAmount() + " bar.getMinimum = " + 
													 bar.getMinimum() + " bar.getMaximum = " + 
													 bar.getMaximum());
    }
    
    public void setValue(double val)
    {
        value = val;
        bar.setValue((int) (val * DIVIDE_BY));
        //				System.out.println("setValue to " + val);
        //				printValues();
    }
    public void setExtent(double val)
    {
        extent = val;
        bar.setVisibleAmount((int) (val * DIVIDE_BY));
        //				System.out.println("setExtent to " + val);
        //				printValues();
    }
    public void setMinimum(double val)
    {
        min = val;
        bar.setMinimum((int) (val * DIVIDE_BY));
        //				System.out.println("setMinimum to " + val);
        //				printValues();
    }
    public void setMaximum(double val)
    {
        max = val;
        bar.setMaximum((int) (val * DIVIDE_BY));
        //				System.out.println("setMaximum to " + val);
        //				printValues();
    }
    
    public void setModel(double vl, double ex, double mn, double mx)
    {
        value = vl;
        extent = ex;
        min = mn;
        max = mx;
        BoundedRangeModel model = bar.getModel();
        model.setRangeProperties((int) (value * DIVIDE_BY),
        (int) (extent * DIVIDE_BY),
        (int) (min * DIVIDE_BY),
        (int) (max * DIVIDE_BY),
        model.getValueIsAdjusting());
        bar.setModel(model);
        //				System.out.println("setModel to " + model);
        //				printValues();
    }
    
    private void setValues()
    {
        value = ((double) bar.getValue()) / DIVIDE_BY;
        extent = ((double) bar.getVisibleAmount()) / DIVIDE_BY;
        min = ((double) bar.getMinimum()) / DIVIDE_BY;
        max = ((double) bar.getMaximum()) / DIVIDE_BY;
    }
    
    public static double convertValueToRequiredNumber(MyScrollBar val)
    {
        if(val.bar.getOrientation() == VERTICAL)
        {
            return val.getMaximum() + val.getMinimum() - val.getValue();
        }
        else if(val.bar.getOrientation() == HORIZONTAL)
        {
            return val.getValue();
        }
        else
        {
            System.err.println
								("Unknown orientation, please use VERTICAL or HORIZONTAL");
            return 0;
        }
    }
    
    public static double convertRequiredNumberToValue(MyScrollBar val,
    double requiredValue)
    {
        if(val.bar.getOrientation() == VERTICAL)
        {
            return val.getMaximum() + val.getMinimum() - requiredValue;
        }
        else if(val.bar.getOrientation() == HORIZONTAL)
        {
            return requiredValue;
        }
        else
        {
            System.err.println
								("Unknown orientation, please use VERTICAL or HORIZONTAL");
            return 0;
        }
    }
    
    class ScrollBarListener implements AdjustmentListener
    {
        public void adjustmentValueChanged(AdjustmentEvent e)
        {
            Double oldValue = new Double(value);
            setValues();
            notifyTargets("scrollBarAdjusted", oldValue, new Double(value));
        }
    }
    
}
