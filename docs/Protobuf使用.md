## 概要

本文章是使用`protobuf`反射的一个总结，假定阅读者已经会搭建`proto`环境并且已会生成`.pb.h`文件，阅读完本篇，你会得到以下几个问题的解决方案的参考：

1. Proto 的正常使用
2. 对定义的任意结构进行遍历
3. 在没有通过`protoc.exe`生成头文件的情况下，通过动态加载`*.proto.`来进行结构对象的生成,并可以使用反射机制进行修改

#### 为了测试所有常用类型，后续所有结构皆用下面定义

```
syntax = "proto3";

message SubEx {
	uint64           sub_uint64           = 1;
	int64            sub_int64            = 2;
	uint32           sub_uint32           = 3;
	int32            sub_int32            = 4;
	double           sub_double           = 5;
	float            sub_float            = 6;
	string           sub_string           = 7;
	bool             sub_bool             = 8;	
	repeated uint64  sub_repeated_uint64  = 9;
	repeated int64   sub_repeated_int64   = 10;
	repeated uint32  sub_repeated_uint32  = 11;
	repeated int32   sub_repeated_int32   = 12;
	repeated double  sub_repeated_double  = 13;
	repeated float   sub_repeated_float   = 14;
	repeated string  sub_repeated_string  = 15;
	repeated bool    sub_repeated_bool    = 16;
}
 
message Example {
	uint64 		     ex_uint64           = 1;
	int64  		     ex_int64            = 2;
	uint32 		     ex_uint32           = 3;
	int32  		     ex_int32            = 4;
	double 		     ex_double           = 5;
	float  		     ex_float            = 6;
	string 		     ex_string           = 7;
	bool   		     ex_bool             = 8;		
	repeated uint64  ex_repeated_uint64  = 9;
	repeated int64   ex_repeated_int64   = 10;
	repeated uint32  ex_repeated_uint32  = 11;
	repeated int32   ex_repeated_int32   = 12;
	repeated double  ex_repeated_double  = 13;
	repeated float   ex_repeated_float   = 14;
	repeated string  ex_repeated_string  = 15;
	repeated bool    ex_repeated_bool    = 16;
	SubEx			 sub_ex              = 17;	
}
```



## Proto使用

```
Example* _example = new Example;
_example->set_int32(1);
std::string _example_str = _example->SerializeAsString();
const uint32_t _length = _example->ByteSizeLong();
char* _char = (char*)malloc(_length);
_example->SerializeToArray(_char, _length);

Example* _example_recovery_str = new Example;
_example_recovery_str->ParseFromString(_example_str);
std::cout << _example_recovery_str.get_int32() << std::endl;

Example* _example_recovery_char = new Example;
_example_recovery_char->ParseFromArray(_char, _length);
std::cout << _example_recovery_char.get_int32() << std::endl;
```

以上的代码片段，为在正常情况使用的示例

通过`new`直接生成我们需要的协议对象接着使用`set` `get`接口对协议对象的数据进行修改，然后再通过`SerializeAsString`或者`SerializeToArray`函数生成对应的二进制数据，再传输到其他目的地通过`ParseFromString`或`ParseFromArray`，达成信息传输的目的，或者直接存储至数据库，完成数据持久化的目的，更多其他类型的使用方法都类似于上述使用方案，网上也有很多的示例故不再赘述

## 功能需求驱动探索

使用前面提到方案一段时间后，接到一个需求，在游戏服务器框架外对数据库存入的`protobuf`信息进行全解析，并支持任意结构消息的增改。

经过一段时间的方案寻找，以及编码实践，最终完成了这一需求，事后进行总结，总的来说可以将解决方案拆分成3+1个步骤。

1. 通过反射生成目标类型的`Message`基类对象。
2. 通过`Reflection`来对目标基类对象进行修改，以及遍历递归输出。
3. 通过动态加载`*.proto`文件来达到动态加载，再结合前两部，就可以达到我们为所欲为的目的。
4. 通过前3个步骤，我们已经完成了我们的所有目的，不过我们可以再进行一些上层封装，如`imgui`等提供操作的友好度生成一个较合格的运维工具，交由策划自由发挥，我们也就可以功成身退了。

上述的所有步骤，将一一列举：

### 阶段1.Protobuf 反射 生成对象

