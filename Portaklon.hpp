#ifndef PORTAKLON_HPP
#define PORTAKLON_HPP

/*
PARAMS
Gain: Ranges [0-1], 0.5 default 
Treble: Ranges [0-1], 0.5 default
Level: Ranges [0-1], 0.5 default
*/

class Portaklon {
private:
  InputBuffer mInputBuffer;
  PreAmpStage mPreAmpStage;
  AmpStage mAmpStage;
  ClippingStage mClippingStage;
  FF1Current mFF1;
  FeedForward2 mFF2;
  SummingAmp mSummingAmp;
  ToneControl mToneControl;
  OutputBuffer mOutputBuffer;

public:

  void init(double sampleRate, int blockSize) {}

  void process(float* inputBuffer, float* outputBuffer, const int numSamples) {
    
    // do all operations on outputBuffer
    std::memcpy(outputBuffer, inputBuffer, numSamples * sizeof(float));

    // 6dB pad
    for (int sample = 0; sample < numSamples; sample++) {
      outputBuffer[sample] *= 0.5f;
    }

    mInputBufferProcessor.processBlock(outputBuffer, numSamples);

    // op amp clipping 
    for (int sample = 0; sample < numSamples; sample++) {
      float mSample = outputBuffer[sample];
      outputBuffer[sample] > 4.5f ? 4.5f : mSample;
      outputBuffer[sample] < -4.5f ? -4.5f : mSample;
    }

    // gain stage
    gainStageProc->processBlock(outputBuffer);

    // tone stage 
    // mToneFilterProcessor.setTreble(trebleParam);
    mToneFilterProcessor.processBlock(outputBuffer, numSamples);

    // inverting op amp with charge pump elevated supply
    for (int sample = 0; sample < numSamples; sample++) {
      outputBuffer[sample] *= -1.f;
      float mSample = outputBuffer[sample];
      outputBuffer[sample] > 11.7f ? 11.7f : mSample;
      outputBuffer[sample] < -13.1f ? -13.1f : mSample;
    }

    // output stage
    // mOutputStageProc.setLevel(levelParam);
    mOutputStageProc.processBlock(outputBuffer, numSamples);

    // remove dc
    for (int sample = 0; sample < numSamples; sample++) {
      outputBuffer[sample] = dcBlocker.processSample(outputBuffer[sample]);
    }
    
  }
  
};

#endif // PORTAKLON_HPP