/*
 * ZoomToOccuranceFactory.java
 *
 * Created on June 24, 2002, 11:46 AM
 */

package org.emboss.grout;

import java.awt.event.MouseListener;
import java.awt.event.MouseEvent;

import com.sun.j3d.utils.picking.*;
import javax.media.j3d.*;
import javax.vecmath.Point3d;
import javax.vecmath.Point2d;
import java.awt.Point;
import java.awt.Dimension;

/*
import ;
*/

public class ZoomToOccuranceFactory implements PickOccuranceFactory, 
		MouseListener
{

		GroutPanel originator;

		Point first;

		public ZoomToOccuranceFactory(GroutPanel origin)
		{
				originator = origin;
		}

		public void PerformPick(double x1, double y1, double x2, double y2)
		{
				// System.out.println("ZoomToOccurance at (" + x1 + ", " + y1 + 
				//													 ") to (" + x2 + ", " + y2  + ")");
		}

		public void PerformPick(double x, double y)
		{
				// System.out.println("ZoomToOccurance at (" + x + ", " + y + ")");
		}

		public void mouseClicked(MouseEvent me)
		{
				// System.out.println("mouseClicked = " + me);
				// System.out.println("mouseClicked = " + me + " from " + me.getSource());
				Canvas3D canvas = (Canvas3D) me.getSource();
				// System.out.println("Canvas3D.getCenterEyeInImagePlate() " + canvas.getCenterEyeInImagePlate());
				// System.out.println("canvas.getParent.canvas.getParent = " + canvas.getParent().getParent());
				OneGraphic graphic1 = (OneGraphic) canvas.getParent().getParent();
				ViewPositionModel model = graphic1.getViewPositionModel();
				PickCanvas pickCanvas = new PickCanvas(canvas,
																							 graphic1.getRendWindContainer().getUniverse().getLocale());
				/*
				PickCanvas pickCanvas = new PickCanvas(((Canvas3D) me.getSource()),
																							 ((OneGraphic) me.getComponent()).getRendWindContainer().getUniverse().getLocale());
				*/
        pickCanvas.setMode(PickTool.GEOMETRY_INTERSECT_INFO);
        pickCanvas.setTolerance(5.0f);
        pickCanvas.setShapeLocation(me);
        
        PickResult pickResult = pickCanvas.pickClosest();

        if (pickResult != null)
        {
          //                    // // System.out.println("in pick result");
          
          PickIntersection pickIntersection = pickResult.getClosestIntersection( pickCanvas.getStartPosition());
          
          if (pickIntersection != null)
          {
            Point3d p3d1 = pickIntersection.getClosestVertexCoordinates();
            Point3d[] coords = pickIntersection.getPrimitiveCoordinates();
            int index = pickIntersection.getClosestVertexIndex();
            int[] arrayIndex = pickIntersection.getPrimitiveCoordinateIndices();
            GeometryArray geometry = pickIntersection.getGeometryArray();
            
            Point3d p3d2 = new Point3d();
            geometry.getCoordinate(index, p3d2);
            
						Point3d top = new Point3d(Double.NEGATIVE_INFINITY, 
																			Double.NEGATIVE_INFINITY, 0);
						Point3d bottom = new Point3d(Double.POSITIVE_INFINITY,
																				 Double.POSITIVE_INFINITY, 0);
            
            // System.out.print("Match between");
            for(int i = 0; i < coords.length; ++i)
            {
              System.err.print(" point " + i + " = " + coords[i]);
							if(coords[i].x > top.x)
							{
							top.x = coords[i].x;
							}
							if(coords[i].y > top.y)
							{
							top.y = coords[i].y;
							}
							if(coords[i].x < bottom.x)
							{
							bottom.x = coords[i].x;
							}
							if(coords[i].y < bottom.y)
							{
							bottom.y = coords[i].y;
							}
            }
            System.err.println("");

						model.setViewPosition(bottom, top);
          }
        } else
        {
          System.err.println("Cannot find line, please try again.");
        }
		}
		
		public void mouseEntered(MouseEvent me)
		{
		}
		public void mouseExited(MouseEvent me)
		{
		}
		public void mousePressed(MouseEvent me)
		{
				first = me.getPoint();
		}
		public void mouseReleased(MouseEvent me)
		{
				if(first.x != (me.getPoint()).x || first.y != (me.getPoint()).y)
				{
						Canvas3D canvas = (Canvas3D) me.getSource();
						// System.out.println("Draged from " + first + " to " + me.getPoint());
						
						Point bottom = new Point();
						Point top = new Point();

						if(first.x > me.getPoint().x)
						{
								bottom.x = me.getPoint().x;
								top.x = first.x;
						}
						else
						{
								top.x = me.getPoint().x;
								bottom.x = first.x;
						}
						if(first.y < me.getPoint().y)
						{
								bottom.y = me.getPoint().y;
								top.y = first.y;
						}
						else
						{
								top.y = me.getPoint().y;
								bottom.y = first.y;
						}

						// System.out.println("top = " + top + " bottom = " + bottom);

						Dimension screenSize = canvas.getScreen3D().getSize();
						Dimension canvasSize = canvas.getSize();
						
						// System.out.println("screenSize " + screenSize );
						// System.out.println("canvasSize " +  canvasSize);


						double changeBottomXBy = (double) bottom.x / (double) canvasSize.width ;
						double changeTopXBy = (double) top.x / (double) canvasSize.width;
						double changeBottomYBy = 1 - (double) bottom.y / (double) canvasSize.height;
						double changeTopYBy = 1 - (double) top.y / (double) canvasSize.height;

						// System.out.println("proportions = " + changeBottomXBy + ", " + changeBottomYBy + ", " + + changeTopXBy + ", " + + changeTopYBy);

						OneGraphic graphic1 = (OneGraphic) canvas.getParent().getParent();
						ViewPositionModel model = graphic1.getViewPositionModel();

						model.setFractionalWindow(changeBottomXBy, changeBottomYBy,
																			changeTopXBy, changeTopYBy);

				}
		}
		

}
