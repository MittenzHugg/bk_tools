#include <string>
#include <fstream>
#include <iostream>
#include "bk_asset.hpp"

int main(int argc, char** argv){
 //parse arguments
    if(argc != 2){
        std::cout << "Incorrect syntex. bk_deflate_code <path/to/input.bin>" << std::endl;
        return 0;
    }
    std::string bin_path = argv[1];

    //try opening ROMS
    size_t lastindex = bin_path.find_last_of("."); 
    std::string rawname = bin_path.substr(0, lastindex); 

    bk_asset code_ass(bk_asset_meta(true, 3), rawname + ".code.bin");
    bk_asset data_ass(bk_asset_meta(true, 3), rawname + ".code.bin");
    
    n64_span s = code_ass.compress();
    std::ofstream of(bin_path, std::ios::out | std::ios::binary);
    of.write((char *) s.begin(), s.size());
    s = data_ass.compress();
    of.write((char *) s.begin(), s.size());
    of.close();

    return 0;
}