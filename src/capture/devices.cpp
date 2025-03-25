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
namespace furry {
    bool devices::init() {
        std::setlocale(LC_ALL, "C");
        std::setlocale(LC_CTYPE, ".UTF-8");
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
        enumerator->EnumAudioEndpoints(eAll, DEVICE_STATE_ACTIVE, &devices);
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
            std::wstring deviceName(prop.pwszVal);
            printf("Device %d: %S\n", i, deviceName.c_str());
            printf("Device ID: %S\n", deviceId);
        }
        CoUninitialize();
        return false;
    }
    //TODO: 剩余部分
} // furry

int main(){
    furry::devices d;
    d.init();
}