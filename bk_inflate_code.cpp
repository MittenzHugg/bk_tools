#include <string>
#include <fstream>
#include <iostream>
#include "bk_asm.hpp"
#include "file_helper.hpp"
#include "yaml-cpp/yaml.h"

int main(int argc, char** argv){
    bool generate_splat_yaml;
    std::string bin_path = "";
    std::string out_path = "";

    //parse arguments
    for(int i = 1; i < argc && i < 4; i++){
        std::string argi= argv[i];
        if(argi == "-y")
            generate_splat_yaml = true;
        else if(bin_path == "")
            bin_path = argi;
        else 
            out_path = argi;
    }

    if(bin_path == "" || out_path == ""){
        std::cout << "Incorrect syntex. bk_inflate_code [-y] <path/to/input.bin> <path/to/output.bin>" << std::endl;
        return 0;
    }

    //try opening ROMS
    size_t lastindex = out_path.find_last_of("."); 
    std::string rawname = out_path.substr(0, lastindex); 

    std::ifstream in_f(bin_path, std::ios::binary | std::ios::in | std::ios::ate);
    size_t file_size = in_f.tellg();
    in_f.seekg(0);

    uint8_t* buffer = (uint8_t *) malloc(file_size);
    in_f.read((char*) buffer, file_size);
    in_f.close();
    n64_span bin(buffer, file_size);
    bk_asm file(bin);

    size_t out_size = file.code().size() + file.data().size();
    uint8_t* out_buffer = (uint8_t*) malloc(out_size);
    uint8_t* out_end = std::copy(file.code().begin(), file.code().end(), out_buffer); 
    out_end = std::copy(file.data().begin(), file.data().end(), out_end);

    if(generate_splat_yaml){
        YAML::Node splat;
        
        splat["basename"] = rawname;
        splat["options"]["find_file_boundaries"] = true;
        splat["options"]["compiler"] = "\"IDO\"";
        splat["options"]["undefined_syms.us.v10.txt"] = "undefined_syms." + rawname + "txt";
        splat["segments"][0]["name"] = "code";
        splat["segments"][0]["type"] = "code";
        splat["segments"][0]["start"] = 0;
        splat["segments"][0]["files"] = 
        splat["segments"][0]["name"] = "data";
        splat["segments"][1]["start"] = file.code().size();

        std::ofstream fout(rawname + ".yaml");
        fout << splat;
        fout.close();
    }

    free(buffer);
    std::ofstream o_f(out_path);
    o_f.write((char*) out_buffer, out_size); 
    o_f.close();

    free(out_buffer);

    return 0;
}
