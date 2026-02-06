#include "../Portaklon.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <fstream>

int main() {
    const int sampleRate = 48000;
    const int blockSize = 1024;
    const int numBlocks = 100; // Process 100 blocks for steady state

    // Parameter ranges: 0.0 to 1.0 in 0.1 steps
    std::vector<float> paramValues = {0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f};

    float globalMax = 0.0f;
    float maxGain = 0.0f, maxTreble = 0.0f, maxLevel = 0.0f;

    std::ofstream outFile("max_outputs.txt");
    outFile << "Gain,Treble,Level,MaxOutput\n";

    for (float gain : paramValues) {
        for (float treble : paramValues) {
            for (float level : paramValues) {
                Portaklon portaklon;
                portaklon.init(sampleRate, blockSize);

                portaklon.setGain(gain);
                portaklon.setTreble(treble);
                portaklon.setLevel(level);

                // Generate test input: sine wave at 1kHz, amplitude 1.0
                std::vector<float> inputBuffer(blockSize);
                std::vector<float> outputBuffer(blockSize);
                float maxOutput = 0.0f;

                for (int block = 0; block < numBlocks; ++block) {
                    for (int i = 0; i < blockSize; ++i) {
                        float t = (block * blockSize + i) / (float)sampleRate;
                        inputBuffer[i] = std::sin(2.0f * M_PI * 1000.0f * t);
                    }

                    portaklon.process(inputBuffer.data(), outputBuffer.data(), blockSize);

                    for (int i = 0; i < blockSize; ++i) {
                        maxOutput = std::max(maxOutput, std::abs(outputBuffer[i]));
                    }
                }

                outFile << gain << "," << treble << "," << level << "," << maxOutput << "\n";

                if (maxOutput > globalMax) {
                    globalMax = maxOutput;
                    maxGain = gain;
                    maxTreble = treble;
                    maxLevel = level;
                }

                std::cout << "Gain: " << gain << ", Treble: " << treble << ", Level: " << level << " -> Max Output: " << maxOutput << std::endl;
            }
        }
    }

    outFile.close();

    std::cout << "\nMaximum output: " << globalMax << " at Gain: " << maxGain << ", Treble: " << maxTreble << ", Level: " << maxLevel << std::endl;

    return 0;
}