#include "dsspch.h"
#include "id_utils.h"

namespace dss
{
	namespace
	{
		uint64_t last_timestamp = 0;
		uint16_t sequence = 0;

		uint32_t bit_permute_step_simple(uint32_t x, uint32_t m, uint8_t shift)
		{
			return ((x & m) << shift) | ((x >> shift) & m);
		}

		std::string int_to_base32(uint64_t value)
		{
			std::string base32 = "0123456789abcdefghijklmnopqrstuv";

			bool negative = false;
			if (value < 0)
			{
				negative = true;
				value = -value;
			}
			
			std::string result;
			while (value > 0)
			{
				result = base32[value % 32] + result;
				value /= 32;
			}

			if (negative)
				result = "-" + result;

			return result;
		}
	}

	std::string generate_id(std::string prefix)
	{
		uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::utc_clock::now().time_since_epoch()).count();

		if (timestamp == last_timestamp)
		{
			sequence++;
		}
		else
		{
			sequence = 0;
		}

		last_timestamp = timestamp;

		uint32_t time_high = timestamp >> 32;
		uint32_t time_low = timestamp & 0xffffffff;

		time_high = bit_permute_step_simple(time_high, 0x55555555, 1);
		time_high = bit_permute_step_simple(time_high, 0x33333333, 2);
		time_high = bit_permute_step_simple(time_high, 0x0f0f0f0f, 4);
		time_high = bit_permute_step_simple(time_high, 0x00ff00ff, 8);
		time_high = bit_permute_step_simple(time_high, 0x0000ffff, 16);
		time_low = bit_permute_step_simple(time_low, 0x55555555, 1);
		time_low = bit_permute_step_simple(time_low, 0x33333333, 2);
		time_low = bit_permute_step_simple(time_low, 0x0f0f0f0f, 4);
		time_low = bit_permute_step_simple(time_low, 0x00ff00ff, 8);
		time_low = bit_permute_step_simple(time_low, 0x0000ffff, 16);
		uint64_t inverted_timestamp = (uint64_t)time_low << 32 | time_high;

		uint32_t inverted_sequence;
		inverted_sequence = bit_permute_step_simple(sequence, 0x55555555, 1);
		inverted_sequence = bit_permute_step_simple(inverted_sequence, 0x33333333, 2);
		inverted_sequence = bit_permute_step_simple(inverted_sequence, 0x0f0f0f0f, 4);
		inverted_sequence = bit_permute_step_simple(inverted_sequence, 0x00ff00ff, 8);
		inverted_sequence = bit_permute_step_simple(inverted_sequence, 0x0000ffff, 16);
		inverted_sequence = inverted_sequence >> 16;

		uint64_t id = inverted_timestamp | inverted_sequence;
		std::string id_str = int_to_base32(id);
		if (id_str.length() < 13)
		{
			id_str = std::string(13 - id_str.length(), '0') + id_str;
		}
		
		return prefix + "_" + id_str;
	}
}
