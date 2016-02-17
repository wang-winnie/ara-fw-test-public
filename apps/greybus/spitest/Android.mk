LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := \
	vendor/google/ara/firmware/test/ara-fw-test/apps/lib/include
LOCAL_SRC_FILES := \
	spitest.c 
LOCAL_MODULE := spitest
LOCAL_STATIC_LIBRARIES := libfwtest
LOCAL_MODULE_TAGS := eng

include $(BUILD_EXECUTABLE)

