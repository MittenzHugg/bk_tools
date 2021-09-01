#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "bk_convert.hpp"

template<typename T>
std::string toHex(T val){
    std::stringstream stream;
    stream << "0x" << std::hex << std::setfill('0') << std::setw(sizeof(val)*2) << std::uppercase << (int)val;
    return stream.str();
};

int main(int argc, char** argv){
    if(argc != 3){
        std::cout << "syntex error!\n ./bk_dialog2yaml <path/to/input.bin> <path/to/output.yaml>" << std::endl;
        return 1;
    }
    std::string in_name = argv[1];
    std::string out_name = argv[2];

    bk_dialog_file dialog_file(in_name);

    YAML::Emitter test;
    bk_dialog2yaml(dialog_file, test);
    
    std::ofstream f_yaml(out_name, std::ios::out);
    f_yaml << test.c_str() << std::endl;
    f_yaml.close();
    return 0;
}