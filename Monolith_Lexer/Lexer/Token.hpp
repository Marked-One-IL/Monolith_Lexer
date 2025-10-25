#pragma once
#include "../Lexer/Tag.hpp"
#include <string_view>
#include <ostream>

namespace Lexer
{
	// This struct does not hold any special logic.
	// It only accept an already calculated tag and content.
	struct Token
	{
		Token(Lexer::Tag new_tag);
		Token(Lexer::Tag new_tag, const std::string_view& new_content);

		friend std::ostream& operator << (std::ostream& stream, const Lexer::Token& token);

		const Tag tag;
		const std::string_view content;
	};
}
