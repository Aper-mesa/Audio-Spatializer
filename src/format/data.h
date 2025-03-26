//
// Created by FurryHeiLi on 2025/3/25.
//
#pragma once

#ifndef DATA_H
#define DATA_H
#include <vector>

namespace furry {
    struct soundData {
        bool success;
        //所有类统一的输出格式，包含音频和配置信息
        short channels; // 声道数
        int sampleRate; // 采样频率
        int frameSize; // 帧大小
        std::vector<float> data; // 音频数据
    };
}
#endif //DATA_H
