LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := \
	vendor/google/ara/firmware/test/ara-fw-test/apps/lib/include
LOCAL_SRC_FILES := \
	camera_task.c \
	camera_main.c
LOCAL_MODULE := cameratest
LOCAL_STATIC_LIBRARIES := libfwtest
LOCAL_MODULE_TAGS := eng

include $(BUILD_EXECUTABLE)

