#ifndef AYTO_MP3_HPP
#define AYTO_MP3_HPP

#include "ayto/utility.hpp"
#include "ayto/exceptions.hpp"
#include <fstream>
#include <vector>
#include <array>

#include "trie.hpp"

#include "ayto/common/ayto_detail_define_macros.hpp"
#define NS_BEGIN namespace AYTO_ROOT_NAMESPACE { AYTO_INLINE_NAMESPACE AYTO_CURRENT_ABI { namespace mp3 {
#define NS_END } } }


NS_BEGIN

namespace id3
{
	using namespace ayto::typedefs;

	struct v1
	{
		char header[3];
		char title[30];
		char artist[30];
		char album[30];
		char year[4];
		char comment[30];
		byte genre[1];
	};


	struct v2
	{
		char identifier[3];
		struct version
		{
			char minor[1];
			char major[1];
		};
		byte flags[1];
		char size[4];

		enum class flag : byte
		{
			unsynchronistaion = 1 << 0,
			extended_header = 1 << 1,
			experimental_indicator = 1 << 2,
			footer_present = 1 << 3
		};

		[[nodiscard]]
		unsigned calculate_size()
		{
			unsigned result{};
			for (int i = 0; i < 4; i++)
			{
				result += size[3 - i] << (7 * i);
			}
			return result;
		}

		bool is_footer_present()
		{
			return
				static_cast<byte>(flags[0])
				&
				static_cast<byte>(flag::footer_present);
		}

		static inline bool is_valid_char(char c)
		{
			using namespace ayto::utility;
			return ascii::is_upper(c) || ascii::is_number(c);
		}

		static trie::Trie frame_tags;
	};

	inline void load()
	{
		std::vector<std::string> frame_tags =
		{
			"hi"
		};

		for (const auto& tag : frame_tags)
		{
			v2::frame_tags.insert(tag);
		}
	}

	inline void clear()
	{
		v2::frame_tags.cleanup();
	}

	trie::Trie v2::frame_tags({ {'0', '9'}, {'A', 'Z'} });

}

class mp3
{
public:
	void read_from_file(const wchar_t* path)
	{
		std::ifstream file;
		file.exceptions(std::ifstream::badbit);
		{
		utility::file_exception_guard guard(file);

		file.open(path, std::ios::in | std::ios::binary | std::ios::ate);
		if (!file.is_open())
		{
			throw ayto::exceptions::file::not_found(path, utility::length(path));
		}

		const std::ifstream::pos_type file_size = file.tellg();
		file.seekg(0, std::ios::beg);

		data = std::vector<char>(file_size);
		file.read(data.data(), file_size);
		}
	}

	void write_to_file(const wchar_t* path)
	{
		std::ofstream file(path, std::ios::out | std::ios::binary | std::ios::ate);
		file.exceptions(std::ofstream::badbit);
		{
			utility::file_exception_guard guard(file);

			if (!file.is_open())
			{
				throw(ayto::exceptions::file::not_found(path, utility::length(path)));
			}

			file.write(data.data(), data.size());
		}
	}

	[[nodiscard]]
	bool has_tag_v2()
	{
		std::array<char, 3> tag_identifier{ 'I', 'D', '3' };
		for (int i = 0; i < 3; i++)
		{
			if (data[i] != tag_identifier[i])
				return false;
		}

		return true;
	}

	[[nodiscard]]
	bool has_tag_v1()
	{
		std::array<char, 3> tag_identifier{ 'T', 'A', 'G' };
		for (int i = 0; i < 3; i++)
		{
			if (data[data.size() - 128 + i] != tag_identifier[i])
			{
				return false;
			}
		}

		return true;
	}

	void interpret_data()
	{
		std::string tagIdentifier = "ID3";
		bool hasTag = true;
		for (int i = 0; i < 3; i++) {
			if (data[i] != tagIdentifier[i]) {
				hasTag = false;
				break;
			}
		}

		sizeof(ayto::mp3::id3::v2)

		if (hasTag) {
			// copy first 10 bytes for the ID3v2 header
			ayto::mp3::id3::v1* header;

			header = (ayto::mp3::id3::v1*)data.data();

			//processID3v2();
			std::cout << header->album;
			//hasID3v2 = true;
		}

		/*
			check if ID3v1 exists (end)
			indication: "TAG" 128 bytes from end
		*/

		tagIdentifier = "TAG";
		hasTag = true;
		for (int i = 0; i < 3; i++) {
			if (data[data.size() - 128 + i] != tagIdentifier[i]) {
				hasTag = false;
				break;
			}
		}

		//if (hasTag) {
		//	// copy last 128 bytes to structure
		//	memcpy(&id3v1, &data[data.size() - 128], sizeof(ID3::ID3v1));

		//	// remove last 128 bytes
		//	data.resize(data.size() - sizeof(ID3::ID3v1));

		//	hasID3v1 = true;
		//}

		//data.shrink_to_fit();



	}

private:
	std::vector<char> data;
};

enum class genres : ayto::typedefs::byte
{
	blues,
	classic_rock,
	country,
	dance,
	disco,
	funk,
	grunge,
	hip_hop,
	jazz,
	metal,
	new_age,
	oldies,
	other,
	pop,
	rhythm_and_blues,
	rap,
	reggae,
	rock,
	techone,
	industrial
};


NS_END

#include "ayto/common/ayto_detail_undefine_macros.hpp"
#endif