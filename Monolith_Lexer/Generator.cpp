#include "Generator.hpp"
#include "Helper.hpp"
#include "Assert.hpp"
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <array>
#include <cctype>
#include <iterator>

Lexer::Generator::Generator(const char* filename) : m_filename(filename)
{
    if (auto opt = Helper::extractFileContent(filename))
    {
        this->m_file = opt.value();
    }
    else
    {
        this->m_errors.emplace_back(std::format("Could not open file: '{}'", filename));
        return;
    }

    for (unsigned char c : this->m_file)
    {
        if (not std::isprint(c) and not std::iscntrl(c))
        {
            // I ain't doing UTF16.
            this->m_errors.emplace_back("Unprintable chars. UTF16 is probably used");
            return;
        }
    }

    // For lexering.
    std::string_view view = this->m_file;
    std::stack<std::size_t> identLevels;
    bool skipIndentFlag = false;
    bool shouldCheckIndentFlag = false;
    
    // For errors.
    std::size_t linesCount = 0;
    std::string_view currentLine = extractUntilNewLine(view);

    // Note: If a extractTAGX method is guaranteed to work only in the current line it's first commands in the function block shall be.
    // std::string_view fixedView = Lexer::Generator::extractUntilNewLine(view);
    // if (fixedView.empty()) return ...;
    // 
    // Then for the rest of scanning you shall only use 'fixedView' not 'view' even if it's (seams) not necessary (The compiler optimize it anyway).
    // Only after scanning and extracting you are allowed to 'view' and you shall return like this:
    //     std::string_view content = ...;
    //     view.remove_prefix(...);
    //     return Lexer::Token(Lexer::Tag..., content);
    // The exceptions of this is extractString3Literal, extractNewLine and skipSpaces.

    while (not view.empty())
    {
        try
        {
            // Sorry for this ugly nesting. if (not skipIndentFlag) must be before auto opt = ...
            if (shouldCheckIndentFlag)
            {
                if (not skipIndentFlag)
                {
                    if (auto opt = Lexer::Generator::extractInDedent(view, identLevels))
                    {
                        for (auto& token : opt.value())
                            this->m_tokens.emplace_back(std::move(token));
                    }
                }
                shouldCheckIndentFlag = false;
                continue;
            }

            Lexer::Generator::skipSpaces(view);

            if (auto opt = Lexer::Generator::extractString3Literal(view))
            {
                this->m_tokens.emplace_back(opt.value());
                continue;
            }
            if (auto opt = Lexer::Generator::extractStringLiteral(view))
            {
                this->m_tokens.emplace_back(opt.value());
                continue;
            }
            if (auto opt = Lexer::Generator::extractCharLiteral(view))
            {
                this->m_tokens.emplace_back(opt.value());
                continue;
            }
            if (auto opt = Lexer::Generator::extractHexLiteral(view))
            {
                this->m_tokens.emplace_back(opt.value());
                continue;
            }
            if (auto opt = Lexer::Generator::extractBinLiteral(view))
            {
                this->m_tokens.emplace_back(opt.value());
                continue;
            }
            if (auto opt = Lexer::Generator::extractSciLiteral(view))
            {
                this->m_tokens.emplace_back(opt.value());
                continue;
            }
            if (auto opt = Lexer::Generator::extractFloatLiteral(view))
            {
                this->m_tokens.emplace_back(opt.value());
                continue;
            }
            if (auto opt = Lexer::Generator::extractIntLiteral(view))
            {
                this->m_tokens.emplace_back(opt.value());
                continue;
            }
            if (auto opt = Lexer::Generator::extractBoolLiteral(view))
            {
                this->m_tokens.emplace_back(opt.value());
                continue;
            }
            if (auto opt = Lexer::Generator::extractNoneLiteral(view))
            {
                this->m_tokens.emplace_back(opt.value());
                continue;
            }
            if (auto opt = Lexer::Generator::extractSymbol(view, skipIndentFlag))
            {
                this->m_tokens.emplace_back(opt.value());
                continue;
            }
            if (auto opt = Lexer::Generator::extractKeyword(view))
            {
                this->m_tokens.emplace_back(opt.value());
                continue;
            }
            if (auto opt = Lexer::Generator::extractNewLine(view))
            {
                currentLine = Lexer::Generator::extractUntilNewLine(view);
                linesCount++;
                shouldCheckIndentFlag = true;
                this->m_tokens.emplace_back(opt.value());
                continue;
            }
            if (auto opt = Lexer::Generator::extractIdentifier(view))
            {
                this->m_tokens.emplace_back(opt.value());
                continue;
            }

            // This section happens if it's a comment. 
            Lexer::Generator::incrementToNextLine(view);
            currentLine = Lexer::Generator::extractUntilNewLine(view);
            linesCount++;
        }
        catch (const Lexer::Generator::Error& error)
        {
            std::string_view fixedLine = currentLine;
            Lexer::Generator::skipSpaces(fixedLine);
            std::size_t distance = (view.data() - fixedLine.data()) + error.column;
            this->m_errors.emplace_back(std::format("At line: {}\nError: {}\n{}\n{:{}s}^", linesCount, error.error, fixedLine, "", distance));
            
            Lexer::Generator::incrementToNextLine(view);
            currentLine = Lexer::Generator::extractUntilNewLine(view);
            linesCount++;
        }
        catch (const std::exception& error)
        {
            Assert_Message(ASSERT_ALWAYS, error.what());
        }
    }
}

