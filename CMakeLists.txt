cmake_minimum_required(VERSION 3.30)
project(Audio_Spatializer)

set(CMAKE_CXX_STANDARD 23)
# 设置 Steam Audio 路径
set(STEAMAUDIO_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/steamaudio/include")
set(STEAMAUDIO_LIB_DIR "${CMAKE_SOURCE_DIR}/steamaudio/lib/windows-x64")

include_directories(src)
include_directories(steamaudio)
include_directories(steamaudio/include)

aux_source_directory(src/capture furry)
add_executable(Audio_Spatializer
        src/test.cpp
        src/test.h
        steamaudio/include/phonon.h
        steamaudio/include/phonon_interfaces.h
        steamaudio/include/phonon_version.h
        ${furry}
        src/capture/process.cpp
        src/capture/process.h
)

link_directories(steamaudio/lib/windows-x64)

target_link_libraries(Audio_Spatializer
        ${CMAKE_SOURCE_DIR}/steamaudio/lib/windows-x64/phonon.lib)

# 将 phonon.dll 复制到输出目录
add_custom_command(TARGET Audio_Spatializer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${STEAMAUDIO_LIB_DIR}/phonon.dll"
        "$<TARGET_FILE_DIR:Audio_Spatializer>"
)

add_compile_options(--enable-clocale)
