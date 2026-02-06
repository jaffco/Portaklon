#ifndef AUDIOPROCESSOR_HPP_INCLUDED
#define AUDIOPROCESSOR_HPP_INCLUDED

class AudioProcessor
{
protected:
    float sampleRate;

    AudioProcessor() = delete;
    AudioProcessor(float sr) : sampleRate(sr) {}
    virtual ~AudioProcessor() = default;

public:
    virtual void processBlock(float* in, float* out, int numSamples) = 0;
};

#endif // AUDIOPROCESSOR_HPP_INCLUDED