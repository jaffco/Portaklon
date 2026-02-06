#ifndef PREAMPSTAGE_H_INCLUDED
#define PREAMPSTAGE_H_INCLUDED

#include <memory>
#include <vector>
#include "../BaseClasses/AudioProcessor.hpp"
#include "wdf.h"

class PreAmpStage : public AudioProcessor
{
public:
    PreAmpStage(float sampleRate, int blockSize) : AudioProcessor(sampleRate)
    {
        C3 = std::make_unique<WaveDigitalFilter::Capacitor> (0.1e-6, this->sampleRate);
        C5 = std::make_unique<WaveDigitalFilter::Capacitor> (68.0e-9, this->sampleRate);
        Vbias.setVoltage (0.0);

        C16 = std::make_unique<WaveDigitalFilter::Capacitor> (1.0e-6, this->sampleRate);
        Vbias2.setVoltage (0.0);
        
        P1 = std::make_unique<WaveDigitalFilter::WDFParallel> (C5.get(), &R6);
        S1 = std::make_unique<WaveDigitalFilter::WDFSeries> (P1.get(), &Vbias);

        P2 = std::make_unique<WaveDigitalFilter::WDFParallel> (&Vbias2, C16.get());
        S2 = std::make_unique<WaveDigitalFilter::WDFSeries> (P2.get(), &R7);

        P3 = std::make_unique<WaveDigitalFilter::WDFParallel> (S1.get(), S2.get());
        S3 = std::make_unique<WaveDigitalFilter::WDFSeries> (P3.get(), C3.get());
        I1 = std::make_unique<WaveDigitalFilter::PolarityInverter> (S3.get());
        Vin.connectToNode (I1.get());

        ff1Current.resize(blockSize);
    }

    void setGain (float gain)
    {
        Vbias.setResistanceValue (gain * 100.0e3);
    }

    inline float processSample (float x)
    {
        Vin.setVoltage (x);

        Vin.incident (I1->reflected());
        auto y = Vbias.voltage() + R6.voltage();
        I1->incident (Vin.reflected());

        return y;
    }

    void processBlock(float* in, float* out, int numSamples) override
    {
        for (int i = 0; i < numSamples; ++i)
        {
            out[i] = processSample(in[i]);
            ff1Current[i] = Vbias2.current();
        }
    }

    std::vector<float> ff1Current;

private:
    WaveDigitalFilter::IdealVoltageSource Vin;
    std::unique_ptr<WaveDigitalFilter::Capacitor> C3;
    std::unique_ptr<WaveDigitalFilter::Capacitor> C5;
    WaveDigitalFilter::Resistor R6 { 10000.0 };
    WaveDigitalFilter::ResistiveVoltageSource Vbias;

    WaveDigitalFilter::Resistor R7 { 1500.0 };
    std::unique_ptr<WaveDigitalFilter::Capacitor> C16;
    WaveDigitalFilter::ResistiveVoltageSource Vbias2 { 15000.0 };

    std::unique_ptr<WaveDigitalFilter::PolarityInverter> I1;
    std::unique_ptr<WaveDigitalFilter::WDFSeries> S1;
    std::unique_ptr<WaveDigitalFilter::WDFSeries> S2;
    std::unique_ptr<WaveDigitalFilter::WDFSeries> S3;
    std::unique_ptr<WaveDigitalFilter::WDFSeries> S4;
    std::unique_ptr<WaveDigitalFilter::WDFParallel> P1;
    std::unique_ptr<WaveDigitalFilter::WDFParallel> P2;
    std::unique_ptr<WaveDigitalFilter::WDFParallel> P3;
};

#endif // PREAMPSTAGE_H_INCLUDED
