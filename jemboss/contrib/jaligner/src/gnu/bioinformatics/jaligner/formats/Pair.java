/**
 * @author Ahmed Moustafa (ahmed at users.sourceforge.net) 
 * $Id: Pair.java,v 1.2 2003/09/10 10:59:22 timc Exp $
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

package gnu.bioinformatics.jaligner.formats;

import gnu.bioinformatics.jaligner.util.Alignment;

/**
 * The simple output format <a href="http://www.hgmp.mrc.ac.uk/Software/EMBOSS/Themes/AlignExamples/align.pair">Pair</a> of the alignment of two sequences.
 *
 * @author Ahmed Moustafa (ahmed at users.sourceforge.net) 
 * @version $Revision: 1.2 $ 
 */

public class Pair {
	
	private static final int NAME_WIDTH = 13;
	private static final int POSITION_WIDTH = 6;
	private static final int SEQUENCE_WIDTH = 50;
	private static final String NEW_LINE = "\n";
	private static final String BLANK = " ";
	
	public static String format(Alignment alignment) {
		char[] sequence1 = alignment.getSequence1();
		char[] sequence2 = alignment.getSequence2();
		
		int length = sequence1.length > sequence2.length ? sequence2.length : sequence1.length;
		
		String name1 = adjustName(alignment.getName1());
		String name2 = adjustName(alignment.getName2());

		StringBuffer buffer = new StringBuffer ( );
		
		StringBuffer preMarkup = new StringBuffer ( );
		for (int j = 0; j < NAME_WIDTH + 1 + POSITION_WIDTH + 1; j++) {
			preMarkup.append(BLANK);
		}
		
		int oldPosition1, position1 = 1 + alignment.getOffset1();
		int oldPosition2, position2 = 1 + alignment.getOffset2();
		
		char[] subsequence1;
		char[] subsequence2;
		char[] markup;
		int line;
		
		for (int i = 0; i * SEQUENCE_WIDTH < length; i++) {
			
			oldPosition1 = position1;
			oldPosition2 = position2;
			
			line = ((i + 1) * SEQUENCE_WIDTH) < length ? (i + 1) * SEQUENCE_WIDTH: length;

			subsequence1 = new char[line - i * SEQUENCE_WIDTH];
			subsequence2 = new char[line - i * SEQUENCE_WIDTH];
			markup = new char[line - i * SEQUENCE_WIDTH];
		
			for (int j = i * SEQUENCE_WIDTH, k = 0; j < line; j++, k++) {
				subsequence1[k] = sequence1[j];
				subsequence2[k] = sequence2[j];
				if (subsequence1[k] == subsequence2[k]) {
					markup[k] = Alignment.MARKUP_IDENTITY;
					position1++;
					position2++;
				} else if (subsequence1[k] == Alignment.GAP) {
					markup[k] = Alignment.MARKUP_GAP;
					position2++;
				} else if (subsequence2[k] == Alignment.GAP) {
					markup[k] = Alignment.MARKUP_GAP;
					position1++;
				} else {
					markup[k] = Alignment.MARKUP_SIMILARITY;
					position1++;
					position2++;
				}
			}

			buffer.append(name1);
			buffer.append(BLANK);
			buffer.append(adjustPosition(new Integer(oldPosition1).toString()));
			buffer.append(BLANK);
			buffer.append(subsequence1);
			buffer.append(BLANK);
			buffer.append(adjustPosition(new Integer(position1 - 1).toString()));
			buffer.append(NEW_LINE);
			
			buffer.append(preMarkup);
			buffer.append(markup);
			buffer.append(NEW_LINE);

			buffer.append(name2);
			buffer.append(BLANK);
			buffer.append(adjustPosition(new Integer(oldPosition2).toString()));
			buffer.append(BLANK);
			buffer.append(subsequence2);
			buffer.append(BLANK);
			buffer.append(adjustPosition(new Integer(position2 - 1).toString()));
			buffer.append(NEW_LINE);
			
			buffer.append(NEW_LINE);
		}
		return buffer.toString();
	}
	
	private static String adjustName(String name) {
		StringBuffer buffer = new StringBuffer ( );
		
		if (name.length() > NAME_WIDTH) {
			buffer.append(name.substring(0, NAME_WIDTH));
		} else {
			buffer.append(name);
			for (int j = buffer.length(); j < NAME_WIDTH; j++) {
				buffer.append (BLANK);
			}
		}
		return buffer.toString();
	}
	
	private static String adjustPosition(String position) {
		StringBuffer buffer1 = new StringBuffer( );
		StringBuffer buffer2 = new StringBuffer( );
		
		if (position.length() > POSITION_WIDTH) {
			buffer1.append(position.substring(position.length() - POSITION_WIDTH, position.length()));
		} else {
			buffer1.append(position);
		}
		
		for (int j = 0, n = POSITION_WIDTH - buffer1.length(); j < n; j++) {
			buffer2.append(BLANK);
		}
		
		return buffer2.append(buffer1).toString();
	}
}