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
*  @author: Copyright (C) Tim Carver
*
***************************************************************/

package org.emboss.jemboss.editor;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.File;
import javax.swing.border.*;
import java.net.URL;

import org.emboss.jemboss.gui.sequenceChooser.SequenceFilter;
import org.emboss.jemboss.gui.filetree.FileEditorDisplay;
import org.emboss.jemboss.gui.ScrollPanel;
import org.emboss.jemboss.gui.Browser;

/**
*  
* Displays a grapical representation of a collection of
* sequences.
*
*/
public class AlignJFrame extends JFrame
{

  private Vector seqs;               // Vector containing Sequence objects
  private Vector graphicSequence;    // Vector containing graphical seqs
  protected JScrollPane jspSequence; // Sequence scrollpane
  protected GraphicSequenceCollection gsc;
  private Matrix mat;
  private PrettyPlotJFrame ppj = null;
  protected JTextField statusField = new JTextField();
  private File sequenceFile = null;
  private Cursor cbusy = new Cursor(Cursor.WAIT_CURSOR);
  private Cursor cdone = new Cursor(Cursor.DEFAULT_CURSOR);
  protected JCheckBoxMenuItem residueColor;
  protected Hashtable currentColour;
  protected boolean useExitMenu = false;  // whether to use 'Exit' or 'Close'
  protected JMenuBar menuBar;
 
  /**
  *
  * @param vseqs	vector containing Sequence objects
  *
  */ 
  public AlignJFrame(Vector vseqs)
  {
    this();
    if(vseqs != null && vseqs.size() > 0)
      openMethod(vseqs);
  }


  /**
  *
  * @param seqFile	sequence file
  *
  */
  public AlignJFrame(File seqFile)
  {
    this();

    SequenceReader sr = new SequenceReader(seqFile);
    sequenceFile = sr.getSequenceFile();
    openMethod(sr.getSequenceVector());
    setTitle("Jemboss Alignment Viewer    :: "+
              sequenceFile.getName());
  }


  /**
  *
  * @param seqString	formatted sequence string
  * @param name 	name of sequence set
  *
  */
  public AlignJFrame(String seqString, String name)
  {
    this();

    SequenceReader sr = new SequenceReader(seqString);
    sequenceFile = null;
    openMethod(sr.getSequenceVector());
    setTitle("Jemboss Alignment Viewer    :: "+name);
  }


  public AlignJFrame()
  {
    this(false);
  }


  /**
  *
  * @param useExitMenu	true if an exit menu is to be displayed
  *			otherwise a close menu is used
  *
  */
  public AlignJFrame(boolean useExitMenu)
  {
    super("Jemboss Alignment Editor");

    this.useExitMenu = useExitMenu;

    final Dimension dScreen = getToolkit().getScreenSize();
    int interval = 10;
    seqs = new Vector();
    mat = new Matrix("resources/resources.jar",
                     "EBLOSUM62");
    
    jspSequence = new JScrollPane();
    jspSequence.getViewport().setBackground(Color.white);

    final JCheckBox leftbutt = new JCheckBox("Select All");
    leftbutt.setBackground(Color.white);
    jspSequence.setCorner(JScrollPane.LOWER_LEFT_CORNER,
                                              leftbutt);
    leftbutt.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        gsc.setSequenceSelection(leftbutt.isSelected());
      }
    });

    final JPanel mainPane = (JPanel)getContentPane();

// set up a menu bar
    menuBar = new JMenuBar();

// File menu
    JMenu fileMenu = new JMenu("File");
    fileMenu.setMnemonic(KeyEvent.VK_F);

// open sequence file
    final JMenuItem calculateCons = new JMenuItem("Consensus");
    final JMenuItem calculatePlotCon = new JMenuItem("Consensus plot");

    JMenuItem openSequence = new JMenuItem("Open...");
    openSequence.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        SequenceReader sr = new SequenceReader();

        if(sr.isReading())
        {
          sequenceFile = sr.getSequenceFile();
          openMethod(sr.getSequenceVector());
          calculateCons.setText("Calculate consensus");
          calculatePlotCon.setText("Calculate consensus plot");
          setTitle("Jemboss Alignment Viewer    :: "+
                    sequenceFile.getName());
        }
      }
    });
    fileMenu.add(openSequence);

// save 
    JMenuItem saveAsMenu = new JMenuItem("Save As...");
    saveAsMenu.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        new SequenceSaver(gsc.getSequenceCollection(),sequenceFile);
      }
    });
    fileMenu.add(saveAsMenu);

  
