#include <jni.h>
#include "ChordDetector.h"
#include "Chromagram.h"
#include <math.h>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif
JNIEXPORT jstring JNICALL
Java_com_chordian_droptablestudents_chordian_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

JNIEXPORT jstring JNICALL
Java_com_chordian_droptablestudents_chordian_ChordBuilder_buildChord(JNIEnv *env, jobject instance,
                                                                     jdoubleArray frame_) {
    jdouble *frame = env->GetDoubleArrayElements(frame_, NULL);

    // TODO
    Chromagram chromagram(512, 44100);

    std::vector<double> chroma = chromagram.getChromagram();

    ChordDetector detector;

    detector.detectChord(chroma);

    std::string chord;

    switch (detector.rootNote) {
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

    switch (detector.quality) {
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

    if (detector.intervals == 7 || detector.intervals == 2) {
        quality += detector.intervals;
    }

    std::string result = chord + quality;

    env->ReleaseDoubleArrayElements(frame_, frame, 0);

    return env->NewStringUTF(result.c_str());
}

ChordDetector::ChordDetector()
{
    bias = 1.06;
    makeChordProfiles();
}

//=======================================================================
void ChordDetector::detectChord (std::vector<double> chroma)
{
    detectChord (&chroma[0]);
}

//=======================================================================
void ChordDetector::detectChord (double* chroma)
{
    for (int i = 0; i < 12; i++)
    {
        chromagram[i] = chroma[i];
    }

    classifyChromagram();
}


//=======================================================================
void ChordDetector::classifyChromagram()
{
    int i;
    int j;
    int fifth;
    int chordindex;

    // remove some of the 5th note energy from chromagram
    for (i = 0; i < 12; i++)
    {
        fifth = (i+7) % 12;
        chromagram[fifth] = chromagram[fifth] - (0.1 * chromagram[i]);

        if (chromagram[fifth] < 0)
        {
            chromagram[fifth] = 0;
        }
    }

    // major chords
    for (j = 0; j < 12; j++)
    {
        chord[j] = calculateChordScore (chromagram,chordProfiles[j], bias, 3);
    }

    // minor chords
    for (j = 12; j < 24; j++)
    {
        chord[j] = calculateChordScore (chromagram, chordProfiles[j], bias, 3);
    }

    // diminished 5th chords
    for (j = 24; j < 36; j++)
    {
        chord[j] = calculateChordScore (chromagram, chordProfiles[j], bias, 3);
    }

    // augmented 5th chords
    for (j = 36; j < 48; j++)
    {
        chord[j] = calculateChordScore (chromagram, chordProfiles[j], bias, 3);
    }

    // sus2 chords
    for (j = 48; j < 60; j++)
    {
        chord[j] = calculateChordScore (chromagram, chordProfiles[j], 1, 3);
    }

    // sus4 chords
    for (j = 60; j < 72; j++)
    {
        chord[j] = calculateChordScore (chromagram, chordProfiles[j], 1, 3);
    }

    // major 7th chords
    for (j = 72; j < 84; j++)
    {
        chord[j] = calculateChordScore (chromagram, chordProfiles[j], 1, 4);
    }

    // minor 7th chords
    for (j = 84; j < 96; j++)
    {
        chord[j] = calculateChordScore (chromagram, chordProfiles[j], bias, 4);
    }

    // dominant 7th chords
    for (j = 96; j < 108; j++)
    {
        chord[j] = calculateChordScore (chromagram, chordProfiles[j], bias, 4);
    }

    chordindex = minimumIndex (chord, 108);

    // major
    if (chordindex < 12)
    {
        rootNote = chordindex;
        quality = Major;
        intervals = 0;
    }

    // minor
    if ((chordindex >= 12) && (chordindex < 24))
    {
        rootNote = chordindex-12;
        quality = Minor;
        intervals = 0;
    }

    // diminished 5th
    if ((chordindex >= 24) && (chordindex < 36))
    {
        rootNote = chordindex-24;
        quality = Dimished5th;
        intervals = 0;
    }

    // augmented 5th
    if ((chordindex >= 36) && (chordindex < 48))
    {
        rootNote = chordindex-36;
        quality = Augmented5th;
        intervals = 0;
    }

    // sus2
    if ((chordindex >= 48) && (chordindex < 60))
    {
        rootNote = chordindex-48;
        quality = Suspended;
        intervals = 2;
    }

    // sus4
    if ((chordindex >= 60) && (chordindex < 72))
    {
        rootNote = chordindex-60;
        quality = Suspended;
        intervals = 4;
    }

    // major 7th
    if ((chordindex >= 72) && (chordindex < 84))
    {
        rootNote = chordindex-72;
        quality = Major;
        intervals = 7;
    }

    // minor 7th
    if ((chordindex >= 84) && (chordindex < 96))
    {
        rootNote = chordindex-84;
        quality = Minor;
        intervals = 7;
    }

    // dominant 7th
    if ((chordindex >= 96) && (chordindex < 108))
    {
        rootNote = chordindex-96;
        quality = Dominant;
        intervals = 7;
    }
}

//=======================================================================
double ChordDetector::calculateChordScore (double* chroma, double* chordProfile, double biasToUse, double N)
{
    double sum = 0;
    double delta;

    for (int i = 0; i < 12; i++)
    {
        sum = sum + ((1 - chordProfile[i]) * (chroma[i] * chroma[i]));
    }

    delta = sqrt (sum) / ((12 - N) * biasToUse);

    return delta;
}

//=======================================================================
int ChordDetector::minimumIndex (double* array, int arrayLength)
{
    double minValue = 100000;
    int minIndex = 0;

    for (int i = 0;i < arrayLength;i++)
    {
        if (array[i] < minValue)
        {
            minValue = array[i];
            minIndex = i;
        }
    }

    return minIndex;
}

//=======================================================================
void ChordDetector::makeChordProfiles()
{
    int i;
    int t;
    int j = 0;
    int root;
    int third;
    int fifth;
    int seventh;

    double v1 = 1;
    double v2 = 1;
    double v3 = 1;

    // set profiles matrix to all zeros
    for (j = 0; j < 108; j++)
    {
        for (t = 0;t < 12;t++)
        {
            chordProfiles[j][t] = 0;
        }
    }

    // reset j to zero to begin creating profiles
    j = 0;

    // major chords
    for (i = 0; i < 12; i++)
    {
        root = i % 12;
        third = (i + 4) % 12;
        fifth = (i + 7) % 12;

        chordProfiles[j][root] = v1;
        chordProfiles[j][third] = v2;
        chordProfiles[j][fifth] = v3;

        j++;
    }

    // minor chords
    for (i = 0; i < 12; i++)
    {
        root = i % 12;
        third = (i + 3) % 12;
        fifth = (i + 7) % 12;

        chordProfiles[j][root] = v1;
        chordProfiles[j][third] = v2;
        chordProfiles[j][fifth] = v3;

        j++;
    }

    // diminished chords
    for (i = 0; i < 12; i++)
    {
        root = i % 12;
        third = (i + 3) % 12;
        fifth = (i + 6) % 12;

        chordProfiles[j][root] = v1;
        chordProfiles[j][third] = v2;
        chordProfiles[j][fifth] = v3;

        j++;
    }

    // augmented chords
    for (i = 0; i < 12; i++)
    {
        root = i % 12;
        third = (i + 4) % 12;
        fifth = (i + 8) % 12;

        chordProfiles[j][root] = v1;
        chordProfiles[j][third] = v2;
        chordProfiles[j][fifth] = v3;

        j++;
    }

    // sus2 chords
    for (i = 0; i < 12; i++)
    {
        root = i % 12;
        third = (i + 2) % 12;
        fifth = (i + 7) % 12;

        chordProfiles[j][root] = v1;
        chordProfiles[j][third] = v2;
        chordProfiles[j][fifth] = v3;

        j++;
    }

    // sus4 chords
    for (i = 0; i < 12; i++)
    {
        root = i % 12;
        third = (i + 5) % 12;
        fifth = (i + 7) % 12;

        chordProfiles[j][root] = v1;
        chordProfiles[j][third] = v2;
        chordProfiles[j][fifth] = v3;

        j++;
    }

    // major 7th chords
    for (i = 0; i < 12; i++)
    {
        root = i % 12;
        third = (i + 4) % 12;
        fifth = (i + 7) % 12;
        seventh = (i + 11) % 12;

        chordProfiles[j][root] = v1;
        chordProfiles[j][third] = v2;
        chordProfiles[j][fifth] = v3;
        chordProfiles[j][seventh] = v3;

        j++;
    }

    // minor 7th chords
    for (i = 0; i < 12; i++)
    {
        root = i % 12;
        third = (i + 3) % 12;
        fifth = (i + 7) % 12;
        seventh = (i + 10) % 12;

        chordProfiles[j][root] = v1;
        chordProfiles[j][third] = v2;
        chordProfiles[j][fifth] = v3;
        chordProfiles[j][seventh] = v3;

        j++;
    }

    // dominant 7th chords
    for (i = 0; i < 12; i++)
    {
        root = i % 12;
        third = (i + 4) % 12;
        fifth = (i + 7) % 12;
        seventh = (i + 10) % 12;

        chordProfiles[j][root] = v1;
        chordProfiles[j][third] = v2;
        chordProfiles[j][fifth] = v3;
        chordProfiles[j][seventh] = v3;

        j++;
    }
}

Chromagram::Chromagram (int frameSize, int fs)
        :  referenceFrequency (130.81278265),
           bufferSize (8192),
           numHarmonics (2),
           numOctaves (2),
           numBinsToSearch (2)
{
    // calculate note frequencies
    for (int i = 0; i < 12; i++)
    {
        noteFrequencies[i] = referenceFrequency * pow (2,(((float) i) / 12));
    }

    // set up FFT
    setupFFT();

    // set buffer size
    buffer.resize (bufferSize);

    // setup chromagram vector
    chromagram.resize (12);

    // initialise chromagram
    for (int i = 0; i < 12; i++)
    {
        chromagram[i] = 0.0;
    }

    // setup magnitude spectrum vector
    magnitudeSpectrum.resize ((bufferSize/2)+1);

    // make window function
    makeHammingWindow();

    // set sampling frequency
    setSamplingFrequency (fs);

    // set input audio frame size
    setInputAudioFrameSize (frameSize);

    // initialise num samples counter
    numSamplesSinceLastCalculation = 0;

    // set chroma calculation interval (in samples at the input audio sampling frequency)
    chromaCalculationInterval = 4096;

    // initialise chroma ready variable
    chromaReady = false;
}

//==================================================================================
Chromagram::~Chromagram()
{
    // ------------------------------------
#ifdef USE_FFTW
    // destroy fft plan
    fftw_destroy_plan (p);
    fftw_free (complexIn);
    fftw_free (complexOut);
#endif

    // ------------------------------------
#ifdef USE_KISS_FFT
    // free the Kiss FFT configuration
    free (cfg);
    delete [] fftIn;
    delete [] fftOut;
#endif
}

//==================================================================================
void Chromagram::processAudioFrame (double* inputAudioFrame)
{
    // create a vector
    std::vector<double> v;

    // use the array to initialise it
    v.assign (inputAudioFrame, inputAudioFrame + inputAudioFrameSize);

    // process the vector
    processAudioFrame (v);
}

//==================================================================================
void Chromagram::processAudioFrame (std::vector<double> inputAudioFrame)
{
    // our default state is that the chroma is not ready
    chromaReady = false;

    // downsample the input audio frame by 4
    downSampleFrame(inputAudioFrame);

    // move samples back
    for (int i = 0; i < bufferSize - downSampledAudioFrameSize; i++)
    {
        buffer[i] = buffer[i + downSampledAudioFrameSize];
    }

    int n = 0;

    // add new samples to buffer
    for (int i = (bufferSize - downSampledAudioFrameSize); i < bufferSize; i++)
    {
        buffer[i] = downsampledInputAudioFrame[n];
        n++;
    }

    // add number of samples from calculation
    numSamplesSinceLastCalculation += inputAudioFrameSize;

    // if we have had enough samples
    if (numSamplesSinceLastCalculation >= chromaCalculationInterval)
    {
        // calculate the chromagram
        calculateChromagram();

        // reset num samples counter
        numSamplesSinceLastCalculation = 0;
    }

}

//==================================================================================
void Chromagram::setInputAudioFrameSize (int frameSize)
{
    inputAudioFrameSize = frameSize;

    downsampledInputAudioFrame.resize (inputAudioFrameSize / 4);

    downSampledAudioFrameSize = (int) downsampledInputAudioFrame.size();
}

//==================================================================================
void Chromagram::setSamplingFrequency (int fs)
{
    samplingFrequency = fs;
}

//==================================================================================
void Chromagram::setChromaCalculationInterval (int numSamples)
{
    chromaCalculationInterval = numSamples;
}

//==================================================================================
std::vector<double> Chromagram::getChromagram()
{
    return chromagram;
}

//==================================================================================
bool Chromagram::isReady()
{
    return chromaReady;
}

//==================================================================================
void Chromagram::setupFFT()
{
    // ------------------------------------------------------
#ifdef USE_FFTW
    complexIn = (fftw_complex*) fftw_malloc (sizeof (fftw_complex) * bufferSize);		// complex array to hold fft data
    complexOut = (fftw_complex*) fftw_malloc (sizeof (fftw_complex) * bufferSize);	// complex array to hold fft data
    p = fftw_plan_dft_1d (bufferSize, complexIn, complexOut, FFTW_FORWARD, FFTW_ESTIMATE);	// FFT plan initialisation
#endif

    // ------------------------------------------------------
#ifdef USE_KISS_FFT
    // initialise the fft time and frequency domain audio frame arrays
    fftIn = new kiss_fft_cpx[bufferSize];
    fftOut = new kiss_fft_cpx[bufferSize];
    cfg = kiss_fft_alloc (bufferSize,0,0,0);
#endif
}


//==================================================================================
void Chromagram::calculateChromagram()
{
    calculateMagnitudeSpectrum();

    double divisorRatio = (((double) samplingFrequency) / 4.0) / ((double)bufferSize);

    for (int n = 0; n < 12; n++)
    {
        double chromaSum = 0.0;

        for (int octave = 1; octave <= numOctaves; octave++)
        {
            double noteSum = 0.0;

            for (int harmonic = 1; harmonic <= numHarmonics; harmonic++)
            {
                int centerBin = round ((noteFrequencies[n] * octave * harmonic) / divisorRatio);
                int minBin = centerBin - (numBinsToSearch * harmonic);
                int maxBin = centerBin + (numBinsToSearch * harmonic);

                double maxVal = 0.0;

                for (int k = minBin; k < maxBin; k++)
                {
                    if (magnitudeSpectrum[k] > maxVal)
                    {
                        maxVal = magnitudeSpectrum[k];
                    }
                }

                noteSum += (maxVal / (double) harmonic);
            }

            chromaSum += noteSum;
        }

        chromagram[n] = chromaSum;
    }

    chromaReady = true;
}

//==================================================================================
void Chromagram::calculateMagnitudeSpectrum()
{

#ifdef USE_FFTW
    // -----------------------------------------------
    // FFTW VERSION
    // -----------------------------------------------
    int i = 0;

    for (int i = 0; i < bufferSize; i++)
    {
        complexIn[i][0] = buffer[i] * window[i];
        complexIn[i][1] = 0.0;
    }

    // execute fft plan, i.e. compute fft of buffer
    fftw_execute (p);

    // compute first (N/2)+1 mag values
    for (i = 0; i < (bufferSize / 2) + 1; i++)
    {
        magnitudeSpectrum[i] = sqrt (pow (complexOut[i][0], 2) + pow (complexOut[i][1], 2));
        magnitudeSpectrum[i] = sqrt (magnitudeSpectrum[i]);
    }
#endif


#ifdef USE_KISS_FFT
    // -----------------------------------------------
    // KISS FFT VERSION
    // -----------------------------------------------
    int i = 0;

    for (int i = 0;i < bufferSize; i++)
    {
        fftIn[i].r = buffer[i] * window[i];
        fftIn[i].i = 0.0;
    }

    // execute kiss fft
    kiss_fft (cfg, fftIn, fftOut);

    // compute first (N/2)+1 mag values
    for (i = 0; i < (bufferSize / 2) + 1; i++)
    {
        magnitudeSpectrum[i] = sqrt (pow (fftOut[i].r, 2) + pow (fftOut[i].i, 2));
        magnitudeSpectrum[i] = sqrt (magnitudeSpectrum[i]);
    }
#endif
}

//==================================================================================
void Chromagram::downSampleFrame (std::vector<double> inputAudioFrame)
{
    std::vector<double> filteredFrame (inputAudioFrameSize);

    float b0,b1,b2,a1,a2;
    float x_1,x_2,y_1,y_2;

    b0 = 0.2929;
    b1 = 0.5858;
    b2 = 0.2929;
    a1 = -0.0000;
    a2 = 0.1716;

    x_1 = 0;
    x_2 = 0;
    y_1 = 0;
    y_2 = 0;

    for (int i = 0; i < inputAudioFrameSize; i++)
    {
        filteredFrame[i] = inputAudioFrame[i] * b0 + x_1 * b1 + x_2 * b2 - y_1 * a1 - y_2 * a2;

        x_2 = x_1;
        x_1 = inputAudioFrame[i];
        y_2 = y_1;
        y_1 = filteredFrame[i];
    }

    for (int i = 0; i < inputAudioFrameSize / 4; i++)
    {
        downsampledInputAudioFrame[i] = filteredFrame[i * 4];
    }
}

//==================================================================================
void Chromagram::makeHammingWindow()
{
    // set the window to the correct size
    window.resize (bufferSize);

    // apply hanning window to buffer
    for (int n = 0; n < bufferSize;n++)
    {
        window[n] = 0.54 - 0.46 * cos (2 * M_PI * (((double) n) / ((double) bufferSize)));
    }
}

//==================================================================================
double Chromagram::round (double val)
{
    return floor (val + 0.5);
}
#ifdef __cplusplus
}
#endif