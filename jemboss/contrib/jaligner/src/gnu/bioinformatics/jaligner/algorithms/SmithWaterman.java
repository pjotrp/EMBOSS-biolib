/**
 * $Id: SmithWaterman.java,v 1.2 2003/09/10 10:59:22 timc Exp $
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

package gnu.bioinformatics.jaligner.algorithms;

import gnu.bioinformatics.jaligner.util.Alignment;

import java.util.ArrayList;
import java.util.List;

/**
 * An implementation of the Smith-Waterman algorithm for pairwise sequence local alignment.
 * The basic algorithm is extended to support affine gap penatlies with Gotoh's improvement.
 * 
 * @author Ahmed Moustafa (ahmed at users.sourceforge.net)
 * @version $Revision: 1.2 $
 */

public class SmithWaterman {
	
	private static final byte DIAGONAL = 0;
	private static final byte LEFT = 1;
	private static final byte UP = 2;
	private static final byte DIAGONAL_LEFT = 3;
	private static final byte DIAGONAL_UP = 4;
	private static final byte LEFT_UP = 5;
	private static final byte DIAGONAL_LEFT_UP = 6;
	
	float score = Float.NEGATIVE_INFINITY;
	int startTracebackRow;
	int startTracebackCol;
	
	/**
	 * Constructor for SmithWaterman
	 */
	public SmithWaterman( ) {
		super( );
	}

	/**
	 * Aligns two sequences by Smith-Waterman algorithm
	 * @param sequence1 1<sup>st</sup> sequence
	 * @param sequence2 2<sup>nd</sup> sequence
	 * @param matrix scoring matrix (filename)
	 * @param open open gap penalty
	 * @param extend extend gap penalty
	 * @return alignment
	 */
	public Alignment align (final char[] sequence1, final char[] sequence2, final float[][] matrix, final float open, final float extend) {
		
		System.out.println ( "Aligning with Smith-Waterman (" + sequence1.length + " by " + sequence2.length + ") ..." );
		
		long start = System.currentTimeMillis ( );
		
		byte[] pointers = construct(sequence1, sequence2, matrix, open, extend);
		Alignment alignment = traceback(sequence1, sequence2, pointers, matrix);
		
		long end = System.currentTimeMillis ( );
		
		System.out.println ( "Finished aligning with Smith-Waterman in " + (end-start) + " milliseconds");
		
		return alignment;
	}

	/**
	 * Aligns two sequences by Smith-Waterman algorithm
	 * @param sequence1 1<sup>st</sup> sequence
	 * @param sequence2 2<sup>nd</sup> sequence
	 * @param matrix the scoring matrix (filename)
	 * @param open open gap penalty
	 * @param extend extend gap penalty
	 * @return array of all possible Alignment
	 */
	public ArrayList alignAll (final char[] sequence1, final char[] sequence2, final float[][] matrix, final float open, final float extend) {
		System.out.println ( "Starting Smith-Waterman..." );
		long start = System.currentTimeMillis ( );
		
		byte[][] pointers;
		ArrayList alignments;
		
		pointers = constructAll(sequence1, sequence2, matrix, open, extend);
		alignments = tracebackAll(sequence1, sequence2, pointers);
		
		long end = System.currentTimeMillis ( );
		System.out.println ( "Finished Smith-Waterman in " + (end-start) + " milliseconds");
		
		return alignments;
	}

	/**
	 * Constructs a pointers matrix for traceback
	 * @param sequence1 1<sup>st</sup> sequence
	 * @param sequence2 2<sup>nd</sup> sequence
	 * @param matrix scoring matrix
	 * @param open open gap penalty
	 * @param extend extend gap penalty
	 * @return matrix of pointers for traceback
	 */
	
	private byte[] construct(final char[] sequence1, final char[] sequence2, final float[][] matrix, final float open, final float extend) {
		int m = sequence1.length + 1;
		int n = sequence2.length + 1;
		
		float[] similarity = new float[n];
		float[] vertical = new float[n];
		float[] horizontal = new float[n];
		
		similarity[0] = vertical[0] = horizontal[0] = 0;
		
		byte[] pointers = new byte[m * n];
		
		float a, b, c, s, o, e, diagonal = 0;
		
		for (int i = 1, row = n; i < m; i++, row += n) {
			for (int j = 1; j < n; j++) {
				
				a = diagonal + matrix[sequence1[i - 1]][sequence2[j - 1]];
				
				o = similarity[j] - open;
				e = vertical[j] - extend;
				b = vertical[j] = o > e ? o : e;
				
				o = similarity[j - 1] - open;
				e = horizontal[j - 1] - extend;
				c = horizontal[j - 1] = o > e ? o : e;

				if (a > b) {
					if (a > c) {
						s = a;
						pointers[row + j] = DIAGONAL;
					} else {
						s = c;
						pointers[row + j] = LEFT;
					}
				} else if (c > b) {
					s = c;
					pointers[row + j] = LEFT;
				} else {
					s = b;
					pointers[row + j] = UP;
				}

				diagonal = similarity[j];
				similarity[j] = s > 0 ? s : 0;

				if (s > score) {
					score = s;
					startTracebackRow = i;
					startTracebackCol = j;
				}
			}
		}
		return pointers;
	}

