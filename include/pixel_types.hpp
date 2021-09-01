#pragma once
#include <cstdint>
#include "n64/n64_span.hpp"

#define SHIFT_R(x,s,b) (((x) >> (s)) & ((1 << (b))-1))

struct rgba_t{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct i4_t{
    i4_t(const n64_span& span, bool lower = false){
        i = span(0);
        if(lower = false)
            i = i >> 4;
        i &= 0xF;
    };
    inline operator rgba_t() const{ return {((i << 4) | i),((i << 4) | i),((i << 4) | i),0xFF};}

    uint8_t i = 0;
};

struct i8_t{
    i8_t(const n64_span& span){
        i = span(0);
    };
    inline operator rgba_t() const{ return {i, i, i, 0xFF};}

    uint8_t i = 0;
};

struct rgba16_t{
    rgba16_t(){};
    rgba16_t(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a){};
    rgba16_t(const n64_span& n64Span){
        uint16_t val = n64Span;
        r = SHIFT_R(val, 11, 5);
        g = SHIFT_R(val, 6, 5);
        b = SHIFT_R(val, 1, 5);
        a = SHIFT_R(val, 0, 1);

        r = (r << 3) | (r >>  2); //5 bits to 8 bits
        g = (g << 3) | (g >>  2); //5 bits to 8 bits
        b = (b << 3) | (b >>  2); //5 bits to 8 bits
        a = (a) ? 0xFF :  0 ;     //1 bit to 8 bits
    };
    inline operator rgba_t() const{ return {r,g,b,a};}
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 0;
};

struct rgba32_t{
    rgba32_t(){};
    rgba32_t(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a){};
    rgba32_t(const n64_span& n64Span){
        r = n64Span(0);
        g = n64Span(1);
        b = n64Span(2);
        a = n64Span(3);
    };
    inline operator rgba_t() const{ return {r,g,b,a};}
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 0;
};