LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := \
	vendor/ara-fw-test/apps/lib/include
LOCAL_SRC_FILES := \
	sdio-task.c \
	sdiotest.c
LOCAL_MODULE := sdiotest
LOCAL_STATIC_LIBRARIES := libfwtest
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

