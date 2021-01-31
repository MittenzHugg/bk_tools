#include <vector>
#include "gif.hpp"

int main(void){
    std::vector<uint8_t> in = {3, 0, 1, 2, 3, 0, 1, 3, 3, 1, 2, 3};
    std::vector<rgb8_t> pal = {{0,0,0},{0xFE,0,0},{0x00,0xFF,0xFF},{0xFF,0xFF,0xFF}};
    gif giffer;
    giffer.set_dimension(1,12);
    giffer.set_global_palette(pal);
    giffer.set_background_color(3);
    int curr_frame = giffer.add_frame(in,1,12);
    giffer.set_frame_transparency(curr_frame, 3);
    giffer.write("out_sample.gif");
    return 0;
}
