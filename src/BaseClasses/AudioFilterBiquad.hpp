#ifndef AUDIO_FILTER_BIQUAD_HPP
#define AUDIO_FILTER_BIQUAD_HPP

class AudioFilterBiquad
{
protected:
    float b0 = 0.0f, b1 = 0.0f, b2 = 0.0f, a1 = 0.0f, a2 = 0.0f;
    float x1 = 0.0f, x2 = 0.0f, y1 = 0.0f, y2 = 0.0f;

public:
    AudioFilterBiquad() = default;

    void setCoefficients(int stage, float coeffs[5])
    {
        // Assuming single stage biquad, stage ignored
        b0 = coeffs[0];
        b1 = coeffs[1];
        b2 = coeffs[2];
        a1 = coeffs[3];
        a2 = coeffs[4];
    }

    void process(float* in, float* out, int numSamples)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            float x = in[i];
            float y = b0 * x + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
            out[i] = y;

            // Update state
            x2 = x1;
            x1 = x;
            y2 = y1;
            y1 = y;
        }
    }
};

#endif // AUDIO_FILTER_BIQUAD_HPP