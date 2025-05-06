#pragma once
#include <fstream>
#include <optional>
#include <vector>
#include <filesystem>

#include "type/type.hpp"

namespace NBT::IO {
	typedef uint8_t u8;
	typedef uint64_t u64;
	using std::ofstream, std::ifstream, std::optional, std::nullopt, std::filesystem::is_regular_file, std::filesystem::file_size, std::filesystem::path;

	inline bool write(const path& _path) noexcept {

	}
}