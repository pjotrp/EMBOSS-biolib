/*
 * ShowSequenceOccuranceFactory.java
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
import java.util.Vector;
import java.util.Enumeration;
import java.util.Hashtable;
import javax.swing.JOptionPane;

import org.emboss.jemboss.editor.*;

public class ShowSequenceOccuranceFactory implements PickOccuranceFactory, 
		GroutAlignJFrameListener
{
  
		private double x1;
		private double y1;
		private double x2;
		private double y2;

		private GroutPanel originator;

		private Hashtable positionMarkersX = null;
		private Hashtable positionMarkersY = null;
		private String xSequence;
		private String ySequence;
		private String xSequenceName;
		private String ySequenceName;

		private GroutAlignJFrame alignEditor;
		private Vector sequences;
		private Vector indents;

		private boolean jumpTo = false;

		public ShowSequenceOccuranceFactory(GroutPanel origin)
		{
				originator = origin;
				xSequence = origin.getXSequence();
				ySequence = origin.getYSequence();
		}

		public void PerformPick()
		{
				
		}

		public void PerformPick(double x, double y)
		{
				
		}
		public void PerformPick(double x, double y, double x2, double y2)
		{
				
		}

		public void mouseClicked(MouseEvent me)
		{
				// System.out.println("mouseClicked = " + me);

				if((xSequence == null) && (ySequence ==null))
				{
						JOptionPane.showMessageDialog(null, "Sorry, I have no sequence for this display", "No Sequence",
																					JOptionPane.ERROR_MESSAGE);
				}
				else
				{

						Canvas3D canvas = (Canvas3D) me.getSource();

						if(!jumpTo)
						{
								Dimension screenSize = canvas.getScreen3D().getSize();
								Dimension canvasSize = canvas.getSize();
						
								// System.out.println("qwe screenSize " + screenSize );
								// System.out.println("qwe canvasSize " +  canvasSize);
								// System.out.println("qwe me.getPoint().y " +  me.getPoint().y);

								OneGraphic graphic1 = (OneGraphic) canvas.getParent().getParent();
								ViewPositionModel model = graphic1.getViewPositionModel();

								double topX = model.getTopX();
								double bottomX = model.getBottomX();
								double topY = model.getTopY();
								double bottomY = model.getBottomY();

								double sequencePosX = ((bottomX + ((topX - bottomX) 
																									 * ((double) me.getPoint().x
																											/(double) canvasSize.width))));
								double sequencePosY = ((bottomY + ((topY - bottomY) 
																									 * (((double) canvasSize.height
																											 - (double) me.getPoint().y)
																											/(double) canvasSize.height))));
								// System.out.println("qwe sequencePosY " +  sequencePosY);

								// System.out.println("sequencePosX " + sequencePosX);
								// System.out.println("me.getPoint().x " + me.getPoint().x);
								// System.out.println("topX " + topX);
								// System.out.println("bottomX " + bottomX);
								//				System.out.println(" " + );
								//				System.out.println(" " + );

								if(positionMarkersX == null)
								{
										// System.out.println("now positionMarkers = " + positionMarkers);
										String xSequenceName = graphic1.getXTitle();
										String ySequenceName = graphic1.getYTitle();

										// System.out.println("xSequenceName = " + xSequenceName + " ySequenceName = " + ySequenceName);

										GroutSequence sequence1 = new GroutSequence(xSequenceName, 
																																xSequence);
										sequences = new Vector();
										indents = new Vector();
										sequences.addElement(sequence1);
								
										double position = 0;

										if(ySequence != null)
										{
												positionMarkersX = 
														originator.addSinglePositionMarker(sequencePosX, graphic1);
												positionMarkersY = 
														originator.addYPositionMarker(sequencePosY, graphic1);
												GroutSequence sequence2 = new GroutSequence(ySequenceName,
																																		ySequence);
												sequences.addElement(sequence2);
												position = alterIndent(sequencePosX, sequencePosY);
										}
										else
										{
												positionMarkersX = 
														originator.addSinglePositionMarker(sequencePosX, graphic1);
												indents.addElement(new Integer(0));
												position = sequencePosX;
										}

										alignEditor = new GroutAlignJFrame(sequences, indents);
										alignEditor.setPosition((int)position);
										// System.out.println("alignEditor.setPosition((int)position) = " + position);
										alignEditor.addGroutAlignJFrameListener(this);
										// System.out.println("adding alignEditor.addGroutAlignJFrameListener");
								}
								else
								{
										// System.out.println("moving the line cos positionMarkers = " + positionMarkers);
										// for(int i = 0; i < positionMarkersX.size(); ++i)
										// {
										// moveXLineTo(((Shape3D) positionMarkersX.elementAt(i)), sequencePosX);
										// }
										// moveYLineTo(((Shape3D) positionMarkersY.elementAt(0)), sequencePosY);
										double position = 0;

										position = alterIndent(sequencePosX, sequencePosY);
										// System.out.println("Setting position to " + position);
										alignEditor.setIndents(indents);
										alignEditor.setPosition((int)position);
								}
						}
						else
						{
								// System.out.println("Show Sequence of line canvas = " + canvas);
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
										// System.out.println("shapes in pick result = " + pickResult.getCompressedGeometryShape3Ds());
          
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
            
												// System.out.println("p3d1 = " + p3d1);
												// System.out.println("coords = " + coords);
												// System.out.println("index = " + index);
												// System.out.println("arrayIndex = " + arrayIndex);
												// System.out.println("geometry = " + geometry);
												// System.out.print("Match between");
												// not sure you need this messin' about, 
												// but it helps with different shapes
												for(int i = 0; i < coords.length; ++i)
												{
														// System.out.print(" point " + i + " = " + coords[i]);
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
												// System.out.println("");

												double position = 0;

												position = alterIndent(((bottom.x / 2) + (top.x / 2)),
																							 ((bottom.y / 2) + (top.y / 2)));
												// System.out.println("Setting position to " + position);
												alignEditor.setIndents(indents);
												alignEditor.setPosition((int)position);
												// model.setViewPosition(bottom, top);
												// System.out.println("
										}
								} else
								{
										System.err.println("Cannot find line, please try again.");
								}
						}
						// System.out.println(xSequence.substring(((int)sequencePosX - 5), ((int)sequencePosX + 5)));
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
		}
		public void mouseReleased(MouseEvent me)
		{
		}

		public void removeLine(Shape3D line)
		{
				// Group parent = ((Group) line.getParent());
				// parent.removeChild(line);

				originator.removeLine(line);
		}
		public void removeLine(Shape3D line, OneGraphic graphic)
		{
				originator.removeLine(line, graphic);
		}

		public void moveXLineTo(Shape3D line, double sequencePosX)
		{
				// IndexedLineStripArray geometry = (IndexedLineStripArray) line.getGeometry();
				for (Enumeration e = line.getAllGeometries() ; e.hasMoreElements() ;)
				{
						IndexedLineStripArray geometry = (IndexedLineStripArray) e.nextElement();

						Point3d point0 = new Point3d();
						geometry.getCoordinate(0, point0);
						point0.x = sequencePosX;
						geometry.setCoordinate(0, point0);
						Point3d point1 = new Point3d();
						geometry.getCoordinate(1, point1);
						point1.x = sequencePosX;
						geometry.setCoordinate(1, point1);
				
				}
		}

		public void moveYLineTo(Shape3D line, double sequencePosY)
		{
				// IndexedLineStripArray geometry = (IndexedLineStripArray) line.getGeometry();
				for (Enumeration e = line.getAllGeometries() ; e.hasMoreElements() ;)
				{
						IndexedLineStripArray geometry = (IndexedLineStripArray) e.nextElement();

						Point3d point0 = new Point3d();
						geometry.getCoordinate(0, point0);
						point0.y = sequencePosY;
						geometry.setCoordinate(0, point0);
						Point3d point1 = new Point3d();
						geometry.getCoordinate(1, point1);
						point1.y = sequencePosY;
						geometry.setCoordinate(1, point1);
				
				}
		}

		public double alterIndent(double sequencePosX, double sequencePosY)
		{
				if(indents.size() < 2)
				{
						indents.setSize(2);
				}
				double position;
				if(sequencePosY > sequencePosX)
				{
						indents.setElementAt(new Integer((int) (sequencePosY 
																									- sequencePosX)), 0);
						indents.setElementAt(new Integer(0), 1);
						position = sequencePosY;
						// System.out.println("(sequencePosY > sequencePosX so 2nd indent = " + (sequencePosY - sequencePosX));
						// System.out.println("(sequencePosY = " +  sequencePosY);
						// System.out.println("(sequencePosX = " +  sequencePosX);
				}
				else
				{
						indents.setElementAt(new Integer(0), 0);
						indents.setElementAt(new Integer((int) (sequencePosX 
																									- sequencePosY)), 1);
						position = sequencePosX;
						// System.out.println("(sequencePosY < sequencePosX so 1st indent = " + (sequencePosX - sequencePosY));
						// System.out.println("(sequencePosY = " +  sequencePosY);
						// System.out.println("(sequencePosX = " +  sequencePosX);
				}
				// System.out.println("indents.elementAt(0) = " + indents.elementAt(0) + " indents.elementAt(1) = " + indents.elementAt(1) + "indents.size()  = " + indents.size());

				return position;
		}

		private void setPickMethod(Boolean jumpTo)
		{
				// System.out.println("setPickMethod to " + jumpTo);
				this.jumpTo = jumpTo.booleanValue();

		}

		public void groutAlignJFrameChanged(GroutAlignJFrameEvent e)
		{
				// System.out.println("groutAlignJFrameChanged by " + e.getPropertyName());
				String whatChanged = e.getPropertyName();
				if(whatChanged.equals("sequencePositionChanged"))
				{
						// for(int i = 0; i < positionMarkersX.size(); ++i)
						for (Enumeration keys = positionMarkersX.keys(); keys.hasMoreElements();) 
						{
								moveXLineTo(((Shape3D) keys.nextElement()),
														((double) (((Integer)e.getNewValue()).intValue() - 
																			 ((Integer)indents.elementAt(0)).intValue())));
						}
						if(ySequence != null)
						{
								/*
								Enumeration keys = positionMarkersY.keys();
								moveYLineTo(((Shape3D) keys.nextElement()),
														((double) (((Integer)e.getNewValue()).intValue() - 
																			 ((Integer)indents.elementAt(1)).intValue())));
								*/
								for (Enumeration keys = positionMarkersY.keys(); keys.hasMoreElements();) 
								{
										moveYLineTo(((Shape3D) keys.nextElement()),
																((double) (((Integer)e.getNewValue()).intValue() - 
																					 ((Integer)indents.elementAt(1)).intValue())));
								}
						}
				}
				else if(whatChanged.equals("windowClosed"))
				{
						// System.out.println("whatChanged.equals(windowClosed");
						// for(int i = 0; i < positionMarkersX.size(); ++i)
						for (Enumeration keys = positionMarkersX.keys();
								 keys.hasMoreElements();) 
						{
								Shape3D key = (Shape3D) keys.nextElement();
								removeLine(key, ((OneGraphic) positionMarkersX.get(key)));
						}
						if(ySequence != null)
						{
								for (Enumeration keys = positionMarkersY.keys();
										 keys.hasMoreElements();) 
								{
										Shape3D key = (Shape3D) keys.nextElement();
										removeLine(key, ((OneGraphic) positionMarkersY.get(key)));
								}
								// removeLine((Shape3D) positionMarkersY.elementAt(0));
						}
						originator.changePickFactory("ShowSequence");
				}
				else if(whatChanged.equals("newAlignJFrame"))
				{
						// System.out.println("originator.changePickFactory(ShowSequence");
						originator.changePickFactory("ShowSequence");
				}
				else if(whatChanged.equals("JumpToNearestAlignment"))
				{
						// System.out.println("originator.changePickFactory(ShowSequence");
						setPickMethod((Boolean) e.getNewValue());
				}
		}
		
}
