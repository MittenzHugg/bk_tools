#include <string>
#include <fstream>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <concepts>

#include "sprite.hpp"
#include "n64/n64_span.hpp"
#include "apng.hpp"
#include "pixel_types.hpp"


std::vector<BKSpriteFrameChunk> texturize(const BKSprite& font, const BKSprite& texture){
    std::vector<BKSpriteFrameChunk> outVal;
    const BKSpriteFrame& img = texture.Frame[0];
    for(auto& Letter : font.Frame[0].Chunk){
        BKSpriteFrameChunk new_letter = Letter;

        int16_t x_min = (img.w - Letter.w)/2;
        int16_t y_min = (img.h - Letter.h)/2;

        for(int x = 0; x < Letter.w; x++){
            for(int y = 0; y < Letter.h; y++){
                rgba_t tPxl  = img.Data[(x + x_min) + (y + y_min)*img.w];
                rgba_t fPxl = Letter.Data[x + Letter.w*y];
                uint8_t sat = fPxl.b/0x1F;
                rgba_t newPxl = {(tPxl.r>>3)*sat, (tPxl.g>>3)*sat, (tPxl.b>>3)*sat, fPxl.a};
                new_letter.Data[x + Letter.w*y] = {(tPxl.r>>3)*sat, (tPxl.g>>3)*sat, (tPxl.b>>3)*sat, fPxl.a};
            }
        }
        outVal.push_back(new_letter);
    }
    return outVal;
}

int main(int argc, char** argv){
    uint16_t alpha = 0;
    uint32_t max_height = 0;
    uint32_t max_width = 0;

    // std::string file_id = argv[1];
    // std::string alphaMask_fileName = "assets/06ec.bin";
    // std::string numbers_fileName = "assets/06ed.bin";
    // std::string texture_fileName = "assets/" + file_id + ".bin";
    // std::string print_string = "Test";

    bk_dialog_file dFile("assets/0e1f.bin");

    dFile.dialog.bottom[0].text = "HELLO WORLD";
    

    bk_dialog_string bottomLast = dFile.dialog.bottom.back();
    dFile.dialog.bottom.pop_back();
    dFile.dialog.bottom.push_back(bk_dialog_string(0x82, "TESTING 1... 2... 3..."));
    dFile.dialog.bottom.push_back(bk_dialog_string(0x83, "4... 5... 6..."));
    dFile.dialog.bottom.push_back(bottomLast);

    dFile.write("out/test.bin");
    // //parse font bin
    // const N64Bin alphaMask_file(alphaMask_fileName);
    // const BKSprite alphaMask(static_cast<n64_span>(alphaMask_file));

    // test_struct testStruct;
      
    
    // testStruct = alphaMask_file;
    // alphaMask_file.get<test_struct_2>();
    // std::vector<uint8_t> testContainer =  alphaMask_file.get<std::vector<uint8_t>>(16);

    // std::cout << "Container : {" << std::hex; 
    // for(const auto& elem: testContainer){
    //     std::cout << " 0x";
    //     std::cout << std::setfill('0') << std::setw(2) << (int) elem;
    // }
    // std::cout << "}" << std::endl;

    // //parse numbers bin
    // const N64Bin numbers_file(numbers_fileName);
    // const BKSprite numbers(static_cast<n64_span>(numbers_file));

    // //parse texture bin
    // const N64Bin texture_file(texture_fileName);
    // const BKSprite texture(static_cast<n64_span>(texture_file));

    // apng apngy;
    // //apngy.set_still(canvas);
    // std::cout << texture.Frame.size() <<std::endl;
    // for(auto& frame : texture.Frame){
    //     apngy.add_frame(frame.Data, frame.w, frame.h);
    // }
    // apngy.set_dimension(texture.Frame[0].w, texture.Frame[0].h);
    // apngy.write("out/" + file_id + ".apng");

   

    // //texture letters
    // auto Letters = texturize(alphaMask, texture);
    // auto Numbers = texturize(numbers, texture);

    // //map to ascii
    // std::vector<BKSpriteFrameChunk*> Ascii(0x80, nullptr);
    // for(char i = 0; i < 9; i++){
    //     Ascii['0' + i] = &Numbers[i];
    // }
    
    // for(char i = 1; i < 27; i++){
    //     Ascii['A' + (i-1)] = &Letters[i];
    // }
    // Ascii[':'] = &Letters[0];
    // Ascii['?'] = &Letters[29];
    // Ascii['('] = &Letters[30];
    // Ascii[')'] = &Letters[31];
    // Ascii['<'] = &Letters[32];
    // Ascii['>'] = &Letters[33];
    // Ascii['"'] = &Letters[34];
    // Ascii['.'] = &Letters[35];
    // Ascii[';'] = &Letters[36];
    // Ascii['-'] = &Letters[37];
    // Ascii['!'] = &Letters[38];
    // Ascii['/'] = &Letters[39];
    // Ascii['\''] = &Letters[40];

    // //generate canvas
    // std::vector<int> x_pos(print_string.size(),0);
    // uint32_t width = (print_string[0] == ' ')? 12 : Ascii[print_string[0]]->w;
    // uint32_t height = Letters[0].h;
    // uint32_t lW = width;
    // uint32_t prev_lW = width;
    // for(int i = 1; i <print_string.size(); i++){
    //     prev_lW = lW;
    //     lW = (print_string[i] == ' ')? 12 : Ascii[print_string[i]]->w;
    //     x_pos[i] = width - (prev_lW/6 + lW/6);
    //     width = x_pos[i] + lW;
    // }
    
    // std::vector<rgba_t> canvas(width*height, {0,0,0,0});
    // for(int i = print_string.size() - 1; i >= 0; i--){
    //     if((print_string[i] != ' '))
    //     {
    //         int iW = Ascii[(char) print_string[i]]->w;
            
    //         for(int iX = 0; iX < iW; iX++){
    //             for(int iY = 0; iY < Ascii[print_string[i]]->h; iY++){
    //                 rgba_t pxl = Ascii[print_string[i]]->Data[iX + iY*iW];
    //                 float alpha = pxl.a/0xff;
    //                 canvas[x_pos[i]+iX + iY*width].r = (float)canvas[x_pos[i]+iX + iY*width].r*(1.0f - alpha);
    //                 canvas[x_pos[i]+iX + iY*width].r += (float)pxl.r*alpha;
    //                 canvas[x_pos[i]+iX + iY*width].g = (float)canvas[x_pos[i]+iX + iY*width].g*(1.0f - alpha);
    //                 canvas[x_pos[i]+iX + iY*width].g += (float)pxl.g*alpha;
    //                 canvas[x_pos[i]+iX + iY*width].b = (float)canvas[x_pos[i]+iX + iY*width].b*(1.0f - alpha);
    //                 canvas[x_pos[i]+iX + iY*width].b += (float)pxl.b*alpha;
    //                 canvas[x_pos[i]+iX + iY*width].a = (float)canvas[x_pos[i]+iX + iY*width].a*(1.0f - alpha);
    //                 canvas[x_pos[i]+iX + iY*width].a += (float)pxl.a*alpha;
    //             }
    //         }
    //     }
    // } 


    // //write canvas to apng
    // apng apngy;
    // //apngy.set_still(canvas);
    // apngy.add_frame(canvas, width, height);
    // apngy.set_dimension(width, height);
    // apngy.write("assets/" + print_string + ".png");
    

    return 0;//*/
}

