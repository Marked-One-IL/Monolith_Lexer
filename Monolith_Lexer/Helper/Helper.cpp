#include "../Helper/Helper.hpp"
#include <fstream>

std::optional<std::string> Helper::extractFileContent(const char* filename)
{
	std::ifstream file(filename, std::ios::in);
	if (not file)
	{
		return std::nullopt;
	}

	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return content;
}