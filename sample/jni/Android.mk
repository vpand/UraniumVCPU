LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)
LOCAL_MODULE := uraniumvm
LOCAL_SRC_FILES := ./uraniumvm_ld_help.c
include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE := uraniumvm_apitest
LOCAL_SHARED_LIBRARIES := uraniumvm
LOCAL_SRC_FILES := ../apitest.cpp

include $(BUILD_EXECUTABLE)
