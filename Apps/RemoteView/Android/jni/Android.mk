ORIGINAL_PATH := $(call my-dir)
PROJECT_PATH := $(call my-dir)/../../../../..
APHEREON_PATH := $(call my-dir)/../../../../../../../Aphereon4
PINGPONG_PATH := $(call my-dir)/../..

include $(APHEREON_PATH)/Android/Aphereon.mk

include $(CLEAR_VARS)
LOCAL_PATH := $(PINGPONG_PATH)
LOCAL_CFLAGS := $(filter-out -fno-rtti,$(LOCAL_CFLAGS))
LOCAL_CFLAGS += -frtti -std=c++0x -fexceptions
LOCAL_MODULE := PingPong
FILE_LIST := $(wildcard $(LOCAL_PATH)/*.cpp)
LOCAL_SRC_FILES += $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_LDLIBS += -llog -lGLESv1_CM -lGLESv2 -landroid -ljnigraphics -lOpenSLES
include $(APHEREON_PATH)/Android/AphStdSO.mk

include $(BUILD_SHARED_LIBRARY)