通过`Proto使用`可以知道，如延续这个思路进行设计的话进行消息的解析，我们需要获得协议的数据结构`Example`，然后进行创建`new Example`，然后通过手动穷举所有的`get` `set`接口来完成读取和修改，但可见的问题有很多，简单举例：协议发生修改时，对应的`get` `set`也要修改，如果新增了 `*.proto`文件，理所应当的，就要对所有新增的字段重新设置，即使可以使用原型模式对所有类型进行管理，如用此方案，则后续将陷入无限的维护深渊，显然是不可行的。

首先，我们先解决如何利用反射来生成`Message`结构：

![QQ图片20200814075957](C:\Users\yxy\Desktop\QQ图片20200814075957.png)

先观察上述图片，在之前使用的`Example`实际上是直接使用的`Message`类，通过直接创建对象来使用。

在图片的右边有3个类，`MessageFactory`，`Descripot`，`DescriptorPool`，利用这几个类的组合，我们就可以实现我们第一个需求，通过反射生成`Message`对象。

简单介绍一下几个类的作用：

`DescriptorPool`当前所有`proto`类型描述的集合。

`Descriptor`为一个协议类型的描述信息，可以通过字符串从`DescriptorPool`中反射获得。

`MessageFactory`则是一个经典的抽象工厂，通过传入不同的`Descriptor`类来生成我们需要的`Message`原型，没错，获取的是`const Message*`的原型，然后再调用`New()`接口，获取我们的`Message`。也就达到了我们的目的，通过反射生成对象。

所以反射生成对象代码如下：

```
//通过反射，到描述池中获取类型的描述信息
const google::protobuf::Descriptor* _des = 	google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName("Example");
//通过消息工厂加上对应的描述信息拿到类型原型
const google::protobuf::Message* _prototype_msg = google::protobuf::MessageFactory::generated_factory()->GetPrototype(_des);
//通过原型生成我们需求的Message
std::shared_ptr<google::protobuf::Message> _shard_t_msg = std::shared_ptr<google::protobuf::Message>(_prototype_msg->New());
google::protobuf::Message* _my_msg = _prototype_msg->New();
delete _my_msg;
```



### 阶段2.通过Reflection来修改属性以及遍历

本阶段的核心类为 `Reflection` 和 `FieldDescriptor`

前者为当前`Message` 的所有反射，`FieldDescriptor`为子属性的描述，将我们的消息`Message`和`FieldDescriptor`传入，如果是`repeated`则额外传入当前下标，即可完成增改的需求。

当然实际情况比上面说的要复杂一点点，`Proto`的除了基本的数据类型，还有几种的数据结构，对应的修改结构也不一样，需要通过`FieldDescriptor`中携带的类型信息进行接口的选择，不过我们已经获取了正确的思路，剩下的就是编码设计的范畴了。

反射修改属性代码示例如下：

```
google::protobuf::Message* _msg = _prototype_msg->New();
const Reflection* _msg_ref = _prototype_msg->GetReflection();
const FieldDescriptor* _fd = _des->FindFieldByName("ex_uint32");    
_msg_ref->SetInt32(_msg,_fd,100);
std::cout << _msg->DebugString() << std::endl;
```

通过上述基础，可以简单的推出遍历的写法：

