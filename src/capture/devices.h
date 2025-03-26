//
// Created by LEGION on 2025/3/25.
//

#ifndef AUDIO_SPATIALIZER_DEVICES_H
#define AUDIO_SPATIALIZER_DEVICES_H
#include <string>
#include <vector>
#include <Windows.h>

namespace furry {
    #define FURRY_ORIGINAL_OUTPUT 1
    #define FURRY_LINKED_AS_INPUT 2
    #define FURRY_LINKED_AS_OUTPUT 3
    #define FURRY_NOT_USED_INPUT 4
    #define FURRY_NOT_USED_OUTPUT 5
    struct deviceInfo{
        LPWSTR deviceId;
        std::string deviceName;
        int deviceType;
    };

    class devices {
        struct outputDevice{
           bool linked = false;
           LPWSTR deviceId;
           std::string deviceName;
           int deviceType;
        };
        struct inputDevice{
            bool linked = false;
            outputDevice *linkedDevice = nullptr;
            LPWSTR deviceId;
            std::string deviceName;
            int deviceType;
        };

        std::vector<inputDevice> inputDevices;
        std::vector<outputDevice> outputDevices;

        public:
            bool init();
            std::vector<deviceInfo> getDevices();
            deviceInfo getOriginalOutputDevice();
            void outputToConsole();
    };

} // furry

#endif //AUDIO_SPATIALIZER_DEVICES_H
