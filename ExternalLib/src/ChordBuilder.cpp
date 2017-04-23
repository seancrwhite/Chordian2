#include "Chromagram.h"
#include "ChordDetector.h"
#include "ChordBuilder.h"
#include <string>

Chromagram chromagram (512, 44100);
ChordDetector detector;

std::string ChordBuilder::buildChord(double* frame){
    chromagram.processAudioFrame(frame);

    if(chromagram.isReady()){
        std::vector<double> chroma = chromagram.getChromagram();

        return _buildChordForChromogram(chroma);
    }

    return 0;
}

int main() {
	return 0;
}

  /** Test method */
  int test(int n){
    return 3;
  }
  /** Delete when done */

  std::string ChordBuilder::_buildChordForChromogram(std::vector<double> chroma){
        detector.detectChord(chroma);

        std::string chord;

        switch(detector.rootNote){
          case 0:
            chord = "A ";
            break;
          case 1:
            chord = "A# ";
            break;
          case 2:
            chord = "B ";
            break;
          case 3:
            chord = "C ";
            break;
          case 4:
            chord = "C# ";
            break;
          case 5:
            chord = "D ";
            break;
          case 6:
            chord = "D# ";
            break;
          case 7:
            chord = "E ";
            break;
          case 8:
            chord = "F ";
            break;
          case 9:
            chord = "F# ";
            break;
          case 10:
            chord = "G ";
            break;
          default:
            chord = "G# ";
        }

        std::string quality;

        switch(detector.quality){
          case 0:
            quality = "Minor ";
            break;
          case 1:
            quality = "Major ";
            break;
          case 2:
            quality = "Suspended ";
            break;
          case 3:
            quality = "Dominant ";
            break;
          case 4:
            quality = "Dimished5th ";
            break;
          case 5:
            quality = "Augmented5th ";
            break;
          default:
            quality = "";
        }

        if(detector.intervals == 7 || detector.intervals == 2){
            quality += detector.intervals;
        }

        return chord + quality;
  }

// std::string ChordBuilder::buildChord(std::vector<double> frame){
//     chromagram.processAudioFrame(frame);

//     if(chromagram.isReady()){
//         std::vector<double> chroma = chromagram.getChromagram();

//         return _buildChordForChromogram(chroma);
//     }

//     return 0;
// }

// std::string ChordBuilder::buildChord(double* frame){
//     chromagram.processAudioFrame(frame);

//     if(chromagram.isReady()){
//         std::vector<double> chroma = chromagram.getChromagram();

//         return _buildChordForChromogram(chroma);
//     }

//     return 0;
// }

// /** Test method */
//   int test(int n){
//     return 3;
//   }

// /** Delete when done */

// std::string ChordBuilder::_buildChordForChromogram(std::vector<double> chroma){
//         detector.detectChord(chroma);

//         std::string chord = std::to_string(detector.rootNote) + std::to_string(detector.quality);

//         if(detector.intervals == 7){
//             chord += 7;
//         }

//         return chord;
// }