```
google::protobuf::Message* _msg = _prototype_msg->New();
const Reflection* _msg_ref = _prototype_msg->GetReflection();
const FieldDescriptor* _fd = _des->FindFieldByName("ex_uint32");    
_msg_ref->SetUInt32(_msg,_fd,100);
const uint32_t _field_count = _des->field_count();
for (size_t _idx = 0; _idx < _field_count; _idx ++) { 
const FieldDescriptor* _fd = _des->field(_idx);
if (_fd->is_repeated()) {
    //用repeated的一套规则，获取数量进行遍历
    const uint32_t _repeated_size = _msg_ref->FieldSize(*_msg,_fd);
    for (uint32_t _repeated_idx = 0; _repeated_idx < _repeated_size; _repeated_idx++) {
        switch (_fd->cpp_type())
        {
            //...
            //CPPTYPE_INT32 = 1,     // TYPE_INT32, TYPE_SINT32, TYPE_SFIXED32
            //CPPTYPE_INT64 = 2,     // TYPE_INT64, TYPE_SINT64, TYPE_SFIXED64
            //CPPTYPE_UINT32 = 3,    // TYPE_UINT32, TYPE_FIXED32
            //CPPTYPE_UINT64 = 4,    // TYPE_UINT64, TYPE_FIXED64
            //CPPTYPE_DOUBLE = 5,    // TYPE_DOUBLE
            //CPPTYPE_FLOAT = 6,     // TYPE_FLOAT
            //CPPTYPE_BOOL = 7,      // TYPE_BOOL
            //CPPTYPE_ENUM = 8,      // TYPE_ENUM
            //CPPTYPE_STRING = 9,    // TYPE_STRING, TYPE_BYTES
            //CPPTYPE_MESSAGE = 10,  // TYPE_MESSAGE, TYPE_GROUP
        case FieldDescriptor::CppType::CPPTYPE_INT32:
            std::cout << _msg_ref->GetRepeatedInt32(*_msg, _fd, _repeated_idx) << std::endl;
            //...
        default:
            break;
        }
    }
}
else {
    switch (_fd->cpp_type())
    {
        //...
    case FieldDescriptor::CppType::CPPTYPE_INT32:
        std::cout << _msg_ref->GetInt32(*_msg, _fd) << std::endl;
        //...
    default:
        break;
    }
        
}
}
```

上述为简单的示例，具体实现可以通过表驱动来优化代码，在`github`中有给出详细的解决方案。

## 阶段3.动态加载协议

前两个阶段实际上已经完成了我的需求，通过反射功能进行遍历，来达到遍历类型的目的，但是在新增`*.proto`时，或者改变了当前的结构时，都需要进行重新编译，无法完全托管至给维护方，从前两个阶段可以得出，只要我们获取了`Descriptor`的信息，就可以通过`MessageFactory`进行原型的生成，所以我们的思路就为如何在运行时刻的从`*.proto`文件中生成`Descriptor`类型。

通过阅读官方文档，我们可以找到解决方案

> https://developers.google.com/protocol-buffers/docs/reference/cpp/google.protobuf.compiler.importer#SourceTreeDescriptorDatabase

通过引入头文件

```
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/dynamic_message.h>
```

我们可以找到如下解决方案：

指定根目录，然后遍历加载目录下所有文件，然后装载`*.proto`文件，目前写法是全部装载，也可以根据需求只加载特定的文件，找出我们需要的`Descriptor`信息,使用 `DynamicMessageFactory`获取原型

```
google::protobuf::compiler::Importer _importer(&_st,nullptr);
for (std::filesystem::directory_iterator _end, _ite(_file_root); _ite != _end; ++_ite){
    if (!is_directory(_ite->path())) {
        _importer.Import(_ite->path().filename().string());
    }
}
const google::protobuf::Descriptor* _des = _des_pool.FindMessageTypeByName("Example");
google::protobuf::DynamicMessageFactory m_dynamic_factory(&_des_pool);
const Message* _prototype_message = m_dynamic_factory.GetPrototype(_des);
Message* _msg = _prototype_message->New();
```

或者

```
const string _proto_file_path = "./Proto";
//设置硬盘根目录
google::protobuf::compiler::DiskSourceTree _st;
_st.MapPath("", _proto_file_path);
    
SourceTreeDescriptorDatabase _stdd(&_st);
google::protobuf::DescriptorPool _des_pool(&_stdd);;
    
//遍历目录下所有文件并进行加载
std::filesystem::path _file_root(_proto_file_path);
std::vector<std::string> _rst_vec;
for (std::filesystem::directory_iterator _end, _ite(_file_root); _ite != _end; ++_ite){
    if (!is_directory(_ite->path())) {
        _des_pool.FindFileByName(_ite->path().filename().string());
    }
}
//动态加载完成,可以使用
const google::protobuf::Descriptor* _des = _des_pool.FindMessageTypeByName("Example");
google::protobuf::DynamicMessageFactory m_dynamic_factory(&_des_pool);
const Message* _prototype_message  = m_dynamic_factory.GetPrototype(_des);
Message* _msg = _prototype_message->New();
```

查看Importer的实现，实际上前一种的写法就是后一种写法的封装。

至此我们已经完成了动态加载`*.proto`文件,并进行遍历的工作。

















