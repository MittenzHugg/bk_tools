#ifndef N64_SOUNDFONT_H
#define N64_SOUNDFONT_H

#include <cstdint>
#include <algorithm>
#include <execution>

#include "json.hpp"

#include "n64/n64_span.hpp"

using json = nlohmann::json;

typedef int32_t ALMicroTime;
typedef uint8_t  ALPan;

class ALEnvelope{
public:
    ALEnvelope(const n64_span & file, const uint32_t offset){
        file(offset).get(
            attackTime, decayTime, releaseTime, 
            attackVolume, decayVolume
        );

    }
    ALMicroTime attackTime;
    ALMicroTime decayTime;
    ALMicroTime releaseTime;
    uint8_t attackVolume;
    uint8_t decayVolume;
};

class ALSound{
public:
    ALSound(const n64_span & file, const uint32_t offset){
        uint32_t env_offset = file(offset);
        if(env_offset){
            envelope = new ALEnvelope(file, env_offset);
        }
    }

    ALEnvelope *envelope;
};

class ALInstrument{
public:
    ALInstrument(const n64_span & file, const uint32_t offset){
        uint32_t pos = file(offset).get( 
            volume,     pan,        priority, 
            flags,      tremType,   tremRate,
            tremDepth,  tremDelay,  vibType,
            vibDepth,   vibDelay,   bendRange,
            soundCount
        );

        auto sound_const = [&](uint32_t offs){return ALSound(file, offs);};
        std::vector<uint32_t> soundOffsets = file(pos).get<std::vector<uint32_t>>(soundCount);
        soundArray.reserve(soundCount);
        std::transform(soundOffsets.begin(), soundOffsets.end(), soundArray.begin(),
            sound_const
        );
    }

    uint8_t volume;
    uint8_t pan;
    uint8_t priority;
    uint8_t flags;
    uint8_t tremType;
    uint8_t tremRate;
    uint8_t tremDepth;
    uint8_t tremDelay;
    uint8_t vibType;
    uint8_t vibRate;
    uint8_t vibDepth;
    uint8_t vibDelay;
    int16_t bendRange;
    int16_t soundCount;
    std::vector<ALSound> soundArray;
};

class ALBank{
public:
    ALBank(const n64_span & file, const uint32_t offset){
        uint32_t percOffset;
        uint32_t pos = file(offset).get(instCount, flags, pad, sampleRate, percOffset);
        std::vector<uint32_t> instOffsets = file(offset + pos).get<std::vector<uint32_t>>(instCount);

        auto inst_const = [&](uint32_t offs){return ALInstrument(file, offs);};
        if(percOffset){
            percussion = ALInstrument(file, percOffset);
        }

        instArray.reserve(instCount);
        std::transform(instOffsets.begin(), instOffsets.end(), instArray.begin(),
            inst_const
        );
    }

    int16_t instCount;
    uint8_t flags;
    uint8_t pad;
    int32_t sampleRate;
    ALInstrument percussion;
    std::vector<ALInstrument> instArray;
};

class ALBankFile{
public:
    ALBankFile(const n64_span & rom, const uint32_t offset){
        revision = rom.get(revision, bankCount);
        bankOffsetArray = rom(0x4).get<std::vector<uint32_t>>(bankCount);

        uint32_t _end = *std::max(bankOffsetArray.begin(), bankOffsetArray.end());
        uint32_t inst_cnt = rom.get<int16_t>(offset + _end);
        _end += 0x0C + inst_cnt * 4;
        _buffer = rom.slice(offset , _end - offset);
        
        bankArray.reserve(bankCount);
        auto bank_const = [&](uint32_t offs){return ALBank(_buffer, offs);};
        std::transform(
            bankOffsetArray.begin(), bankOffsetArray.end(), bankArray.begin(),
            bank_const
        );
    }

    ~ALBankFile(){

    }

    void extract(std::string path){
        
    }

    int16_t revision;
    int16_t bankCount;
    std::vector<ALBank> bankArray;
    inline const size_t size(void){return _buffer.size();};

private:
    n64_span _buffer;
    std::vector<uint32_t> bankOffsetArray;
};
#endif
