//
// Created by Apermesa on 2025/3/20.
//

#include "test.h"
#pragma comment(lib, "phonon.lib")
std::vector<float> load_input_audio(const std::string& filename) {
    std::ifstream file(filename.c_str(), std::ios::binary);

    file.seekg(0, std::ios::end);
    auto fileSize = file.tellg();
    auto numSamples = static_cast<int>(fileSize / sizeof(float));

    std::vector<float> inputAudio(numSamples);
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char *>(inputAudio.data()), fileSize);

    return inputAudio;
}

void save_output_audio(const std::string& filename, std::vector<float> outputaudio) {
    std::ofstream file(filename.c_str(), std::ios::binary);
    file.write(reinterpret_cast<char *>(outputaudio.data()), outputaudio.size() * sizeof(float));
}

int main2(int argc, char **argv) {
    auto inputAudio = load_input_audio(R"(C:\Users\Apermesa\Desktop\input.raw)");

    IPLContextSettings contextSettings{};
    contextSettings.version = STEAMAUDIO_VERSION;

    IPLContext context{};
    iplContextCreate(&contextSettings, &context);

    constexpr auto samplingRate = 44100;
    constexpr auto frameSize = 1024;
    IPLAudioSettings audioSettings{samplingRate, frameSize};

    IPLHRTFSettings hrtfSettings;
    hrtfSettings.type = IPL_HRTFTYPE_DEFAULT;
    hrtfSettings.volume = 1.0f;

    IPLHRTF hrtf{};
    iplHRTFCreate(context, &audioSettings, &hrtfSettings, &hrtf);

    IPLBinauralEffectSettings effectSettings;
    effectSettings.hrtf = hrtf;

    IPLBinauralEffect effect{};
    iplBinauralEffectCreate(context, &audioSettings, &effectSettings, &effect);

    std::vector<float> outputAudioFrame(2 * frameSize);
    std::vector<float> outputaudio;

    auto numFrames = static_cast<int>(inputAudio.size() / frameSize);
    float *inData[] = {inputAudio.data()};

    IPLAudioBuffer inBuffer{1, audioSettings.frameSize, inData};

    IPLAudioBuffer outBuffer;
    iplAudioBufferAllocate(context, 2, audioSettings.frameSize, &outBuffer);

    for (auto i = 0; i < numFrames; ++i) {
        IPLBinauralEffectParams params;
        params.direction = IPLVector3{1.0f, 1.0f, 1.0f};
        params.interpolation = IPL_HRTFINTERPOLATION_NEAREST;
        params.spatialBlend = 1.0f;
        params.hrtf = hrtf;
        params.peakDelays = nullptr;

        iplBinauralEffectApply(effect, &params, &inBuffer, &outBuffer);

        iplAudioBufferInterleave(context, &outBuffer, outputAudioFrame.data());

        std::copy(std::begin(outputAudioFrame), std::end(outputAudioFrame), std::back_inserter(outputaudio));

        inData[0] += audioSettings.frameSize;
    }

    iplAudioBufferFree(context, &outBuffer);
    iplBinauralEffectRelease(&effect);
    iplHRTFRelease(&hrtf);
    iplContextRelease(&context);

    save_output_audio(R"(C:\Users\Apermesa\Documents\GitHub\Audio-Spatializer\dh2.raw)", outputaudio);
    return 0;
}
