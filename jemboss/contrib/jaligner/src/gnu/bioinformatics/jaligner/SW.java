/**
 * $Id: SW.java,v 1.1 2003/09/09 10:47:44 timc Exp $
 * @author Ahmed Moustafa (ahmed at users.sourceforge.net)
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
 */

package gnu.bioinformatics.jaligner;

import gnu.bioinformatics.jaligner.algorithms.SmithWaterman;
import gnu.bioinformatics.jaligner.formats.FASTA;
import gnu.bioinformatics.jaligner.formats.Pair;
import gnu.bioinformatics.jaligner.util.Alignment;
import gnu.bioinformatics.jaligner.util.Matrices;
import gnu.bioinformatics.jaligner.util.Parser;

import java.io.File;
import java.io.FileOutputStream;
import java.io.PrintStream;
import java.text.DecimalFormat;
import java.util.Date;
import java.util.List;

/**
 * A front-end class to the Smith-Waterman algorithm class.
 * It provides static methods to called by other class,
 * and a main method to be called from the command line.
 * 
 * @author Ahmed Moustafa (ahmed at users.sourceforge.net)
 * @version $Revision: 1.1 $
 */

public class SW {
	
	public static final float DEFAULT_OPEN_GAP_PENALTY = 10.0f;
	public static final float DEFAULT_EXTEND_GAP_PENALTY = 0.5f;
	public static final String DEFAULT_MATRIX = "BLOSUM62";
	
	public static void main (String[] args) {
		try {
			String s1 = null;
			String s2 = null;
			String matrix = DEFAULT_MATRIX;
			float open = DEFAULT_OPEN_GAP_PENALTY;
			float extend = DEFAULT_EXTEND_GAP_PENALTY;
			boolean all = false;
			String file = null;
			
			for (int i = 0; i < args.length; i++) {
				if (args[i].charAt(0) == '-') {
					if (args[i].equals("-m")) {
						matrix = args[++i]; 
					} else if (args[i].equals("-o")) {
						open = Float.parseFloat(args[++i]);
					} else if (args[i].equals("-e")) {
						extend = Float.parseFloat(args[++i]);
					} else if (args[i].equals("-a")) {
						all = true;
					} else if (args[i].equals("-f")) {
						file = args[++i];
					} else if (args[i].equals("-h") || args[i].equals("-help")) {
						printUsage();
						System.exit(0);
					} else {
						System.out.println ( "Unrecognized option: " + args[i]);
						printUsage();
						System.exit(1);
					}
				} else {
					if (s1 == null) {
						s1 = args[i];
					} else if (s2 == null) {
						s2 = args[i];
					} else {
						System.out.println ( "Unexpected token: " + args[i]);
						printUsage();
						System.exit(1);
					}
				}
			}
			
			if (s1 == null) {
				System.out.println ( "Sequence #1 is null" );
				printUsage();
				System.exit(1);
			}
			if (s2 == null) {
				System.out.println ( "Sequence #2 is null" );
				printUsage();
				System.exit(1);
			}
			
			FASTA fasta1 = Parser.loadFASTA(s1);
			FASTA fasta2 = Parser.loadFASTA(s2);
			
			if (file != null) {
				System.setOut(new PrintStream(new FileOutputStream(file, true)));
			}
			
			printHeader(file == null ? "stdout" : file);
			
			if (all) {
				List alignments = alignAll(fasta1, fasta2, matrix, open, extend);
				System.out.println ( "# of alignments = " + alignments.size ( ) );
				System.out.println ( );
				Alignment alignment = null;
				for (int i = 0, n = alignments.size(); i < n; i++) {
					alignment = (Alignment) alignments.get ( i );
					printStatistics (alignment, matrix, open, extend);
					System.out.println ( Pair.format(alignment) );
					printTailer();
				}
			} else {
				Alignment alignment = align(fasta1, fasta2, matrix, open, extend);
				printStatistics (alignment, matrix, open, extend);
				System.out.println ( Pair.format(alignment) );
				printTailer();
			}
		} catch (Exception e) {
			e.printStackTrace();
			System.exit(1);
		}
	}	
	
