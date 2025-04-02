//
// Created by FurryHeiLi on 2025/3/26.
//

#include <initguid.h>
#include "devices.h"
#include <Windows.h>
#include <mmdeviceapi.h>
#include <combaseapi.h>
#include <iostream>
#include <functiondiscoverykeys_devpkey.h>
#include <propsys.h>
#include <locale>
#include <codecvt>
#include "process.h"
#include <audioclient.h>
#include <winerror.h>
#include <utility>
#include <audiopolicy.h>
#include <audiosessiontypes.h>
#include <audevcod.h>
#include <endpointvolume.h>

using namespace furry;
namespace furry {
    Process::Process(deviceInfo inputDevice, deviceInfo outputDevice) {
        this->inputDevice = std::move(inputDevice);
        this->outputDevice = std::move(outputDevice);
    }

}

[[noreturn]] void run(deviceInfo in, deviceInfo out, BYTE *(*processFunction) (BYTE *input)){
    const IID IID_IAudioClient = __uuidof(IAudioClient);
    const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);
    IAudioClient *pAudioClientC = NULL;
    IAudioClient *pAudioClientR = NULL;
    IAudioCaptureClient *pCaptureClient = NULL;
    IAudioRenderClient *pRenderClient = NULL;
    WAVEFORMATEX *pwfx = NULL;
    WAVEFORMATEX *after = NULL;
    UINT32 bufferFrameCount_in;
    UINT32 bufferFrameCount_out;
    UINT32 numFramesAvailable_in;
    UINT32 numFramesAvailable_out;
    UINT32 numFramesPadding;
    BYTE *tmp;
    DWORD flags;
// 音频输出端配置
    out.device->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&pAudioClientR);
    pAudioClientR->GetMixFormat(&pwfx);
    HRESULT h = pAudioClientR->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, pwfx, &after);
    pAudioClientR->Initialize(AUDCLNT_SHAREMODE_SHARED,
                              0, 100000000,0, pwfx, NULL);
    pAudioClientR->GetBufferSize(&bufferFrameCount_out);
    pAudioClientR->GetService(IID_IAudioRenderClient, (void**)&pRenderClient);
    // 音频劫持端配置
    in.device->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&pAudioClientC);
    pAudioClientC->GetMixFormat(&after);
    pAudioClientC->Initialize(AUDCLNT_SHAREMODE_SHARED,
                             AUDCLNT_STREAMFLAGS_LOOPBACK, 100000000,0, pwfx, NULL);
    pAudioClientC->GetBufferSize(&bufferFrameCount_in);
    pAudioClientC->GetService(IID_IAudioCaptureClient, (void**)&pCaptureClient);


    REFERENCE_TIME rt=0;
    pRenderClient->GetBuffer(bufferFrameCount_out, &tmp);
    tmp = (BYTE*)malloc(bufferFrameCount_out * pwfx->nBlockAlign);
    pRenderClient->ReleaseBuffer(bufferFrameCount_out, flags);
    rt = (double)10000000 * bufferFrameCount_out / pwfx->nSamplesPerSec;

    pAudioClientC->Start();
    HRESULT hr2 = pAudioClientR->Start();

    HRESULT hr;
    UINT32 lengthPacket;
/**
    IAudioEndpointVolume *pVolume = NULL;
    out.device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pVolume);
    BOOL mute;
    pVolume->GetMute(&mute);
    pVolume->SetMute(FALSE, NULL);
    UINT channelCount;
    pVolume->GetChannelCount(&channelCount);
    pVolume->SetMasterVolumeLevelScalar(1.0, NULL);
    FLOAT level;
    pVolume->GetChannelVolumeLevel(0, &level);
    pVolume->SetChannelVolumeLevel(0, 100, NULL);
    pVolume->SetMasterVolumeLevel(100.0, NULL);
**/
    while(true){
        BYTE *data_in = nullptr;
        pCaptureClient->GetNextPacketSize(&lengthPacket);
        int offset = 0;
        while (lengthPacket != 0) {
            hr = pCaptureClient->GetBuffer(&data_in, &numFramesAvailable_in, &flags, NULL, NULL);
            pAudioClientR->GetCurrentPadding(&numFramesPadding);
            UINT32 padding = bufferFrameCount_out - numFramesPadding;
            UINT32 writeFrames = std::min(numFramesAvailable_in, padding);
            BYTE *data_out = nullptr;
            pRenderClient->GetBuffer(writeFrames, &data_out);
            memcpy(data_out, data_in, writeFrames * pwfx->nBlockAlign);
            pRenderClient->ReleaseBuffer(writeFrames, flags);
            pCaptureClient->ReleaseBuffer(numFramesAvailable_in);
            pCaptureClient->GetNextPacketSize(&lengthPacket);
        }


    }

    //
}
BYTE *processFunction(BYTE *input){
    return input;
}
int main(){
    devices d = devices();
    d.init();
    deviceInfo ds = d.getOriginalOutputDevice();
    std::vector<deviceInfo> fff = d.getDevices();
    deviceInfo output;
    for(auto & i : fff){
        printf("Device Name: %s\n", i.deviceName.c_str());
        printf("Device ID: %ls\n", i.deviceId);
        if(wcscmp(i.deviceId, ds.deviceId) != 0){
            output = i;
            std::cout << "Selected device: " << i.deviceName << std::endl;
            std::cout << "Output device: " << ds.deviceName << std::endl;
            break;
        }
    }
    run(ds, output, processFunction);

}