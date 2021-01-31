#pragma once
#include <cstdint>
#include <array>
#include <vector>
#include <string>

struct rgb8_t
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

class gif{
    public:
        inline void set_dimension(uint16_t h, uint16_t w){
            _screen_desc.width = w; 
            _screen_desc.height = h;
        };

        inline void set_global_palette(const std::vector<rgb8_t>& pal){
            _g_color_tbl = pal;
            //set g_tbl_size
            _screen_desc.g_color_tbl_flag = 1;
            _screen_desc.g_color_tbl_size = 0;
            uint32_t max_val = 2;
            while(max_val < pal.size() && _screen_desc.g_color_tbl_size < 7){
                max_val <<= 1;
                _screen_desc.g_color_tbl_size++;
            }
        }

        int  add_frame(const std::vector<uint8_t>& img, uint16_t h, uint16_t w);

        inline void set_frame_palette(int frame_indx, const std::vector<rgb8_t>& pal){
            _frames[frame_indx].img.l_color_tbl = pal;
            _frames[frame_indx].img.l_color_tbl_flag = 1;
            _frames[frame_indx].img.l_color_tbl_size = 0;
            uint32_t max_val = 2;
            while(max_val < pal.size() && _frames[frame_indx].img.l_color_tbl_size < 7){
                max_val <<= 1;
                _frames[frame_indx].img.l_color_tbl_size++;
            }
        }

        inline void set_infinite_loop(bool val){
            _infite_loop = val;
        }

        inline void set_background_color(int color_indx){
            _screen_desc.bgnd_color_indx = color_indx;
        }

        inline void set_delay(uint16_t delay){
            for(auto &_f : _frames){
                if(_f.gfx_ctl == NULL)
                    _f.gfx_ctl = new _graphic_control_extension;
               _f.gfx_ctl->delay_time = delay;
            }
        }

        inline void set_frame_delay(int frame_indx, uint16_t delay){
            if(_frames[frame_indx].gfx_ctl == NULL)
                _frames[frame_indx].gfx_ctl = new _graphic_control_extension;
            _frames[frame_indx].gfx_ctl->delay_time = delay;
        }

        inline void set_frame_position(int frame_indx, uint16_t left_pad, uint16_t top_pad){
            _frames[frame_indx].img.left = left_pad;
            _frames[frame_indx].img.top = top_pad;
        }

        inline void set_frame_transparency(int frame_indx, uint8_t color_indx){
            /*if(_frames[frame_indx].gfx_ctl == NULL)
                _frames[frame_indx].gfx_ctl = new _graphic_control_extension;
            _frames[frame_indx].gfx_ctl->transparent_color_flag = 1;
            _frames[frame_indx].gfx_ctl->transparent_color_index = color_indx;*/
        }

        inline int frame_count(void){ 
            return _frames.size(); 
        }

        void write(std::string file_path);

    
        std::vector<uint8_t> _compress(uint16_t start_val, const std::vector<uint8_t>& in);
        private:
        
        class _image_descriptor{
            public:
                const char separator = 0x2C;
                uint16_t left = 0;
                uint16_t top = 0;
                uint16_t width;
                uint16_t height;
                uint8_t  l_color_tbl_size:3 = 0;
                uint8_t  res:2 = 0;
                uint8_t  sort_flag:1 = 0;
                uint8_t  interlaced_flag:1 = 0;
                uint8_t  l_color_tbl_flag:1 = 0;
                //local color table
                std::vector<rgb8_t> l_color_tbl;
                //Table-based image data
                uint8_t lzw_min_code_size;
                std::vector<uint8_t> image_data;
                const char block_terminator = 0;
        };
        
        struct _graphic_control_extension{
            public:
                const char ext_introducer = 0x21;
                const char gfx_ctl_label = 0xF9;
                const char block_size = 0x04;
                uint8_t transparent_color_flag:1 = 0;
                uint8_t user_input_flag:1 = 0;
                uint8_t disposal_method:3 = 2;
                uint8_t reserved:3 = 0;
                uint16_t delay_time = 3; //30 fps
                uint8_t transparent_color_index = 0;
                const char block_terminator = 0;
        };

        class _gif_frame{
            public:
            //~_gif_frame(void){delete gfx_ctl;}
            _graphic_control_extension* gfx_ctl = NULL;
            _image_descriptor img;
        };

        struct header{
            const std::array<char, 3> sig = {'G','I','F'};
            const std::array<char, 3> ver = {'8','9','a'};
        } _header;
        
        struct logical_screen_decriptor{
            uint16_t width;
            uint16_t height;
            uint8_t g_color_tbl_size:3 = 0;
            uint8_t sort_flag:1 = 0;
            uint8_t color_resolution:3 = 7;
            uint8_t g_color_tbl_flag:1 = 0;
            uint8_t bgnd_color_indx = 0;
            uint8_t pxl_aspect_ratio = 0;
        } _screen_desc;
        bool _infite_loop = true;
        uint16_t _default_delay = 3;
        std::vector<rgb8_t> _g_color_tbl;
        std::vector<_gif_frame> _frames;
        const char _trailer = 0x3B;
};