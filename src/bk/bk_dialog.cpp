#include "bk/bk_dialog.hpp"

bk_dialog::bk_dialog(const n64_span& span){
    uint32_t offset = 0;
    bottom_length = span(offset++);
    bottom = span(offset).get<std::vector<bk_dialog_string>>(bottom_length);
    offset += n64_size_finder(bottom);
    top_length = span(offset++);
    top = span(offset).get<std::vector<bk_dialog_string>>(top_length);
};

void bk_dialog::n64_out(n64_span& out){
        bottom_length = bottom.size();
        top_length = top.size();
        out.set(bottom_length, bottom, top_length, top);
    };