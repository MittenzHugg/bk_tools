#include <string>
#include <iostream>
#include <fstream>
#include "bk_convert.hpp"

int main(int argc, char** argv){
    if(argc != 3){
        std::cout << "syntex error!\n ./bk_yaml2dialog <path/to/input.yaml> <path/to/output.bin>" << std::endl;
        return 1;
    }
    std::string in_name = argv[1];
    std::string out_name = argv[2];

    YAML::Node node = YAML::LoadFile(in_name);
    if(!node["file_type"]){
        std::cout << "Error: Unable to Parse YAML File" << std::endl;
        return 1;
    }
    if(node["file_type"].as<std::string>() != "bk_dialog"){
        std::cout << "Error: yaml is wrong format" << std::endl;
        return 1;
    }

    bk_dialog_file dFile = bk_yaml2dialog(node);
    dFile.write(out_name);
    return 0;
}