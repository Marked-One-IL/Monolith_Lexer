#pragma once
#include <string>
#include <optional>

namespace Helper
{
	std::optional<std::string> extractFileContent(const char* filename);
}