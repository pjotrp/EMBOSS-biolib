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

/**
 * LabelWithVerticalText.java
 *
 *
 * Created: Fri Mar 28 10:34:34 2003
 *
 * @author <a href="mailto:">Mr H. Morgan</a>
 * @version
 */

import java.awt.*;
import java.awt.geom.*;
import java.awt.image.BufferedImage;
import javax.swing.*;
import java.awt.font.FontRenderContext;

/** This is for the titles.  It handles the text being writen verticlly for the
 * y-axis
 */
public class LabelWithVerticalText extends JLabel implements java.io.Serializable
{
    String text = null;
    
    public static final int HORIZONTAL = 0;
    public static final int VERTICAL = 1;
    
    private int orientation = VERTICAL;
    
    private Font exFont = new Font("TimesRoman",Font.PLAIN, 16);
    private Color foreground = Color.black;
    private Color background = Color.white;
    private	double textHeight = 15;
    
    public LabelWithVerticalText()
    {
        super();
    }
    
    public LabelWithVerticalText(String txt)
    {
        super();
        text = txt;
    }
    
    public void setText(String txt)
    {
        text = txt;
        repaint();
    }
    
    public String getText()
    {
        return text;
    }
    
    public void setFont(Font font)
    {
        exFont = font;
        checkFontHeight();
        repaint();
    }
    
    public void setForeground(Color colour)
    {
        foreground = colour;
        repaint();
    }
    
    public void setBackground(Color colour)
    {
        background = colour;
        super.setBackground(background);
        repaint();
    }
    
    public void setOrientation(int ori)
    {
        orientation = ori;
    }
    
    public void paint(Graphics g)
    {
        drawText(g);
    }
    
    private void drawText(Graphics g)
    {
        // System.out.println("drawText text = " + text);
        Graphics2D g2d = (Graphics2D)g;
        
        double textLength = caluculateTextLength(text, exFont);
        int labelLength = caluculateLabelLength(this);
        Dimension size = getSize();
        
        // System.out.println("orientation = " + orientation);
        // System.out.println("size = " + size);
        // System.out.println("labelLength = " + labelLength);
        // System.out.println("textLength = " + textLength);
        
        float textPosition;
        if(orientation == VERTICAL)
        {
            textPosition = size.height -
            ((((float) labelLength) - ((float) textLength)) / 2);
            if(textPosition > labelLength)
            {
                textPosition = labelLength;
            }
        }
        else
        {
            textPosition = ((((float) labelLength) - ((float) textLength)) / 2);
            if(textPosition < 0)
            {
                textPosition = 0.0f;
            }
        }
        
        g2d.setFont(exFont);
        g2d.setBackground(background);
        g2d.setColor(foreground);
        // System.out.println("textPosition = " + textPosition);
        if(orientation == VERTICAL)
        {
            g2d.rotate(Math.PI * 1.5);
            g2d.drawString(text, -textPosition, ((float) textHeight - 2));
        }
        else
        {
            // System.out.println("text = " + text);
            // System.out.println("getSize() = " + getSize());
                                                /*
                                                System.out.println(" = " + );
                                                 */
            // g2d.drawString(text, 2.0f, textPosition);
            // g2d.drawString(text, 0.0f, 0.0f);
            // g2d.drawString(text, textPosition, ((float) textHeight));
            g2d.drawString(text, textPosition, ((float) (textHeight * 0.7)));
        }
    }
    
    private double caluculateTextLength(String text, Font font)
    {
        // if(orientation == VERTICAL)
        if(true)
        {
            return font.getStringBounds
            (text, new FontRenderContext(null, false, true)).getWidth();
        }
        else
        {
            return font.getStringBounds
            (text, new FontRenderContext(null, false, true)).getHeight();
        }
    }
    
    /** Makes sure the text will fit in the label */    
    private void checkFontHeight()
    {
        textHeight = exFont.getStringBounds
        (text, new FontRenderContext(null, false, true)).getHeight();
        if(orientation == VERTICAL)
        {
            int textHeightInt = ((int) Math.floor(textHeight));
            // System.out.println("textHeight = " + textHeight);
            // System.out.println("getSize().width = " + getSize());
            if(getSize().width != textHeight)
            {
                super.setSize(textHeightInt, getSize().height);
                super.setMinimumSize
                (new Dimension(textHeightInt, getSize().height));
                super.setMaximumSize
                (new Dimension(textHeightInt, getSize().height));
                super.setPreferredSize
                (new Dimension(textHeightInt, getSize().height));
                // revalidate();
                resizeAllParents();
            }
        }
        else
        {
            int textHeightInt = ((int) Math.floor(textHeight));
            // System.out.println("textHeight = " + textHeight);
            // System.out.println("getSize().width = " + getSize());
            if(getSize().height != textHeight)
            {
                super.setSize
                (new Dimension(getSize().width, textHeightInt));
                super.setMinimumSize
                (new Dimension(getSize().width, textHeightInt));
                super.setMaximumSize
                (new Dimension(getSize().width, textHeightInt));
                super.setPreferredSize
                (new Dimension(getSize().width, textHeightInt));
                // revalidate();
                resizeAllParents();
            }
        }
    }
    
    private int caluculateLabelLength(JLabel label)
    {
        if(orientation == VERTICAL)
        {
            return label.getHeight();
        }
        else
        {
            return label.getWidth();
        }
    }
    
    /** redraws all the parents so this fits */    
    private void resizeAllParents()
    {
        for(Component component = this; component != null;
        component = component.getParent())
        {
            if(component instanceof GroutPanel)
            {
                // ((JComponent) component).revalidate();
                ((GroutPanel) component).removeBlankBoxes(false);
            }
            else if(component instanceof JComponent)
            {
                ((JComponent) component).revalidate();
            }
            else
            {
                component.repaint();
            }
        }
    }
    
}
