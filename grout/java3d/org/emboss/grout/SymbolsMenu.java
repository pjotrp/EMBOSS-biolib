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
 * SymbolsMenu.java
 *
 */

package org.emboss.grout;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.Vector;

import com.sun.j3d.utils.picking.*;
import javax.media.j3d.*;
import javax.vecmath.Point3f;
import javax.vecmath.Point3d;
import javax.vecmath.Point2d;
import javax.vecmath.Color4f;
import javax.vecmath.Color3f;
import javax.vecmath.Vector3d;
import javax.vecmath.AxisAngle4d;

import java.awt.Dimension;
import com.sun.j3d.utils.geometry.*;
import javax.media.j3d.Canvas3D;

/** this menu allsows people to select line symbols */
public class SymbolsMenu extends javax.swing.JFrame
{
    /** the transferable graphic to add symbols to */    
    private TransferableGraphic tr;
    /** the mouse event that generated the menu */    
    private MouseEvent me;
    private ActionEvent e;
    
    private Canvas3D canvas;
    private OneGraphic graphic1;
    private PickCanvas pickCanvas;
    private PickResult pickResult;
    
    private ButtonGroup symbolButtonGroup;
    private ButtonGroup frequencyButtonGroup;
    private JRadioButton boxSymbolButton;
    private JRadioButton crossSymbolButton;
    private JRadioButton triangleSymbolButton;
    private JRadioButton barSymbolButton;
    private JRadioButton frequencyPerLineButton;
    private JTextField frequencyPerLineTextField;
    private JRadioButton frequencyPerSymbolButton;
    private JTextField frequencyPerSymbolTextField;
    private JRadioButton frequencyPerPointButton;
    private JTextField frequencyPerPointTextField;
    private JButton removeButton;
    private JButton doButton;
    
    private int frequency;
    private int validIndexCount;
    private Vector points;
    private BranchGroup intermidiateGroup;
    private BranchGroup branchGroup;
    private PickIntersection pickIntersection;
    private GeometryArray geometryArray;
    
    private static int PER_POINT = 0;
    private static int PER_LINE = 1;
    private static int PER_SYMBOL = 2;
    
    private double labelXSize;
    private double labelYSize;
    
    private	JMenuItem setMenuItem;
    private	JMenuItem autoMenuItem;
    
    public SymbolsMenu(TransferableGraphic tran, MouseEvent mev, ActionEvent ev)
    {
        super("Set Symbols");
        
        tr = tran;
        me = mev;
        e = ev;
        points = new Vector();
        
        canvas = (Canvas3D) me.getSource();
        graphic1 = (OneGraphic) canvas.getParent().getParent();
        pickCanvas = new PickCanvas
        (canvas,
        graphic1.getRendWindContainer().getUniverse().getLocale());
        pickCanvas.setMode(PickTool.GEOMETRY_INTERSECT_INFO);
        pickCanvas.setTolerance(5.0f);
        pickCanvas.setShapeLocation(me);
        
        pickResult = pickCanvas.pickClosest();
        
        if (pickResult != null)
        {
            if(thisIsALine(pickResult))
            {
                pickIntersection = pickResult.getClosestIntersection
                (pickCanvas.getStartPosition());
                geometryArray = 	pickIntersection.getGeometryArray();
                // System.out.println("geometryArray = " + geometryArray);
                
                validIndexCount = ((IndexedLineStripArray) geometryArray)
                .getValidIndexCount();
            }
        }
        
        intermidiateGroup = tr.getSymbols();
        
        getContentPane().setLayout(new GridLayout(0,1));
        
        getContentPane().add(new JLabel("Type of Symbol"));
        JPanel symbolsPanel = new JPanel();
        symbolButtonGroup = new ButtonGroup();
        boxSymbolButton = new JRadioButton("Box");
        boxSymbolButton.setSelected(true);
        crossSymbolButton = new JRadioButton("Cross");
        symbolButtonGroup.add(boxSymbolButton);
        symbolButtonGroup.add(crossSymbolButton);
        symbolsPanel.add(boxSymbolButton);
        symbolsPanel.add(crossSymbolButton);
        triangleSymbolButton = new JRadioButton("Triangle");
        symbolButtonGroup.add(triangleSymbolButton);
        symbolsPanel.add(triangleSymbolButton);
        barSymbolButton = new JRadioButton("Bar");
        symbolButtonGroup.add(barSymbolButton);
        symbolsPanel.add(barSymbolButton);
        
        getContentPane().add(symbolsPanel);
        
        getContentPane().add(new JLabel("Frequency of symbol"));
        getContentPane().add(new JLabel("Number of lines = "
        + (validIndexCount - 1)));
        frequencyButtonGroup = new ButtonGroup();
        frequencyPerLineButton = new JRadioButton("Symbols per line");
        frequencyPerLineButton.setSelected(true);
        frequencyButtonGroup.add(frequencyPerLineButton);
        getContentPane().add(frequencyPerLineButton);
        frequencyPerLineTextField = new JTextField();
        frequencyPerLineTextField.setText("1");;
        getContentPane().add(frequencyPerLineTextField);
        frequencyPerSymbolButton = new JRadioButton("Lines per Symbol");
        frequencyButtonGroup.add(frequencyPerSymbolButton);
        getContentPane().add(frequencyPerSymbolButton);
        frequencyPerSymbolTextField = new JTextField();
        frequencyPerSymbolTextField.setText("1");;
        getContentPane().add(frequencyPerSymbolTextField);
        frequencyPerPointButton = new JRadioButton
						("On data points, points per symbol");
        frequencyButtonGroup.add(frequencyPerPointButton);
        getContentPane().add(frequencyPerPointButton);
        frequencyPerPointTextField = new JTextField();
        frequencyPerPointTextField.setText("1");;
        getContentPane().add(frequencyPerPointTextField);
        
        
        commonConstrution();
        
        labelXSize = (tr.getMaxX() - tr.getMinX()) / 100;
        labelYSize = (tr.getMaxY() - tr.getMinY()) / 100;
        
    }
    
