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

/*
 * RendWindContain.java
 *
 * Created on April 24, 2002, 9:31 AM
 */
import java.awt.Color;
import java.awt.Panel;
import java.awt.BorderLayout;
import javax.media.j3d.Canvas3D;
import javax.swing.JPanel;
import java.awt.image.BufferedImage;
import javax.media.j3d.ImageComponent2D;
import javax.swing.JOptionPane;
import java.util.Vector;
import java.util.Hashtable;
import java.util.Enumeration;
import java.awt.datatransfer.*;
import java.awt.dnd.*;
import javax.media.j3d.SceneGraphPath;
import com.sun.j3d.utils.picking.PickResult;
import com.sun.j3d.utils.picking.PickCanvas;
import com.sun.j3d.utils.picking.PickTool;
import com.sun.j3d.utils.picking.PickResult;
import com.sun.j3d.utils.picking.PickIntersection;
//import ;
//import ;

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

import org.ietf.uri.ContentHandlerFactory;
import org.ietf.uri.FileNameMap;
import org.ietf.uri.URI;

import org.web3d.j3d.loaders.VRML97Loader;
import org.web3d.net.content.VRMLContentHandlerFactory;
import org.web3d.net.content.VRMLFileNameMap;

/** This handles displaying the canvas */
public class RendWindContainer extends JPanel implements
java.io.Serializable,
ParserListener,
ViewPositionModelListener,
ComponentListener,
GroutPanelListener
{
    /** A canvas to display world */
    public DragableCanvas3D canvas;
    
    /** The J3D universe to hold everything */
    SimpleUniverse  universe;
    
    //    /** The loader implementation used to load VRML97 content */
    //    VRML97Loader    vrmlLoader;
    
    /** The loaded scene */
    Scene scene = null;
    
    /** The whole scenegraph's root */
    BranchGroup     sceneRoot;
    
    /** A transform for examine navigation style */
    TransformGroup  examineGroup;
    
    /* A group to hold the loaded scene */
    BranchGroup     sceneGroup;
    
    /* A transform for the viewer position */
    /** view position TR */    
    TransformGroup  vpTransGroup;
    
    /* A transform to Scale the scene to fit the window */
    /** this fits the scene to the window */    
    TransformGroup transformToFitGroup;
    
    /** Cursor to display when waiting */
    Cursor waitCursor;
    
    /** Cursor to display normally */
    Cursor handCursor;
    
    /* A simple headlight for lighting if none provided */
    /** */    
    DirectionalLight headLight;
    
    /* The View */
    View view;
    
    String filename = null;
    
    /** view position */    
    Vector3d pos = new Vector3d(0,0,10);
    /** view scale */    
    Vector3d scale = new Vector3d(1,1,1);
    Transform3D eyeTrans = new Transform3D();
    Transform3D viewPositionEyeTrans = new Transform3D();
    Transform3D viewTrans = new Transform3D();
    Transform3D centreingTransform = new Transform3D();
    Point3d centre = null;
    
    /** this graphics number */    
    int graphicNumber = 0;
    
    private Vector targets = new Vector();
    
    private Background background;
    
    private static Color3f INITIAL_COLOUR = new Color3f(1, 1, 1);
    
    /** BranchGroup to add pick lines to */
    private	BranchGroup lineGroup = new BranchGroup();
    /** the lines added by sequence view */    
    private Hashtable lines = new Hashtable();
    
    /** the scenes displayed in this canvas */    
    private Vector scenes = new Vector();
    
    private DragSource dragSource;
    
    public RendWindContainer()
    {
        super(new BorderLayout());
        //        setSize(300, 400);
        setPreferredSize(new Dimension(400, 400));
        
        GraphicsConfigTemplate3D template = new GraphicsConfigTemplate3D();
        template.setDoubleBuffer(template.REQUIRED);
        GraphicsEnvironment env = GraphicsEnvironment.getLocalGraphicsEnvironment();
        GraphicsDevice dev = env.getDefaultScreenDevice();
        GraphicsConfiguration gfxConfig = dev.getBestConfiguration(template);
        
        canvas = new DragableCanvas3D(gfxConfig, false);
        canvas.setDoubleBufferEnable(true);
        add(BorderLayout.CENTER, canvas);
        
        waitCursor = new Cursor(Cursor.WAIT_CURSOR);
        handCursor = new Cursor(Cursor.HAND_CURSOR);
        
        universe = new SimpleUniverse(canvas);
        universe.setJ3DThreadPriority(Thread.NORM_PRIORITY);
        //        universe.setJ3DThreadPriority(Thread.MIN_PRIORITY);
        ViewingPlatform viewingPlatform = universe.getViewingPlatform();
        vpTransGroup = viewingPlatform.getViewPlatformTransform();
        Viewer viewer = universe.getViewer();
        view = viewer.getView();
        
        // TODO: Decide a real value for this.  Can't be infinite because
        // front/back ratio decides z precision
        view.setBackClipDistance(10000);
        
        
        
        setupNavigation();
        
/*        vrmlLoader = new VRML97Loader(VRML97Loader.LOAD_ALL);
 
        // if the file is a directory, ignore it
        File f = new File(filename);
        if(f.exists() && f.isFile())
        {
            gotoLocation(filename);
        } else
        {
            // // System.out.println("No file specified");
        }*/
    }
    
    /** Constructor with offscreen switch
     * @param offScreen
     */    
    public RendWindContainer(boolean offScreen)
    {
        super(new BorderLayout());
        setPreferredSize(new Dimension(400, 400));
        
        GraphicsConfigTemplate3D template = new GraphicsConfigTemplate3D();
        template.setDoubleBuffer(template.REQUIRED);
        GraphicsEnvironment env = GraphicsEnvironment.getLocalGraphicsEnvironment();
        GraphicsDevice dev = env.getDefaultScreenDevice();
        GraphicsConfiguration gfxConfig = dev.getBestConfiguration(template);
        
        canvas = new DragableCanvas3D(gfxConfig, offScreen);
        canvas.setDoubleBufferEnable(true);
        if(!offScreen)
        {
            canvas.setDoubleBufferEnable(true);
            add(BorderLayout.CENTER, canvas);
        } else
        {
            setOffScreenBuffer();
            setScreen3DSize();
        }
        
        waitCursor = new Cursor(Cursor.WAIT_CURSOR);
        handCursor = new Cursor(Cursor.HAND_CURSOR);
        
        universe = new SimpleUniverse(canvas);
        universe.setJ3DThreadPriority(Thread.NORM_PRIORITY);
        ViewingPlatform viewingPlatform = universe.getViewingPlatform();
        vpTransGroup = viewingPlatform.getViewPlatformTransform();
        Viewer viewer = universe.getViewer();
        view = viewer.getView();
        
        // TODO: Decide a real value for this.  Can't be infinite because
        // front/back ratio decides z precision
				// done elsewhere (when you load the file)
        view.setBackClipDistance(10000);
        
        setupNavigation();
    }
    
    public Canvas3D getCanvas()
    {
        return canvas;
    }
    
    public SimpleUniverse getUniverse()
    {
        return universe;
    }
    
    public void setFilename(String fn)
    {
        filename = fn;
    }
    
    public void setGraphicNumber(int gn)
    {
        graphicNumber = gn;
    }
    
    public void setLocationListener(DragableCanvas3DListener lis)
    {
        canvas.setLocationListener(lis);
    }
    
    public int getGraphicNumber()
    {
        return graphicNumber;
    }
    
    public Vector getScenes()
    {
        return scenes;
    }
    
    /** Load a new world
     * @param sceneGroup the world to load
     */
    public void setSceneGroup(BranchGroup sceneGroup)
    {
        // now you have it, compile it
        //    sceneGroup.compile();
        
        this.sceneGroup = sceneGroup;
        sceneGroup.setCapability(Node.ENABLE_PICK_REPORTING);
        // add the scene group  to the scene
        // // System.out.println("examineGroup " + examineGroup);
        // System.out.println("setSceneGroup sceneGroup " + sceneGroup);
        //        examineGroup.addChild(sceneGroup);
        
        // // System.out.println("bs ssg = " + sceneGroup);
        
    }
    
    public void setViewPosition(ViewPositionModel model)
    {
        pos = new Vector3d(model.getViewPosX(), model.getViewPosY(), model.getOverallScale());
        
        examineGroup.addChild(sceneGroup);
        scenes.addElement(sceneGroup);
        
        //        examineGroup.setTransform(t3d);
        
        // now that the scene group is "live" we can inquire the bounds
        setViewpoint();
        setupLighting();
    }
    
    public void setTransform(ViewPositionModel model)
    {
        pos.x = model.getViewPosX();
        pos.y = model.getViewPosY();
        pos.z = model.getOverallScale();
        eyeTrans.setTranslation(pos);
        vpTransGroup.setTransform(eyeTrans);
    }
    
    /**
     * Setup the scene's view
     */
    void setViewpoint()
    {
        Transform3D eyeTrans = new Transform3D();
        
        //        pos = new Vector3d(0,0,10);
        // put the View at the standard VRML default position 0,0,10
        eyeTrans.set(pos);
        viewTrans.mul(eyeTrans);
        // // System.out.println("bs sv = " + sceneGroup);
        
        // set the view transform
        vpTransGroup.setTransform(viewTrans);
    }
    
    /**
     * Setup the navigation system.  We use a simple examine behavior
     */
    private void setupNavigation()
    {
        canvas.addComponentListener(this);
        canvas.getView().setWindowEyepointPolicy(View.RELATIVE_TO_WINDOW);
        
        sceneRoot = new BranchGroup();
        sceneRoot.setCapability(TransformGroup.ALLOW_CHILDREN_EXTEND);
        
        examineGroup = new TransformGroup();
        examineGroup.setCapability(TransformGroup.ALLOW_CHILDREN_EXTEND);
        examineGroup.setCapability(TransformGroup.ALLOW_CHILDREN_READ);
        examineGroup.setCapability(TransformGroup.ALLOW_CHILDREN_WRITE);
        examineGroup.setCapability(TransformGroup.ALLOW_TRANSFORM_READ);
        examineGroup.setCapability(TransformGroup.ALLOW_TRANSFORM_WRITE);
        sceneRoot.addChild(examineGroup);
        
        BoundingSphere behaviorBounds = 
						new BoundingSphere(new Point3d(), Double.MAX_VALUE);
        
        background = new Background(INITIAL_COLOUR);
        background.setApplicationBounds(behaviorBounds);
        background.setCapability(Background.ALLOW_COLOR_WRITE);
        sceneRoot.addChild(background);
        
/*        MouseRotate mr = new MouseRotate();
        mr.setTransformGroup(examineGroup);
        mr.setSchedulingBounds(behaviorBounds);
        sceneRoot.addChild(mr);
 
        MouseTranslate mt = new MouseTranslate();
        mt.setTransformGroup(examineGroup);
        mt.setSchedulingBounds(behaviorBounds);
        sceneRoot.addChild(mt);
 
        MouseZoom mz = new MouseZoom();
        mz.setTransformGroup(examineGroup);
        mz.setSchedulingBounds(behaviorBounds);
        sceneRoot.addChild(mz); */
        
        universe.addBranchGraph(sceneRoot);
    }
    
    /**
     * Setup the worlds lighting.  If none is provided in the VRML file then
     * we create a simple headlight
     */
    private void setupLighting()
    {
        Light lights[] = null;
        
        if(scene != null)
            lights = scene.getLightNodes();
        
        if (lights == null)
        {
            BranchGroup lightBG = new BranchGroup();
            BoundingSphere lightBounds =
            new BoundingSphere(new Point3d(), Double.MAX_VALUE);
            headLight = new DirectionalLight(new Color3f(1.0f,1.0f,1.0f),
            new Vector3f(0,0,-1));
            headLight.setCapability(Light.ALLOW_STATE_WRITE);
            headLight.setInfluencingBounds(lightBounds);
            
            lightBG.addChild(headLight);
            sceneRoot.addChild(lightBG);
        }
    }
    
    /** sets the off screen buffer to the canvas */    
    public void setOffScreenBuffer()
    {
        BufferedImage image = new BufferedImage(400, 400, BufferedImage.TYPE_INT_RGB);
        ImageComponent2D imageComponent = new ImageComponent2D(ImageComponent2D.FORMAT_RGB, image);
        canvas.setOffScreenBuffer(imageComponent);
    }
    
    public BufferedImage getOffScreenBuffer()
    {
        ImageComponent2D imageComponent = canvas.getOffScreenBuffer();
        // // System.out.println("got imageComponent = " + imageComponent);
        BufferedImage image = imageComponent.getImage();
        // // System.out.println("got image = " + image);
        return image;
    }
    
    /** used when there is no real screen */    
    public void setScreen3DSize()
    {
        canvas.getScreen3D().setSize(new Dimension(1280, 1024));
        canvas.getScreen3D().setPhysicalScreenWidth(0.36124);
        canvas.getScreen3D().setPhysicalScreenHeight(0.28899555);
    }
    
    /** do the off screen rendering */    
    public void renderOffScreenBuffer()
    {
        canvas.renderOffScreenBuffer();
        canvas.waitForOffScreenRendering();
    }
    
    private void setViewParameters(ViewPositionModel model)
    {
        scale.x = model.getScaleX();
        scale.y = model.getScaleY();
        //      setViewpoint();
        viewPositionEyeTrans.setScale(scale);
        canvas.stopRenderer();
        setTransform(model);
        examineGroup.setTransform(viewPositionEyeTrans);
        
        double largestDimension = (model.getDiffX() > model.getDiffY()) ?
        model.getDiffX() : model.getDiffY();
        
        view.setBackClipDistance(largestDimension);
        
        canvas.startRenderer();
        //			model.printValues();
    }
    
    /** This removes the original scene */    
    public BranchGroup detachScene()
    {
        examineGroup.removeChild(sceneGroup);
        // System.out.println("detachScene sceneGroup = " + sceneGroup);
        scenes.removeElement(sceneGroup);
        return sceneGroup;
    }
    
    /** This removes a specific scene */    
    public BranchGroup detachScene(BranchGroup branchGroup)
    {
        examineGroup.removeChild(branchGroup);
        scenes.removeElement(branchGroup);
        return sceneGroup;
    }
    
    public void addScene(BranchGroup sc)
    {
        // System.out.println("BranchGroup = " + sc);
        scenes.addElement(sc);
        // System.out.println("scenes = " + scenes);
        examineGroup.addChild(sc);
    }
    
    public void addSceneScaled(TransferableGraphic graphic)
    {
        TransformGroup transform = new TransformGroup();
        transform.addChild(graphic.getSceneRoot());
        BranchGroup group = new BranchGroup();
        group.addChild(transform);
        graphic.setSceneRoot(group);
        
        scenes.addElement(group);
        // System.out.println("scenes = " + scenes);
        examineGroup.addChild(group);
    }
    
    /** Repond to a parser changed event */    
    public void parserChanged(ParserEvent e)
    {
        setSceneGroup((BranchGroup)(e.getGraphics()).elementAt(graphicNumber));
        
    }
    
    public synchronized void addRendWindContainerListener
				(RendWindContainerListener listener)
    {
        targets.addElement(listener);
    }
    
    public synchronized void removeRendWindContainerListener
				(RendWindContainerListener listener)
    {
        targets.removeElement(listener);
    }
    
    /** Convenience method */    
    protected void notifyTargets(String propertyName, 
																 Object oldValue, Object newValue)
    {
        Vector l;
        RendWindContainerEvent s = new RendWindContainerEvent
						((Object) this, propertyName, oldValue, newValue);
        synchronized(this)
        {
            l = (Vector) targets.clone();
        }
        for (int i = 0; i < l.size(); i++)
        {
            RendWindContainerListener sl = 
								(RendWindContainerListener) l.elementAt(i);
            sl.rendWindContainerChanged(s);
        }
    }
    
    public void setBackgroundColour(String colour)
    {
        if(colour.equals("Black"))
        {
            background.setColor(0, 0, 0);
        }
        else if(colour.equals("White"))
        {
            background.setColor(1, 1, 1);
        }
        else if(colour.equals("Pink"))
        {
            background.setColor(1, 0, 1);
        }
        else if(colour.equals("LightGray"))
        {
            background.setColor(0.8f, 0.8f, 0.8f);
        }
        
    }
    
    /** This adds the line for sequence view
     * @param x1
     * @param y1
     * @param x2
     * @param y2
     * @return The added line
     */    
    public Shape3D addLine(double x1, double y1, double x2, double y2)
    {
        // System.out.println("adding line " + x1 + y1 + x2 + y2);
        
        Point3d[] vertices = new Point3d[2];
        
        vertices[0] = new Point3d(x1, y1, 0);
        vertices[1] = new Point3d(x2, y2, 0);
        
        int[] indices =
        { 0, 1};
        
        int[] lineStripCounts =
        { 2 };
        
        IndexedLineStripArray geometry =
        new IndexedLineStripArray(vertices.length,
        GeometryArray.COLOR_3 + GeometryArray.COORDINATES,
        indices.length,
        lineStripCounts);
        geometry.setCoordinates(0, vertices);
        geometry.setCoordinateIndices(0, indices);
        
        geometry.setCapability(IndexedGeometryArray.ALLOW_COORDINATE_READ);
        geometry.setCapability(IndexedGeometryArray.ALLOW_COORDINATE_WRITE);
        geometry.setCapability(IndexedGeometryArray.ALLOW_COUNT_READ);
        geometry.setCapability(IndexedGeometryArray.ALLOW_FORMAT_READ);
        geometry.setCapability(IndexedGeometryArray.ALLOW_COORDINATE_INDEX_READ);
        geometry.setCapability(IndexedGeometryArray.ALLOW_COLOR_INDEX_READ );
        geometry.setCapability(IndexedGeometryArray.ALLOW_COLOR_INDEX_WRITE );
        geometry.setCapability(IndexedGeometryArray.ALLOW_COLOR_READ );
        geometry.setCapability(IndexedGeometryArray.ALLOW_COLOR_WRITE );
        
        Shape3D shape = new Shape3D(geometry);
        
        shape.setCapability(Shape3D.ALLOW_GEOMETRY_READ);
        shape.setCapability(Shape3D.ALLOW_BOUNDS_READ);
        shape.setCapability(Shape3D.ALLOW_APPEARANCE_READ);
        shape.setCapability(Shape3D.ALLOW_APPEARANCE_WRITE);
        
        BranchGroup lineGroup2 = new BranchGroup();
        // System.out.println("lineGroup = " + lineGroup);
        lineGroup2.setCapability(BranchGroup.ALLOW_DETACH);
        lineGroup2.addChild(shape);
        
        lines.put(shape, lineGroup2);
        // System.out.println("lines.get(line) = " + lines.get(shape) + " shape = " + shape);
        // iterateThroughHashtable(lines);
        
        examineGroup.addChild(lineGroup2);
        
        return shape;
    }
    
    public void removeLine(Shape3D line)
    {
        examineGroup.removeChild((BranchGroup) lines.remove(line));
    }
    
    /** Just for debuging */    
    private void iterateThroughHashtable(Hashtable table)
    {
        // System.out.println("table " + table);
        Enumeration keys = table.keys();
        
        for (int i = 0; keys.hasMoreElements(); ++i)
        {
            Object key = keys.nextElement();
        }
    }
    
    public void propertyChange(java.beans.PropertyChangeEvent propertyChangeEvent)
    {
    }
    
    public void groutPanelChanged(GroutPanelEvent e)
    {
        if(e.getPropertyName().equals("WindowEyepointPolicy"))
        {
            canvas.getView().setWindowEyepointPolicy
								(((Integer) e.getNewValue()).intValue());
        }
        if(e.getPropertyName().equals("FitViewToWindow"))
        {
            notifyTargets("FitViewToWindow", (Object) new Boolean(true),
													(Object) new Boolean(false));
        }
        if(e.getPropertyName().startsWith("Colour"))
        {
            // this needs 1.4
            // setBackgroundColour
						// 		(e.getPropertyName().replaceFirst("Colour", ""));
            setBackgroundColour(e.getPropertyName().substring(6));
        }
        
    }
    
    public void controlContainerChanged(ControlContainerEvent e)
    {
        Transform3D eyeTrans = new Transform3D();
    }
    
    public void viewPositionModelChanged(ViewPositionModelEvent e)
    {
        ViewPositionModel model = (ViewPositionModel) e.getSource();
        viewPositionEyeTrans.setIdentity();
        if(e.getPropertyName().equals("parser"))
        {
            setViewParameters(model);
            setViewPosition(model);
        } else if(e.getPropertyName().equals("reset") ||
									e.getPropertyName().equals("fitViewToWindow"))
        {
            scale.x = model.getScaleX();
            scale.y = model.getScaleY();
            setViewpoint();
            viewPositionEyeTrans.setScale(scale);
            examineGroup.setTransform(viewPositionEyeTrans);
            setTransform(model);
        } else if(e.getPropertyName().equals("transformXFactor"))
        {
            setTransform(model);
        } else if(e.getPropertyName().equals("transformYFactor"))
        {
            setTransform(model);
        } else if(e.getPropertyName().equals("transformZFactor"))
        {
            setTransform(model);
        } else if(e.getPropertyName().equals("scaleXFactor"))
        {
            setViewParameters(model);
        } else if(e.getPropertyName().equals("scaleYFactor"))
        {
            setViewParameters(model);
            //			printValues(e);
        }
    }
    
    public void componentResized(ComponentEvent e)
    {
        notifyTargets("RendWindContainerResized", (Object) canvas.getSize(),
											(Object) canvas.getSize());
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
    
    /** Just for debuging */    
    public void printValues(ViewPositionModelEvent e)
    {
        ViewPositionModel model = (ViewPositionModel) e.getSource();
        System.out.print("$%@scaleX = " + model.getScaleX() + " scaleY = " 
												 + model.getScaleY() + " ratio = " + 
												 (model.getScaleX() / model.getScaleY()));
        if((model.getScaleX() / model.getScaleY()) != model.getXOverY())
        {
            System.out.print(" wrong should be " + model.getXOverY());
        }
        System.out.print("$%@scaleXOnly = " + model.getScaleXOnly() + 
												 " scaleYOnly = " + model.getScaleYOnly() + " ratio = "
												 + (model.getScaleXOnly() / model.getScaleYOnly()));
        if((model.getScaleXOnly() / model.getScaleYOnly()) != model.getXOverY())
        {
            System.out.print(" wrong should be " + model.getXOverY());
        }
        System.out.println("");
        System.out.print("scale.x = " + scale.x + " scale.y = " + scale.y +
												 " ratio = " + (scale.x / scale.y) + " scale.z = " +
												 scale.z);
        System.out.print("pos.x = " + pos.x + " pos.y = " + pos.y + " pos.z = "
												 + pos.z);
        System.out.println("");
    }
    
    public TransferableGraphic pickGraphic(Canvas3D pickFromCanvas,
    Point pickLocation)
    {
        OneGraphic parent = ((OneGraphic) getParent());
        GroutPanel grandparent = ((GroutPanel) parent.getParent());
        int yPos = grandparent.getGraphicsVector().indexOf(parent);
        BranchGroup branchGroup;

        branchGroup = doPicking(pickFromCanvas, pickLocation);
        
        if(branchGroup== null)
        {
            return null;
        }
        else
        {
            TransferableGraphic graphic = 
								parent.getTransferableGraphic(branchGroup);
            // System.out.println("graphic = " + graphic);
            if(graphic == null)
            {
                // System.out.println("kfjbvartu parent = " + parent);
                graphic = new TransferableGraphic(branchGroup, parent, parent,
																									0, yPos,
																									parent.getViewPositionModel(),
																									parent.getMetadata());
                // not sure about this line
                // parent.setOriginalGraphic(graphic);
            }
            else
            {
                // System.out.println("kfjbvartu 2 parent = " + parent);
                graphic.setPresentGraphic(parent);
            }
            
            return graphic;
        }
    }
    
    public BranchGroup doPicking(Canvas3D pickFromCanvas, Point pickLocation)
    {
        PickCanvas pickCanvas = new PickCanvas(pickFromCanvas,
        universe.getLocale());
        pickCanvas.setMode(PickTool.GEOMETRY_INTERSECT_INFO);
        pickCanvas.setTolerance(5.0f);
        // Point pickLocation = e.getDragOrigin();
        pickCanvas.setShapeLocation(pickLocation.x, pickLocation.y);
        
        PickResult pickResult = pickCanvas.pickClosest();
        
        BranchGroup returnGroup = null;
        if (pickResult != null)
        {
            SceneGraphPath path = pickResult.getSceneGraphPath();
            int pathLimit = path.nodeCount();
            // Vector scenes = parent.getScenes();
            int scenesLimit = scenes.size();
            // System.out.println("scenesLimit = " + scenesLimit);
            Node node;
            for(int i = 0; ((i < pathLimit) && (returnGroup == null)); ++i)
            {
                node = path.getNode(i);
                for(int j = 0;
                ((j < scenesLimit) && (returnGroup == null));
                ++j)
                {
                    if(node.equals(scenes.elementAt(j)))
                    {
                        returnGroup = (BranchGroup) scenes.elementAt(j);
                    }
                }
            }
        }
        return returnGroup;
    }
    
}