	/**
	 * Returns alignments to sequence1 and sequence2 using Smith-Waterman.
	 * All the returned alignments will have the same maximum score.
	 * @param sequence1	1st sequence in plain format
	 * @param sequence2	2nd sequence in plain format
	 * @param matrix	scoring matrix
	 * @param open		open gap penalty
	 * @param extend	extend gap penalty
	 * @return an array of alignments
	 * @throws Exception
	 * @see Alignment
	 */
	public static List alignAll(String sequence1, String sequence2, String matrix, float open, float extend) throws Exception {
		
		char[] ch1 = Parser.prepareSequence(sequence1.toUpperCase());
		char[] ch2 = Parser.prepareSequence(sequence2.toUpperCase());
		
		return alignAll(ch1, ch2, matrix, open, extend);
	}
	
	/**
	 * Returns an alignment to sequence1 and sequence2 using Smith-Waterman.
	 * @param sequence1	1st sequence in plain format
	 * @param sequence2	2nd sequence in plain format
	 * @param matrix	scoring matrix
	 * @param open		open gap penalty
	 * @param extend	extend gap penalty
	 * @return an alignment
	 * @throws Exception
	 * @see Alignment
	 */
	public static Alignment align (String sequence1, String sequence2, String matrix, float open, float extend) throws Exception {
		Alignment alignment = align(sequence1.toUpperCase(), sequence2.toUpperCase(), matrix, open, extend);
		return alignment;
	}
	

	/**
	 * Returns alignments to sequence1 and sequence2 using Smith-Waterman.
	 * All the returned alignments will have the same maximum score.
	 * @param fasta1	1st sequence in FASTA format
	 * @param fasta2	2nd sequence in FASTA format
	 * @param matrix	scoring matrix
	 * @param open		open gap penalty
	 * @param extend	extend gap penalty
	 * @return an array of alignments
	 * @throws Exception
	 * @see Alignment
	 * @see FASTA
	 */
	public static List alignAll (FASTA fasta1, FASTA fasta2, String matrix, float open, float extend) throws Exception {
		char[] ch1 = fasta1.getSequence();
		char[] ch2 = fasta2.getSequence();
		List alignments = alignAll(ch1, ch2, matrix, open, extend);
		for (int i = 0, n = alignments.size(); i < n; i++) { 
			((Alignment)alignments.get(i)).setName1(fasta1.getName());
			((Alignment)alignments.get(i)).setName2(fasta2.getName());
		}
		return alignments;
	}

	/**
	 * Returns alignments to sequence1 and sequence2 using Smith-Waterman.
	 * @param fasta1	1st sequence in FASTA format
	 * @param fasta2	2nd sequence in FASTA format
	 * @param matrix	scoring matrix
	 * @param open		open gap penalty
	 * @param extend	extend gap penalty
	 * @return an alignment
	 * @throws Exception
	 * @see Alignment
	 * @see FASTA
	 */
	public static Alignment align (FASTA fasta1, FASTA fasta2, String matrix, float open, float extend) throws Exception {
		char[] ch1 = fasta1.getSequence();
		char[] ch2 = fasta2.getSequence();
		Alignment alignment = align(ch1, ch2, matrix, open, extend);
		alignment.setName1(fasta1.getName());
		alignment.setName2(fasta2.getName());
		return alignment;
	}

	/**
	 * Returns alignments to sequence1 and sequence2 using Smith-Waterman.
	 * All the returned alignments will have the same maximum score.
	 * @param file1		1st sequence in FASTA format in file1
	 * @param file2		2nd sequence in FASTA format in file2
	 * @param matrix	scoring matrix
	 * @param open		open gap penalty
	 * @param extend	extend gap penalty
	 * @return list of alignments
	 * @throws Exception
	 * @see Alignment
	 * @see FASTA
	 */
	public static List alignAll (File file1, File file2, String matrix, float open, float extend) throws Exception {
		
		FASTA fasta1 = Parser.loadFASTA(file1);
		FASTA fasta2 = Parser.loadFASTA(file2);
		
		return alignAll (fasta1, fasta2, matrix, open, extend);
	}

