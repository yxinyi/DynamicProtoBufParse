#pragma once
#include "google/protobuf/message.h"
#include "google/protobuf/dynamic_message.h"
#include "google/protobuf/compiler/importer.h"
#include <string>
#include <iostream>
#include <functional>
#include <filesystem>

using namespace std;
using namespace google::protobuf;
using namespace google::protobuf::compiler;

using RepeateFDCallback = function<void(Message& msg_, const FieldDescriptor& field_des_, const size_t m_index)>;
static RepeateFDCallback s_default_repeate_cb = [](Message& msg_, const FieldDescriptor& field_des_, const size_t index_) {
    //std::cout << "type_name:  " << field_des_.type_name() << std::endl;
    //std::cout << "camelcase_name:  " << field_des_.camelcase_name() << std::endl;
    //std::cout << "full_name:  " << field_des_.full_name() << std::endl;
    //std::cout << "json_name:  " << field_des_.json_name() << std::endl;
    //std::cout << "PrintableNameForExtension:  " << field_des_.PrintableNameForExtension() << std::endl;
    //std::cout << "lowercase_name:  " << field_des_.lowercase_name() << std::endl;
    //要判断除了Message之外的所有类型
    switch (field_des_.cpp_type())
    {
    case FieldDescriptor::CPPTYPE_INT32:
        std::cout << field_des_.full_name() << " : " << index_ << " : " << msg_.GetReflection()->GetRepeatedInt32(msg_, &field_des_, index_) << std::endl;
        break;
    case FieldDescriptor::CPPTYPE_INT64:
        std::cout << field_des_.full_name() << " : " << index_ << " : " << msg_.GetReflection()->GetRepeatedInt64(msg_, &field_des_, index_) << std::endl;
        break;
    case FieldDescriptor::CPPTYPE_UINT32:
        std::cout << field_des_.full_name() << " : " << index_ << " : " << msg_.GetReflection()->GetRepeatedUInt32(msg_, &field_des_, index_) << std::endl;
        break;
    case FieldDescriptor::CPPTYPE_UINT64:
        std::cout << field_des_.full_name() << " : " << index_ << " : " << msg_.GetReflection()->GetRepeatedUInt64(msg_, &field_des_, index_) << std::endl;
        break;
    case FieldDescriptor::CPPTYPE_DOUBLE:
        std::cout << field_des_.full_name() << " : " << index_ << " : " << msg_.GetReflection()->GetRepeatedDouble(msg_, &field_des_, index_) << std::endl;
        break;
    case FieldDescriptor::CPPTYPE_FLOAT:
        std::cout << field_des_.full_name() << " : " << index_ << " : " << msg_.GetReflection()->GetRepeatedFloat(msg_, &field_des_, index_) << std::endl;
        break;
    case FieldDescriptor::CPPTYPE_BOOL:
        std::cout << field_des_.full_name() << " : " << index_ << " : " << msg_.GetReflection()->GetRepeatedBool(msg_, &field_des_, index_) << std::endl;
        break;
    case FieldDescriptor::CPPTYPE_ENUM:
        std::cout << field_des_.full_name() << " : " << index_ << " : " << msg_.GetReflection()->GetRepeatedEnum(msg_, &field_des_, index_) << std::endl;
        break;
    case FieldDescriptor::CPPTYPE_STRING:
        std::cout << field_des_.full_name() << " : " << index_ << " : " << msg_.GetReflection()->GetRepeatedString(msg_, &field_des_, index_) << std::endl;
        break;
    default:
        break;
    }
};
using FDCallback = function<void(Message& msg_, const FieldDescriptor& field_des_)>;
static FDCallback s_default_fd_cb = [](Message& msg_, const FieldDescriptor& field_des_) {
    //std::cout << "type_name:  " << field_des_.type_name() << std::endl;
    //std::cout << "camelcase_name:  " << field_des_.camelcase_name() << std::endl;
    //std::cout << "full_name:  " << field_des_.full_name() << std::endl;
    //std::cout << "json_name:  " << field_des_.json_name() << std::endl;
    //std::cout << "PrintableNameForExtension:  " << field_des_.PrintableNameForExtension() << std::endl;
    //std::cout << "lowercase_name:  " << field_des_.lowercase_name() << std::endl;
    switch (field_des_.cpp_type())
    {
    case FieldDescriptor::CPPTYPE_INT32:
        std::cout << field_des_.full_name() << " : " << msg_.GetReflection()->GetInt32(msg_, &field_des_) << std::endl;
        break;
    case FieldDescriptor::CPPTYPE_INT64:
        std::cout << field_des_.full_name() << " : " << msg_.GetReflection()->GetInt64(msg_, &field_des_) << std::endl;
        break;
    case FieldDescriptor::CPPTYPE_UINT32:
        std::cout << field_des_.full_name() << " : " << msg_.GetReflection()->GetUInt32(msg_, &field_des_) << std::endl;
        break;
    case FieldDescriptor::CPPTYPE_UINT64:
        std::cout << field_des_.full_name() << " : " << msg_.GetReflection()->GetUInt64(msg_, &field_des_) << std::endl;
        break;
    case FieldDescriptor::CPPTYPE_DOUBLE:
        std::cout << field_des_.full_name() << " : " << msg_.GetReflection()->GetDouble(msg_, &field_des_) << std::endl;
        break;
    case FieldDescriptor::CPPTYPE_FLOAT:
        std::cout << field_des_.full_name() << " : " << msg_.GetReflection()->GetFloat(msg_, &field_des_) << std::endl;
        break;
    case FieldDescriptor::CPPTYPE_BOOL:
        std::cout << field_des_.full_name() << " : " << msg_.GetReflection()->GetBool(msg_, &field_des_) << std::endl;
        break;
    case FieldDescriptor::CPPTYPE_ENUM:
        std::cout << field_des_.full_name() << " : " << msg_.GetReflection()->GetEnum(msg_, &field_des_) << std::endl;
        break;
    case FieldDescriptor::CPPTYPE_STRING:
        std::cout << field_des_.full_name() << " : " << msg_.GetReflection()->GetString(msg_, &field_des_) << std::endl;
        break;
    default:
        break;
    }
};

static const string s_proto_path = "./proto";

class ProtoBufParse {
public:
    static ProtoBufParse& GetInstance() {
        static ProtoBufParse s_protobuf_parse;
        return s_protobuf_parse;
    }
    bool Parse(const string& msg_name_, const char* binary_begin_, const uint32_t length_, RepeateFDCallback repeated_cb_ = s_default_repeate_cb, FDCallback field_descriptor_cb_ = s_default_fd_cb);
    bool Parse(const string& msg_name_, const string& binary_str_, RepeateFDCallback repeated_cb_ = s_default_repeate_cb, FDCallback field_descriptor_cb_ = s_default_fd_cb);
    Message* BuildToMessage(const string& msg_name_, const char* binary_begin_, const uint32_t length_);
    Message* BuildToMessage(const string& msg_name_, const string& binary_str_);
    Message* CreateBlankMessage(const string& msg_name_);


private:
    ProtoBufParse();
    ~ProtoBufParse();
    DynamicMessageFactory m_dynamic_msg_factory;
    DiskSourceTree        m_srouce_tree;
    Importer              m_importer;
};