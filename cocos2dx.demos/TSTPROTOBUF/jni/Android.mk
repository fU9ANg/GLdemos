LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := gprotobuf

LOCAL_MODULE_FILENAME := libprotobuf

LOCAL_SRC_FILES := ../Classes/google/protobuf/compiler/importer.cc \
                   ../Classes/google/protobuf/compiler/parser.cc \
                   ../Classes/google/protobuf/descriptor.cc \
                   ../Classes/google/protobuf/descriptor_database.cc \
                   ../Classes/google/protobuf/descriptor.pb.cc \
                   ../Classes/google/protobuf/dynamic_message.cc \
                   ../Classes/google/protobuf/extension_set.cc \
                   ../Classes/google/protobuf/extension_set_heavy.cc \
                   ../Classes/google/protobuf/generated_message_reflection.cc \
                   ../Classes/google/protobuf/generated_message_util.cc \
                   ../Classes/google/protobuf/io/coded_stream.cc \
                   ../Classes/google/protobuf/io/gzip_stream.cc \
                   ../Classes/google/protobuf/io/printer.cc \
                   ../Classes/google/protobuf/io/tokenizer.cc \
                   ../Classes/google/protobuf/io/zero_copy_stream.cc \
                   ../Classes/google/protobuf/io/zero_copy_stream_impl.cc \
                   ../Classes/google/protobuf/io/zero_copy_stream_impl_lite.cc \
                   ../Classes/google/protobuf/message.cc \
                   ../Classes/google/protobuf/message_lite.cc \
                   ../Classes/google/protobuf/reflection_ops.cc \
                   ../Classes/google/protobuf/repeated_field.cc \
                   ../Classes/google/protobuf/service.cc \
                   ../Classes/google/protobuf/stubs/atomicops_internals_x86_gcc.cc \
                   ../Classes/google/protobuf/stubs/atomicops_internals_x86_msvc.cc \
                   ../Classes/google/protobuf/stubs/common.cc \
                   ../Classes/google/protobuf/stubs/once.cc \
                   ../Classes/google/protobuf/stubs/stringprintf.cc \
                   ../Classes/google/protobuf/stubs/structurally_valid.cc \
                   ../Classes/google/protobuf/stubs/strutil.cc \
                   ../Classes/google/protobuf/stubs/substitute.cc \
                   ../Classes/google/protobuf/text_format.cc \
                   ../Classes/google/protobuf/unknown_field_set.cc \
                   ../Classes/google/protobuf/wire_format.cc \
                   ../Classes/google/protobuf/wire_format_lite.cc
                   
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../Classes/google
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../Classes/google/protobuf
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../Classes/google/protobuf/compiler
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../Classes/google/protobuf/io
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../Classes/google/protobuf/stubs
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../Classes/google/protobuf/testing
            
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := game_shared

LOCAL_MODULE_FILENAME := libgame

LOCAL_SRC_FILES := hellocpp/main.cpp \
                    ../Classes/AppDelegate.cpp \
                    ../Classes/HelloWorldScene.cpp \
                       \
                    ../Classes/message/proto/login.pb.cc
                      
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../Classes
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../Classes/google
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../Classes/google/protobuf
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../Classes/google/protobuf/compiler
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../Classes/google/protobuf/io
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../Classes/google/protobuf/stubs
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../Classes/google/protobuf/testing
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../Classes/message

LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static cocosdenshion_static cocos_extension_static gprotobuf
            
include $(BUILD_SHARED_LIBRARY)

$(call import-module,CocosDenshion/android) \
$(call import-module,cocos2dx) \
$(call import-module,extensions)