bool Lexer::Generator::empty(void) const
{
    return this->m_tokens.empty();
}
std::size_t Lexer::Generator::size(void) const
{
    return this->m_tokens.size();
}
const Lexer::Token& Lexer::Generator::operator [] (std::size_t index) const
{
    return this->m_tokens[index];
}
std::vector<Lexer::Token>::const_iterator Lexer::Generator::begin(void) const
{
    return this->m_tokens.begin();
}
std::vector<Lexer::Token>::const_iterator Lexer::Generator::end(void) const
{
    return this->m_tokens.end();
}

bool Lexer::Generator::didPass(void) const
{
    return this->m_errors.empty();
}

void Lexer::Generator::skipSpaces(std::string_view& view)
{
    while (not view.empty() and (view.front() == ' ' or view.front() == '\t')) 
        view.remove_prefix(1);
}
std::size_t Lexer::Generator::extractSpacesLevel(const std::string_view& view)
{
    std::string_view fixedView = Lexer::Generator::extractUntilNewLine(view);
    if (fixedView.empty()) return 0;

    std::size_t level = 0;

    while (not fixedView.empty() and (fixedView.front() == ' ' or fixedView.front() == '\t'))
    {
        level += fixedView.front() == ' ';
        level += (fixedView.front() == '\t') * 2;
        fixedView.remove_prefix(1);
    }
    
    return level;
}
std::string_view Lexer::Generator::extractUntilNotAlnum(const std::string_view& view)
{
    std::string_view fixedView = Lexer::Generator::extractUntilNewLine(view);
    if (fixedView.empty()) return fixedView;

    std::size_t i = 0;
    for (i = 0; i < fixedView.size(); i++)
    {
        unsigned char c = static_cast<unsigned char>(fixedView[i]);
        if (not std::isalnum(c) and c != '_')
        {
            break;
        }
    }

    return fixedView.substr(0, i);
}
std::string_view Lexer::Generator::extractUntilNotAlnum(const std::string_view& view, char ignoreOnce, const char* errorIfNotOnce)
{
    std::string_view fixedView = Lexer::Generator::extractUntilNewLine(view);
    if (fixedView.empty()) return fixedView;

    std::size_t i = 0;
    bool seen = false;

    for (i = 0; i < fixedView.size(); i++)
    {
        if (fixedView[i] == ignoreOnce)
        {
            if (seen)
            {
                throw Lexer::Generator::Error(errorIfNotOnce, i);
            }
            seen = true;
            continue;
        }

        unsigned char c = static_cast<unsigned char>(fixedView[i]);
        if (not std::isalnum(c))
        {
            break;
        }
    }

    return fixedView.substr(0, i);
}
std::string_view Lexer::Generator::extractUntilNewLine(const std::string_view& view)
{
    std::size_t newLinePos = view.find('\n');
    std::string_view content = view;

    if (newLinePos != std::string_view::npos)
    {
        content = view.substr(0, newLinePos);
    }

    return content;
}
void Lexer::Generator::incrementToNextLine(std::string_view& view)
{
    while (not view.empty() and (view.front() != '\n')) view.remove_prefix(1);
    if (not view.empty()) view.remove_prefix(1);
}

