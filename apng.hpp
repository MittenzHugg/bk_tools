#ifndef APNG_H
#define APNG_H


#include <cstdint>
#include <array>
#include <vector>
#include <string>

#include "n64_span.h" //big-endian handler
#include "libdeflate.h" //libdeflate_zlib_compress()

enum APNG_DISPOSE_OP{
    NONE,
    BACKGROUND,
    PREVIOUS
};

enum APNG_BLEND_OP{
    SOURCE,
    OVER
};

struct rgba32_t
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

class apng{
    public:
        inline void set_dimension(uint16_t w, uint16_t h){
            _IHDR.width = w;
            _IHDR.height = h;
        };

        int add_frame(const std::vector<rgba32_t>& img, uint16_t w, uint16_t h);

        inline void set_loop_count(uint32_t cnt){
            _acTL.num_plays = cnt;
        }

        inline void set_still(const std::vector<rgba32_t>& img){
            _IDAT.data = img;
            _still = true;
        }

        inline void set_delay(uint16_t num, uint16_t den = 0){
            for(auto &_f:_frames ){
                _f.ctl.delay_num = num;
                _f.ctl.delay_den = den;
            }
            _delay_num = num;
            _delay_den = den;
        }

        inline void set_frame_delay(int frame_indx, uint16_t num, uint16_t den = 0){
            _frames[frame_indx].ctl.delay_num = num;
            _frames[frame_indx].ctl.delay_den = den;
        }

        inline void set_frame_position(int frame_indx, uint32_t x, uint32_t y){
            _frames[frame_indx].ctl.x_offset = x;
            _frames[frame_indx].ctl.y_offset = y;
        }
        inline int frame_count(void){return _frames.size();}

        void write(std::string file_path);

    private:
        const std::array<char, 8> _signature = {'\x89','P','N', 'G', '\x0D', '\x0A', '\x1A', '\x0A'};
        static uint32_t _crc(const n64_span& span);
        static void chunk_write(std::ofstream &of, const n64_span& span);
        class IHDR{
            public:
                void write(std::ofstream& of);
                const std::array<char, 4> type = {'I', 'H', 'D', 'R'};
                uint32_t width = 0;
                uint32_t height = 0;
                uint8_t  bit_depth = 8;
                uint8_t  color_type = 6; //true_color
                uint8_t  compression_method = 0; //zlib
                uint8_t  filter_method = 0; //deflate
                uint8_t  interlace_method = 0; //none
        } _IHDR;

        struct acTL{ //animation contrl
            void write(std::ofstream& of);
            const std::array<char, 4> type = {'a', 'c', 'T', 'L'};
            uint32_t num_frames;
            uint32_t num_plays = 0; //infinite
            uint32_t crc;
        } _acTL;

        struct fcTL{ //frame contrl
            void write(std::ofstream& of, int &seq_num);
            const std::array<char, 4> type = {'f', 'c', 'T', 'L'};
            uint32_t sequence_number;
            uint32_t width;
            uint32_t height;
            uint32_t x_offset=0;
            uint32_t y_offset=0;
            uint16_t delay_num=1; //numerator
            uint16_t delay_den =15; //den 0=100
            uint8_t dispose_op = static_cast<uint8_t>(APNG_DISPOSE_OP::NONE);
            uint8_t blend_op= static_cast<uint8_t>(APNG_BLEND_OP::SOURCE);
        };

        struct fdAT{ //frame data
            void write(std::ofstream& of, uint32_t w, int &seq_num);
            std::array<char, 4> type = {'f', 'd', 'A', 'T'};
            std::vector<rgba32_t> data;
        };

        struct _apng_frame{
            inline void write(std::ofstream& of, int& seq_num){ctl.write(of, seq_num); data.write(of, ctl.width, seq_num);}
            fcTL ctl;
            fdAT data;
        };

        struct IEND{
            void write(std::ofstream& of);
            const std::array<char, 4> type = {'I', 'E', 'N', 'D'};
        }_IEND;

       fdAT _IDAT;
       std::vector<_apng_frame> _frames;
       uint16_t _delay_num = 3;
       uint16_t _delay_den = 0;
       bool _still = false;

};

#endif