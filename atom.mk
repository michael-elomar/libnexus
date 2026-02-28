LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libnexus
LOCAL_CATEGORY_PATH := libs
LOCAL_DESCRIPTION := Messaging library based on unix sockets
LOCAL_EXPORT_LDLIBS := -lnexus

LOCAL_LIBRARIES += \
	libneutron

LOCAL_EXPORT_C_INCLUDES := \
	$(LOCAL_PATH)/include

LOCAL_CMAKE_CONFIGURE_ARGS := \
	-DCMAKE_MODULE_PATH=$(TARGET_OUT_STAGING)/usr/lib/cmake/neutron \
	-DBUILD_EXAMPLES=OFF

LOCAL_CMAKE_CONFIGURE_ENV +=\
	BUILD_TYPE=Debug \
	VERSION=1.0.0

include $(BUILD_CMAKE)