std::optional<Lexer::Token> Lexer::Generator::extractString3Literal(std::string_view& view)
{
    // Format: [Start: """], [Value: *any*], [End: """]

    if (not view.starts_with("\"\"\"")) return std::nullopt;

    std::size_t endPos = view.find("\"\"\"", std::strlen("\"\"\""));

    if (endPos == std::string_view::npos)
    {
        throw Lexer::Generator::Error("Triple string literal does not end");
    }

    endPos += std::strlen("\"\"\"");
    std::string_view content = view.substr(0, endPos);
    view.remove_prefix(endPos);
    return Lexer::Token(Lexer::Tag::STRING3_LITERAL, content);
}
std::optional<Lexer::Token> Lexer::Generator::extractStringLiteral(std::string_view& view)
{
    std::string_view fixedView = Lexer::Generator::extractUntilNewLine(view);
    if (fixedView.empty()) return std::nullopt;

    // Format: [Start: "], [Value: *any*], [End: "]

    if (not (fixedView.front() == '\"')) return std::nullopt;

    char prev = ' ';
    auto endPosIt = std::find_if(fixedView.begin() + 1, fixedView.end(), [&prev] (char c) -> bool
    {
        if (c == '\"' and prev != '\\')
        {
            return true;
        }

        prev = c;
        return false;
    });

    if (endPosIt == fixedView.end())
    {
        throw Lexer::Generator::Error("String literal does not end at current line");
    }

    std::size_t endPos = std::distance(fixedView.begin(), endPosIt) + std::strlen("\"");
    std::string_view content = fixedView.substr(0, endPos);
    view.remove_prefix(endPos);
    return Lexer::Token(Lexer::Tag::STRING_LITERAL, content);
}
std::optional<Lexer::Token> Lexer::Generator::extractCharLiteral(std::string_view& view)
{
    std::string_view fixedView = Lexer::Generator::extractUntilNewLine(view);
    if (fixedView.empty()) return std::nullopt;

    // Format: [Start: '], [Value: *any*], [End: ']

    if (not (fixedView.front() == '\'')) return std::nullopt;

    char prev = ' ';
    auto endPosIt = std::find_if(fixedView.begin() + 1, fixedView.end(), [&prev](char c) -> bool
    {
        if (c == '\'' and prev != '\\')
        {
            return true;
        }

        prev = c;
        return false;
    });

    if (endPosIt == fixedView.end())
    {
        throw Lexer::Generator::Error("Character literal does not end at current line");
    }

    std::size_t endPos = std::distance(fixedView.begin(), endPosIt) + std::strlen("\'");
    std::string_view content = fixedView.substr(0, endPos);
    view.remove_prefix(endPos);
    return Lexer::Token(Lexer::Tag::CHAR_LITERAL, content);
}
std::optional<Lexer::Token> Lexer::Generator::extractHexLiteral(std::string_view& view)
{
    std::string_view fixedView = Lexer::Generator::extractUntilNewLine(view);
    if (fixedView.empty()) return std::nullopt;

    // Format: // Bin format: [Signature: 0x/0X], [Value: Digits (123456789ABCDEFabcdef)] 

    if (not fixedView.starts_with("0x") and not fixedView.starts_with("0X")) return std::nullopt;
    
    std::string_view possibleHex = Lexer::Generator::extractUntilNotAlnum(fixedView.substr(std::strlen("0x")));
    if (possibleHex.empty())
        throw Lexer::Generator::Error("Invalid Hexadecimal literal");

    for (std::size_t i = 0; i < possibleHex.size(); i++)
    {
        unsigned char c = static_cast<unsigned char>(possibleHex[i]);
        if (not std::isdigit(c) and std::string_view("ABCDEFabcdef").find(static_cast<char>(c)) == std::string_view::npos)
        {
            throw Lexer::Generator::Error("Invalid Hexadecimal literal", i);
        }
    }

    std::size_t contentSize = possibleHex.size() + std::strlen("0x");
    std::string_view content = fixedView.substr(0, contentSize);
    view.remove_prefix(contentSize);
    return Lexer::Token(Lexer::Tag::HEX_LITERAL, content);
}

