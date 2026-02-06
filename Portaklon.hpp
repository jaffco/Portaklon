#ifndef PORTAKLON_HPP
#define PORTAKLON_HPP

/*
PARAMS
Gain: Ranges [0-1], 0.5 default 
Treble: Ranges [0-1], 0.5 default
Level: Ranges [0-1], 0.5 default
*/

#include <memory>
#include <vector>
#include "src/CommonProcessors/InputBuffer.h"
#include "src/CommonProcessors/ToneControl.h"
#include "src/CommonProcessors/OutputBuffer.h"
#include "src/GainStageProcessors/PreAmpStage.h"
#include "src/GainStageProcessors/AmpStage.h"
#include "src/GainStageProcessors/ClippingStage.h"
#include "src/GainStageProcessors/FF1Current.h"
#include "src/GainStageProcessors/FeedForward2.h"
#include "src/GainStageProcessors/SummingAmp.h"

class Portaklon {
private:
  InputBuffer mInputBuffer;
  std::unique_ptr<PreAmpStage> mPreAmpStage;
  AmpStage mAmpStage;
  std::unique_ptr<ClippingStage> mClippingStage;
  std::unique_ptr<FF1Current> mFF1;
  std::unique_ptr<FeedForward2> mFF2;
  SummingAmp mSummingAmp;
  ToneControl mToneControl;
  OutputBuffer mOutputBuffer;

  std::vector<float> tempBuffers[10]; // temporary buffers for processing

public:

  void init(float sampleRate, int blockSize) {
    mPreAmpStage = std::make_unique<PreAmpStage>(sampleRate, blockSize);
    mClippingStage = std::make_unique<ClippingStage>(sampleRate);
    mFF2 = std::make_unique<FeedForward2>(sampleRate);
    mFF1 = std::make_unique<FF1Current>(*mPreAmpStage, sampleRate);

    for (auto& buf : tempBuffers) {
      buf.resize(blockSize);
    }

    // Set defaults
    setGain(0.5f);
    setTreble(0.5f);
    setLevel(0.5f);
  }

  void process(float* inputBuffer, float* outputBuffer, const int numSamples) {
    // Input -> InputBuffer -> PreAmpStage
    mInputBuffer.process(inputBuffer, tempBuffers[0].data(), numSamples);
    mPreAmpStage->processBlock(tempBuffers[0].data(), tempBuffers[1].data(), numSamples);

    // PreAmpStage -> AmpStage -> ClippingStage
    mAmpStage.process(tempBuffers[1].data(), tempBuffers[2].data(), numSamples);
    mClippingStage->processBlock(tempBuffers[2].data(), tempBuffers[3].data(), numSamples);

    // Input -> FF2
    mInputBuffer.process(inputBuffer, tempBuffers[4].data(), numSamples);
    mFF2->processBlock(tempBuffers[4].data(), tempBuffers[5].data(), numSamples);

    // FF1 from PreAmpStage
    mFF1->processBlock(nullptr, tempBuffers[6].data(), numSamples); // FF1 doesn't use input

    // Sum: FF1 + ClippingStage + FF2
    for (int i = 0; i < numSamples; ++i) {
      tempBuffers[7][i] = tempBuffers[6][i] + tempBuffers[3][i] + tempBuffers[5][i];
    }

    // Sum -> SummingAmp -> ToneControl -> OutputBuffer
    mSummingAmp.process(tempBuffers[7].data(), tempBuffers[8].data(), numSamples);
    mToneControl.process(tempBuffers[8].data(), tempBuffers[9].data(), numSamples);
    mOutputBuffer.process(tempBuffers[9].data(), outputBuffer, numSamples);
  }

  // Parameter setters
  void setGain(float gain) {
    if (mPreAmpStage) mPreAmpStage->setGain(gain);
    mAmpStage.setGain(gain);
    if (mFF2) mFF2->setGain(gain);
  }

  void setTreble(float treble) {
    mToneControl.setTreble(treble);
  }

  void setLevel(float level) {
    mOutputBuffer.setLevel(level);
  }
  
};

#endif // PORTAKLON_HPP