//
// Created by LEGION on 2025/3/25.
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
namespace furry {
    bool devices::init() {
        // 清理原有设备信息
        inputDevices.clear();
        outputDevices.clear();
        // 设置编码格式
        std::setlocale(LC_ALL, "C");
        std::setlocale(LC_CTYPE, "");
        SetConsoleOutputCP(CP_REGION);
        IMMDeviceCollection *devices = nullptr;
        IMMDeviceEnumerator *enumerator = nullptr;
        IPropertyStore *propertyStore = nullptr;
        IMMDevice *device = nullptr;
        const CLSID M = __uuidof(MMDeviceEnumerator);
        const IID IM = __uuidof(IMMDeviceEnumerator);
        std::cout << "list" << std::endl;
        CoInitialize(nullptr);
        HRESULT hr = CoCreateInstance(M, nullptr, CLSCTX_ALL, IM, (void**)&enumerator);
        // 输出hr代表的错误
        switch(hr){
            case S_OK:
                std::cout << "S_OK" << std::endl;
                break;
            case E_NOINTERFACE:
                std::cout << "E_NOINTERFACE" << std::endl;
                break;
            case REGDB_E_CLASSNOTREG:
                std::cout << "REGDB" << std::endl;
                break;
            case E_POINTER:
                std::cout << "E_POINTER" << std::endl;
                break;
            default:
                std::cout << "Unknown error" << std::endl;
                break;
        }
        enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &devices);
        // 获得输出设备
        UINT count;
        devices->GetCount(&count);
        for(UINT i=0; i<count; i++){
            devices->Item(i, &device);
            LPWSTR deviceId = nullptr;
            device->GetId(&deviceId);
            device->OpenPropertyStore(STGM_READ, &propertyStore);
            PROPVARIANT prop;
            PropVariantInit(&prop);
            propertyStore->GetValue(PKEY_Device_FriendlyName, &prop);
            std::string deviceName = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(prop.pwszVal);
            printf("OUTPUT Device %d: %s\n", i, deviceName.c_str());
            printf("DEVICE ID: %ls\n", deviceId);
            outputDevice opTmp = {false, deviceId, deviceName, device, FURRY_NOT_USED_OUTPUT};
            outputDevices.push_back(opTmp);
        }
        devices->Release();
        // 获得输入设备
        enumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &devices);
        devices->GetCount(&count);
        for(UINT i=0; i<count; i++){
            devices->Item(i, &device);
            LPWSTR deviceId = nullptr;
            device->GetId(&deviceId);
            device->OpenPropertyStore(STGM_READ, &propertyStore);
            PROPVARIANT prop;
            PropVariantInit(&prop);
// GBK格式
            propertyStore->GetValue(PKEY_Device_FriendlyName, &prop);
            std::string deviceName = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(prop.pwszVal);

            printf("INPUT Device %d: %s\n", i, deviceName.c_str());
            printf("DEVICE ID: %ls\n", deviceId);

            inputDevice ipTmp = {false, nullptr, deviceId, device, deviceName, FURRY_NOT_USED_INPUT};
            inputDevices.push_back(ipTmp);
        }
        // 识别系统音频输出设备
        LPWSTR deviceId = nullptr;
        enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
        for(auto & inputDevice : inputDevices) {
            if (inputDevice.deviceId == deviceId) {
                inputDevice.deviceType = FURRY_ORIGINAL_OUTPUT;
                break;
            }
        }
        CoUninitialize();
        return true;
    }

    std::vector<deviceInfo> devices::getDevices() {
        std::vector<deviceInfo> devices;
        for (auto & device : outputDevices) {
            deviceInfo dv = {device.deviceId, device.deviceName, device.deviceType, device.device};
            devices.push_back(dv);
        }
        for (auto & device : inputDevices) {
            deviceInfo dv = {device.deviceId, device.deviceName, device.deviceType, device.device};
            devices.push_back(dv);
        }
        return devices;
    }

    deviceInfo devices::getOriginalOutputDevice() {
        for (auto & device : outputDevices) {
            if (device.deviceType == FURRY_ORIGINAL_OUTPUT) {
                return deviceInfo {device.deviceId, device.deviceName, device.deviceType};
            }
        }
        return deviceInfo {nullptr, "", FURRY_NOT_USED_OUTPUT};
    }
    void devices::outputToConsole() {
        std::vector<deviceInfo> devices = getDevices();
        for (auto & device : devices) {
            printf("Device Name: %s\n", device.deviceName.c_str());
            printf("Device ID: %ls\n", device.deviceId);
            printf("Device Type: %d\n", device.deviceType);
        }
    }



    //TODO: 剩余部分
}