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
//import ;
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

public class RendWindContainer extends JPanel implements 
		java.io.Serializable, 
		ParserListener, 
		ViewPositionModelListener,
		ComponentListener,
		GroutPanelListener
{
  /** A canvas to display world */
  public Canvas3D    canvas;
  
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
  TransformGroup  vpTransGroup;
  
  /* A transform to Scale the scene to fit the window */
	TransformGroup transformToFitGroup;

  /** Cursor to display when waiting */
  Cursor waitCursor;
  
  /** Cursor to display normally */
  Cursor handCursor;
  
  /* A simple headlight for lighting if none provided */
  DirectionalLight headLight;
  
  String filename = null;
  
  Vector3d pos = new Vector3d(0,0,10);
  Vector3d scale = new Vector3d(1,1,1);
  Transform3D eyeTrans = new Transform3D();
  Transform3D viewPositionEyeTrans = new Transform3D();
  Transform3D viewTrans = new Transform3D();
  Transform3D centreingTransform = new Transform3D();
  Point3d centre = null;
  
	int graphicNumber = 0;

  private Vector targets = new Vector();

		private Background background;

	private static Color3f INITIAL_COLOUR = new Color3f (1, 1, 1);

  /** BranchGroup to add pick lines to */
		private	BranchGroup lineGroup = new BranchGroup();
		private Hashtable lines = new Hashtable();

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
    
    canvas = new Canvas3D(gfxConfig, false);
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
    View view = viewer.getView();
    
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
  
  public RendWindContainer(boolean offScreen)
  {
    super(new BorderLayout());
    setPreferredSize(new Dimension(400, 400));
    
    GraphicsConfigTemplate3D template = new GraphicsConfigTemplate3D();
    template.setDoubleBuffer(template.REQUIRED);
    GraphicsEnvironment env = GraphicsEnvironment.getLocalGraphicsEnvironment();
    //        // // System.out.println("Is env headless? = " + env.isHeadlessInstance());
    GraphicsDevice dev = env.getDefaultScreenDevice();
    GraphicsConfiguration gfxConfig = dev.getBestConfiguration(template);
    
    canvas = new Canvas3D(gfxConfig, offScreen);
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
    View view = viewer.getView();
    
    // TODO: Decide a real value for this.  Can't be infinite because
    // front/back ratio decides z precision
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
  
  public int getGraphicNumber()
  {
			return graphicNumber;
  }
  
  /**
   * Load a new world
   *
   * @param location The world to load
   */
  public void setSceneGroup(BranchGroup sceneGroup)
  {
    // now you have it, compile it
//    sceneGroup.compile();
    
    this.sceneGroup = sceneGroup;
    
    // add the scene group  to the scene
    // // System.out.println("examineGroup " + examineGroup);
     // System.out.println("setSceneGroup sceneGroup " + sceneGroup);
    //        examineGroup.addChild(sceneGroup);
    
    // // System.out.println("bs ssg = " + sceneGroup);
    
  }
  
  public void setViewPosition(ViewPositionModel model)
  {
    //        BoundingSphere bs = Parser.calculateBoundingSphere(sceneGroup);
/*        BoundingSphere bs = (BoundingSphere)sceneGroup.getBounds();
        Point3d center = new Point3d();
        bs.getCenter(center);
        double radius = bs.getRadius();
        // // System.out.println("radius = " + radius + " center = " + center); */
    
/*      this does not work.  I shall have to write a scene graph parser to do it for me.
        BoundingBox bb = (BoundingBox)sceneGroup.getBounds();
        Point3d lower = null;
        Point3d upper = null;
        bb.getLower(lower);
        bb.getUpper(upper);
        // // System.out.println("lower = " + lower.toString() + " higher = " + upper.toString()); */
    
/*        Transform3D t3d = new Transform3D();
        center.negate();
        center.z = 0 - (Math.pow(3, 0.5) * radius); */
    
    //        Transform3D centreingTransform = new Transform3D();
    pos = new Vector3d(model.getViewPosX(), model.getViewPosY(), model.getOverallScale());
    //		examineGroup.setTransform(pos);
/*		// // System.out.println("center = " + centre);
 
        centreingTransform.set(1.0f, new Vector3f(centre));
//        t3d.set(1.0f, new Vector3f(1000f, 1000f, 1000f));
        TransformGroup centeredObjects = new TransformGroup(centreingTransform);
        centeredObjects.setCapability(TransformGroup.ALLOW_CHILDREN_EXTEND);
        centeredObjects.setCapability(TransformGroup.ALLOW_CHILDREN_READ);
        centeredObjects.setCapability(TransformGroup.ALLOW_CHILDREN_WRITE);
        centeredObjects.setCapability(TransformGroup.ALLOW_TRANSFORM_READ);
        centeredObjects.setCapability(TransformGroup.ALLOW_TRANSFORM_WRITE);
        BranchGroup intermediate = new BranchGroup();
        intermediate.addChild(centeredObjects);
        examineGroup.addChild(intermediate);
        centeredObjects.addChild(sceneGroup);*/
    
    // System.out.println("adding sceneGroup to examineGroupkjgbka " + sceneGroup);

		/* What a load of bull */
		/*
		Vector3d scale = new Vector3d(model.getScaleXForFit(), 
																	model.getScaleYForFit(), 1);
		Transform3D scaleForFit = new Transform3D();
		scaleForFit.setScale(scale);

		transformToFitGroup = new TransformGroup();
		transformToFitGroup.setTransform(scaleForFit);
		transformToFitGroup.addChild(sceneGroup);

		BranchGroup intermidiate = new BranchGroup();
    intermidiate.addChild(transformToFitGroup);

    examineGroup.addChild(intermidiate);
		*/
    
    examineGroup.addChild(sceneGroup);

    //        examineGroup.setTransform(t3d);
    
    // now that the scene group is "live" we can inquire the bounds
    setViewpoint();
    setupLighting();
  }
  
  public void setTransform(ViewPositionModel model)
  {
    //        Transform3D eyeTrans = new Transform3D();
    //		pos = new Vector3d(model.getViewPosX(), model.getViewPosY(), model.getOverallScale());
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
			//			canvas.getView().setWindowEyepointPolicy(View.RELATIVE_TO_FIELD_OF_VIEW);
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

    BoundingSphere behaviorBounds = new BoundingSphere(new Point3d(), Double.MAX_VALUE);
    
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
  
  public void setOffScreenBuffer()
  {
    BufferedImage image = new BufferedImage(400, 400, BufferedImage.TYPE_INT_RGB);
    // // System.out.println("got image = " + image);
    //		ImageComponent2D imageComponent = new ImageComponent2D(ImageComponent2D.FORMAT_RGB, 400, 400);
    ImageComponent2D imageComponent = new ImageComponent2D(ImageComponent2D.FORMAT_RGB, image);
    // // System.out.println("got imageComponent (setting) = " + imageComponent);
    //		imageComponent.set(image);
    canvas.setOffScreenBuffer(imageComponent);
  }
  
  public BufferedImage getOffScreenBuffer()
  {
    //		BufferedImage image = new BufferedImage(400, 400, BufferedImage.TYPE_INT_RGB);
    ImageComponent2D imageComponent = canvas.getOffScreenBuffer();
    // // System.out.println("got imageComponent = " + imageComponent);
    BufferedImage image = imageComponent.getImage();
    // // System.out.println("got image = " + image);
    return image;
  }
  
  public void setScreen3DSize()
  {
    canvas.getScreen3D().setSize(new Dimension(1280, 1024));
    canvas.getScreen3D().setPhysicalScreenWidth(0.36124);
    canvas.getScreen3D().setPhysicalScreenHeight(0.28899555);
  }
  
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
      canvas.startRenderer();
			//			model.printValues();
  }

  public BranchGroup detachScene()
		{
				examineGroup.removeChild(sceneGroup);
				return sceneGroup;
		}
  
  public void addScene(BranchGroup sc)
		{
				examineGroup.addChild(sc);
		}
  
  public void parserChanged(ParserEvent e)
  {
    setSceneGroup((BranchGroup)(e.getGraphics()).elementAt(graphicNumber));
    // System.out.println("Parser changed in Renderer setSceneGroup to no " + graphicNumber);
  }
  
  public synchronized void addRendWindContainerListener(RendWindContainerListener listener)
  {
    targets.addElement(listener);
  }
  
  public synchronized void removeRendWindContainerListener(RendWindContainerListener listener)
  {
    targets.removeElement(listener);
  }
  
  protected void notifyTargets(String propertyName, Object oldValue, Object newValue)
  {
    Vector l;
    RendWindContainerEvent s = new RendWindContainerEvent((Object) this, propertyName, oldValue, newValue);
    synchronized(this)
    {
      l = (Vector) targets.clone();
    }
    for (int i = 0; i < l.size(); i++)
    {
      RendWindContainerListener sl = (RendWindContainerListener) l.elementAt(i);
      sl.rendWindContainerChanged(s);
    }
  }

		private void setBackgroundColour(String colour)
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
				
		}

		public Shape3D addLine(double x1, double y1, double x2, double y2)
		{
				// System.out.println("adding line " + x1 + y1 + x2 + y2);
				
				Point3d[] vertices = new Point3d[2];
     
				vertices[0] = new Point3d(x1, y1, 0);
				vertices[1] = new Point3d(x2, y2, 0);
     
				int[] indices = { 0, 1};
     
				int[] lineStripCounts = { 2 };
     
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

				/*
					Color3f colour = new Color3f(0, 0, 0);
					geometry.setColor(0, colour);
					geometry.setColor(1, colour);
				*/

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
				// System.out.println("lines.get(line) = " + lines.get(line) + " line = " + line);
				// iterateThroughHashtable(lines);
				examineGroup.removeChild((BranchGroup) lines.remove(line));
		}

		private void iterateThroughHashtable(Hashtable table)
		{
				// System.out.println("table " + table);
				Enumeration keys = table.keys();
				
				for (int i = 0; keys.hasMoreElements(); ++i) 
				{
						Object key = keys.nextElement();
						// System.out.println("Key " + i + " = " + key + " value = " + table.get(key));
				}
		}
  
  public void propertyChange(java.beans.PropertyChangeEvent propertyChangeEvent)
  {
  }
  
  public void groutPanelChanged(GroutPanelEvent e)
  {
    if(e.getPropertyName().equals("WindowEyepointPolicy"))
    {
				canvas.getView().setWindowEyepointPolicy(((Integer) e.getNewValue()).intValue());
				// System.out.println("getWindowEyepointPolicy = " + canvas.getView().getWindowEyepointPolicy());
		}
    if(e.getPropertyName().equals("FitViewToWindow"))
    {
			notifyTargets("FitViewToWindow", (Object) new Boolean(true), (Object) new Boolean(false));
		}
    if(e.getPropertyName().startsWith("Colour"))
    {
	// this needs 1.4
	//	setBackgroundColour(e.getPropertyName().replaceFirst("Colour", ""));
	setBackgroundColour(e.getPropertyName().substring(6));
    }

	}

  public void controlContainerChanged(ControlContainerEvent e)
  {
    // // System.out.println("Controls changed in Renderer");
    Transform3D eyeTrans = new Transform3D();
    // put the View at the standard VRML default position 0,0,10
		// I do not think I want any of this
		/*
    else if(e.getPropertyName().equals("transformXFactor"))
    {
      //            pos = new Vector3f(((Double) e.getNewValue()).doubleValue(), 0, 10);
      centre.x = ((Double) e.getNewValue()).doubleValue();
      // // System.out.println("((Double) e.getNewValue()).doubleValue() = " + ((Double) e.getNewValue()).doubleValue());
      
    } else if(e.getPropertyName().equals("transformYFactor"))
    {
      //            pos = new Vector3f(0, ((Double) e.getNewValue()).doubleValue(), 10);
      pos.y = ((Double) e.getNewValue()).doubleValue();
      eyeTrans.setTranslation(pos);
      vpTransGroup.setTransform(eyeTrans);
    } else if(e.getPropertyName().equals("transformZFactor"))
    {
      //            pos = new Vector3f(0, 0, ((Double) e.getNewValue()).doubleValue());
      pos.z = -((Double) e.getNewValue()).doubleValue();
      eyeTrans.setTranslation(pos);
      vpTransGroup.setTransform(eyeTrans);
    } else if(e.getPropertyName().equals("scaleXFactor"))
    {
      //            pos = new Vector3f(0, 0, ((Double) e.getNewValue()).doubleValue());
      scale.x = ((Double) e.getNewValue()).doubleValue();
      eyeTrans.setScale(scale);
      examineGroup.setTransform(eyeTrans);
    }
		*/
    //        viewTrans.mul(eyeTrans);
    
    // set the view transform
    //        vpTransGroup.setTransform(viewTrans);
    //        examineGroup.setTransform(eyeTrans);
  }
  
  public void viewPositionModelChanged(ViewPositionModelEvent e)
  {
    ViewPositionModel model = (ViewPositionModel) e.getSource();
    // // System.out.println("Model Changed in Renderer by " + e.getPropertyName() + " to " + e.getNewValue());
    //       Transform3D eyeTrans = new Transform3D();
    viewPositionEyeTrans.setIdentity();
    if(e.getPropertyName().equals("parser"))
    {
				/*
      scale.x = model.getScaleX();
      scale.y = model.getScaleY();
			//      setViewpoint();
      viewPositionEyeTrans.setScale(scale);
      canvas.stopRenderer();
      setTransform(model);
      examineGroup.setTransform(viewPositionEyeTrans);
      canvas.startRenderer();
				*/
				setViewParameters(model);
      setViewPosition(model);
    } else if(e.getPropertyName().equals("reset") || e.getPropertyName().equals("fitViewToWindow"))
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
					/*
      //            scale.x = ((Double) e.getNewValue()).doubleValue();
      if(model.getScaleFactorsLocked())
      {
        scale.x = model.getScaleXOnly();
        scale.y = model.getScaleYOnly();
      } else
      {
        scale.x = model.getScaleXOnly();
//        scale.x = ((Double) e.getNewValue()).doubleValue();
      }
      viewPositionEyeTrans.setScale(scale);
      canvas.stopRenderer();
      setTransform(model);
      examineGroup.setTransform(viewPositionEyeTrans);
      canvas.startRenderer();
      //			setTransform(model);
      //			printValues(e);
					*/
				setViewParameters(model);
    } else if(e.getPropertyName().equals("scaleYFactor"))
    {
      //            scale.y = ((Double) e.getNewValue()).doubleValue();
				/*
      if(model.getScaleFactorsLocked())
      {
        scale.x = model.getScaleXOnly();
        scale.y = model.getScaleYOnly();
      } else
      {
        scale.y = model.getScaleYOnly();
      }
      viewPositionEyeTrans.setScale(scale);
      canvas.stopRenderer();
      examineGroup.setTransform(viewPositionEyeTrans);
      setTransform(model);
      canvas.startRenderer();
					*/
				setViewParameters(model);
      //			printValues(e);
    }
    //		JOptionPane.showMessageDialog(canvas, ("ViewPositionModel changed by " + e.getPropertyName() + " from " + e.getSource()));
  }

  public void componentResized(ComponentEvent e)
  {
			// System.out.println("RendWindContainerResized " + graphicNumber + " to " + canvas.getSize());
			notifyTargets("RendWindContainerResized", (Object) canvas.getSize(), (Object) canvas.getSize());
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
  
  public void printValues(ViewPositionModelEvent e)
  {
    ViewPositionModel model = (ViewPositionModel) e.getSource();
    // System.out.print("$%@scaleX = " + model.getScaleX() + " scaleY = " + model.getScaleY() + " ratio = " + (model.getScaleX() / model.getScaleY()));
    if((model.getScaleX() / model.getScaleY()) != model.getXOverY())
    {
      // System.out.print(" wrong should be " + model.getXOverY());
    }
    // System.out.print("$%@scaleXOnly = " + model.getScaleXOnly() + " scaleYOnly = " + model.getScaleYOnly() + " ratio = " + (model.getScaleXOnly() / model.getScaleYOnly()));
    if((model.getScaleXOnly() / model.getScaleYOnly()) != model.getXOverY())
    {
      // System.out.print(" wrong should be " + model.getXOverY());
    }
    // // System.out.println("");
    // System.out.print("scale.x = " + scale.x + " scale.y = " + scale.y + " ratio = " + (scale.x / scale.y) + " scale.z = " + scale.z);
    // System.out.print("pos.x = " + pos.x + " pos.y = " + pos.y + " pos.z = " + pos.z);
    // // System.out.println("");
  }
  
}
