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
            ProcessorFactory::get()._map[key] = new (std::nothrow) T();
        }
        
        template<typename... Args>
        Register(const std::string& key, Args... args)
        {
            ProcessorFactory::get()._map[key] = new (std::nothrow) T(args...);
        }
    };
    
    static Processor * produce(const std::string& key)
    {
        if (_map.find(key) == _map.end()) {
            throw std::invalid_argument("Processor is not exist!");
        }
            
        return _map[key];
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
    
    static std::map<std::string, Processor*> _map;
};

std::map<std::string, Processor *> ProcessorFactory::_map;


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
REGISTER_PROCESSOR(Lu2Processor, "lu2");

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