// print
    JMenu printMenu = new JMenu("Print");
    fileMenu.add(printMenu);

    JMenuItem print = new JMenuItem("Print Postscript...");
    print.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        new PrintAlignment(gsc);
      }
    });
    printMenu.add(print);

//
    JMenuItem printImage = new JMenuItem("Print png/jpeg Image...");
    printImage.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        PrintAlignmentImage pai = new PrintAlignmentImage(gsc);
        pai.print();
      }
    });
    printMenu.add(printImage);
    

// print preview
    JMenuItem printPreview = new JMenuItem("Print Preview...");
    printPreview.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        PrintAlignmentImage pai = new PrintAlignmentImage(gsc);
        pai.printPreview();
      }
    });
    fileMenu.add(printPreview);

    fileMenu.add(new JSeparator());
    if(!useExitMenu)
    {
      JMenuItem close = new JMenuItem("Close");
      close.setAccelerator(KeyStroke.getKeyStroke(
              KeyEvent.VK_E, ActionEvent.CTRL_MASK));

      close.addActionListener(new ActionListener()
      {
        public void actionPerformed(ActionEvent e)
        {
          dispose();
        }
      });
      fileMenu.add(close);
    }
    else         // exit
    {
      JMenuItem fileMenuExit = new JMenuItem("Exit");
      fileMenuExit.addActionListener(new ActionListener()
      {
        public void actionPerformed(ActionEvent e)
        {
          System.exit(0);
        }
      });
      fileMenu.add(fileMenuExit);
    }

    menuBar.add(fileMenu);
  
// View menu
    JMenu viewMenu = new JMenu("View");
    viewMenu.setMnemonic(KeyEvent.VK_V);

// find pattern
    JMenuItem findMenu = new JMenuItem("Find pattern");
    viewMenu.add(findMenu);
    final PatternJFrame patFrame = new PatternJFrame();
    findMenu.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        Point pos = getLocationOnScreen();
        pos.y = pos.y - patFrame.getHeight();
        if(pos.y+patFrame.getHeight() > dScreen.getHeight())
          pos.x = (int)(dScreen.getWidth()-patFrame.getHeight());
        
        patFrame.setLocation(pos);
        patFrame.setGraphic(gsc);
        patFrame.setVisible(true);
        patFrame.toFront();
      }
    });
    viewMenu.add(new JSeparator());

// matrix display
    JMenuItem showMatrix = new JMenuItem("Matrix Display");
    viewMenu.add(showMatrix);
    final MatrixJFrame matFrame = new MatrixJFrame(mat,statusField,
                                                   this);
    showMatrix.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        matFrame.setMatrix(mat);
        matFrame.setVisible(true);
        matFrame.toFront();
      }
    });

// colour display
    JMenuItem showColour = new JMenuItem("Colour Display");
    viewMenu.add(showColour);
    final ColourJFrame colFrame = new ColourJFrame(this);
    showColour.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        Point pos = getLocationOnScreen();
        pos.x = pos.x + getWidth();
        if(pos.x+colFrame.getWidth() > dScreen.getWidth())
          pos.x = (int)(dScreen.getWidth()-colFrame.getWidth());

        colFrame.setLocation(pos);
        colFrame.setCurrentColour(currentColour);
        colFrame.setVisible(true);
        colFrame.toFront();
      }
    });
    viewMenu.add(new JSeparator());
     
    colourMenus(viewMenu);
   
//pretty plot
    final JMenuItem pretty = new JMenuItem("Identical Matches");
    viewMenu.add(pretty);
    viewMenu.add(new JSeparator());

//draw black box
    final JCheckBoxMenuItem drawBoxes = new JCheckBoxMenuItem("Draw boxes",false);
    drawBoxes.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        gsc.setDrawBoxes(drawBoxes.isSelected());
      }
    });
    viewMenu.add(drawBoxes);

//draw colored boxes
    final JCheckBoxMenuItem drawColorBox = new JCheckBoxMenuItem("Colour boxes",true);
    drawColorBox.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        gsc.setDrawColor(drawColorBox.isSelected());
      }
    });
    viewMenu.add(drawColorBox);

    pretty.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        if(ppj == null)
          ppj = new PrettyPlotJFrame(gsc);
        gsc.setMatrix(mat);
        ppj.setVisible(true);
      }
    });
    menuBar.add(viewMenu);

