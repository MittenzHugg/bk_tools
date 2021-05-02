#include "bk_textures.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>

bk_ci8::bk_ci8(const n64_span& span){
    //read in pallette
    //head of 0x18

    std::vector<uint16_t>pal_raw = span.to_vector<uint16_t>(0x18, 256);
    std::transform(pal_raw.begin(), pal_raw.end(), std::back_inserter(_palette),
        [&](uint16_t val)-> rgb8_t{
            rgb8_t color;
            color.r = (val >> 8) & 0x00F8;
            color.g = (val >> 3) & 0x00F8;
            color.b = (val << 2) & 0x00F8;
            return color;
        }
    );
    for(_transparent_index = 0; _transparent_index < pal_raw.size() && (pal_raw[_transparent_index] & 0x0001); _transparent_index++);
    _width = span.get<uint16_t>(4);
    _height = span.get<uint16_t>(6);
    uint16_t _chunk_cnt = span.get<uint16_t>(8);

    _pixel_data.resize(_width*_height,_transparent_index);
    uint32_t chunk_offset = 0x18 +256*sizeof(uint16_t);
    for(int ci = 0; ci < _chunk_cnt; ci++){
        int16_t chunk_x = span.get<int16_t>(chunk_offset);
        int16_t chunk_y = span.get<int16_t>(chunk_offset + 2);
        int16_t chunk_w = span.get<int16_t>(chunk_offset + 4);
        int16_t chunk_h = span.get<int16_t>(chunk_offset + 6);
        std::vector<uint8_t> tmp = span.to_vector<uint8_t>(chunk_offset + 0x8, chunk_w*chunk_h);
    
        for(int i = 0; i < chunk_h; i++){
            auto row = tmp.begin() + i*chunk_w;
            std::copy(row, row + chunk_w, _pixel_data.begin() + (chunk_x) + (chunk_y + i)*_width);
        }
        chunk_offset += 0x8 + chunk_w*chunk_h;
    }
}

bk_ci4::bk_ci4(const n64_span& span){
    //read in pallette
    //head of 0x18

    std::vector<uint16_t>pal_raw = span.to_vector<uint16_t>(0x18, 16);
    std::transform(pal_raw.begin(), pal_raw.end(), std::back_inserter(_palette),
        [&](uint16_t val)-> rgb8_t{
            rgb8_t color;
            color.r = (val >> 8) & 0x00F8;
            color.g = (val >> 3) & 0x00F8;
            color.b = (val << 2) & 0x00F8;
            return color;
        }
    );
    for(_transparent_index = 0; _transparent_index < pal_raw.size() && (pal_raw[_transparent_index] & 0x0001); _transparent_index++);

    _width = span.get<uint16_t>(4);
    _height = span.get<uint16_t>(6);
    uint16_t _chunk_cnt = span.get<uint16_t>(8);

    _pixel_data.resize(_width*_height,_transparent_index);
    uint32_t chunk_offset = 0x18 +16*sizeof(uint16_t);
    for(int ci = 0; ci < _chunk_cnt; ci++){
        int16_t chunk_x = span.get<int16_t>(chunk_offset);
        int16_t chunk_y = span.get<int16_t>(chunk_offset + 2);
        int16_t chunk_w = span.get<int16_t>(chunk_offset + 4);
        int16_t chunk_h = span.get<int16_t>(chunk_offset + 6);
        std::vector<uint8_t> tmp0 = span.to_vector<uint8_t>(chunk_offset + 0x8, (chunk_w*chunk_h + 1)/2);
        std::vector<uint8_t> tmp;
        tmp.reserve(chunk_w*chunk_h);
        for(int i = 0; i < chunk_w*chunk_h; i++){
            tmp.push_back((i & 1) ? (tmp0[i/2] & 0x0F) : (tmp0[i/2] >> 4));
        }
        for(int i = 0; i < chunk_h; i++){
            auto row = tmp.begin() + i*chunk_w;
            std::copy(row, row + chunk_w, _pixel_data.begin() + (chunk_x) + (chunk_y + i)*_width);
        }
        chunk_offset += 0x8 + chunk_w*chunk_h;
    }
}

