# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.

project("annotateimages")


set(COMMON_DIR common)
set(skia_private_dir common/include/private)
set(skia_core_dir common/include/core)
set(skia_effects_dir common/include/effects)
set(skia_codec_dir common/include/codec)
set(skia_config_dir common/include/config)
set(skia_gpu_dir common/include/gpu)
set(wb_draw_dir draw)
set(libjpeg_dir common/libjpeg-turbo)
set(libzip_dir common/libzip)
set(log_dir Log)
set(fb_dir Fb)

include_directories(${COMMON_DIR})
include_directories(${skia_private_dir})
include_directories(${skia_core_dir})
include_directories(${skia_effects_dir})
include_directories(${skia_codec_dir})
include_directories(${skia_config_dir})
include_directories(${skia_gpu_dir})
include_directories(${wb_draw_dir})
include_directories(${libjpeg_dir})
include_directories(${libzip_dir})
include_directories(${log_dir})
include_directories(${fb_dir})

file(GLOB ACTION_LIST "Action/*.cpp")
file(GLOB LOG_LIST "Log/*.cpp")
file(GLOB FB_LIST "Fb/*.cpp")
file(GLOB SHOW_LIST "Show/*.cpp")


add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
        ${ACTION_LIST} ${LOG_LIST} ${FB_LIST} ${SHOW_LIST})

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )