#include <string>
#include <fstream>
#include <iostream>
#include "bk_asm.hpp"
#include "file_helper.hpp"

int main(int argc, char** argv){
 //parse arguments
    if(argc != 4){
        std::cout << "Incorrect syntex. bk_deflate_code <output.bin> <code.bin> <data.bin>" << std::endl;
        return 0;
    }
    std::string o_path = argv[1];
    std::string c_path = argv[2];
    std::string d_path = argv[3];

    //try opening ROMS
    std::ifstream c_s(c_path, std::ios::in | std::ios::binary | std::ios::ate);
    uint32_t c_size = c_s.tellg();
    size_t final_c = c_size;
    uint8_t *c_buffer = (uint8_t *)malloc(c_size);
    
    n64_span c_span(c_buffer, c_size); 
    c_s.seekg(0);
	c_s.read((char*)c_buffer + 6, c_size);
	c_s.close(); 

    std::ifstream d_s(d_path, std::ios::in | std::ios::binary | std::ios::ate);
    uint32_t d_size = d_s.tellg();
    uint8_t *d_buffer = (uint8_t *)malloc(d_size);
    n64_span d_span(d_buffer, d_size); 
    d_s.seekg(0);
	d_s.read((char*)d_buffer + 6, d_size);
	d_s.close(); 

    std::cout << "code size: " << std::to_string(c_size) << std::endl;
    std::cout << "data size: " << std::to_string(d_size) << std::endl;

    bk_asm assembly(c_span, d_span);
    span_write(assembly.compressed(), o_path);

    return 0;
}