bk_rgba16::bk_rgba16(const n64_span& span){
    _width = span.get<uint16_t>(4);
    _height = span.get<uint16_t>(6);
    uint16_t _chunk_cnt = span.get<uint16_t>(8);
    _pixel_data.resize(_width* _height, {0x0, 0x00, 0x00,0x00});
    std::cout << " w: " << _width; 
    std::cout << " h: " << _height; 
    std::cout << std::endl << std::flush;

    uint32_t chunk_offset = 0x14;
    for(int ci = 0; ci < _chunk_cnt; ci++){
        std::cout << "chunk " << ci << ":"; 
        int16_t chunk_x = span.get<int16_t>(chunk_offset);
        int16_t chunk_y = span.get<int16_t>(chunk_offset + 2);
        int16_t chunk_w = span.get<int16_t>(chunk_offset + 4);
        int16_t chunk_h = span.get<int16_t>(chunk_offset + 6);

        chunk_offset +=8;
        while(chunk_offset & 0x07){chunk_offset++;}

        std::cout << " x: " << chunk_x; 
        std::cout << " y: " << chunk_y; 
        std::cout << " w: " << chunk_w; 
        std::cout << " h: " << chunk_h; 
        std::cout << std::endl << std::flush;
        std::vector<uint16_t>pxl_raw = span.to_vector<uint16_t>(chunk_offset, chunk_w*chunk_h);
        std::vector<rgba32_t>ch_pxls;
        ch_pxls.reserve(chunk_w*chunk_h);
        std::transform(pxl_raw.begin(), pxl_raw.end(), std::back_inserter(ch_pxls),
            [&](uint16_t val)-> rgba32_t{
                rgba32_t color;
                color.r = (val >> 8) & 0x00F8;
                color.r |= (color.r & 0x0080)? 0x07: 0x0;

                color.g = (val >> 3) & 0x00F8;
                color.g |= (color.g & 0x0080)? 0x07: 0x0;

                color.b = (val << 2) & 0x00F8;
                color.b |= (color.b & 0x0080)? 0x07: 0x0;

                color.a = (val & 0x0001)? 0xFF: 0x0;
                return color;
            }
        );
    
        //auto row = ch_pxls.begin();
        //std::copy(row, row + chunk_w, _pixel_data.begin() + (chunk_x));
        for(int i = 0; i < chunk_h; i++){
           auto row = ch_pxls.begin() + i*chunk_w;
           std::copy(row, row + chunk_w, _pixel_data.begin() + (chunk_x) + (chunk_y + i)*_width);
        }
        chunk_offset += 0x8 + chunk_w*chunk_h*sizeof(uint16_t);
    }
} 

bk_rgba32::bk_rgba32(const n64_span& span, bool explode){
    _width = explode ? 32 :span.get<uint16_t>(4);
    _height = explode ? 32 : span.get<uint16_t>(6);
    uint16_t _chunk_cnt = span.get<uint16_t>(8);
    if(explode) _width *= _chunk_cnt;
    _pixel_data.resize(_width* _height, {0x0, 0x00, 0x00,0x00});
    std::cout << " w: " << _width; 
    std::cout << " h: " << _height; 
    std::cout << std::endl << std::flush;

    uint32_t chunk_offset = 0x14;
    for(int ci = 0; ci < _chunk_cnt; ci++){
        std::cout << "chunk " << ci << ":"; 
        int16_t chunk_x = (explode) ? 32*ci : span.get<int16_t>(chunk_offset);
        int16_t chunk_y = explode ? 0 : span.get<int16_t>(chunk_offset + 2);
        int16_t chunk_w = span.get<int16_t>(chunk_offset + 4);
        int16_t chunk_h = span.get<int16_t>(chunk_offset + 6);
        
        chunk_offset +=8;
        while(chunk_offset & 0x07){chunk_offset++;}
        std::cout << " x: " << chunk_x; 
        std::cout << " y: " << chunk_y; 
        std::cout << " w: " << chunk_w; 
        std::cout << " h: " << chunk_h; 
        std::cout << std::endl << std::flush;
        std::vector<uint32_t>pxl_raw = span.to_vector<uint32_t>(chunk_offset, chunk_w*chunk_h);
        std::vector<rgba32_t>ch_pxls;
        ch_pxls.reserve(chunk_w*chunk_h);
        std::transform(pxl_raw.begin(), pxl_raw.end(), std::back_inserter(ch_pxls),
            [&](uint32_t val)-> rgba32_t{
                rgba32_t color;
                color.r = (val >> 24) & 0x00FF;
                color.g = (val >> 16) & 0x00FF;
                color.b = (val << 8) & 0x00FF;
                color.a = val & 0x00FF;
                return color;
            }
        );
    
        //auto row = ch_pxls.begin();
        //std::copy(row, row + chunk_w, _pixel_data.begin() + (chunk_x));
        for(int i = 0; i < chunk_h; i++){
           auto row = ch_pxls.begin() + i*chunk_w;
           std::copy(row, row + chunk_w, _pixel_data.begin() + (chunk_x) + (chunk_y + i)*_width);
        }
        chunk_offset += chunk_w*chunk_h*sizeof(uint32_t);
    }
} 