// calculate menu
    JMenu calculateMenu = new JMenu("Calculate");
    menuBar.add(calculateMenu);

// consensus sequence
    final ConsensusOptions options = new ConsensusOptions(jspSequence);
    calculateCons.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        setCursor(cbusy);
        gsc.deleteSequence("Consensus");
        Consensus conseq = new Consensus(mat,   
                    gsc.getSequenceCollection(),
                    options.getPlurality(),
                    options.getCase(),
                    options.getIdentity());

        int fontSize = gsc.getFontSize();
        gsc.addSequence(conseq.getConsensusSequence(),true,5,fontSize);

        if(pretty.isSelected())
          gsc.setPrettyPlot(pretty.isSelected(),ppj);

        Dimension dpane = gsc.getPanelSize();
        gsc.setPreferredSize(dpane);
        gsc.setNamePanelWidth(gsc.getNameWidth());
        jspSequence.setViewportView(gsc);
        setCursor(cdone);
        calculateCons.setText("Recalculate consensus");
      }
    });
    calculateMenu.add(calculateCons);

    JMenuItem consOptions = new JMenuItem("Set consensus options...");
    consOptions.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        try
        {
          Vector vseq = gsc.getSequenceCollection();
          Enumeration enum = vseq.elements();
          float wgt = 0.f;
          while(enum.hasMoreElements())
          {
            Sequence s = (Sequence)enum.nextElement();
            if(!s.getName().equals("Consensus"))
              wgt+=s.getWeight();
          }

          options.setCase(wgt/2.f);
          options.setPlurality(wgt/2.f);
          options.setGraphicSequenceCollection(gsc);
        }
        catch(NullPointerException npe){}
        options.setMatrix(mat);
        options.setVisible(true);
      }
    });
    calculateMenu.add(consOptions);

    calculateMenu.add(new JSeparator());

// consensus plot
    calculatePlotCon.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        setCursor(cbusy);
        gsc.showConsensusPlot(mat,2);
        setCursor(cdone);
        calculatePlotCon.setText("Recalculate Consensus plot");
      }
    });
    calculateMenu.add(calculatePlotCon);
    calculateMenu.add(new JSeparator());

// sort by id
    JMenuItem test = new JMenuItem("Sort by ID");
    calculateMenu.add(test);
    test.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        gsc.idSort();
        jspSequence.setViewportView(gsc);
      }
    });
    
// font menu
    String sizes[] = {"10", "12", "14", "16", "18"};
    final JComboBox fntSize = new JComboBox(sizes);
    fntSize.setSelectedItem("12");
    menuBar.add(fntSize);
    fntSize.setEditable(true);
    Dimension dfont = new Dimension(50,20);
    fntSize.setPreferredSize(dfont);
    fntSize.setMaximumSize(dfont);
    fntSize.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        String fsize = (String)fntSize.getSelectedItem();
        if(gsc !=null)
          gsc.setFontSizeForCollection(Integer.parseInt(fsize));
      }
    });

    setJMenuBar(menuBar);

// help manu
    JMenu helpMenu = new JMenu("Help");
    menuBar.add(helpMenu);
    
    JMenuItem aboutMenu = new JMenuItem("About");
    helpMenu.add(aboutMenu);
    aboutMenu.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        ClassLoader cl = this.getClass().getClassLoader();
        try
        {
          URL inURL = cl.getResource("resources/readmeAlign.html");
          new Browser(inURL,"resources/readmeAlign.html");
        }
        catch (Exception ex)
        {
          JOptionPane.showMessageDialog(null,
                              "Jemboss Alignment Viewer Guide not found!",
                              "Error", JOptionPane.ERROR_MESSAGE);
        }
      }
    });

