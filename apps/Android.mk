# Copyright 2006 The Android Open Source Project

# Setting LOCAL_PATH will mess up all-subdir-makefiles, so do it beforehand.
SUBDIR_MAKEFILES := $(call all-named-subdir-makefiles,lib greybus functional)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

include $(SUBDIR_MAKEFILES)

