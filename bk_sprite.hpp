#pragma once
#include "n64/n64_span.hpp"
#include "gif.hpp"
#include "apng.hpp"

enum sprite_txtr_frmt{
    CI4 = (1 << 0),
    CI8 = (1 << 2),
    RGBA16 = (1 << 10),
    RGBA32 = (1 << 11)
};

class bk_sprite{
    public:
        bk_sprite(const n64_span& span);
        gif toGIF(bool explode);
        apng toAPNG(bool explode);
        sprite_txtr_frmt _format;
    private:
        uint16_t _frame_cnt;
        
        n64_span _unk4_unk10;
        std::vector<uint32_t> _frame_adr;
        std::vector<n64_span> _frame_data;
        n64_span _data;
};
