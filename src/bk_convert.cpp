#include "bk_convert.hpp"
#include <iostream>
#include <iomanip>

template<typename T>
static std::string toHex(T val){
    std::stringstream stream;
    stream << "0x" << std::hex << std::setfill('0') << std::setw(sizeof(val)*2) << std::uppercase << (int)val;
    return stream.str();
};

void bk_dialog2yaml(const bk_dialog_file& dialog_file, YAML::Emitter& emitter){
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "file_type" << YAML::Value << "bk_dialog";
    emitter << YAML::Key << "data" << YAML::Value;
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "bottom" << YAML::Value;
    emitter << YAML::BeginSeq;
    for(const auto& elem: dialog_file.dialog.bottom){
        emitter << YAML::Flow << YAML::BeginSeq;
        emitter << toHex(elem.command) << elem.text;
        emitter << YAML::EndSeq;
    }
    emitter << YAML::EndSeq;
    emitter << YAML::Key << "top" << YAML::Value;
    emitter << YAML::BeginSeq;
    for(const auto& elem: dialog_file.dialog.top){
        emitter  << YAML::Flow << YAML::BeginSeq;
        emitter << toHex(elem.command) << elem.text;
        emitter << YAML::EndSeq;
    }
    emitter << YAML::EndSeq;
    emitter << YAML::EndMap;
    emitter << YAML::EndMap;
}

bk_dialog_file bk_yaml2dialog(YAML::Node const & node){
    bk_dialog_file out;
    
    for(const YAML::Node& e_node : node["data"]["bottom"]){
        out.dialog.bottom.push_back(bk_dialog_string(e_node[0].as<int>(), e_node[1].as<std::string>()));
    }

    for(const YAML::Node& e_node : node["data"]["top"]){
        out.dialog.top.push_back(bk_dialog_string(e_node[0].as<int>(), e_node[1].as<std::string>()));
    }
    return out;
}