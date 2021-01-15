#include "bk_asm.hpp"
#include <iostream>
#include <iomanip>

bk_asm::bk_asm(const n64_span& span){
    _comp_span = new n64_span(span);
}

bk_asm::~bk_asm(void){
    if(_comp_buff != nullptr){
        free(_comp_buff);
        _comp_buff = nullptr;
    }
    if(_code_buff != nullptr){
        free(_code_buff);
        _code_buff = nullptr;
    }
    if(_data_buff != nullptr){
        free(_data_buff);
        _data_buff = nullptr;
    }

    if(decomper != nullptr)
        free(decomper);
        decomper = nullptr;

    if(_comp_span != nullptr){
        free(_comp_span);
        _comp_span = nullptr;
    }
    if(_code_span != nullptr){
        free(_code_span);
        _code_span = nullptr;
    }
    if(_data_span != nullptr){
        free(_data_span);
        _data_span = nullptr;
    }
}

const n64_span& bk_asm::code(void) const{
    if(_code_span == nullptr){
        if(_comp_span == nullptr)
            throw "Asm file never initialized";

        _decomp_method();

        if(_code_span == nullptr)
            throw "Unable to decompress file";
    }
    return *_code_span;
}

const n64_span& bk_asm::data(void) const{
    if(_data_span == nullptr){
        if(_comp_span == nullptr)
            throw "Asm file never initialized";

        _decomp_method();

        if(_data_span == nullptr)
            throw "Unable to decompress file";
    }
    return *_data_span;
}

void bk_asm::_decomp_method(void) const{
    if(decomper == nullptr)
        decomper = libdeflate_alloc_decompressor();

    size_t code_comp_size = _comp_span->size();
    
    size_t tmpCompSize;
    size_t codeSize = 0;
    size_t dataSize = 0;
    size_t codeCompSize;
    size_t dataCompSize;

	//decompress Code
    size_t code_size = _comp_span->get<uint32_t>(2); 
    _code_buff = (uint8_t*)malloc(code_size);
    enum libdeflate_result decompResult = libdeflate_deflate_decompress_ex( decomper, 
        _comp_span->begin() + 6, _comp_span->size() - 6, 
        _code_buff, code_size, &codeCompSize, &codeSize
    );
    _code_span = new n64_span(_code_buff, codeSize);
    std::cout << std::hex << "code_comp_size " << (uint32_t) codeCompSize <<std::endl;
    
    //decompress data
    codeCompSize +=6;
    const n64_span& data_cmp_span = _comp_span->slice(codeCompSize, _comp_span->size() - codeCompSize);
    size_t data_size = _comp_span->get<uint32_t>(codeCompSize + 2);
    _data_buff = (uint8_t*)malloc(data_size); 
    decompResult = libdeflate_deflate_decompress_ex( decomper, 
        data_cmp_span.begin() + 6, 
        data_cmp_span.size() - 6, 
        _data_buff, data_size, &dataCompSize, &dataSize
    );
    _data_span = new n64_span(_data_buff, dataSize);

    return;
}