	/**
	 * Returns an alignment to sequence1 and sequence2 using Smith-Waterman.
	 * @param file1		1st sequence in FASTA format in file1
	 * @param file2		2nd sequence in FASTA format in file2
	 * @param matrix	scoring matrix
	 * @param open		open gap penalty
	 * @param extend	extend gap penalty
	 * @return an alignment
	 * @throws Exception
	 * @see Alignment
	 * @see FASTA
	 */
	public static Alignment align (File file1, File file2, String matrix, float open, float extend) throws Exception {
		Alignment alignment = align(file1, file2, matrix, open, extend);
		return alignment;
	}

	
	/**
	 * Returns possible aligments with the same maximum alignment score
	 * All the returned alignments will have the same maximum score.
	 * @param	sequence1	1st sequence
	 * @param	sequence2	2nd sequence
	 * @param	matrix		the scoring matrix (filename)
	 * @param	open		open gap penalty
	 * @param	extend		extend gap penalty
	 * @return	a list of alignments
	 * @see		Alignment
	 */
	public static List alignAll (char[] sequence1, char[] sequence2, String matrix, float open, float extend) throws Exception {
		Matrices matrices = Matrices.getInstance();
		SmithWaterman sw = new SmithWaterman( );
		List alignments = sw.alignAll(sequence1, sequence2, matrices.get(matrix), open, extend);
		return alignments;
	}
	
	/**
	 * Returns an alignment
	 * @param	sequence1	1st sequence
	 * @param	sequence2	2nd sequence
	 * @param	matrix		the scoring matrix (filename)
	 * @param	open		open gap penalty
	 * @param	extend		extend gap penalty
	 * @return alignment
	 * @see		Alignment
	 */
	public static Alignment align (char[] sequence1, char[] sequence2, String matrix, float open, float extend) throws Exception {
		Matrices matrices = Matrices.getInstance();
		SmithWaterman sw = new SmithWaterman( );
		Alignment alignment = sw.align(sequence1, sequence2, matrices.get(matrix), open, extend);
		return alignment;
	}
	
	/**
	 * Prints the usage of the SW through the command line 
	 *
	 */
	private static void printUsage( ) {
		System.out.println ( ) ;
		System.out.println ( "Usage:" );
		System.out.println ( "java -classpath jaligner.jar:matrices.jar gnu.bioinformatics.jaligner.SW <seq1> <seq2> [-m matrix] [-o open] [-e extend] [-a] [-f output]" );
		System.out.println ( "or") ;
		System.out.println ( "java -jar jaligner.jar <seq1> <seq2> <-m matrix> [-o open] [-e extend] [-a] [-f output]" );
		System.out.println ( "Example:" );
		System.out.println ( "java -classpath jaligner.jar:matrices.jar gnu.bioinformatics.jaligner.SW seq1.fasta seq2.fasta -m BLOSUM62 -o 10.0 -e 0.5 -a -f out.txt" );
		System.out.println ( ) ;
	}

	private static void printHeader (String output) {
		System.out.println("########################################");
		System.out.println("# Program:\t" + SW.class.getName());
		System.out.println("# Rundate:\t" + new Date().toString ( ));
		System.out.println("# Report_file:\t" + output);
		System.out.println("########################################");
	}

	private static void printStatistics (Alignment alignment, String matrix, float open, float extend) {
		DecimalFormat f1 = new DecimalFormat( "0.00" );
		DecimalFormat f2 = new DecimalFormat( "0.00%" );

		int length = alignment.getSequence1().length;
		int identity = alignment.getIdentity();
		int similarity = alignment.getSimilarity();
		int gaps = alignment.getGaps();
		float score = alignment.getScore();

		System.out.println("#=======================================");
		System.out.println("#");
		System.out.println("# Aligned_sequences: 2");
		System.out.println("# 1: " + alignment.getName1());
		System.out.println("# 2: " + alignment.getName2());
		System.out.println("# Matrix: " + matrix);
		System.out.println("# Gap_penalty: " + open);
		System.out.println("# Extend_penalty: " + extend);
		System.out.println("#");
		System.out.println("# Length: " + length );
		System.out.println("# Identity: " +  identity + "/" + length + " (" + f2.format(identity/(float)length) + ")");  
		System.out.println("# Similarity: " + similarity + "/" + length + " (" + f2.format(similarity/(float)length) + ")");
		System.out.println("# Gaps: " + gaps + "/" + length + " (" + f2.format(gaps/(float)length) + ")");
		System.out.println("# Score: " + f1.format(alignment.getScore()));
		System.out.println("#");
		System.out.println("#");
		System.out.println("#=======================================");
	}

	private static void printTailer ( ) {
		System.out.println("#---------------------------------------");
		System.out.println("# Finished:\t" + new Date().toString ( ) );
		System.out.println("#---------------------------------------");
	}
}