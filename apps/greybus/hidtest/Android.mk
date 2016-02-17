LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := \
	vendor/google/ara/firmware/test/ara-fw-test/apps/lib/include \
	external/udev/include
LOCAL_SRC_FILES := \
	hid-task.c \
	hidtest.c
LOCAL_MODULE := hidtest
LOCAL_STATIC_LIBRARIES := libfwtest
LOCAL_SHARED_LIBRARIES := libudev
LOCAL_MODULE_TAGS := eng

include $(BUILD_EXECUTABLE)

