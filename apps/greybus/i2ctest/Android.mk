LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := vendor/google/ara/firmware/test/ara-fw-test/apps/lib/include
LOCAL_SRC_FILES := \
	i2c-task.c \
	i2ctest.c
LOCAL_MODULE := i2ctest
LOCAL_STATIC_LIBRARIES := libfwtest
LOCAL_MODULE_TAGS := eng

include $(BUILD_EXECUTABLE)

