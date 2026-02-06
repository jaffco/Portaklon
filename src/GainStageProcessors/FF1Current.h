#ifndef FF1CURRENT_H_INCLUDED
#define FF1CURRENT_H_INCLUDED

#include "../BaseClasses/AudioProcessor.hpp"
#include "PreAmpStage.h"

class FF1Current : public AudioProcessor
{
public:
    FF1Current(PreAmpStage& preAmp, float sampleRate) : AudioProcessor(sampleRate), preAmp(preAmp) {}

    void processBlock(float* in, float* out, int numSamples) override
    {
        for (int i = 0; i < numSamples; ++i)
        {
            out[i] = 1000.f * preAmp.ff1Current[i];
        }
    }

private:
    PreAmpStage& preAmp;
};

#endif // FF1CURRENT_H_INCLUDED
