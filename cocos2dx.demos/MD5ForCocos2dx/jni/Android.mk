LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := game_shared

LOCAL_MODULE_FILENAME := libgame

LOCAL_SRC_FILES := hellocpp/main.cpp \
                   ../Classes/AppDelegate.cpp \
                   ../Classes/HelloWorldScene.cpp \
                   ../Classes/MD5/MD5ChecksumDefines.cpp
                   
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../Classes   
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../Classes/MD5                

LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static cocosdenshion_static cocos_extension_static
            
include $(BUILD_SHARED_LIBRARY)

$(call import-add-path,$(HOME)/cocos2d-x-2.1.5/cocos2dx/platform/third_party/android/prebuilt)
$(call import-add-path,$(HOME)/cocos2d-x-2.1.5)

$(call import-module,CocosDenshion/android) \
$(call import-module,cocos2dx) \
$(call import-module,extensions)