    /** the common bit for constructing this object */    
    private void commonConstrution()
    {
        //				JPanel buttons = new JPanel(new BorderLayout());
        JPanel buttons = new JPanel();
        doButton = new JButton("Do Symbols");
        doButton.addActionListener(new MenuListener());
        buttons.add(doButton);
        
        removeButton = new JButton("Remove Symbols");
        removeButton.addActionListener(new MenuListener());
        buttons.add(removeButton);
        
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
    
    class MenuListener implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
            if(e.getSource() == doButton)
            {
                int pointsPerLine = -1;
                if(frequencyPerLineButton.isSelected())
                {
                    pointsPerLine = PER_LINE;
                    try
                    {
                        frequency = Integer.parseInt
                        (frequencyPerLineTextField.getText());
                    }
                    catch(NumberFormatException ex)
                    {
                        frequency = 0;
                    }
                }
                else if(frequencyPerSymbolButton.isSelected())
                {
                    pointsPerLine = PER_SYMBOL;
                    try
                    {
                        frequency = Integer.parseInt
                        (frequencyPerSymbolTextField.getText());
                    }
                    catch(NumberFormatException ex)
                    {
                        frequency = 0;
                    }
                }
                else if(frequencyPerPointButton.isSelected())
                {
                    pointsPerLine = PER_POINT;
                    try
                    {
                        frequency = Integer.parseInt
                        (frequencyPerPointTextField.getText());
                    }
                    catch(NumberFormatException ex)
                    {
                        frequency = 0;
                    }
                }
                
                if(frequency != 0)
                {
                    if(pointsPerLine == PER_LINE)
                    {
                        points = new Vector();
                        Point3d centrePoint = new Point3d();
                        Point3d previousPoint = new Point3d();
                        geometryArray.getCoordinate(0, previousPoint);
                        for(int i = 1; i < validIndexCount;
                        ++i)
                        {
                            Point3d point = new Point3d();
                            geometryArray.getCoordinate(i, point);
                            for(double j = 1; j <= frequency; ++j)
                            {
                                if(!point.equals(previousPoint))
                                {
                                    centrePoint = new Point3d();
                                    centrePoint.x =
                                    ((point.x - previousPoint.x) *
                                    (j / (frequency + 1))) +
                                    previousPoint.x;

                                    centrePoint.y =
                                    ((point.y - previousPoint.y) *
                                    (j / (frequency + 1))) +
                                    previousPoint.y;
                                    centrePoint.z =
                                    ((point.z - previousPoint.z) *
                                    (j / (frequency + 1))) +
                                    previousPoint.z;
                                    points.addElement(centrePoint);
                                }
                            }
                            previousPoint = point;
                        }
                    }
                    else if(pointsPerLine == PER_SYMBOL)
                    {
                        points = new Vector();
                        Point3d centrePoint = new Point3d();
                        Point3d previousPoint = new Point3d();
                        geometryArray.getCoordinate(0, previousPoint);
                        for(int i = 1; i < validIndexCount; ++i)
                        {
                            Point3d point = new Point3d();
                            geometryArray.getCoordinate(i, point);
                            //System.out.println("point " + i + " = " + point);
                            if(!point.equals(previousPoint) &&
                            (Math.IEEEremainder(i, frequency) == 0))
                            {
                                centrePoint = new Point3d();
                                centrePoint.x = (point.x + previousPoint.x) / 2;
                                centrePoint.y = (point.y + previousPoint.y) / 2;
                                centrePoint.z = (point.z + previousPoint.z) / 2;
                                points.addElement(centrePoint);
                            }
                            previousPoint = point;
                        }
                    }
                    else if(pointsPerLine == PER_POINT)
                    {
                        points = new Vector();
                        for(int i = 0; i < validIndexCount; ++i)
                        {
                            if(Math.IEEEremainder(i, frequency) == 0)
                            {
                                Point3d point = new Point3d();
                                geometryArray.getCoordinate(i, point);
                                Point3d pointToAdd = new Point3d(point);
                                points.addElement(pointToAdd);
                            }
                        }
                    }
                    
                    addSymbolToThesePoints(points, tr, me, pickResult,
                    pickIntersection, geometryArray);
                }
                else
                {
                    doNoNumberWarning();
                }
            }
            else if(e.getSource() == removeButton)
            {
                branchGroup.removeChild(intermidiateGroup);
                tr.setSymbols(null);
            }
        }
    }
    
    
    
    
    
    private void addSymbolToThesePoints(Vector points, TransferableGraphic tr,
    MouseEvent me,
    PickResult pickResult,
    PickIntersection pickIntersection,
    GeometryArray geometryArray)
    {
        
        SceneGraphPath sceneGraphPath = pickResult.getSceneGraphPath();
        
        // System.out.println("sceneGraphPath = " + sceneGraphPath);
        
        branchGroup = null;
        
        for(int i = 0;
        ((i < sceneGraphPath.nodeCount()) && (branchGroup == null));
        ++i)
        {
            Node node = sceneGraphPath.getNode
            (sceneGraphPath.nodeCount() - i - 1);
            if(node instanceof BranchGroup)
            {
                branchGroup = ((BranchGroup) node);
            }
        }
        
        // System.out.println("branchGroup = " + branchGroup);
        if(intermidiateGroup != null)
        {
            branchGroup.removeChild(intermidiateGroup);
        }
        
                                /*
                                System.out.println(" = " + );
                                 */
        intermidiateGroup = new BranchGroup();
        intermidiateGroup.setCapability(Group.ALLOW_CHILDREN_READ);
        intermidiateGroup.setCapability(Group.ALLOW_CHILDREN_WRITE);
        intermidiateGroup.setCapability(Group.ALLOW_CHILDREN_EXTEND);
        intermidiateGroup.setCapability(BranchGroup.ALLOW_DETACH);
        
        for(int i = 0; i < points.size(); ++i)
        {
            Node label = makeLabel();
            
            TransformGroup transformGroup = new TransformGroup();
            transformGroup.setCapability(Group.ALLOW_CHILDREN_READ);
            transformGroup.setCapability(Group.ALLOW_CHILDREN_WRITE);
            transformGroup.setCapability(Group.ALLOW_CHILDREN_EXTEND);
            
            Transform3D transform3D = new Transform3D();
            Vector3d trans = new Vector3d((Point3d) points.elementAt(i));
            transform3D.setTranslation(trans);
            
            transformGroup.setTransform(transform3D);
            transformGroup.addChild(label);
            
            intermidiateGroup.addChild(transformGroup);
        }
        
        // Parser.setAllCapabilitys(intermidiateGroup, 0);
        Parser.setAllCapabilitys(intermidiateGroup);
        
        branchGroup.addChild(intermidiateGroup);
        tr.setSymbols(intermidiateGroup);
    }
    
    
    private Node makeLabel()
    {
        Node returnLabel = null;
        
        if(boxSymbolButton.isSelected())
        {
            Appearance appearance = new Appearance();
            Material material = new Material();
            material.setDiffuseColor(0.0f, 0.0f, 0.0f);
            material.setAmbientColor(0.0f, 0.0f, 0.0f);
            material.setSpecularColor(0.0f, 0.0f, 0.0f);
            material.setEmissiveColor(0.0f, 0.0f, 0.0f);
            appearance.setMaterial(material);
            
            com.sun.j3d.utils.geometry.Box label =
            new com.sun.j3d.utils.geometry.Box((float) labelXSize,
            (float) labelYSize,
            0.01f, appearance);
            
            
            for(int k = 0; k <= 5; ++k)
            {
                label.getShape(k).setCapability(Shape3D.ALLOW_GEOMETRY_READ);
                label.getShape(k).setCapability(Shape3D.ALLOW_BOUNDS_READ);
                label.getShape(k).setCapability(Shape3D.ALLOW_APPEARANCE_READ);
                label.getShape(k).setCapability(Shape3D.ALLOW_APPEARANCE_WRITE);
            }
            returnLabel = (Node) label;
        }
        else if(crossSymbolButton.isSelected())
        {
            
            
            com.sun.j3d.utils.geometry.Cylinder NWSEBar = null;
            com.sun.j3d.utils.geometry.Cylinder NESWBar = null;
            float barLength = (float) Math.pow(8, 0.5);
            {
                Appearance appearance = new Appearance();
                Material material = new Material();
                material.setDiffuseColor(0.0f, 0.0f, 0.0f);
                material.setAmbientColor(0.0f, 0.0f, 0.0f);
                material.setSpecularColor(0.0f, 0.0f, 0.0f);
                material.setEmissiveColor(0.0f, 0.0f, 0.0f);
                appearance.setMaterial(material);
                
                NWSEBar = new com.sun.j3d.utils.geometry.Cylinder
                ((((float) labelXSize) * 0.1f),
                (((float) labelYSize) * barLength),
                appearance);
                
                
                for(int k = 0; k <= 2; ++k)
                {
                    NWSEBar.getShape(k).setCapability
                    (Shape3D.ALLOW_GEOMETRY_READ);
                    NWSEBar.getShape(k).setCapability
                    (Shape3D.ALLOW_BOUNDS_READ);
                    NWSEBar.getShape(k).setCapability
                    (Shape3D.ALLOW_APPEARANCE_READ);
                    NWSEBar.getShape(k).setCapability
                    (Shape3D.ALLOW_APPEARANCE_WRITE);
                }
            }
            {
                Appearance appearance = new Appearance();
                Material material = new Material();
                material.setDiffuseColor(0.0f, 0.0f, 0.0f);
                material.setAmbientColor(0.0f, 0.0f, 0.0f);
                material.setSpecularColor(0.0f, 0.0f, 0.0f);
                material.setEmissiveColor(0.0f, 0.0f, 0.0f);
                appearance.setMaterial(material);
                
                NESWBar = new com.sun.j3d.utils.geometry.Cylinder
                ((((float) labelXSize) * 0.1f),
                (((float) labelYSize) * barLength),
                appearance);
                
                
                for(int k = 0; k <= 2; ++k)
                {
                    NESWBar.getShape(k).setCapability
                    (Shape3D.ALLOW_GEOMETRY_READ);
                    NESWBar.getShape(k).setCapability
                    (Shape3D.ALLOW_BOUNDS_READ);
                    NESWBar.getShape(k).setCapability
                    (Shape3D.ALLOW_APPEARANCE_READ);
                    NESWBar.getShape(k).setCapability
                    (Shape3D.ALLOW_APPEARANCE_WRITE);
                }
            }
            returnLabel = new BranchGroup();
            
            TransformGroup transformGroupNESW = new TransformGroup();
            transformGroupNESW.setCapability(Group.ALLOW_CHILDREN_READ);
            transformGroupNESW.setCapability(Group.ALLOW_CHILDREN_WRITE);
            transformGroupNESW.setCapability(Group.ALLOW_CHILDREN_EXTEND);
            
            Transform3D transform3DNESW = new Transform3D();
            Vector3d axis = new Vector3d(0.0, 0.0, 1.0);
            double angle = Math.PI / 4;
            AxisAngle4d rotNESW = new AxisAngle4d(axis, angle );
            transform3DNESW.setRotation(rotNESW);
            
            transformGroupNESW.setTransform(transform3DNESW);
            transformGroupNESW.addChild(NESWBar);
            
            ((BranchGroup) returnLabel).addChild(transformGroupNESW);
            
            TransformGroup transformGroupNWSE = new TransformGroup();
            transformGroupNWSE.setCapability(Group.ALLOW_CHILDREN_READ);
            transformGroupNWSE.setCapability(Group.ALLOW_CHILDREN_WRITE);
            transformGroupNWSE.setCapability(Group.ALLOW_CHILDREN_EXTEND);
            
            Transform3D transform3DNWSE = new Transform3D();
            AxisAngle4d rotNWSE = new AxisAngle4d(axis, -angle );
            transform3DNWSE.setRotation(rotNWSE);
            
            transformGroupNWSE.setTransform(transform3DNWSE);
            transformGroupNWSE.addChild(NWSEBar);
            
            ((BranchGroup) returnLabel).addChild(transformGroupNWSE);
            
            // ((BranchGroup) returnLabel).addChild(label);
            
        }
        else if(triangleSymbolButton.isSelected())
        {
            Appearance appearance = new Appearance();
            Material material = new Material();
            material.setDiffuseColor(0.0f, 0.0f, 0.0f);
            material.setAmbientColor(0.0f, 0.0f, 0.0f);
            material.setSpecularColor(0.0f, 0.0f, 0.0f);
            material.setEmissiveColor(0.0f, 0.0f, 0.0f);
            appearance.setMaterial(material);
            
            com.sun.j3d.utils.geometry.Cone label =
            new com.sun.j3d.utils.geometry.Cone((float) labelXSize,
            (((float) labelYSize) * 2),
            appearance);
            
            
            for(int k = 0; k <= 1; ++k)
            {
                label.getShape(k).setCapability(Shape3D.ALLOW_GEOMETRY_READ);
                label.getShape(k).setCapability(Shape3D.ALLOW_BOUNDS_READ);
                label.getShape(k).setCapability(Shape3D.ALLOW_APPEARANCE_READ);
                label.getShape(k).setCapability(Shape3D.ALLOW_APPEARANCE_WRITE);
            }
            
            // returnLabel = (Node) label;
            returnLabel = new TransformGroup();
            Billboard billboard = new Billboard((TransformGroup)returnLabel,
            Billboard.ROTATE_ABOUT_POINT,
            new Point3f(0.0f, 0.0f, 0.0f));
            ((TransformGroup) returnLabel).addChild(label);
        }
        else if(barSymbolButton.isSelected())
        {
            Appearance appearance = new Appearance();
            Material material = new Material();
            material.setDiffuseColor(0.0f, 0.0f, 0.0f);
            material.setAmbientColor(0.0f, 0.0f, 0.0f);
            material.setSpecularColor(0.0f, 0.0f, 0.0f);
            material.setEmissiveColor(0.0f, 0.0f, 0.0f);
            appearance.setMaterial(material);
            
            com.sun.j3d.utils.geometry.Cylinder label =
            new com.sun.j3d.utils.geometry.Cylinder
            ((((float) labelXSize) * 0.1f), (((float) labelYSize) * 2),
            appearance);
            
            
            for(int k = 0; k <= 2; ++k)
            {
                label.getShape(k).setCapability(Shape3D.ALLOW_GEOMETRY_READ);
                label.getShape(k).setCapability(Shape3D.ALLOW_BOUNDS_READ);
                label.getShape(k).setCapability(Shape3D.ALLOW_APPEARANCE_READ);
                label.getShape(k).setCapability(Shape3D.ALLOW_APPEARANCE_WRITE);
            }
            returnLabel = (Node) label;
        }
        
        
        
        // System.out.println("returnLabel = " + returnLabel);
        
        
        return (Node) returnLabel;
    }
    
    
    private boolean thisIsALine(PickResult pickResult)
    {
        SceneGraphPath sceneGraphPath = pickResult.getSceneGraphPath();
        
        
        Node shape = sceneGraphPath.getObject();
        
        if(shape instanceof Shape3D)
        {
            Geometry geometry = ((Shape3D) shape).getGeometry();
            // System.out.println("geometry = " + geometry);
            if(geometry instanceof IndexedLineStripArray)
            {
                return true;
            }
        }
        return false;
    }
    
    /** warn that the value entered is not a valid number */    
    private void doNoNumberWarning()
    {
        JOptionPane.showMessageDialog
        (this, "Please enter a non-zero interger");
    }
    
}
