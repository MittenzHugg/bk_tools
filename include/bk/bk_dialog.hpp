#ifndef __BK_DIALOG_H__
#define __BK_DIALOG_H__

#include "n64/n64_span.hpp"
#include <string>

class bk_dialog_string{
  public:
    bk_dialog_string() = default;
    bk_dialog_string(const uint8_t cmd, const std::string& str = ""): command(cmd), text(str) {};
    bk_dialog_string(const n64_span& span){
        span.get(command, length);
        text= span.get(2);
    };
    inline std::size_t n64_size(){
        return n64_size_finder(command, length) + text.size() + 1;
    };
    uint8_t command = 0x04;
    std::string text = "";

    inline void n64_out(n64_span& out){
        length = text.size() + 1;
        out.set(command, length, text);
    };
  private:
    uint8_t length = 1;
};

class bk_dialog{
  public:
    bk_dialog() = default;
    bk_dialog(const n64_span& span);
    inline std::size_t n64_size(){
        return n64_size_finder(bottom_length, bottom, top_length, top);
    };
    void n64_out(n64_span& out);

    std::vector<bk_dialog_string> bottom = {};
    std::vector<bk_dialog_string> top = {};
  private:
    uint8_t bottom_length = 0;
    uint8_t top_length = 0;
};

class bk_dialog_file{
  public:
    bk_dialog_file() = default;
    N64_SIMPLE_STRUCT(bk_dialog_file, unk0, unk1, unk2, dialog);
    N64_SIMPLE_FILE(bk_dialog_file, unk0, unk1, unk2, dialog);

    uint8_t unk0 = 0x01;
    uint8_t unk1 = 0x03;
    uint8_t unk2 = 0x00;
    class bk_dialog dialog;
};

#endif