	/**
	 * Constructs a pointers matrix for traceback
	 * @param sequence1	1<sup>st</sup> sequence
	 * @param sequence2	2<sup>nd</sup> sequence
	 * @param matrix the scoring matrix
 	 * @param open open gap penalty
	 * @param extend extend gap penalty
	 * @return matrix of pointers for traceback
	 */
	private byte[][] constructAll(final char[] sequence1, final char[] sequence2, final float[][] matrix, final float open, final float extend) {

		int m = sequence1.length + 1;
		int n = sequence2.length + 1;
		
		float[][] similarity = new float[m][n];
		float[][] ix = new float[m][n];
		float[][] iy = new float[m][n];
		
		similarity[0][0] = 0;

		// Initialize the first column to zeros
		for (int i = 0; i < m; i++) {
			ix[i][0] = -open - (i - 1) * extend;
		}
		
		// Initialize the first row to zeros
		for (int j = 0; j < n; j++) {
			iy[0][j] = -open - (j - 1) * extend;
		}
		
		byte[][] pointers = new byte[m][n];
		float a, b, b1, b2, c, c1, c2, s;
		
		for (int i = 1; i < m; i++) {
			for (int j = 1; j < n; j++) {
				
				a = similarity[i - 1][j - 1] + matrix[sequence1[i - 1]][sequence2[j - 1]];
				
				b1 = similarity[i - 1][j] - open;
				b2 = ix[i - 1][j] - extend;
				b = ix[i][j] = b1 > b2 ? b1 : b2;
				
				c1 = similarity[i][j - 1] - open;
				c2 = iy[i][j - 1] - extend;
				c = iy[i][j] = c1 > c2 ? c1 : c2;
				
				if (a > b) {
					if (a > c) {
						s = a;
						pointers[i][j] = DIAGONAL;
					} else if (a == c) {
						s = a;
						pointers[i][j] = DIAGONAL_LEFT;
					} else {
						s = c;
						pointers[i][j] = LEFT;
					}
				} else if (a == b) {
					if (a > c) {
						s = a;
						pointers[i][j] = DIAGONAL_UP;
					} else if (a == c) {
						s = a;
						pointers[i][j] = DIAGONAL_LEFT_UP;
					} else {
						s = c;
						pointers[i][j] = LEFT;
					}
				} else if (b > c) {
					s = b;
					pointers[i][j] = UP;
				} else if (b == c) {
					s = b;
					pointers[i][j] = LEFT_UP;
				} else {
					s = c;
					pointers[i][j] = LEFT;
				}
				
				similarity[i][j] = s > 0 ? s : 0;

				// The traceback start is the maximum score
				if (s > score) {
					score = s;
					startTracebackRow = i;
					startTracebackCol = j;
				}
			}
		}
		return pointers;
	}

	/**
	 * Returns alignment for two sequences based on the passed array of pointers
	 * @param sequence1 1<sup>st</sup> sequence
	 * @param sequence2 2<sup>nd</sup> sequence
	 * @param pointers matrix of pointers
	 * @return alignment
	 */
	private Alignment traceback(final char[] sequence1, final char[] sequence2, final byte[] pointers, final float[][] matrix) {

		int maxlen = sequence1.length + sequence2.length;
		char[] array1 = new char[maxlen];
		char[] array2 = new char[maxlen];
		
		int len1 = 0;
		int len2 = 0;
		
		int identity = 0;
		int similarity = 0;
		int gaps = 0;
		
		char c1, c2;

		int x = startTracebackRow;
		int y = startTracebackCol;
		int n = sequence2.length + 1;
		int row = x * n;
		
		int direction;
		
		int oldx = x, oldy = y;

		// Start the traceback
		while (x != 0 && y != 0) {
			switch (pointers[row + y]) {
				case UP			:	array1[len1++] = sequence1[x-1];
									array2[len2++] = Alignment.GAP;
									gaps++;
									x--;
									row -= n;
									break;

				case DIAGONAL	:	c1 = sequence1[x-1];
									c2 = sequence2[y-1];
									array1[len1++] = c1;
									array2[len2++] = c2;
									if (c1 == c2) {
										identity++;
										similarity++;
									} else if (matrix[c1][c2] > 0) {
										similarity++;
									}
									x--;
									y--;
									row -= n;
									break;

				case LEFT		:	array1[len1++] = Alignment.GAP;
									array2[len2++] = sequence2[y-1];
									gaps++;
									y--;
									break;
			}
		}

		Alignment alignment = new Alignment();
		alignment.setScore(score);
		alignment.setIdentity(identity);
		alignment.setSimilarity(similarity);
		alignment.setGaps(gaps);
		alignment.setOffset1(x);
		alignment.setOffset2(y);
		
		char[] aligned1 = new char[len1];
		for (int i = len1 - 1, j = 0; i >= 0; i--, j++) {
			aligned1[j] = array1[i];
		}
		alignment.setSequence1(aligned1);
		
		char[] aligned2 = new char[len2];
		for (int i = len2 - 1, j = 0; i >= 0; i--, j++) {
			aligned2[j] = array2[i];
		}
		alignment.setSequence2(aligned2);

		return alignment;
	}
	
