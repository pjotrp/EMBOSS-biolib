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
 * ImageWriter.java
 *
 * Created on October 4th 2002
 */

package org.emboss.grout;

import com.sun.j3d.utils.image.*;
import java.awt.*;
import java.io.*;
import javax.media.j3d.*;
import javax.vecmath.*;
import java.awt.image.BufferedImage;
import com.sun.image.codec.jpeg.*;
import java.util.*;

// Printing imports

import java.awt.print.*;
import javax.imageio.ImageIO;

// JAI imports
// import javax.media.jai.JAI;
import java.awt.image.renderable.ParameterBlock;

// Other imports
import javax.swing.*;

/** This hadles a lot of stuff for writing images.  Static methods and implements
 * printable for writing to printer or postscript
 */
public class ImageWriter implements Printable
{
    
    BufferedImage image = null;
    
    
    public ImageWriter()
    {
        
    }
    
    /**
     * generates a file containing the content of the Canvas3D
     *
     * @param Canvas3D canvas
     * @param File file
     * @param String format
     */
    public static void fromCanvas3DToFile(Canvas3D canvas, File file, 
																					String format )
    {
        BufferedImage image = createBufferedImageFromCanvas3D(canvas);
        writeImageToFile(image, file, format );
    }
    
    
    /**
     * generates a file containing the image
     *
     * @param BufferedImage image
     * @param File file
     * @param String format
     */
    public static void writeImageToFile(BufferedImage image,
    File file, String format )
    {
        if(format.equals("jpeg") || format.equals("png"))
        {
            if(isJava14orMore())
            {
                try
                {
                    ImageIO.write(image, format, file);
                    // System.out.println("Saving using ImageIO");
                    
                    String[] writer_names =  ImageIO.getWriterFormatNames();
                    
                    
                }catch ( IOException e )
                {
                    e.printStackTrace();
                }
            } else if(hasJAI())
            {
                
                String filename = file.getAbsolutePath();
								// need to comment this line back in to use JAI.  
								// Does not compile without JAI with it in
                // JAI.create("filestore", image, filename, format);
            } else
            {
                System.err.println("Format " + format + 
																	 " not supported without Java 1.4+ or JAI.");
            }
        } else if(format.equals("bmp") || format.equals("tiff") 
									|| format.equals("pnm"))
        {
            if(hasJAI())
            {
                String filename = file.getAbsolutePath();
								// need to comment this line back in to use JAI.  
								// Does not compile without JAI with it in
                // JAI.create("filestore", image, filename, format);
            } else
            {
                System.err.println("Format " + format + 
																	 " not supported without JAI.");
            }
        } else if(format.equals("print"))
        {
            ImageWriter writer = new ImageWriter();
            writer.setImage(image);
            PrinterJob printerJob = PrinterJob.getPrinterJob();
            Book book = new Book();
            book.append(writer , new PageFormat());
            printerJob.setPageable(book);
            boolean doPrint = printerJob.printDialog();
            if (doPrint)
            {
                try
                {
                    printerJob.print();
                } catch (PrinterException exception)
                {
                    
                    System.err.println("Printing error: " + exception);
                }
            }
        } else
        {
            System.err.println("Format " + format +
															 " not supported.  Please specify jpeg, " + 
															 "png or print");
        }
    }
    
    /**
     * generates a BufferedImage containing the content of the Canvas3D
     *
     * @param Canvas3D canvas
     * @return The BufferedImage
     */
    public static BufferedImage createBufferedImageFromCanvas3D(Canvas3D canvas)
    {
        // System.out.println("Doing rendering");
        //		canvas.renderOffScreenBuffer();
        // System.out.println("waiting for rendering");
        canvas.waitForOffScreenRendering();
        // System.out.println("Getting GraphicsContext3D");
        GraphicsContext3D  ctx = canvas.getGraphicsContext3D();
        int w = canvas.getWidth();
        int h = canvas.getHeight();
        // System.out.println("Getting BufferedImage");
        BufferedImage bi = new BufferedImage( w, h, BufferedImage.TYPE_INT_RGB );
        // System.out.println("Getting ImageComponent2D");
        ImageComponent2D im = new ImageComponent2D( ImageComponent.FORMAT_RGB, bi );
        // System.out.println("Getting Raster");
        Raster ras = new Raster( new Point3f( -1.0f, -1.0f, -1.0f ), Raster.RASTER_COLOR, 0, 0, w, h, im, null );
        ctx.flush(true);
        ctx.readRaster( ras );
        // System.out.println("Getting ras.getImage().getImage()");
        return ras.getImage().getImage();
    }
    
    /**
     * generates a BufferedImage containing the content of the Component
     *
     * @param Canvas3D canvas
     * @return The BufferedImage
     */
    public static BufferedImage createBufferedImageFromComponent
    (Component component)
    {
        // System.out.println("ImageWriter component = " + component);
        BufferedImage image = (BufferedImage)component.createImage
        (component.getWidth(), component.getHeight());
        // System.out.println("ImageWriter image = " + image);
        Graphics graphics = image.getGraphics();
        if(graphics != null)
        {
            component.paintAll(graphics);
        }
        return image;
    }
    