std::optional<Lexer::Token> Lexer::Generator::extractBinLiteral(std::string_view& view)
{
    std::string_view fixedView = Lexer::Generator::extractUntilNewLine(view);
    if (fixedView.empty()) return std::nullopt;

    // Format: [Signature: 0b/0B], [Value: Digits (01)] 

    if (not fixedView.starts_with("0b") and not fixedView.starts_with("0B")) return std::nullopt;

    std::string_view possibleBin = Lexer::Generator::extractUntilNotAlnum(fixedView.substr(std::strlen("0b")));
    if (possibleBin.empty())
        throw Lexer::Generator::Error("Invalid binary literal");

    for (std::size_t i = 0; i < possibleBin.size(); i++)
    {
        if (possibleBin[i] != '0' and possibleBin[i] != '1')
        {
            throw Lexer::Generator::Error("Invalid binary literal", i);
        }
    }

    std::size_t contentSize = possibleBin.size() + std::strlen("0b");
    std::string_view content = fixedView.substr(0, contentSize);
    view.remove_prefix(contentSize);
    return Lexer::Token(Lexer::Tag::BIN_LITERAL, content);
}
std::optional<Lexer::Token> Lexer::Generator::extractSciLiteral(std::string_view& view)
{
    std::string_view fixedView = Lexer::Generator::extractUntilNewLine(view);
    if (fixedView.empty()) return std::nullopt;

    // Format: [Base: Digits], [Signature: e/E], (Optional: + is default) [Direction: +/-], [Distance: Digits].

    if (not std::isdigit(static_cast<unsigned char>(fixedView.front()))) return std::nullopt;

    std::string_view currentView = fixedView;

    std::string_view possibleBase;
    for (std::size_t i = 0; i < fixedView.size(); i++)
    {
        currentView.remove_prefix(1);
        unsigned char c = static_cast<unsigned char>(fixedView[i]);
        if (std::tolower(c) == 'e')
        {
            possibleBase = fixedView.substr(0, i);
            break;
        }
        if (not std::isdigit(c))
        {
            return std::nullopt;
        }
    }

    if ((currentView.size() < 3) or ((currentView[0] != '+' and currentView[0] != '-') and not std::isdigit(static_cast<unsigned char>(currentView[0]))))
    {
        return std::nullopt;
    }

    char direction = currentView[0];
    currentView.remove_prefix(1);

    std::string_view distance = Lexer::Generator::extractUntilNotAlnum(currentView);
    for (std::size_t i = 0; i < distance.size(); i++)
    {
        unsigned char c = static_cast<unsigned char>(distance[i]);
        if (not std::isdigit(c))
        {
            throw Lexer::Generator::Error("Invalid scientific notation literal", i);
        }
    }

    std::size_t contentSize = possibleBase.size() + std::strlen("e+") + distance.size();
    std::string_view content = fixedView.substr(0, contentSize);
    view.remove_prefix(contentSize);
    return Lexer::Token(Lexer::Tag::SCI_LITERAL, content);
}
std::optional<Lexer::Token> Lexer::Generator::extractFloatLiteral(std::string_view& view)
{
    std::string_view fixedView = Lexer::Generator::extractUntilNewLine(view);
    if (fixedView.empty()) return std::nullopt;

    // Format: [Value (Integer part): Digits], [Signature: '.' (Dot)], [Value (Decimal part): Digits]

    if (not std::isdigit(static_cast<unsigned char>(fixedView.front()))) return std::nullopt;

    std::string_view possibleFloat = Lexer::Generator::extractUntilNotAlnum(fixedView, '.', "Invalid float literal");

    bool seenDigit = false;
    bool seenDot = false;
    for (std::size_t i = 0; i < possibleFloat.size(); i++)
    {
        unsigned char c = static_cast<unsigned char>(possibleFloat[i]);

        if (std::isdigit(c))
            seenDigit = true;
        else if (c == '.')
            seenDot = true;
        else
            throw Lexer::Generator::Error("Invalid float literal", i);
    }
    if (not (seenDigit and seenDot))
        return std::nullopt;

    std::string_view content = possibleFloat;
    view.remove_prefix(content.size());
    return Lexer::Token(Lexer::Tag::FLOAT_LITERAL, content);
}
std::optional<Lexer::Token> Lexer::Generator::extractIntLiteral(std::string_view& view)
{
    std::string_view fixedView = Lexer::Generator::extractUntilNewLine(view);
    if (fixedView.empty()) return std::nullopt;

    // Format: [Value: Digits]

    if (not std::isdigit(static_cast<unsigned char>(fixedView.front()))) return std::nullopt;

    std::string_view possibleInt = Lexer::Generator::extractUntilNotAlnum(fixedView, '.', "Invalid integer literal");

    for (std::size_t i = 0; i < possibleInt.size(); i++)
    {
        unsigned char c = static_cast<unsigned char>(possibleInt[i]);

        if (not std::isdigit(c))
        {
            throw Lexer::Generator::Error("Invalid integer literal", i);
        }
    }

    std::string_view content = possibleInt;
    view.remove_prefix(content.size());
    return Lexer::Token(Lexer::Tag::INT_LITERAL, content);
}
std::optional<Lexer::Token> Lexer::Generator::extractBoolLiteral(std::string_view& view)
{
    std::string_view fixedView = Lexer::Generator::extractUntilNewLine(view);
    if (fixedView.empty()) return std::nullopt;

    // Format: [Value: String]

    std::string_view possibleBool = Lexer::Generator::extractUntilNotAlnum(fixedView);

    if (possibleBool == "True" or possibleBool == "False")
    {
        std::string_view content = possibleBool;
        view.remove_prefix(content.size());
        return Lexer::Token(Lexer::Tag::BOOL_LITERAL, content);
    }

    return std::nullopt;
}
std::optional<Lexer::Token> Lexer::Generator::extractNoneLiteral(std::string_view& view)
{
    std::string_view fixedView = Lexer::Generator::extractUntilNewLine(view);
    if (fixedView.empty()) return std::nullopt;

    // Format: [Value: String]

    std::string_view possibleNone = Lexer::Generator::extractUntilNotAlnum(fixedView);

    if (possibleNone == "None")
    {
        std::string_view content = possibleNone;
        view.remove_prefix(content.size());
        return Lexer::Token(Lexer::Tag::NONE_LITERAL, content);
    }

    return std::nullopt;
}
std::optional<Lexer::Token> Lexer::Generator::extractSymbol(std::string_view& view, bool& skipIndentFlag)
{
    std::string_view fixedView = Lexer::Generator::extractUntilNewLine(view);
    if (fixedView.empty()) return std::nullopt;

    // Format: [Value: String]

    constexpr std::array<std::string_view, 5> wordSymbols =
    {
        "and", "or", "not", "is", "as"
    };
    constexpr std::array<std::string_view, 3> punctuator3Symbols =
    {
        "<<=", ">>=", "..."
    };
    constexpr std::array<std::string_view, 18> punctuator2Symbols =
    {
        "++", "+=", "--", "-=", "*=", "/=", "%=", ">=", "<=", ">>", "<<", "|=", "&=", "^=", "==", "!=", "->", "::"
    };
    constexpr std::array<char, 20> punctuator1symbols =
    {
        '+', '-', '*', '/', '%', '<', '>', '|', '&', '^', '~', '=', '.', ',', '(', ')', '[', ']', '?', ':'
    };

    std::string_view possibleWordSymbol = Lexer::Generator::extractUntilNotAlnum(fixedView);
    if (not possibleWordSymbol.empty())
    {
        for (auto& wordSymbol : wordSymbols)
        {
            if (possibleWordSymbol == wordSymbol)
            {
                view.remove_prefix(possibleWordSymbol.size());
                return Token(Lexer::Tag::SYMBOL, possibleWordSymbol);
            }
        }
        return std::nullopt;
    }
    for (auto& punctuator3Symbol : punctuator3Symbols)
    {
        if (fixedView.starts_with(punctuator3Symbol))
        {
            constexpr std::size_t OFFSET = 3;
            std::string_view content = fixedView.substr(0, OFFSET);
            view.remove_prefix(OFFSET);
            return Token(Lexer::Tag::SYMBOL, content);
        }
    }
    for (auto& punctuator2Symbol : punctuator2Symbols)
    {
        if (fixedView.starts_with(punctuator2Symbol))
        {
            constexpr std::size_t OFFSET = 2;
            std::string_view content = fixedView.substr(0, OFFSET);
            view.remove_prefix(OFFSET);
            return Token(Lexer::Tag::SYMBOL, content);
        }
    }
    for (char punctuator1Symbol : punctuator1symbols)
    {
        if (fixedView.starts_with(punctuator1Symbol))
        {
            if (punctuator1Symbol == '(' or punctuator1Symbol == '[')
            {
                skipIndentFlag = true;
            }
            else if ((punctuator1Symbol == ')' or punctuator1Symbol == ']') and skipIndentFlag)
            {
                skipIndentFlag = false;
            }

            constexpr std::size_t OFFSET = 1;
            std::string_view content = fixedView.substr(0, OFFSET);
            view.remove_prefix(OFFSET);
            return Token(Lexer::Tag::SYMBOL, content);
        }
    }

    return std::nullopt;
}
std::optional<Lexer::Token> Lexer::Generator::extractKeyword(std::string_view& view)
{
    std::string_view fixedView = Lexer::Generator::extractUntilNewLine(view);
    if (fixedView.empty()) return std::nullopt;

    // Format: [Value: String]

    static constexpr std::array<std::string_view, 35> keywords =
    {
        "if", "elif", "else",
        "for", "while", "switch", "case", "default",
        "break", "continue",
        "label", "goto",
        "def", "return", "class",
        "const", "static",
        "int8", "uint8",
        "int16", "uint16",
        "int32", "uint32",
        "int64", "uint64",
        "float", "double",
        "import",
        "ptr", "ref", "dref", "arr",
        "enum", "namespace", "typedef"
    };

    std::string_view possibleKeyword = Lexer::Generator::extractUntilNotAlnum(fixedView);
    if (possibleKeyword.empty())
    {
        return std::nullopt;
    }

    for (std::size_t i = 0; i < keywords.size(); i++)
    {
        if (keywords[i] == possibleKeyword)
        {
            view.remove_prefix(possibleKeyword.size());
            return Token(Lexer::Tag::KEYWORD, possibleKeyword);
        }
    }

    return std::nullopt;
}
std::optional<Lexer::Token> Lexer::Generator::extractNewLine(std::string_view& view)
{
    // Format: [Value: '\n']

    if (view.front() == '\n')
    {
        view.remove_prefix(std::strlen("\n"));
        return Lexer::Token(Lexer::Tag::NEW_LINE);
    }

    return std::nullopt;
}
std::optional<std::vector<Lexer::Token>> Lexer::Generator::extractInDedent(std::string_view& view, std::stack<std::size_t>& identLevels)
{
    std::string_view fixedView = Lexer::Generator::extractUntilNewLine(view);
    if (fixedView.empty()) return std::nullopt;

    // Format: [Value: String of space\tab-s]

    std::size_t newLevel = Lexer::Generator::extractSpacesLevel(fixedView);

    if (newLevel == 0)
        return std::nullopt;

    if (identLevels.empty() or newLevel > identLevels.top())
    {
        identLevels.push(newLevel);
        return std::vector<Lexer::Token>{ Lexer::Token(Lexer::Tag::INDENT) };
    }
    else if (identLevels.top() == newLevel)
    {
        return std::nullopt;
    }

    std::vector<Lexer::Token> dedents;
    while (not identLevels.empty() and identLevels.top() > newLevel) 
    {
        identLevels.pop();
        dedents.emplace_back(Lexer::Tag::DEDENT);
    }
    if (identLevels.empty() or identLevels.top() != newLevel)
        throw Lexer::Generator::Error("Indent (spacing) doesn't match previous indents");

    return dedents;
}
std::optional<Lexer::Token> Lexer::Generator::extractIdentifier(std::string_view& view)
{
    std::string_view fixedView = Lexer::Generator::extractUntilNewLine(view);
    if (fixedView.empty()) return std::nullopt;

    // Format: [Value: String]

    if (fixedView.front() == '#') return std::nullopt;
    if (not std::isalnum(fixedView.front()) and fixedView.front() != '_') throw Lexer::Generator::Error("Invalid character");

    std::string_view content = Lexer::Generator::extractUntilNotAlnum(fixedView);
    view.remove_prefix(content.size());
    return Lexer::Token(Lexer::Tag::IDENTIFIER, content);
}

std::ostream& Lexer::operator << (std::ostream& stream, const Lexer::Generator& generator)
{
    std::copy(generator.begin(), generator.end(), std::ostream_iterator<Lexer::Token>(stream));

    if (not generator.didPass())
    {
        stream << "\nAt file: " << generator.m_filename << "\n\n";
        std::copy(generator.m_errors.begin(), generator.m_errors.end(), std::ostream_iterator<std::string>(stream, "\n"));
    }

    return stream;
}

Lexer::Generator::Error::Error(const char* new_error, std::size_t new_column)
    : error(new_error), column(new_column)
{
}
