#include "ProtoBufParse.h"
//#include "protoc/example.pb.h"
#include <fstream>

void testMain() {
    //Example* _example =  new Example;
    //_example->set_ex_bool(true);
    //_example->set_ex_int32(-1);
    //_example->set_ex_uint32(1);
    //_example->set_ex_int64(-2);
    //_example->set_ex_uint64(2);
    //_example->set_ex_double(2.f);
    //_example->set_ex_float(1.f);
    //_example->set_ex_string("_exsample->set_ex_string");
    //_example->add_ex_repeated_bool(true);
    //_example->add_ex_repeated_uint64(20);
    //_example->add_ex_repeated_int64(-20);
    //_example->add_ex_repeated_uint32(32);
    //_example->add_ex_repeated_int32(-32);
    //_example->add_ex_repeated_double(1.4);
    //_example->add_ex_repeated_float(1.5f);
    //_example->add_ex_repeated_string("add_ex_repeated_string");
    //_example->add_ex_repeated_bool(false);
    //
    //SubEx* _sub_example = _example->mutable_sub_ex();
    //_sub_example->set_sub_bool(true);
    //_sub_example->set_sub_int32(-1);
    //_sub_example->set_sub_uint32(1);
    //_sub_example->set_sub_int64(-2);
    //_sub_example->set_sub_uint64(2);
    //_sub_example->set_sub_double(2.f);
    //_sub_example->set_sub_float(1.f);
    //_sub_example->set_sub_string("_exsample->set_ex_string");
    //_sub_example->add_sub_repeated_bool(true);
    //_sub_example->add_sub_repeated_uint64(20);
    //_sub_example->add_sub_repeated_int64(-20);
    //_sub_example->add_sub_repeated_uint32(32);
    //_sub_example->add_sub_repeated_int32(-32);
    //_sub_example->add_sub_repeated_double(1.4);
    //_sub_example->add_sub_repeated_float(1.5f);
    //_sub_example->add_sub_repeated_string("add_ex_repeated_string");
    //_sub_example->add_sub_repeated_bool(false);
    //
    //std::cout << _example->DebugString() << std::endl;
    //ofstream  _file("./test_binary");
    //_file << _example->SerializePartialAsString();
    //_file.close();

    //文件内容是通过上述代码生成
    ifstream  _file("./test_binary");
    std::string _binary_str((std::istreambuf_iterator<char>(_file)),
        std::istreambuf_iterator<char>());
    

    static RepeateFDCallback repeate_fd_cb = [](Message& msg_, const FieldDescriptor& field_des_, const size_t index_) {
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
    FDCallback fd_cb = [](Message& msg_, const FieldDescriptor& field_des_) {
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


    ProtoBufParse::GetInstance().Parse("Example", _binary_str, repeate_fd_cb, fd_cb);
}



int main() {
    testMain();
    system("pause");
    return 0;
}