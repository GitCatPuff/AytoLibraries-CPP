#ifndef AYTO_SOUND_WAV_HPP
#define AYTO_SOUND_WAV_HPP

#include <fstream>

#include "../common/ayto_detail_define_macros.hpp"
#define NS_BEGIN namespace AYTO_ROOT_NAMESPACE { AYTO_INLINE_NAMESPACE AYTO_CURRENT_ABI { namespace sound::wav {
#define NS_END } } }
NS_BEGIN

const static std::string chunk_id = "RIFF";
const static std::string chunk_size = "....";
const static std::string format = "WAVE";

const static std::string subchunk1_id = "fmt ";
const static int subchunk1_size = 16;
const static int audio_format = 1;
const static int num_channels = 2;
const static int sample_rate = 44100;
const static int byte_rate = sample_rate * num_channels * (subchunk1_size / 8);
const static int block_align = num_channels * (subchunk1_size / 8);
const static int bits_per_sample = 16;

const static std::string subchunk2_id = "data";
const static std::string subchunk2_size = "....";

void write_as_bytes(std::ofstream& file, int value, int byte_size)
{
	file.write(reinterpret_cast<const char*>(&value), byte_size);
	
	
	
}

NS_END
#include "../common/ayto_detail_undefine_macros.hpp"
#endif
