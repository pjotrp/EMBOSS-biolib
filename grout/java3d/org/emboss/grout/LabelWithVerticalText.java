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

public class LabelWithVerticalText extends JLabel implements java.io.Serializable
{
		String text = null;

		public static final int HORIZONTAL = 0;
		public static final int VERTICAL = 1;

		private int orientation = VERTICAL;

		private Font exFont = new Font("TimesRoman",Font.PLAIN, 16);
		private Color foreground = Color.black;
		private Color background = Color.white;

		public LabelWithVerticalText ()
		{
				super();
		}
		
		public LabelWithVerticalText (String txt)
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

				float textPosition = (((float) labelLength) - ((float) textLength)) / 2;
				if(textPosition < 0)
				{
						textPosition = 0.0f;
				}

				g2d.setFont(exFont);
				g2d.setBackground(background);
				g2d.setColor(foreground);
				if(orientation == VERTICAL)
				{
						g2d.rotate(Math.PI * 0.5);
				}
				// System.out.println("textPosition = " + textPosition);
				// g2d.drawString(text, 2.0f, textPosition);
				g2d.drawString(text, textPosition, 0.0f);
    }

		private double caluculateTextLength(String text, Font font)
		{
				// System.out.println("font.getStringBounds() = " + font.getStringBounds(text, new FontRenderContext(null, false, true)));
				/*
				System.out.println(" = " + );
				System.out.println(" = " + );
				*/
				return font.getStringBounds(text, new FontRenderContext(null, false, true)).getWidth();
		}

		private int caluculateLabelLength(JLabel label)
		{

				return label.getHeight();
		}

  		
}