    private static BufferedImage add3DImage(BufferedImage basic, 
																						BufferedImage complex)
    {
        Graphics2D graphics = basic.createGraphics();
        if(graphics != null)
        {
            graphics.drawImage(complex , null, 
															 (basic.getWidth() - complex.getWidth()), 0);
        }
        return basic;
    }
    
    public static BufferedImage joinImages(BufferedImage[] images, 
																					 int[] xCoords, int[] yCoords)
    {
        if((images.length != xCoords.length) || 
					 (xCoords.length != yCoords.length))
        {
            System.err.println("You are going to get an array error.  " + 
															 "I should do something about this"); 
        }
        
        GridBagLayout gridbagRP = new GridBagLayout();
        JPanel panel = new JPanel(gridbagRP);
        GridBagConstraints cRP = new GridBagConstraints();
        panel.setBackground(Color.white);
        
        int maxX =0;
        int maxY =0;
        
        for(int coordNumber = 0; coordNumber < xCoords.length; ++coordNumber)
        {
            if(xCoords[coordNumber] > maxX)
            {
                maxX = xCoords[coordNumber];
            }
            if(yCoords[coordNumber] > maxY)
            {
                maxY = yCoords[coordNumber];
            }
        }
        
        BufferedImage[][] imageArray = new BufferedImage[maxX + 1][maxY + 1];
        
        // System.out.println("maxX = " + maxX );
        // System.out.println("maxY = " + maxY );
        // System.out.println("images.length = " +  images.length);
        // System.out.println("xCoords.length = " +  xCoords.length);
        // System.out.println("yCoords.length = " +  yCoords.length);
        
        for(int imageNumber = 0; imageNumber < images.length; ++imageNumber)
        {
            imageArray[xCoords[imageNumber]][yCoords[imageNumber]] = images[imageNumber];
        }
        
        int totalWidth = 0;
        int totalHeight = 0;
        
        int[] columnWidth = new int[maxX + 1];
        int[] rowHeight = new int[maxY + 1];
        
        for(int coord = 0; coord < columnWidth.length; ++coord)
        {
            columnWidth[coord] = 0;
        }
        
        for(int coord = 0; coord < rowHeight.length; ++coord)
        {
            rowHeight[coord] = 0;
        }
        
        for(int xCoord = 0; xCoord < imageArray.length; ++xCoord)
        {
            for(int yCoord = 0; yCoord < imageArray[xCoord].length; ++yCoord)
            {
                if(imageArray[xCoord][yCoord] != null)
                {
                    if(columnWidth[xCoord] < 
											 imageArray[xCoord][yCoord].getWidth())
                    {
                        columnWidth[xCoord] = 
														imageArray[xCoord][yCoord].getWidth();
                    }
                    if(rowHeight[yCoord] < 
											 imageArray[xCoord][yCoord].getHeight())
                    {
                        rowHeight[yCoord] = 
														imageArray[xCoord][yCoord].getHeight();
                    }
                }
            }
        }
        
        for(int coord = 0; coord < columnWidth.length; ++coord)
        {
            totalWidth += columnWidth[coord];
        }
        
        for(int coord = 0; coord < rowHeight.length; ++coord)
        {
            totalHeight += rowHeight[coord];
        }
        
        // System.out.println("totalWidth = " + totalWidth);
        // System.out.println("totalHeight = " + totalHeight);
        // System.out.println("columnWidth[0] = " + columnWidth[0]);
        // System.out.println("rowHeight[0] = " + rowHeight[0]);
        
        BufferedImage returnImage = new BufferedImage(totalWidth, 
																											totalHeight, 
																											BufferedImage.TYPE_INT_RGB);
        
        Graphics2D graphics = returnImage.createGraphics();
        if(graphics != null)
        {
            graphics.setColor(Color.black);
            graphics.fillRect(0, 0, totalWidth, totalHeight);
            int xPos = 0;
            for(int xCoord = 0; xCoord < imageArray.length; ++xCoord)
            {
                int yPos = 0;
                for(int yCoord = 0; yCoord < imageArray[xCoord].length; ++yCoord)
                {
                    if(imageArray[xCoord][yCoord] != null)
                    {
                        graphics.drawImage(imageArray[xCoord][yCoord], 
																					 xPos, yPos, null);
                    }
                    yPos += rowHeight[yCoord];
                }
                xPos += columnWidth[xCoord];
            }
        }
        return returnImage;
        
    }
    
