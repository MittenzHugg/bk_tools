#include "bk_sprite.hpp"
#include "bk_textures.hpp"
#include "gif.hpp"
#include <algorithm>
#include <iostream>

bk_sprite::bk_sprite(const n64_span& span){
    _frame_cnt = span.get<uint16_t>();
    std::cout << "frames: " << _frame_cnt << std::endl;
    _format = static_cast<sprite_txtr_frmt>(span.get<uint16_t>(2));
    _unk4_unk10 = span.slice(0x04, 0x10-0x04);
    _frame_adr = span.to_vector<uint32_t>(0x10,_frame_cnt);
    //seperate header from image data
    _data = span.slice(0x10 + _frame_cnt*sizeof(uint32_t));
    _frame_adr.push_back(_data.size());
    for(int i = 0; i<_frame_adr.size()-1; i++){
        _frame_data.push_back(_data.slice(_frame_adr[i], _frame_adr[i+1]-_frame_adr[i]));
    }
    if(_format == CI4){
        std::cout << "format: CI4" << std::endl;
    } else if(_format == CI8){
        std::cout << "format: CI8" << std::endl;
    } else if(_format == RGBA16){
        std::cout << "format: RGBA16 " << std::endl;
    } else if(_format == RGBA32){
        std::cout << "format: RGBA32 " << std::endl;
    } else {
        std::cout << "unknownformat: " << _format << std::endl;
    }
}

gif bk_sprite::toGIF(void){
    gif giffy;
    uint16_t alpha = 0;
    uint32_t max_height = 0;
    uint32_t max_width = 0;
    if(_format == CI4){
        
        for(n64_span& _f_span : _frame_data){
            bk_ci4 tmp(_f_span);
            int curr_frame = giffy.add_frame(tmp._pixel_data, tmp._height, tmp._width);
            giffy.set_frame_palette(curr_frame,tmp._palette);
            alpha = tmp._transparent_index;
            giffy.set_frame_transparency(curr_frame,tmp._transparent_index);
            max_width = std::max(max_width, tmp._width);
            max_height = std::max(max_height, tmp._height);
        }
    } else if(_format == CI8){
        for(n64_span& _f_span : _frame_data){
            bk_ci8 tmp(_f_span);
            int curr_frame = giffy.add_frame(tmp._pixel_data, tmp._height, tmp._width);
            giffy.set_frame_palette(curr_frame,tmp._palette);
            alpha = tmp._transparent_index;
            giffy.set_frame_transparency(curr_frame,tmp._transparent_index);
            max_width = std::max(max_width, tmp._width);
            max_height = std::max(max_height, tmp._height);
        }
    } else{
        std::cout << "format: unsupported " << _format << std::endl;
        return giffy;
    }
    giffy.set_background_color(alpha);
    giffy.set_dimension(max_height, max_width);
    return giffy;
}

apng bk_sprite::toAPNG(void){
    apng giffy;
    uint16_t alpha = 0;
    uint32_t max_height = 0;
    uint32_t max_width = 0;
    if(_format == RGBA16){
        for(n64_span& _f_span : _frame_data){
            bk_rgba16 tmp(_f_span);
            int curr_frame = giffy.add_frame(tmp._pixel_data, tmp._width, tmp._height);
            max_width = std::max(max_width, tmp._width);
            max_height = std::max(max_height, tmp._height);
        }
    }
    if(_format == RGBA32){
        for(n64_span& _f_span : _frame_data){
            bk_rgba32 tmp(_f_span);
            int curr_frame = giffy.add_frame(tmp._pixel_data, tmp._width, tmp._height);
            max_width = std::max(max_width, tmp._width);
            max_height = std::max(max_height, tmp._height);
        }
    }
    giffy.set_dimension(max_width, max_height);
    return giffy;
}