// set size of sequence panel
    Dimension d = new Dimension(700,300);
    jspSequence.setPreferredSize(d);

    JPanel seqNamePanel = new JPanel(new BorderLayout());
    seqNamePanel.add(jspSequence,BorderLayout.CENTER);

    mainPane.add(jspSequence,BorderLayout.CENTER);

    Border loweredbevel = BorderFactory.createLoweredBevelBorder();
    Border raisedbevel = BorderFactory.createRaisedBevelBorder();
    Border compound = BorderFactory.createCompoundBorder(raisedbevel,loweredbevel);
    statusField.setBorder(compound);
    statusField.setEditable(false);
    statusField.setText("Current matrix: "+mat.getCurrentMatrixName());
    mainPane.add(statusField,BorderLayout.SOUTH);

    addWindowListener(new winExit());
    pack();
    setLocation( (int)(dScreen.getWidth()-getWidth())/3,
                 (int)(dScreen.getHeight()-getHeight())/3 );
    setVisible(true);
  }

  /**
  *
  * Set the scoring matrix
  * @param mat 	scoring matrix to use
  *
  */
  public void setMatrix(Matrix mat)
  {
    this.mat = mat;
  }

  /**
  *
  * Force a re-display of the sequences with a new colour
  * scheme.
  * @param hash		hash of the colour scheme
  *
  */
  public void repaintSequences(Hashtable hash)
  {
    gsc.setColorScheme(hash);
    gsc.repaint();
  }

  /**
  *
  * Given a Vector of Sequence display them in the editor
  * @param seqVector	vector containing Sequence objects
  *
  */
  protected void openMethod(Vector seqVector)
  {
    gsc = new GraphicSequenceCollection(seqVector,
                                   jspSequence,true,false,true,true,
                                   statusField);
// set colour scheme
    gsc.setColorScheme(SequenceProperties.residueColor);
    currentColour = (Hashtable)SequenceProperties.residueColor.clone();
    residueColor.setSelected(true);
    jspSequence.setViewportView(gsc);

    colourScheme("Residue colour");
  }


  /**
  *
  * Update the status bar with the selected colour scheme
  * being used.
  * @param colScheme 	name of colour scheme
  *
  */
  protected void colourScheme(String colScheme)
  {
    String status = statusField.getText();
    int ncol = status.indexOf("Colour Scheme: ");
    if(ncol > -1)
      statusField.setText(status.substring(0,ncol)+
                          "Colour Scheme: "+colScheme);
    else
      statusField.setText(status+"              "+
                          "Colour Scheme: "+colScheme);
  }

  /**
  *
  * Constructs the colour menus
  *
  */
  private void colourMenus(JMenu viewMenu)
  {
    ButtonGroup group = new ButtonGroup();

// property colour menus
    JMenu propertyMenu = new JMenu("Colour by Property");
    viewMenu.add(propertyMenu);

    JCheckBoxMenuItem acidColor =
                new JCheckBoxMenuItem("Red=acidic, Blue=basic");
    propertyMenu.add(acidColor);
    group.add(acidColor);
    acidColor.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        gsc.setColorScheme(SequenceProperties.acidColor);
        jspSequence.setViewportView(gsc);
        currentColour = SequenceProperties.acidColor;
        colourScheme("Red=acidic, Blue=basic");
      }
    });

    JCheckBoxMenuItem polarColor =
                new JCheckBoxMenuItem("Red=polar");
    propertyMenu.add(polarColor);
    group.add(polarColor);
    polarColor.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        gsc.setColorScheme(SequenceProperties.polarColor);
        jspSequence.setViewportView(gsc);
        currentColour = SequenceProperties.polarColor;
        colourScheme("Red=polar");
      }
    });

    JCheckBoxMenuItem hydrophobicColor = 
               new JCheckBoxMenuItem("Red=Hydrophobic");
    propertyMenu.add(hydrophobicColor);
    group.add(hydrophobicColor);
    hydrophobicColor.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        gsc.setColorScheme(SequenceProperties.hydrophobicColor);
        jspSequence.setViewportView(gsc);
        currentColour = SequenceProperties.hydrophobicColor;
        colourScheme("Red=Hydrophobic");
      }
    });

    JCheckBoxMenuItem aromaticColor = 
               new JCheckBoxMenuItem("Red=Aromatic, Blue=Aliphatic");
    propertyMenu.add(aromaticColor);
    group.add(aromaticColor);
    aromaticColor.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        gsc.setColorScheme(SequenceProperties.aromaticColor);
        jspSequence.setViewportView(gsc);
        currentColour = SequenceProperties.aromaticColor;
        colourScheme("Red=Aromatic, Blue=Aliphatic");
      }
    });

    JCheckBoxMenuItem surfaceColor =
               new JCheckBoxMenuItem("Red=Surface, Blue=Buried");
    propertyMenu.add(surfaceColor);
    group.add(surfaceColor);
    surfaceColor.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        gsc.setColorScheme(SequenceProperties.surfaceColor);
        jspSequence.setViewportView(gsc);
        currentColour = SequenceProperties.surfaceColor;
        colourScheme("Red=Surface, Blue=Buried");
      }
    });

    JCheckBoxMenuItem chargeColor  =
               new JCheckBoxMenuItem("Red=Positive, Blue=Negative");
    propertyMenu.add(chargeColor);
    group.add(chargeColor);
    chargeColor.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        gsc.setColorScheme(SequenceProperties.chargeColor);
        jspSequence.setViewportView(gsc);
        currentColour = SequenceProperties.chargeColor;
        colourScheme("Red=Positive, Blue=Negative");
      }
    });

    JCheckBoxMenuItem sizeColor  =
               new JCheckBoxMenuItem("Red=Tiny, Green=Small, Blue=Large");
    propertyMenu.add(sizeColor);
    group.add(sizeColor);
    sizeColor.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        gsc.setColorScheme(SequenceProperties.sizeColor);
        jspSequence.setViewportView(gsc);
        currentColour = SequenceProperties.sizeColor;
        colourScheme("Red=Tiny, Green=Small, Blue=Large");
      }
    });

