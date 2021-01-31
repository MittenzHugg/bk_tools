#include <map>
#include <iostream>
#include <fstream>
#include <iterator>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <array>
#include "gif.hpp"



        
int gif::add_frame(const std::vector<uint8_t>& img, uint16_t h, uint16_t w){
    _gif_frame tmp;
    tmp.img.width = w;
    tmp.img.height = h;
    tmp.img.image_data = img;
    _frames.push_back(tmp);
    return _frames.size()-1;
}

void gif::write(std::string file_path){
    std::ofstream of(file_path, std::ios::out | std::ios::binary);
    of.write((char*)&_header, 6);
    of.write((char*)&_screen_desc, 7);
    //write global palette
    if(_screen_desc.g_color_tbl_flag){
        
        for(const auto& color : _g_color_tbl){
            of.write((char*)&color, sizeof(rgb8_t));
        }
    }
    //write loop
    const std::array<uint8_t, 19> netscape_loop= {'!', 0xff, 0x0b, 'N','E','T','S','C','A','P','E','2','.','0', 0x03, 0x01, 0x00, 0x00, 0x00};
    if(_infite_loop)
        of.write((char*) &netscape_loop[0], netscape_loop.size());

    //write each frame
    for(auto& frame: _frames){
        //graphic ctl extension
        if(frame.gfx_ctl != NULL){
            of.write((char *) frame.gfx_ctl, 8);
        }

        //image_descriptor
        of.write((char *) &frame.img.separator, 1);
        of.write((char *) &frame.img.left, 4*sizeof(uint16_t) + 1);
        //write local palette
        if(frame.img.l_color_tbl_flag){
            for(const auto& color : frame.img.l_color_tbl){
                of.write((char*)&color, sizeof(rgb8_t));
            }
        }
        //write image data
        frame.img.lzw_min_code_size = (frame.img.l_color_tbl_flag)? frame.img.l_color_tbl_size : _screen_desc.g_color_tbl_size;
        frame.img.lzw_min_code_size++;
        of.write((char *) &frame.img.lzw_min_code_size, 1);
        auto image_vec = _compress((1 << frame.img.lzw_min_code_size)-1, frame.img.image_data);
        uint8_t subblock_size = 0;
        for(auto image_it = image_vec.begin(); image_it != image_vec.end(); image_it += subblock_size){
            subblock_size = std::min(0x3F,(int) std::distance(image_it, image_vec.end()));
            of.write((char *)&subblock_size, 1);
            of.write((char*)&(*image_it), subblock_size);
        }
        of.write((char *)&frame.img.block_terminator, 1);
    }
    of.write((char*)&_trailer,1);
    of.close();
    return;
}

std::vector<uint8_t> gif::_compress(uint16_t tbl_max, const std::vector<uint8_t>& in){
    std::vector<uint8_t> out;
    
    //push words to tmp_out(upper)
    uint32_t tmp_out = 0;
    int r_bits = -8;
    int l_bits = 0;

    //generate clear and end words
    uint16_t clear_word = 1;
    int bit_count = 1;
    while(clear_word < tbl_max){
        clear_word = clear_word << 1;
        bit_count++;
    }
    uint32_t end_word = clear_word + 1;
    uint32_t next_val = end_word + 1;

    //seed library
    std::map<std::vector<uint8_t>, uint32_t> dictionary;
    for(int i = 0; i < clear_word; i++){
        std::vector<uint8_t> tmp_key = {i};
        dictionary.insert_or_assign(tmp_key, i);
    }

    tmp_out = static_cast<uint32_t>(clear_word) << 8;
    for(int l_bits = bit_count; l_bits > 0; l_bits--){
        tmp_out = tmp_out >> 1;
        r_bits++;
        if(r_bits == 0){
            out.push_back(static_cast<uint8_t>(tmp_out & 0x000000FF));
            r_bits -= 8;
        }
    }
    
    auto position = in.begin();
    auto next_position = position + 1;
    auto look_ahead = position + 1;    
    auto in_end = in.end();

    while(position != in_end){
        //find next largest word
        std::vector<uint8_t> next_word(position, look_ahead);
        while(dictionary.contains(next_word) &&  look_ahead != in_end){
            next_position = look_ahead;
            look_ahead++;
            next_word = std::vector<uint8_t>(position, look_ahead);
        }
        std::vector<uint8_t> current_word = std::vector<uint8_t>(position, next_position);
        
        tmp_out = (tmp_out & 0x000000FF) | (dictionary.at(current_word) << 8);
        for(int l_bits = bit_count; l_bits > 0; l_bits--){
            tmp_out = tmp_out >> 1;
            r_bits++;
            if(r_bits == 0){
                out.push_back(static_cast<uint8_t>(tmp_out & 0x000000FF));
                r_bits -= 8;
            }
        }

        dictionary.insert_or_assign(next_word, next_val);
        if(next_val == (0x01 << bit_count)){
            bit_count++;
            if(bit_count > 12){
                std::cout << "exceeds bit count value" << std::endl;
            }
        }

        next_val++;
        position = next_position;
        next_position = position + 1;
        look_ahead = position + 1;
    }

    tmp_out = (tmp_out & 0x000000FF) | (end_word << 8);
    for(int l_bits = bit_count; l_bits > 0; l_bits--){
        tmp_out = tmp_out >> 1;
        r_bits++;
        if(r_bits == 0){
            out.push_back(static_cast<uint8_t>(tmp_out & 0x000000FF));
            r_bits -= 8;
        }
    }

    tmp_out = (tmp_out & 0x000000FF);
    while(r_bits > -8){
        tmp_out = tmp_out >> 1;
        r_bits++;
        if(r_bits == 0){
            out.push_back(static_cast<uint8_t>(tmp_out & 0x000000FF));
        }
    }

    return out;

}