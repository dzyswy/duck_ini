#include "ini/ini_file.h"


void value_test();

int main(int argc, char* argv[])
{
    google::InstallFailureSignalHandler();
    google::InitGoogleLogging(argv[0]);

    FLAGS_stderrthreshold = 0;
    FLAGS_minloglevel = 0;

    //value_test();

    
    duck::ini::Root root;
    root.load("../docs/config.ini");

    
    // root.set<float>("ISP_AWB", "r_gain", 1.1);
    // root.set<float>("ISP_AWB", "g_gain", 1.2);
    // root.set<float>("ISP_AWB", "b_gain", 1.3);

    // root.set<unsigned int>("ISP_AE", "gray_level", 1024);
    // root.set<float>("ISP_AE", "ae_gain", 15.0);

    std::cout << root.str() << std::endl;


    float r_gain = root.get<float>("ISP_AWB", "r_gain", 1.0);
    float g_gain = root.get<float>("ISP_AWB", "g_gain", 1.0);
    float b_gain = root.get<float>("ISP_AWB", "b_gain", 1.0);

    unsigned int gray_level = root.get<unsigned int>("ISP_AE", "gray_level", 512);
    float ae_gain = root.get<float>("ISP_AE", "ae_gain", 1.0);

    std::string dev_name = root.get<std::string>("DEVICE", "dev_name", "AI_ISP");

    std::cout << "r_gain=" << r_gain << std::endl;
    std::cout << "g_gain=" << g_gain << std::endl;
    std::cout << "b_gain=" << b_gain << std::endl;

    std::cout << "gray_level=" << gray_level << std::endl;
    std::cout << "ae_gain=" << ae_gain << std::endl;

    std::cout << "dev_name=" << dev_name << std::endl;

    root.set<std::string>("DEVICE", "dev_name", dev_name);

    root.save("config.ini");

    return 0;
}

void value_test() 
{

    duck::ini::Value v_int;
    duck::ini::Value v_long;
    duck::ini::Value v_ll;
    duck::ini::Value v_uint;
    duck::ini::Value v_ulong;
    duck::ini::Value v_ull;

    duck::ini::Value v_bool;
    duck::ini::Value v_float;
    duck::ini::Value v_double;
    duck::ini::Value v_string;

    v_int.set<int>(-123);
    v_long.set<long>(-1234);
    v_ll.set<long long>(-12345);

    v_uint.set<unsigned int>(123);
    v_ulong.set<unsigned long>(1234);
    v_ull.set<unsigned long long>(12345);

    v_bool.set<bool>(true);
    v_float.set<float>(3.14);
    v_double.set<double>(3.1415926);
    //v_string.set("hello"); 
    //v_string.set(std::string("hello"));

    int r_int = v_int.get<int>();
    long r_long = v_long.get<long>();
    long long r_ll = v_ll.get<long long>();

    unsigned int r_uint = v_uint.get<unsigned int>();
    unsigned long r_ulong = v_ulong.get<unsigned long>();
    unsigned long long r_ull = v_ull.get<unsigned long long>();

    bool r_bool = v_bool.get<bool>();
    float r_float = v_float.get<float>();
    double r_double = v_double.get<double>();
    std::string r_string = v_string.get<std::string>();

    std::cout << "r_int=" << r_int << ", typeid=" << typeid(r_int).name() << std::endl;
    std::cout << "r_long=" << r_long << ", typeid=" << typeid(r_long).name() << std::endl;
    std::cout << "r_ll=" << r_ll << ", typeid=" << typeid(r_ll).name() << std::endl;

    std::cout << "r_uint=" << r_uint << ", typeid=" << typeid(r_uint).name() << std::endl;
    std::cout << "r_ulong=" << r_ulong << ", typeid=" << typeid(r_ulong).name() << std::endl;
    std::cout << "r_ull=" << r_ull << ", typeid=" << typeid(r_ull).name() << std::endl;

    std::cout << "r_bool=" << r_bool << ", typeid=" << typeid(r_bool).name() << std::endl;
    std::cout << "r_float=" << r_float << ", typeid=" << typeid(r_float).name() << std::endl;
    std::cout << "r_double=" << r_double << ", typeid=" << typeid(r_double).name() << std::endl;
    std::cout << "r_string=" << r_string << ", typeid=" << typeid(r_string).name() << std::endl;

}









