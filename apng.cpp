#include <map>
#include <iostream>
#include <fstream>
#include <iterator>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <array>
#include "apng.hpp"
#include "libdeflate.h"

int apng::add_frame(const std::vector<rgba_t>& img, uint16_t w, uint16_t h){
    _apng_frame tmp;
    tmp.ctl.width = w;
    tmp.ctl.height = h;
    tmp.data.data = img;
    _frames.push_back(tmp);
    _acTL.num_frames = _frames.size();
    return _frames.size() - 1;
}

void apng::write(std::string file_path){
    std::cout << "apng::write()" << std::endl << std::flush;
    std::ofstream of(file_path, std::ios::out | std::ios::binary);
    of.write(&_signature[0], _signature.size());
    _IHDR.write(of);
    _acTL.write(of);
    
    //write still or configure first from
    if(_still){
        _IDAT.type = {'I','D','A','T'};
        int tmp;
        _IDAT.write(of, _IHDR.width, tmp);
    } else {
        _frames[0].data.type = {'I','D','A','T'};
    }

    int seq_num = 0;
    for(auto& frm : _frames){
        frm.write(of, seq_num);
    }
    //_frames[0].write(of);
    
    _IEND.write(of);
    of.close();
    return;
}

uint32_t apng::_crc(const n64_span& span){
    uint32_t crc = 0xFFFFFFFF;
    std::vector<uint8_t> bytes = span.get<std::vector<uint8_t>>(span.size()/sizeof(uint8_t));
    for( uint8_t b : bytes){
        uint32_t code = (crc ^ (uint32_t) b) & 0xff;
        for(int i = 0; i< 8; i++){
           code = (code & 1) ? (0xedb88320L ^ (code >> 1)) :  (code >> 1);
        }
        crc = code ^ (crc >> 8);
    }
    return crc;
}

void apng::chunk_write(std::ofstream &of, const n64_span& span){
    uint8_t* tmp = (uint8_t*) malloc(4 + span.size() + 4);
    n64_span tmp_s(tmp, 4 + span.size() + 4);
    tmp_s.set<uint32_t>(span.size() - 4, 0); //size

    std::copy_n(span.begin(), span.size(), &tmp_s[4]);//type and data

    uint32_t crc = ~_crc(span);
    tmp_s.set(crc, 4 + span.size());//crc

    of.write((char*)tmp_s.begin(), tmp_s.size());
    free(tmp);
}

void apng::IHDR::write(std::ofstream& of){
    std::cout << "apng::IHDR::write()" << std::endl << std::flush;
    uint8_t* tmp = (uint8_t*) malloc( 4 + 0xD);
    n64_span tmp_s(tmp, 4 + 0xD);
    std::copy_n(type.begin(), type.size(), tmp_s.begin());
    uint32_t offset = 4;
    tmp_s.seq_set(width, offset);
    tmp_s.seq_set(height, offset);
    tmp_s.seq_set(bit_depth, offset);
    tmp_s.seq_set(color_type, offset);
    tmp_s.seq_set(compression_method, offset);
    tmp_s.seq_set(filter_method, offset);
    tmp_s.seq_set(interlace_method, offset);
    chunk_write(of, tmp_s);
    free(tmp);
}

void apng::acTL::write(std::ofstream& of){
    std::cout << "apng::acTL::write()" << std::endl << std::flush;
    uint8_t* tmp = (uint8_t*) malloc( 4 + 0x8);
    n64_span tmp_s(tmp, 4 + 0x8);
    std::copy_n(type.begin(), type.size(), tmp_s.begin());
    uint32_t offset = 4;
    tmp_s.seq_set(num_frames, offset);
    tmp_s.seq_set(num_plays, offset);
    chunk_write(of, tmp_s);
    free(tmp);
}

void apng::fcTL::write(std::ofstream& of, int& seq_num){
    std::cout << "apng::fcTL::write()" << std::endl << std::flush;
    uint8_t* tmp = (uint8_t*) malloc( 4 + 0x1A);
    n64_span tmp_s(tmp, 4 + 0x1A);
    std::copy_n(type.begin(), type.size(), tmp_s.begin());
    uint32_t offset = 4;
    tmp_s.seq_set<uint32_t>(seq_num, offset);
    seq_num++;
    tmp_s.seq_set(width, offset);
    tmp_s.seq_set(height, offset);
    tmp_s.seq_set(x_offset, offset);
    tmp_s.seq_set(y_offset, offset);
    tmp_s.seq_set(delay_num, offset);
    tmp_s.seq_set(delay_den, offset);
    tmp_s.seq_set(dispose_op, offset);
    tmp_s.seq_set(blend_op, offset);
    chunk_write(of, tmp_s);
    free(tmp);
}

void apng::fdAT::write(std::ofstream& of, uint32_t w, int& seq_num){
    //data to std::vector;
    std::cout << "apng::fdAT::write()" << std::endl << std::flush;

    std::cout << "pixels to uint8_t[] " << data.size() << std::endl << std::flush;
    uint8_t * ucomp = (uint8_t*) malloc(data.size() *4 + data.size()/w);
    int j = 0;
    for(int i = 0; i < data.size(); i++){
        if((i % w) == 0){
            ucomp[j + i*4] = 0;
            j++;
        }
        ucomp[j + i*4] = data[i].r;
        ucomp[j + i*4 + 1] = data[i].g;
        ucomp[j + i*4 + 2] = data[i].b;
        ucomp[j + i*4 + 3] = data[i].a;
    }
    
    //compress
    libdeflate_compressor * comper = libdeflate_alloc_compressor(6);
    uint8_t * comp_data = (uint8_t*) malloc(data.size() *4);
    size_t outSize = libdeflate_zlib_compress(comper, ucomp, data.size()*4, comp_data, data.size()*4);
    libdeflate_free_compressor(comper);
    free(ucomp);
    bool is_frame = !(type == std::array<char, 4>{'I','D','A','T'});
    int data_offset = (is_frame)? 8 : 4 ;
    uint8_t * tmp = (uint8_t*) malloc(outSize + data_offset);
    n64_span tmp_s(tmp, outSize + data_offset);
    std::copy_n(type.begin(), type.size(), tmp_s.begin());
    if(is_frame){
        tmp_s.set<uint32_t>(seq_num,4);
        seq_num++;
    }
    std::copy_n(comp_data, outSize, tmp + data_offset);
    free(comp_data);

    chunk_write(of, tmp_s);
    free(tmp);
}

void apng::IEND::write(std::ofstream& of){
    std::cout << "apng::IEND::write()" << std::endl << std::flush;

    n64_span tmp_s((uint8_t *) &type[0], 4);
    chunk_write(of, tmp_s);
}
