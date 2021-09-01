/**********************************************************
 * n64_span.h
 **********************************************************
 * N64 Span:
 *     This class represents a span of big endian memory.
 *     Think of it as a "view" of a memory section.
 *     Does not convey any ownership of the object
 * Author: Mittenz 5/15/2020
 **********************************************************/
#pragma once
#include <type_traits>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cstdint>
#include <concepts>
#include <string>
#include <fstream>
#include <iostream>

class n64_span;
class n64_bin;

/* macros */
#define N64_SIMPLE_CONSTRUCTOR(NAME, MEMBERS...) \
    NAME(const n64_span& span){\
        span.get(MEMBERS);\
    }

#define N64_SIMPLE_SIZE(MEMBERS...) \
    inline std::size_t n64_size(){\
        return n64_size_finder(MEMBERS);\
    }

#define N64_SIMPLE_OUT(MEMBERS...) \
    inline void n64_out(n64_span& out){\
        out.set(MEMBERS);\
    }

#define N64_SIMPLE_STRUCT(NAME, MEMBERS...) \
    N64_SIMPLE_CONSTRUCTOR(NAME, MEMBERS);\
    N64_SIMPLE_SIZE(MEMBERS);\
    N64_SIMPLE_OUT(MEMBERS)

#define N64_SIMPLE_FILE(NAME, MEMBERS...) \
    NAME(const std::string& filename){\
        n64_bin in_bin(filename);\
        in_bin.get(MEMBERS);\
    }\
    void write(const std::string& filename){\
        n64_bin out_bin;\
        out_bin.resize(n64_size());\
        n64_out(out_bin);\
        out_bin.write(filename);\
    }


/* concepts */
template<typename T>
concept FundamentalType = std::is_fundamental<T>::value;

template<typename T>
concept N64NativeStructType = std::is_constructible_v<T, const n64_span&> 
    && !std::is_fundamental<T>::value 
    && requires (T a) { a.n64_size();};

template<typename C>
concept N64Container = FundamentalType<typename C::value_type> || N64NativeStructType<typename C::value_type>;

template<typename T>
concept N64Constructible = FundamentalType<T> || N64NativeStructType<T>;

template<typename T>
concept N64Compatible = N64Constructible<T> || N64Container<T>;

/* supporting functions */
template<FundamentalType T> 
T n64_be_get(const n64_span& span);

template<N64NativeStructType T> 
T n64_be_get(const n64_span& span);

template<N64Container T> 
T n64_be_get(const n64_span& span, std::size_t count);

template<FundamentalType T, typename... Types>
constexpr std::size_t n64_size_finder(T arg, Types ... args){
    if constexpr(sizeof...(args) > 0)
        return sizeof(arg) + n64_size_finder(args...);
    else
        return sizeof(arg);
};

template<N64NativeStructType T, typename... Types>
constexpr std::size_t n64_size_finder(T arg, Types ... args){
    if constexpr(sizeof...(args) > 0)
        return arg.n64_size() + n64_size_finder(args...);
    else
        return arg.n64_size();
};

template<N64Container C, typename... Types>
constexpr std::size_t n64_size_finder(C container, Types ...  args){
    std::size_t out = 0;
    for(const auto& elem: container){
        out += n64_size_finder(elem);
    }
    if constexpr(sizeof...(args) > 0)
        out += n64_size_finder(args...);
    return out;
};

class n64_span
{
public:
    n64_span() = default;
    n64_span(const n64_span& src)
        :_begin(src._begin)
        , _end(src._end){}
    n64_span(uint8_t* const begin, const size_t size) 
        : _begin(begin)
        , _end(begin + size) {}

    inline uint8_t* const begin(void) const { return _begin; }
    inline uint8_t* const end(void) const { return _end; }
    inline const size_t size(void) const { return _end - _begin; }
    
    const uint8_t& operator[](int index) const { if (index >= size())exit(0); return _begin[index]; }

    uint8_t& operator[](int index) { if (index >= size())exit(0); return _begin[index]; }
 
    n64_span operator()(uint32_t i) const{ return slice(i, size() - i); }

    template <typename T>
    inline operator T() const{ return get<T>();}

    n64_span& operator=(const n64_span& other) {
        if (this != &other) {
            _begin = other._begin;
            _end = other._end;
        }
        return *this;
    }

    inline bool operator==(const n64_span& rhs) const {
        return (size() != rhs.size())
            ? false 
            : std::equal(begin(), end(), rhs.begin());
    }

