//
// Created by FurryHeiLi on 2025/3/26.
//

#ifndef PROCESS_H
#define PROCESS_H
#include <thread>

#include "devices.h"

namespace furry {

class Process {
    deviceInfo inputDevice;
    deviceInfo outputDevice;
    bool running=false;
    bool muteInput=false;
    std::thread fuck;
public:
    BYTE *(*processFunction) (BYTE *input);
    Process(deviceInfo inputDevice, deviceInfo outputDevice);
    void start(BYTE *(*) (BYTE *input));
    void stop();
    void setInputDevice(deviceInfo inputDevice);
    void setOutputDevice(deviceInfo outputDevice);
    class watcher {
        watcher(bool* loop, deviceInfo inputDevice, deviceInfo outputDevice);
    };


};

} // furry

#endif //PROCESS_H
