/** @author Sean White and Matthew Doran */

#ifndef CHORDBUILDER_H
#define CHORDBUILDER_H

#include <vector>
#include <string>

class ChordBuilder
{
	public:
		/** Constructor */
		ChordBuilder();
		      /** Accepts some array of 12 doubles*/
			std::string buildChord(double* frame);

			int test(int n);

      std::string _buildChordForChromogram(std::vector<double> chroma);
};

#endif
