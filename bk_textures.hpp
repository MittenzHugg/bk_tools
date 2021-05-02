#pragma once
#include <string>
#include "n64_span.h"
#include "gif.hpp"
#include "apng.hpp"


class bk_ci8{
    public:
        bk_ci8(const n64_span& span); 
        uint32_t _width;
        uint32_t _height;
        std::vector<rgb8_t>_palette;
        std::vector<uint8_t> _pixel_data;
        uint8_t _transparent_index = 0;
};

class bk_ci4{
    public:
        bk_ci4(const n64_span& span); 
        uint32_t _width;
        uint32_t _height;
        std::vector<rgb8_t>_palette;
        std::vector<uint8_t> _pixel_data;
        uint8_t _transparent_index = 0;
        
};

class bk_rgba16{
    public:
        bk_rgba16(const n64_span& span); 
        uint32_t _width;
        uint32_t _height;
        std::vector<rgba32_t> _pixel_data;
        uint8_t _transparent_index = 0;
        
};

class bk_rgba32{
    public:
        bk_rgba32(const n64_span& span, bool explode); 
        uint32_t _width;
        uint32_t _height;
        std::vector<rgba32_t> _pixel_data;
        uint8_t _transparent_index = 0;
};