    public static BufferedImage joinImagesGirdBag(BufferedImage[] images,
    int[] xCoords,
    int[] yCoords,
    double[] xWeights,
    double[] yWeights,
    int[] widths,
    int[] heights,
    int[] anchors,
    int[] fills,
    Insets[] insets,
    int[] ipadxs,
    int[] ipadys)
    {
        if(xWeights == null)
        {
            xWeights = new double[images.length];
            for(int i = 0; i < images.length; ++i)
            {
                xWeights[i] = 1;
            }
        }
        if(yWeights == null)
        {
            yWeights = new double[images.length];
            for(int i = 0; i < images.length; ++i)
            {
                yWeights[i] = 1;
            }
        }
        if(widths == null)
        {
            widths = new int[images.length];
            for(int i = 0; i < images.length; ++i)
            {
                widths[i] = 1;
            }
        }
        if(heights == null)
        {
            heights = new int[images.length];
            for(int i = 0; i < images.length; ++i)
            {
                heights[i] = 1;
            }
        }
        if(anchors == null)
        {
            anchors = new int[images.length];
            for(int i = 0; i < images.length; ++i)
            {
                anchors[i] = GridBagConstraints.CENTER;
            }
        }
        if(fills == null)
        {
            fills = new int[images.length];
            for(int i = 0; i < images.length; ++i)
            {
                fills[i] = GridBagConstraints.NONE;
            }
        }
        if(insets == null)
        {
            insets = new Insets[images.length];
            for(int i = 0; i < images.length; ++i)
            {
                insets[i] = null;
            }
        }
        if(ipadxs == null)
        {
            ipadxs = new int[images.length];
            for(int i = 0; i < images.length; ++i)
            {
                ipadxs[i] = 0;
            }
        }
        if(ipadys == null)
        {
            ipadys = new int[images.length];
            for(int i = 0; i < images.length; ++i)
            {
                ipadys[i] = 0;
            }
        }
        if((images.length != xCoords.length) || (xCoords.length != yCoords.length))
        {
            System.err.println("You are going to get an array error.  " + 
															 "I should do something about this"); 
        }
        
        GridBagLayout gridbagRP = new GridBagLayout();
        JPanel panel = new JPanel(gridbagRP);
        panel.setBackground(Color.white);
        JFrame frame2 = new JFrame("temp frame");
        frame2.getContentPane().add(panel);
        frame2.show();
        
        for(int i = 0; i < images.length; ++i)
        {
            JPanel imagePanel = new JPanel();
            JFrame frame = new JFrame("temp frame");
            frame.getContentPane().add(imagePanel);
            frame.show();
            // System.out.println("images[i] = " + images[i] + " i = " + i);
            imagePanel.setSize(images[i].getWidth(), images[i].getHeight());
            Graphics2D graphics = (Graphics2D) imagePanel.getGraphics();
            graphics.drawImage(images[i], null, 0, 0);
            
            GridBagConstraints cRP = new GridBagConstraints(xCoords[i],
            yCoords[i],
            widths[i],
            heights[i],
            xWeights[i],
            yWeights[i],
            anchors[i],
            fills[i],
            new Insets(0,0,0,0),
            ipadxs[i],
            ipadys[i]);
            gridbagRP.setConstraints(imagePanel, cRP);
            panel.add(imagePanel);
            frame.pack();
            frame.validate();
        }
        
        return createBufferedImageFromComponent(panel);
    }
    
    public static boolean isJava14orMore()
    {
        boolean result = (
													System.getProperty("java.specification.version"))
						                .equals("1.4");
        return result;
        //		return false;
    }
    
    public static boolean hasJAI()
    {
        boolean result = false;
        String dirs = System.getProperty("java.ext.dirs") + 
						System.getProperty("path.separator") + 
						System.getProperty("java.class.path");
        StringTokenizer st = new StringTokenizer(dirs, 
																								 System.getProperty("path.separator"));
        while (st.hasMoreTokens() && !result)
        {
            File dir = new File(st.nextToken());
            // System.out.println("dir = " + dir);
            if(dir.isDirectory())
            {
                String[] files = dir.list();
                for (int i = 0; (i<files.length && !result); ++i)
                {
                    // System.out.println("files " + i + " = " + files[i]);
                    if(files[i].equals("jai_core.jar"))
                    {
                        result = true;
                    }
                }
            } else if(dir.isFile())
            {
                // System.out.println("filename = " + dir.getName());
                if(dir.getName().equals("jai_core.jar"))
                {
                    result = true;
                }
            } else
            {
                // Not really sure what to do here, no point in 
								// generating an error, but should never happen
            }
        }
        
        return result;
    }
    
    /** Sets the image of this object, for printing */    
    private void setImage(BufferedImage newImage)
    {
        image = newImage;
    }
    
    public int print(Graphics g, PageFormat format, int pageIndex)
    {
        Graphics2D g2d = (Graphics2D) g;
        g2d.translate(format.getImageableX(), format.getImageableY());
        g2d.drawImage(image, null, null);
        
        return Printable.PAGE_EXISTS;
    }
    
}