// other colour schemes
    JCheckBoxMenuItem taylor = new JCheckBoxMenuItem("Taylor Colour");
    viewMenu.add(taylor);
    group.add(taylor);
    taylor.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        gsc.setColorScheme(SequenceProperties.taylorColor);
        jspSequence.setViewportView(gsc);
        currentColour = SequenceProperties.taylorColor;
        colourScheme("Taylor");
      }
    });

    residueColor = new JCheckBoxMenuItem("Residue Colour");
    viewMenu.add(residueColor);
    group.add(residueColor);
    residueColor.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        gsc.setColorScheme(SequenceProperties.residueColor);
        jspSequence.setViewportView(gsc);
        currentColour = SequenceProperties.residueColor;
        colourScheme("Residue");
      }
    });

    JCheckBoxMenuItem rasmolColor = new JCheckBoxMenuItem("Rasmol Colour");
    viewMenu.add(rasmolColor);
    group.add(rasmolColor);
    rasmolColor.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        gsc.setColorScheme(SequenceProperties.rasmolColor);
        jspSequence.setViewportView(gsc);
        currentColour = SequenceProperties.rasmolColor;
        colourScheme("Rasmol");
      }
    });

    JCheckBoxMenuItem nuc = new JCheckBoxMenuItem("Nucleotide Colour");
    viewMenu.add(nuc);
    group.add(nuc);
    nuc.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        gsc.setColorScheme(SequenceProperties.baseColor);
        jspSequence.setViewportView(gsc);
        currentColour = SequenceProperties.baseColor;
        colourScheme("Nucleotide");
      }
    });
    viewMenu.add(new JSeparator());

  }
  
  /**
  *
  * Extends WindowAdapter to close window
  *
  */
  class winExit extends WindowAdapter
  {
     public void windowClosing(WindowEvent we)
     {
        dispose();
     }
  }


  public static void main(String args[])
  {
    Vector seqs = new Vector();
    Sequence s = new Sequence("Seq2","ggcagcttaagccaaacattcccaaatctatgaagcagggcccattgttggtcagttgtt"+
"atttgcaatgaagcacagttctgatcatgtttaaagtggaggcacgcagggcaggagtgc"+
"ttgagcccaagcaaaggatggaaaaaaataagcctttgttgggtaaaaaaggactgtctg"+
"agactttcatttgttctgtgcaacatataagtcaatacagataagtcttcctctgcaaac"+
"ttcactaaaaagcctgggggttctggcagtctagattaaaatgcttgcacatgcagaaac"+
"ctctggggacaaagacacacttccactgaattatactctgctttaaaaaaatccccaaaa"+
"gcaaatgatcagaaatgtagaaattaatggaaggatttaaacatgaccttctcgttcaat"+
"atctactgttttttagttaaggaattacttgtgaacagataattgagattcattgctccg"+
"gcatgaaatatactaataattttattccaccagagttgctgcacatttggagacaccttc"+
"ctaagttgcagtttttgtatgtgtgcatgtagttttgttcagtgtcagcctgcactgcac"+
"agcagcacatttctgcaggggagtgagcacacatacgcactgttggtacaattgccggtg"+
"cagacatttctacctcctgacattttgcagcctacattccctgagggctgtgtgctgagg"+
"gaactgtcagagaagggctatgtgggagtgcatgccacagctgctggctggcttacttct"+
"tccttctcgctggctgtaatttccaccacggtcaggcagccagttccggcccacggttct"+
"gttgtgtagacagcagagactttggagacccggatgtcgcacgccaggtgcaagaggtgg"+
"gaatgggagaaaaggagtgacgtgggagcggagggtctgtatgtgtgcacttgggcacgt"+
"atatgtgtgctctgaaggtcaggattgccagggcaaagtagcacagtctggtatagtctg"+
"aagaagcggctgctcagctgcagaagccctctggtccggcaggatgggaacggctgcctt"+
"gccttctgcccacaccctagggacatgagctgtccttccaaacagagctccaggcactct"+
"cttggggacagcatggcaggctctgtgtggtagcagtgcctgggagttggccttttactc"+
"attgttgaaataatttttgtttattatttatttaacgatacatatatttatatatttatc"+
"aatggggtatctgcagggatgttttgacaccatcttccaggatggagattatttgtgaag"+
"acttcagtagaatcccaggactaaacgtctaaattttttctccaaacttgactgacttgg"+
"gaaaaccaggtgaatagaataagagctgaatgttttaagtaataaacgttcaaactgctc"+
"taagtaaaaaaatgcattttactgcaatgaatttctagaatatttttcccccaaagctat"+
"gcctcctaacccttaaatggtgaacaactggtttcttgctacagctcactgccatttctt"+
"cttactatcatcactaggtttcctaagattcactcatacagtattatttgaagattcagc"+
"tttgttctgtgaatgtcatcttaggattgtgtctatattcttttgcttatttctttttac"+
"tctgggcctctcatactagtaagattttaaaaagccttttcttctctgtatgtttggctc"+
"accaaggcgaaatatatattcttctctttttcatttctcaagaataaacctcatctgctt"+
"ttttgtttttctgtgttttggcttggtactgaatgactcaactgctcggttttaaagttc"+
"aaagtgtaagtacttagggttagtactgcttatttcaataatgttgacggtgactatctt"+
"tggaaagcagtaacatgctgtcttagaaatgacattaataatgggcttaaacaaatgaat"+
"aggggggtccccccactctccttttgtatgcctatgtgtgtctgatttgttaaaagatgg"+
"acagggaattgattgcagagtgtcgcttccttctaaagtagttttattttgtctactgtt"+
"agtatttaaagatcctggaggtggacataaggaataaatggaagagaaaagtagatattg"+
"tatggtggctactaaaaggaaattcaaaaagtcttagaacccgagcacctgagcaaactg"+
"cagtagtcaaaatatttatctcatgttaaagaaaggcaaatctagtgtaagaaatgagta"+
"ccatatagggttttgaagttcatatactagaaacacttaaaagatatcatttcagatatt"+
"acgtttggcattgttcttaagtatttatatctttgagtcaagctgataattaaaaaaaat"+
"ctgttaatggagtgtatatttcataatgtatcaaaatggtgtctatacctaaggtagcat"+
"tattgaagagagatatgtttatgtagtaagttattaacataatgagtaacaaataatgtt"+
"tccagaagaaaggaaaacacattttcagagtgcgtttttatcagaggaagacaaaaatac"+
"acacccctctccagtagcttatttttacaaagccggcccagtgaattagaaaaacaaagc"+
"acttggatatgatttttggaaagcccaggtacacttattattcaaaatgcacttttactg");

    Sequence s1 = new Sequence("Seq1a","ACTATACAGAGTAGACTgTATAGAtTATAAGCGACATACGAGAGACGAC");
    s1.reverseSequence();
//  seqs.add(s1);
//  seqs.add(s);
//  seqs.add(new Sequence("Seq1b","ACTATACAGAGTAGACTgTATAGAtTATAAGCGACATACGAGAGACGAC"));
//  seqs.add(new Sequence("Seq2a","AAAAAACAGAGTAGACTgTATAGAtTATAAGCGACATACGAGAGACGAC"));
//  seqs.add(new Sequence("Seq3","ACTATACAGAGTAGACTgTATAGAtTATAAGCGACATACGAGAGACGAC"));
//  seqs.add(new Sequence("Seq4","AAAAAACAGAGTAGACTgTATAGAtTATAAGCGACATACGAGAGACGAC"));
//  seqs.add(new Sequence("Seq5","ACTATACAGAGTAGACTgTATAGAtTATAAGCGACATACGAGAGACGAC"));
//  seqs.add(new Sequence("Seq6","AAAAAACAGAGTAGACTgTATAGAtTATAAGCGACATACGAGAGACGAC"));
//  seqs.add(new Sequence("Seq111","ACTATACAGAGTAGACTgTATAGAtTATAAGCGACATACGAGAGACGAC"));


    new AlignJFrame(true);
  }


}

