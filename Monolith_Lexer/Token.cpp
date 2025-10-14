#include "Token.hpp"
#include "Assert.hpp"

Lexer::Token::Token(Lexer::Tag new_tag)
    : tag(new_tag)
{
    Assert_Message(this->tag >= Tag::STRING3_LITERAL and this->tag <= Tag::IDENTIFIER, std::format("Unknown Tag: {}", static_cast<int>(this->tag)));
}

Lexer::Token::Token(Lexer::Tag new_tag, const std::string_view& new_content)
    : tag(new_tag), content(new_content)
{
    Assert_Message(this->tag >= Tag::STRING3_LITERAL and this->tag <= Tag::IDENTIFIER, std::format("Unknown Tag: {}", static_cast<int>(this->tag)));
}

std::ostream& Lexer::operator << (std::ostream& stream, const Lexer::Token& token)
{
    // Format: [Tag: 'Content']
    
    // Not multi-thread safe but it's not a big deal now.
    static std::size_t indentCount = 0;
    static bool shouldPrintIndents = false;

    if (shouldPrintIndents)
    {
        if (token.tag == Lexer::Tag::INDENT) indentCount++;
        else if (token.tag == Lexer::Tag::DEDENT) indentCount--;

        for (std::size_t i = 0; i < indentCount; i++)
        {
            stream << '\t';
        }
    }

    switch (token.tag)
    {
    case Lexer::Tag::NEW_LINE:
        shouldPrintIndents = true;
        stream << "[NEW_LINE]\n";
        return stream;
    case Lexer::Tag::INDENT:
        shouldPrintIndents = true;
        stream << "[INDENT]\n";
        return stream;
    case Lexer::Tag::DEDENT:
        shouldPrintIndents = true;
        stream << "[DEDENT]\n";
        return stream;
    }

    shouldPrintIndents = false;

	stream << '[';
	switch (token.tag)
	{
    case Tag::STRING3_LITERAL:
        stream << "STRING3_LITERAL";
        break;
    case Tag::STRING_LITERAL:
        stream << "STRING_LITERAL";
        break;
    case Tag::CHAR_LITERAL:
        stream << "CHAR_LITERAL";
        break;
    case Tag::HEX_LITERAL:
        stream << "HEX_LITERAL";
        break;
    case Tag::BIN_LITERAL:
        stream << "BIN_LITERAL";
        break;
    case Tag::OCT_LITERAL:
        stream << "OCT_LITERAL";
        break;
    case Tag::SCI_LITERAL:
        stream << "SCI_LITERAL";
        break;
    case Tag::FLOAT_LITERAL:
        stream << "FLOAT_LITERAL";
        break;
    case Tag::INT_LITERAL:
        stream << "INT_LITERAL";
        break;
    case Tag::BOOL_LITERAL:
        stream << "BOOL_LITERAL";
        break;
    case Tag::NONE_LITERAL:
        stream << "NONE_LITERAL";
        break;
    case Tag::SYMBOL:
        stream << "SYMBOL";
        break;
    case Tag::KEYWORD:
        stream << "KEYWORD";
        break;
    case Tag::NEW_LINE:
        stream << "NEW_LINE";
        break;
    case Tag::INDENT:
        stream << "INDENT";
        break;
    case Tag::DEDENT:
        stream << "DEDENT";
        break;
    case Tag::IDENTIFIER:
        stream << "IDENTIFIER";
        break;

    default:
        Assert_Message(ASSERT_ALWAYS, std::format("Unknown Tag: {}", static_cast<int>(token.tag)));
	}
    stream << ": '" << token.content << "'] ";

	return stream;
}