    inline bool operator!=(const n64_span& rhs) const{
        return !operator==(rhs);
    }

    inline n64_span slice(uint32_t offset, size_t size = -1) const{
        size_t _size = (size == -1) ? this->size() - offset : size;
        return n64_span(_begin + offset, size);
    }

    //return fundemental types from span
    template <N64Constructible T> 
    inline T get(uint32_t offset = 0) const {
        return n64_be_get<T>(slice(offset));
    }

    template <N64Container T> 
    inline T get(uint32_t cnt, uint32_t offset = 0) const {
        return n64_be_get<T>(slice(offset), cnt);
    }

    inline std::string get(uint32_t offset = 0) const{
        return std::string(reinterpret_cast<char*>(_begin + offset));
    }

    //only invoke if 2 or more inputs
    template<N64Compatible ... Types>
    inline std::size_t get(Types& ... args) const{
        span_parser(*this, args...);
        return n64_size_finder(args...);
    }

    //write fundemental types to span
    template <FundamentalType T> 
    void set(T value, uint32_t offset = 0) {
        uint8_t size = sizeof(T);
        uint8_t* start = begin() + offset;
        T tmp = value;
        while (size) {
            size--;
            start[size] = (uint8_t) (tmp & 0x00FF);
            tmp = tmp >> 8;
        }
        return;
    }

    template <N64NativeStructType T> 
    void set(T structure, uint32_t offset = 0) {
        structure.n64_out(*this);
        return;
    }

    template <N64Container T> 
    void set(T container, uint32_t offset = 0) {
        uint32_t pos = 0;
        for(const auto& elem: container){
            slice(pos).set(elem);
            pos += n64_size_finder(elem);
        }
        return;
    }

    template<N64Compatible T, N64Compatible ... Types>
    void set(const T& arg, const Types& ... args){
        set(arg);
        if(sizeof...(args) > 0)
            slice(n64_size_finder(arg)).set(args...);
    }

    template <typename T> void seq_set(T value, uint32_t& offset){
        set(value, offset);
        offset += sizeof(T);
        return;
    }

    uint8_t* _begin = nullptr;
    uint8_t* _end = nullptr;
private:

};

template<FundamentalType T> 
T n64_be_get(const n64_span& span) {
    uint8_t size = sizeof(T);
    uint8_t count = 1;
    uint8_t* start = span.begin();
    T value = (T)(start[0]);
    while (count < size) {
        value = value << 8;
        value |= (T)start[count];
        count++;
    }
    return value;
};

template<N64NativeStructType T> 
T n64_be_get(const n64_span& span) {
    return T(span);
};

template<N64Container C> 
C n64_be_get(const n64_span& span, std::size_t count){
    C container(count);
    uint32_t pos = 0;
    for(auto& elem : container){
        elem = span(pos).get<typename C::value_type>();
        pos += n64_size_finder(elem);
    }
    return container;
};

template<N64Compatible T, N64Compatible... Types>
void span_parser(const n64_span& span, T& arg, Types& ... args){
    arg = span;
    if constexpr(sizeof...(args) > 0){
        span_parser(span(n64_size_finder(arg)), args...);
    }
};

class n64_bin : public n64_span{
    public:
        n64_bin() = default;
        n64_bin(const std::string& filename){
            std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);
            if(!file)
                return;

            size_t bin_size = file.tellg();
            _buffer = (uint8_t *)malloc(bin_size);
            if(_buffer == nullptr)
               return;

            file.seekg(0);
            file.read((char*)_buffer, bin_size); //copy ROM to RAM
            file.close();

            _begin = _buffer;
            _end = _buffer + bin_size; 
        };

        ~n64_bin(){
            if(_buffer != nullptr)
                free(_buffer);
        };

        void write(const std::string& filename){
            std::ofstream of(filename, std::ios::out | std::ios::binary);
            of.write((char *) begin(), size());
            of.close();
        }

        void resize(std::size_t size){
            if(_buffer == nullptr){
                _buffer = reinterpret_cast<uint8_t *>(malloc(size));
            }
            else{
                _buffer = reinterpret_cast<uint8_t *>(realloc(_buffer, size));
            }
            if(_buffer != nullptr){
                _begin = _buffer;
                _end = _buffer + size;
            }
        };
    private:
        uint8_t *_buffer = nullptr;
};

/*
Copyright (c) 2020 Michael "Mittenz" Salino-Hugg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/