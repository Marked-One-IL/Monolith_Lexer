#pragma once
#include "../Lexer/Token.hpp"

#include <vector>
#include <string_view>
#include <stack>
#include <optional>

namespace Lexer
{
	class Generator
	{
	public:
		Generator(const char* filename);

		bool empty(void) const;
		std::size_t size(void) const;
		const Lexer::Token& operator [] (std::size_t index) const;
		std::vector<Lexer::Token>::const_iterator begin(void) const;
		std::vector<Lexer::Token>::const_iterator end(void) const;

		friend std::ostream& operator << (std::ostream& stream, const Lexer::Generator& generator);

		bool didPass(void) const;

	private:
		struct Error
		{
			Error(const char* new_error, std::size_t new_column = 0);

			const char* const error;
			const std::size_t column;
		};

		static void skipSpaces(std::string_view& view);
		static void incrementToNextLine(std::string_view& view);

		static std::optional<std::size_t> extractSpacesLevel(const std::string_view& view);
		static std::optional<std::string_view> extractUntilNewLine(const std::string_view& view);
		static std::optional<std::string_view> extractUntilNotAlnum(const std::string_view& view);
		
		static std::optional<Lexer::Token> extractString3Literal(std::string_view& view);
		static std::optional<Lexer::Token> extractStringLiteral(std::string_view& view);
		static std::optional<Lexer::Token> extractCharLiteral(std::string_view& view);
		static std::optional<Lexer::Token> extractHexLiteral(std::string_view& view);
		static std::optional<Lexer::Token> extractBinLiteral(std::string_view& view);
		static std::optional<Lexer::Token> extractOctLiteral(std::string_view& view);
		static std::optional<Lexer::Token> extractSciLiteral(std::string_view& view);
		static std::optional<Lexer::Token> extractFloatLiteral(std::string_view& view);
		static std::optional<Lexer::Token> extractIntLiteral(std::string_view& view);
		static std::optional<Lexer::Token> extractBoolLiteral(std::string_view& view);
		static std::optional<Lexer::Token> extractNoneLiteral(std::string_view& view);
		static std::optional<Lexer::Token> extractSymbol(std::string_view& view, std::size_t& skipIndentFlag);
		static std::optional<Lexer::Token> extractKeyword(std::string_view& view);
		static std::optional<Lexer::Token> extractNewLine(std::string_view& view);
		static std::optional<std::vector<Lexer::Token>> extractInDedent(std::string_view& view, std::stack<std::size_t>& identLevels);
		static std::optional<Lexer::Token> extractIdentifier(std::string_view& view);

		const char* m_filename;
		std::string m_file; // Life of the string cannot be in the constructor but in the class itself.
		std::vector<Lexer::Token> m_tokens;
		std::vector<std::string> m_errors;
	};
}
