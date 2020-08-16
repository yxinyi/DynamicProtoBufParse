#include "ProtoBufParse.h"

std::vector<std::string> getFileName(const string& root_path_) {
    std::filesystem::path _file_root(root_path_);
    std::vector<std::string> _rst_vec;
    for (filesystem::directory_iterator _end, _ite(_file_root); _ite != _end; ++_ite)
    {
        if (!is_directory(_ite->path()))
            _rst_vec.push_back(_ite->path().filename().string());
    };
    return _rst_vec;
}

static void parseProto( google::protobuf::Message& msg_, RepeateFDCallback repeated_fd_cb_, FDCallback fd_cb_) {
    //获取与传入 msg 类型相匹配的描述符信息
    const google::protobuf::Reflection* _ref = msg_.GetReflection();
    const google::protobuf::Descriptor* _des = msg_.GetDescriptor();
    const size_t _field_count = _des->field_count();
    //遍历当前消息所有类型
    for (size_t _idx = 0; _idx < _field_count; _idx++) {
        //获取属性描述符信息
        const google::protobuf::FieldDescriptor* _field_des = _des->field(_idx);
        //std::cout << _idx << " : "<<_field_des->cpp_type_name() << std::endl;
        //如果是 repeted 类型则进行遍历
        if (_field_des->is_repeated()) {
            const size_t _repeated_size = _ref->FieldSize(msg_, _field_des);
            for (size_t _repeated_idx = 0; _repeated_idx < _repeated_size; _repeated_idx++) {
                if (_field_des->cpp_type() == google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE) {
                    if (google::protobuf::Message* _msg = _ref->MutableRepeatedMessage(&msg_, _field_des, _repeated_idx)) {
                        parseProto(*_msg, repeated_fd_cb_, fd_cb_);
                    }
                }
                else {
                    repeated_fd_cb_(msg_, *_field_des, _repeated_idx);
                }
            }
        }
        else {
            if (_field_des->cpp_type() == google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE) {
                if (google::protobuf::Message* _msg = _ref->MutableMessage(&msg_, _field_des)) {
                    parseProto(*_msg, repeated_fd_cb_, fd_cb_);
                }
            }
            else {
                fd_cb_(msg_, *_field_des);
            }
        }
    }
}

///////////////////////////////////////////////////

ProtoBufParse::ProtoBufParse() :m_importer(&m_srouce_tree, nullptr) {
    m_srouce_tree.MapPath("", s_proto_path);
    std::vector<string > _files_name = getFileName(s_proto_path);
    for (auto&& _it : _files_name) {
        std::cout << "relaodDaymicProto: [" << _it << "]" << std::endl;
        m_importer.Import(_it);
    }
}
ProtoBufParse::~ProtoBufParse() {

}

bool ProtoBufParse::Parse(const string& msg_name_, const char* binary_begin_, const uint32_t length_, RepeateFDCallback repeated_fd_cb_, FDCallback fd_cb_) {
    Message* _msg = BuildToMessage(msg_name_, binary_begin_, length_);
    if (!_msg) {
        return false;
    }
    
    parseProto(*_msg, repeated_fd_cb_, fd_cb_);
    return true;
}
bool ProtoBufParse::Parse(const string& msg_name_, const string& binary_str_, RepeateFDCallback repeated_fd_cb_, FDCallback fd_cb_) {
    return Parse(msg_name_, binary_str_.data(), binary_str_.size(), repeated_fd_cb_, fd_cb_);
}
Message* ProtoBufParse::BuildToMessage(const string& msg_name_, const char* binary_begin_, const uint32_t length_) {
    Message* _msg = nullptr;
    if (_msg = CreateBlankMessage(msg_name_)) {
        _msg->ParseFromArray(binary_begin_, length_);
    }
    return _msg;
}
Message* ProtoBufParse::BuildToMessage(const string& msg_name_, const string& binary_str_) {
    return BuildToMessage(msg_name_,binary_str_.data(), binary_str_.size());
}
Message* ProtoBufParse::CreateBlankMessage(const string& msg_name_) {
    Message* _msg = nullptr;
    if (const Descriptor* _des = DescriptorPool::generated_pool()->FindMessageTypeByName(msg_name_)) {
        const Message* _prototype_msg = MessageFactory::generated_factory()->GetPrototype(_des);
        if (!_prototype_msg) {
            return nullptr;
        }
        _msg = _prototype_msg->New();
    }
    if (const Descriptor* _des = m_importer.pool()->FindMessageTypeByName(msg_name_)) {
        const Message* _prototype_msg = m_dynamic_msg_factory.GetPrototype(_des);
        if (!_prototype_msg) {
            return nullptr;
        }
        _msg = _prototype_msg->New();
    }
    return _msg;
}
