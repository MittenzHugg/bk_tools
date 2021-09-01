#pragma once
#include <iostream>
#include "pixel_types.hpp"

enum SpriteFormat:int16_t{
    CI4 = (1 << 0),
    CI8 = (1 << 2),
    I4 = (1 << 5),
    I8 = (1 << 6),
    RGBA16 = (1 << 10),
    RGBA32 = (1 << 11)
};

class BKSpriteFrameChunk{
    public:
        BKSpriteFrameChunk(const n64_span& span, uint32_t& offset, const SpriteFormat type){
            x = span.get<int16_t>(offset + 0);
            y = span.get<int16_t>(offset + 2);
            w = span.get<int16_t>(offset + 4);
            h = span.get<int16_t>(offset + 6);

            offset += 0x8;
            while(offset % 8){
                offset++;
            }
            Data.reserve(w*h);
            switch(type){
                case RGBA16:
                    for(int i = 0; i < w*h; i++){
                        Data.push_back(static_cast<rgba_t>(rgba16_t(span(offset))));
                        offset += sizeof(uint16_t);
                    }
                    break;
                case RGBA32:
                    for(int i = 0; i < w*h; i++){
                        Data.push_back(static_cast<rgba_t>(rgba32_t(span(offset))));
                        offset += sizeof(uint32_t);
                    }
                    break;
                case I4:
                    int i;
                    for(i = 0; i < w*h; i++){
                        Data.push_back(static_cast<rgba_t>(i4_t(span(offset), i & 1)));
                        if(i & 1)
                            offset += sizeof(uint8_t);
                    }
                    if(i & 1)
                        offset += sizeof(uint8_t);
                    break;
                case I8:
                    for(int i = 0; i < w*h; i++){
                        Data.push_back(static_cast<rgba_t>(i8_t(span(offset))));
                        offset += sizeof(uint8_t);
                    }
                    break;
            }
        }

        BKSpriteFrameChunk(const n64_span& span, uint32_t& offset, const SpriteFormat type, const std::vector<rgba_t>& palette){
            x = span.get<int16_t>(offset + 0);
            y = span.get<int16_t>(offset + 2);
            w = span.get<int16_t>(offset + 4);
            h = span.get<int16_t>(offset + 6);

            offset += 0x8;
            while(offset % 8){
                offset++;
            }
            Data.reserve(w*h);
            switch(type){
                case CI4:
                    int i;
                    for(i = 0; i < w*h; i++){
                        if(!(i & 1)){
                            Data.push_back(palette[span.get<uint8_t>(offset) & 0x0F]);
                        }
                        else
                        {
                            Data.push_back(palette[(span.get<uint8_t>(offset) >> 4) & 0x0F]);
                            offset += sizeof(uint8_t);
                        }
                    }
                    if(i & 1)
                        offset += sizeof(uint8_t);
                    break;
                case CI8:
                    for(int i = 0; i < w*h; i++){
                        Data.push_back(palette[span.get<uint8_t>(offset)]);
                        offset += sizeof(uint8_t);
                    }
                    break;
            }
        }

        //native
        int16_t x = 0;
        int16_t y = 0;
        int16_t w = 0;
        int16_t h = 0;

        std::vector<rgba_t> Data;
};

struct BKSpriteFrame{
    BKSpriteFrame(const n64_span& span, uint32_t offset, const SpriteFormat type){
        w = span.get<int16_t>(offset + 4);
        h = span.get<int16_t>(offset + 6);
        chunkCnt = span.get<int16_t>(offset + 8);

        offset += 0x14;
        Chunk.reserve(chunkCnt);
        switch(type){
            case RGBA16:
            case RGBA32:
            case I4:
            case I8:
                for(int i = 0; i< chunkCnt; i++){
                    Chunk.push_back(BKSpriteFrameChunk(span, offset, type));
                }
                break;
            case CI4:
                while(offset % 8){
                    offset++;
                }
                pal.reserve(16);
                for(uint32_t i = 0; i < 16; i++){
                    pal[i] = static_cast<rgba_t>(rgba16_t(span(offset)));
                    offset += 2;
                }
                for(int i = 0; i < chunkCnt; i++){
                    Chunk.push_back(BKSpriteFrameChunk(span, offset, type, pal));
                }
                break;
            case CI8:
                while(offset % 8){
                    offset++;
                }
                pal.reserve(256);
                for(uint32_t i = 0; i < 256; i++){
                    pal[i] = static_cast<rgba_t>(rgba16_t(span(offset)));
                    offset += 2;
                }
                for(int i = 0; i < chunkCnt; i++){
                    Chunk.push_back(BKSpriteFrameChunk(span, offset, type, pal));
                }
                break;
            default:
                std::cout << "undefined type" << std::endl;
                break;
        }
        if(type == I4)
            return;
        Data.resize(w*h,{0,0,0,0});
        for( auto& iChunk : Chunk){
            for(int iX = 0; iX < iChunk.w; iX++){
                for(int iY = 0; iY < iChunk.h; iY++){
                    Data[iChunk.x + iX + (iChunk.y + iY)*w] = iChunk.Data[iX +iY*iChunk.w];
                }
            }
        }
    }
    //native
    int16_t unk0;
    int16_t unk2;
    int16_t w;
    int16_t h;
    int16_t chunkCnt;
    int16_t unkA;
    uint8_t padC[8];


    std::vector<rgba_t> pal;
    std::vector<BKSpriteFrameChunk> Chunk;
    std::vector<rgba_t> Data;
};

class BKSprite{
    public:
        BKSprite(const n64_span& span){
            uint32_t offset;
            frame_count = span.get<int16_t>();
            type = static_cast<SpriteFormat>(span.get<int16_t>(2));
            unk4 = span.get<int16_t>(4);
            unk6 = span.get<int16_t>(6);
            unk8 = span.get<int16_t>(8);
            unkA = span.get<int16_t>(0xA);
            unkC = span.get<uint32_t>(0xC);
            offset = 0x10;

            FrameOffset = span(offset).get<std::vector<uint32_t>>(frame_count);
            offset += frame_count*sizeof(uint32_t);

            Frame.reserve(frame_count);
            for(int i; i < frame_count; i++){
                //need to pass offset for alignment reasons
                Frame.push_back(BKSpriteFrame(span, offset + FrameOffset[i], type));
            }
        }

        //native
        int16_t frame_count;
        SpriteFormat type;
        int16_t unk4;
        int16_t unk6;
        int16_t unk8;
        int16_t unkA;
        uint32_t unkC;

        std::vector<uint32_t> FrameOffset;
        std::vector<BKSpriteFrame> Frame;
};