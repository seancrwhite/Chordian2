#include "Chromagram.h"
#include "ChordDetector.h"
#include "ChordBuilder.h"
#include <string>

Chromagram chromagram (512, 44100);
ChordDetector detector;

class ChordBuilder
{
public:
  ChordBuilder();
  ~ChordBuilder();

  std::string ChordBuilder::buildChord(double* frame){
    chromagram.processAudioFrame(frame);

    if(chromagram.isReady()){
        std::vector<double> chroma = chromagram.getChromagram();

        return _buildChordForChromogram(chroma);
    }

    return 0;
  }

  /** Test method */
  int test(int n){
    return 3;
  }
  /** Delete when done */

  std::string ChordBuilder::_buildChordForChromogram(std::vector<double> chroma){
        detector.detectChord(chroma);

        std::string chord = std::to_string(detector.rootNote) + std::to_string(detector.quality);

        if(detector.intervals == 7){
            chord += 7;
        }

        return chord;
  }
};

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
