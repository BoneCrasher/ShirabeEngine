LOCAL_PATH := $(call my-dir)

# Clear all LOCAL_* variables
include $(CLEAR_VARS)

# Build module with name:
LOCAL_MODULE       := ShirabeAndroidPlatformLayer_GpuAPI
# Have the compiler respect:
LOCAL_CPPFLAGS     := -wall
LOCAL_CPP_FEATURES := exceptions
# Compile and link the subsequent sources:
LOCAL_CPP_INCLUDES := appleduckit_shirabe_engine_platformlayer_android_graphics_gpu_api_GpuApiInterface.cpp
# Configure STL
LOCAL_STL          := c++_shared
# We use some external code - Link it!
# (!) Use -lGLESv3 to use GLES3.# functionality from API 18 and onwards.
LOCAL_LDLIBS       := -llog -landroid -lEGL -lGLESv2

# Letse go.
include $(BUILD_SHARED_LIBRARY)