	/**
	 * Returns alignment for two sequences based on the passed array of pointers
	 * @param sequence1 1<sup>st</sup> sequence
	 * @param sequence2 2<sup>nd</sup> sequence
	 * @param pointers matrix of pointers
	 * @return array of all possible alignments
	 */
	private ArrayList tracebackAll(final char[] sequence1, final char[] sequence2, final byte[][] pointers) {
		int maxlen = sequence1.length + sequence2.length;
		char[] c1 = new char[maxlen];
		char[] c2 = new char[maxlen];
		int len1;
		int len2;

		int identity = 0;
		int similarity = 0;
		int gaps = 0;
		
		Alignment alignment = null;
		List alignments = new ArrayList ( );
		boolean hasMoreAlignment = true;

		int x;
		int y;
		
		// Start the traceback
		while (hasMoreAlignment) {
			hasMoreAlignment = false;
			len1 = len2 = 0;
			identity = similarity = gaps = 0;
			x = startTracebackRow;
			y = startTracebackCol;
			
			while (x != 0 && y != 0) {
				
				switch (pointers[x][y]) {
					case UP					:	c1[len1++] = sequence1[x-1];
												c2[len2++] = Alignment.GAP;
												gaps++;
												x--;
												break;
												
					case LEFT_UP			:	pointers[x][y] = UP;
												hasMoreAlignment = true;
												break;
					
					case LEFT				:	c1[len1++] = Alignment.GAP;
												c2[len2++] = sequence2[y-1];
												gaps++;
												y--;
												break;
												
					case DIAGONAL_UP		:	pointers[x][y] = UP;
												hasMoreAlignment = true;	
												c1[len1++] = sequence1[x-1];
												c2[len2++] = sequence2[y-1];
												if (sequence1[x-1] == sequence2[y-1])
													identity++;
												similarity++;
												x--;
												y--;
												break;
												
					case DIAGONAL_LEFT		:	pointers[x][y] = LEFT;
												hasMoreAlignment = true;
												c1[len1++] = sequence1[x-1];
												c2[len2++] = sequence2[y-1];
												if (sequence1[x-1] == sequence2[y-1])
													identity++;
												similarity++;
												x--;
												y--;
												break;
												
					case DIAGONAL_LEFT_UP	:	pointers[x][y] = DIAGONAL_UP;
												hasMoreAlignment = true;
												c1[len1++] = sequence1[x-1];
												c2[len2++] = sequence2[y-1];
												if (sequence1[x-1] == sequence2[y-1])
													identity++;
												similarity++;
												x--;
												y--;
												break;
												
					case DIAGONAL			:	c1[len1++] = sequence1[x-1];
												c2[len2++] = sequence2[y-1];
												if (sequence1[x-1] == sequence2[y-1])
													identity++;
												similarity++;
												x--;
												y--;
												break;
				}
			}
			alignment = new Alignment();
			alignment.setScore(score);
			alignment.setIdentity(identity);
			alignment.setSimilarity(similarity);
			alignment.setGaps(gaps);
			alignment.setOffset1(x);
			alignment.setOffset2(y);
			
			char[] aligned1 = new char[len1];
			for (int i = len1 - 1, j = 0; i >= 0; i--, j++) {
				aligned1[j] = c1[i];
			}
			alignment.setSequence1(aligned1);
			
			char[] aligned2 = new char[len2];
			for (int i = len2 - 1, j = 0; i >= 0; i--, j++) {
				aligned2[j] = c2[i];
			}
			alignment.setSequence2(aligned2);

			alignments.add(alignment);
		}
		return (ArrayList) alignments;
	}
}