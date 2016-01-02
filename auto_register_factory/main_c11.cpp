//工厂方法是最简单地创建派生类对象的方法，也是很常用的，工厂方法内部一般使用switch-case根据不同的key去创建不同的派生类对象。
//需要生产的类越来越多，switch-case会越来越长，显然这种简单工厂已经不堪负荷。
//要消除这些长长的switch-case语句是一个需要解决的问题，而自动注册的对象工厂则是一个比较优雅的解决方案。
//自动注册的对象工厂遵循了开放-封闭原则，新增对象时无需修改原有代码，仅仅需要扩展即可，彻底地消除了switch-case语句。
//本代码参考自：http://geek.csdn.net/news/detail/48963
//低版本gcc可能编译不过去，原因是有一个bug：https://gcc.gnu.org/bugzilla/show_bug.cgi?id=41934

#include <map>
#include <iostream>
#include <string>
#include <functional>
#include <memory>

class Processor {
public:
    virtual ~ Processor() { }
    virtual void process() = 0;
};


class ProcessorFactory {
public:
    template<typename T>
    class Register {
    public:
        Register(const std::string& key)
        {
            ProcessorFactory::get()._map.emplace(
                        key, [] {return new (std::nothrow) T();}
                    );
        }
        
        template<typename... Args>
        Register(const std::string& key, Args... args)
        {
            ProcessorFactory::get()._map.emplace(
                        key, [&] {return new (std::nothrow) T(args...);}
                    );
        }
    };
    
    static Processor * produce(const std::string& key)
    {
        if (_map.find(key) == _map.end()) {
            throw std::invalid_argument("Processor is not exist!");
        }
            
        return _map[key]();
    }
    
private:
    ProcessorFactory() { };
    ProcessorFactory(const ProcessorFactory&) = delete;
    ProcessorFactory(ProcessorFactory&&) = delete;
    
    static ProcessorFactory& get()
    {
        static ProcessorFactory instance;
        return instance;
    }
    
    static std::map<std::string, std::function<Processor* ()>> _map;
};

std::map<std::string, std::function<Processor* ()>> ProcessorFactory::_map;


#define REGISTER_PROCESSOR_VNAME(T) reg_processor_##T##_
#define REGISTER_PROCESSOR(T, ...) \
    static ProcessorFactory::Register<T> REGISTER_PROCESSOR_VNAME(T)(__VA_ARGS__);
//#define REGISTER_PROCESSOR(T, key) \
    static ProcessorFactory::Register<T> REGISTER_PROCESSOR_VNAME(T)(key);
//#define REGISTER_PROCESSOR(T, key, ...) \
    static ProcessorFactory::Register<T> REGISTER_PROCESSOR_VNAME(T)(key, __VA_ARGS__);


class Lu2Processor: public Processor {
public:
    Lu2Processor() { }
    Lu2Processor(int ) { }
    ~ Lu2Processor() { }
    void process() { std::cout << "lu2" << std::endl; }
};
REGISTER_PROCESSOR(Lu2Processor, "lu2", 1);

class NovaProcessor: public Processor {
public:
    NovaProcessor() { }
    NovaProcessor(int ) { }
    ~ NovaProcessor() { }
    void process() { std::cout << "nova" << std::endl; }
};
REGISTER_PROCESSOR(NovaProcessor, "nova", 2);


int main()
{
    Processor * lu2_processor = ProcessorFactory::produce("lu2");
    lu2_processor->process();
    Processor * nova_processor = ProcessorFactory::produce("nova");
    nova_processor->process();
}
