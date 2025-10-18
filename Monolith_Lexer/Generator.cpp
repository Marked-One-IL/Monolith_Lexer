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
    std::size_t linesCount = 1;
    std::string_view currentLine;
    if (auto opt2 = Lexer::Generator::extractUntilNewLine(view)) currentLine = opt2.value();
    else return;

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
            if (auto opt = Lexer::Generator::extractOctLiteral(view))
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
                if (auto opt2 = Lexer::Generator::extractUntilNewLine(view)) currentLine = opt2.value();
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
            if (auto opt2 = Lexer::Generator::extractUntilNewLine(view)) currentLine = opt2.value();
            linesCount++;
        }
        catch (const Lexer::Generator::Error& error)
        {
            std::string_view fixedLine = currentLine;
            Lexer::Generator::skipSpaces(fixedLine);
            std::size_t distance = (view.data() - fixedLine.data()) + error.column;
            this->m_errors.emplace_back(std::format("At line: {}\nError: {}\n{}\n{:{}s}^", linesCount, error.error, fixedLine, "", distance));
            
            Lexer::Generator::incrementToNextLine(view);
            if (auto opt2 = Lexer::Generator::extractUntilNewLine(view)) currentLine = opt2.value();
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
void Lexer::Generator::incrementToNextLine(std::string_view& view)
{
    while (not view.empty() and (view.front() != '\n')) view.remove_prefix(1);
    if (not view.empty()) view.remove_prefix(1);
}

std::optional<std::size_t> Lexer::Generator::extractSpacesLevel(const std::string_view& view)
{
    // Forced Code.
    std::string_view fixedView;
    if (auto opt = Lexer::Generator::extractUntilNewLine(view)) fixedView = opt.value();
    else return std::nullopt;

    // Scan.
    std::size_t level = 0;
    while (not fixedView.empty() and (fixedView.front() == ' ' or fixedView.front() == '\t'))
    {
        level += fixedView.front() == ' ';
        level += (fixedView.front() == '\t') * 2;
        fixedView.remove_prefix(1);
    }
    if (level == 0) return std::nullopt;

    // Return.
    return level;
}
std::optional<std::string_view> Lexer::Generator::extractUntilNewLine(const std::string_view& view)
{
    // Scan.
    std::size_t newLinePos = view.find('\n');
    std::string_view content = view;
    if (newLinePos != std::string_view::npos)
    {
        content = view.substr(0, newLinePos);
    }
    if (content.empty()) return std::nullopt;

    // Return.
    return content;
}
std::optional<std::string_view> Lexer::Generator::extractUntilNotAlnum(const std::string_view& view)
{
    // Forced Code.
    std::string_view fixedView;
    if (auto opt = Lexer::Generator::extractUntilNewLine(view)) fixedView = opt.value();
    else return std::nullopt;
    
    // Scan.
    std::size_t i = 0;
    for (i = 0; i < fixedView.size(); i++)
    {
        unsigned char c = static_cast<unsigned char>(fixedView[i]);
        if (not std::isalnum(c) and c != '_')
        {
            break;
        }
    }
    if (i == 0) return std::nullopt;

    // Return.
    return fixedView.substr(0, i);
}

std::optional<Lexer::Token> Lexer::Generator::extractString3Literal(std::string_view& view)
{
    // Early return.
    if (not view.starts_with("\"\"\"")) return std::nullopt;

    // Scan.
    std::size_t endPos = view.find("\"\"\"", std::strlen("\"\"\""));
    if (endPos == std::string_view::npos)
    {
        throw Lexer::Generator::Error("Triple string literal does not end");
    }
    endPos += std::strlen("\"\"\"");

    // Return.
    std::string_view content = view.substr(0, endPos);
    view.remove_prefix(endPos);
    return Lexer::Token(Lexer::Tag::STRING3_LITERAL, content);
}
std::optional<Lexer::Token> Lexer::Generator::extractStringLiteral(std::string_view& view)
{
    // Forced Code.
    std::string_view fixedView;
    if (auto opt = Lexer::Generator::extractUntilNewLine(view)) fixedView = opt.value();
    else return std::nullopt;

    // Early return.
    if (not (fixedView.front() == '\"')) return std::nullopt;

    // Scan.
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

    // Return.
    std::string_view content = fixedView.substr(0, endPos);
    view.remove_prefix(endPos);
    return Lexer::Token(Lexer::Tag::STRING_LITERAL, content);
}
std::optional<Lexer::Token> Lexer::Generator::extractCharLiteral(std::string_view& view)
{
    // Forced Code.
    std::string_view fixedView;
    if (auto opt = Lexer::Generator::extractUntilNewLine(view)) fixedView = opt.value();
    else return std::nullopt;

    // Early return.
    if (not (fixedView.front() == '\'')) return std::nullopt;

    // Scan.
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
    if (fixedView.substr(0, endPos).size() >= 4) throw Lexer::Generator::Error("Character literal is more than character");

    // Return.
    std::string_view content = fixedView.substr(0, endPos);
    view.remove_prefix(endPos);
    return Lexer::Token(Lexer::Tag::CHAR_LITERAL, content);
}
std::optional<Lexer::Token> Lexer::Generator::extractHexLiteral(std::string_view& view)
{
    // Forced Code.
    std::size_t totalSize = 0;
    std::string_view fixedView;
    if (auto opt = Lexer::Generator::extractUntilNewLine(view)) fixedView = opt.value();
    else return std::nullopt;

    // Early errors/return.
    if ((not fixedView.starts_with("0x") and not fixedView.starts_with("0X"))) return std::nullopt;
    if (fixedView.size() <= 2) throw Lexer::Generator::Error("Invalid hexadecimal literal");
    totalSize += std::strlen("0x");

    // Scan.
    std::string_view afterSignature = fixedView.substr(totalSize);
    for (afterSignature; not afterSignature.empty(); afterSignature.remove_prefix(1))
    {
        totalSize++;

        unsigned char c = static_cast<unsigned char>(afterSignature.front());

        if (not std::isalnum(c)) break;
        if (std::string_view("0123456789ABCDEFabcdef").find(c) == std::string_view::npos) throw Lexer::Generator::Error("Invalid hexadecimal literal", totalSize);
    }

    // Return.
    std::string_view content = fixedView.substr(0, totalSize);
    view.remove_prefix(content.size());
    return Lexer::Token(Lexer::Tag::HEX_LITERAL, content);
}

std::optional<Lexer::Token> Lexer::Generator::extractBinLiteral(std::string_view& view)
{
    std::size_t totalSize = 0;
    std::string_view fixedView;
    if (auto opt = Lexer::Generator::extractUntilNewLine(view)) fixedView = opt.value();
    else return std::nullopt;

    // Early return.
    if ((not fixedView.starts_with("0b") and not fixedView.starts_with("0B"))) return std::nullopt;
    if (fixedView.size() <= 2) throw Lexer::Generator::Error("Invalid binary literal");
    totalSize += std::strlen("0b");

    // Scan.
    std::string_view afterSignature = fixedView.substr(totalSize);
    for (afterSignature; not afterSignature.empty(); afterSignature.remove_prefix(1))
    {
        totalSize++;

        unsigned char c = static_cast<unsigned char>(afterSignature.front());

        if (not std::isalnum(c)) break;
        if (std::string_view("01").find(c) == std::string_view::npos) throw Lexer::Generator::Error("Invalid binary literal", totalSize);
    }

    // Return.
    std::string_view content = fixedView.substr(0, totalSize);
    view.remove_prefix(content.size());
    return Lexer::Token(Lexer::Tag::BIN_LITERAL, content);
}
std::optional<Lexer::Token> Lexer::Generator::extractOctLiteral(std::string_view& view)
{
    // Forced Code.
    std::size_t totalSize = 0;
    std::string_view fixedView;
    if (auto opt = Lexer::Generator::extractUntilNewLine(view)) fixedView = opt.value();
    else return std::nullopt;

    // Early return.
    if ((not fixedView.starts_with("0o") and not fixedView.starts_with("0O"))) return std::nullopt;
    if (fixedView.size() <= 2) throw Lexer::Generator::Error("Invalid octal literal");
    totalSize += std::strlen("0o");

    // Scan.
    std::string_view afterSignature = fixedView.substr(totalSize);
    for (afterSignature; not afterSignature.empty(); afterSignature.remove_prefix(1))
    {
        totalSize++;

        unsigned char c = static_cast<unsigned char>(afterSignature.front());

        if (not std::isalnum(c)) break;
        if (std::string_view("01234567").find(c) == std::string_view::npos) throw Lexer::Generator::Error("Invalid octal literal", totalSize);
    }

    // Return.
    std::string_view content = fixedView.substr(0, totalSize);
    view.remove_prefix(content.size());
    return Lexer::Token(Lexer::Tag::OCT_LITERAL, content);
}
std::optional<Lexer::Token> Lexer::Generator::extractSciLiteral(std::string_view& view)
{
    // Forced Code.
    std::size_t totalSize = 0;
    std::string_view fixedView;
    if (auto opt = Lexer::Generator::extractUntilNewLine(view)) fixedView = opt.value();
    else return std::nullopt;

    // Early return.
    if (fixedView.size() <= 2) return std::nullopt;
    if (fixedView.front() == '.' and not std::isdigit(static_cast<unsigned char>(fixedView[1]))) return std::nullopt; // '.a'
    if (fixedView[1] == '.' and not std::isdigit(static_cast<unsigned char>(fixedView.front()))) return std::nullopt; // 'a.'
    if (fixedView.front() != '.' and not std::isdigit(static_cast<unsigned char>(fixedView.front()))) return std::nullopt; // 'a'

    // Scan 1.
    bool seenE = false;
    bool seenDot = false;
    for (std::size_t i = 0; i < fixedView.size(); i++)
    {
        totalSize++;

        unsigned char c = std::tolower(static_cast<unsigned char>(fixedView[i]));

        if (c == 'e')
        {
            seenE = true;
            break;
        }

        if (c == '.')
        {
            if (seenDot)
            {
                throw Lexer::Generator::Error("Invalid float literal", totalSize);
            }
            seenDot = true;
            continue;
        }

        if (std::isalpha(c)) throw Lexer::Generator::Error("Invalid float literal", totalSize);
        if (not std::isalnum(c)) break;
    }
    if (not seenE) return std::nullopt;

    std::string_view afterE = fixedView.substr(totalSize);
    if (afterE.empty()) throw Lexer::Generator::Error("Invalid scientific notation literal", totalSize);

    // Scan 2.
    if (afterE.front() == '+' or afterE.front() == '-')
    {
        totalSize++;
    }
    for (afterE; not afterE.empty(); afterE.remove_prefix(1))
    {
        totalSize++;

        unsigned char c = std::tolower(static_cast<unsigned char>(afterE.front()));

        if (std::isalpha(c)) throw Lexer::Generator::Error("Invalid scientific notation literal", totalSize);
        if (not std::isalnum(c)) break;
    }

    // Return.
    std::string_view content = fixedView.substr(0, totalSize);
    view.remove_prefix(content.size());
    return Lexer::Token(Lexer::Tag::SCI_LITERAL, content);
}
std::optional<Lexer::Token> Lexer::Generator::extractFloatLiteral(std::string_view& view)
{
    // Forced Code.
    std::string_view fixedView;
    if (auto opt = Lexer::Generator::extractUntilNewLine(view)) fixedView = opt.value();
    else return std::nullopt;

    // Early return.
    if (fixedView.size() <= 1) return std::nullopt;
    if (fixedView.front() == '.' and not std::isdigit(static_cast<unsigned char>(fixedView[1]))) return std::nullopt; // '.a'
    if (fixedView[1] == '.' and not std::isdigit(static_cast<unsigned char>(fixedView.front()))) return std::nullopt; // 'a.'
    if (fixedView.front() != '.' and not std::isdigit(static_cast<unsigned char>(fixedView.front()))) return std::nullopt; // 'a'

    // Scan.
    bool seenDot = false;
    std::size_t i = 0;
    for (i = 0; i < fixedView.size(); i++)
    {
        unsigned char c = static_cast<unsigned char>(fixedView[i]);

        if (c == '.')
        {
            if (seenDot)
            {
                throw Lexer::Generator::Error("Invalid float literal", i);
            }
            seenDot = true;
            continue;
        }

        if (std::isalpha(c)) throw Lexer::Generator::Error("Invalid float literal", i);
        if (not std::isalnum(c)) break;
    }
    if (not seenDot) return std::nullopt;

    // Return.
    std::string_view content = fixedView.substr(0, i);
    view.remove_prefix(content.size());
    return Lexer::Token(Lexer::Tag::FLOAT_LITERAL, content);
}
std::optional<Lexer::Token> Lexer::Generator::extractIntLiteral(std::string_view& view)
{
    // Forced Code.
    std::string_view fixedView;
    if (auto opt = Lexer::Generator::extractUntilNewLine(view)) fixedView = opt.value();
    else return std::nullopt;

    // Early return.
    if (not std::isdigit(static_cast<unsigned char>(fixedView.front()))) return std::nullopt;

    // Scan.
    std::size_t i = 0;
    for (i = 0; i < fixedView.size(); i++)
    {
        unsigned char c = static_cast<unsigned char>(fixedView[i]);

        if (std::isalpha(c)) throw Lexer::Generator::Error("Invalid integer literal", i);
        if (not std::isalnum(c)) break;
    }

    // Return.
    std::string_view content = fixedView.substr(0, i);
    view.remove_prefix(content.size());
    return Lexer::Token(Lexer::Tag::INT_LITERAL, content);
}
std::optional<Lexer::Token> Lexer::Generator::extractBoolLiteral(std::string_view& view)
{
    // Forced Code.
    std::string_view fixedView;
    if (auto opt = Lexer::Generator::extractUntilNewLine(view)) fixedView = opt.value();
    else return std::nullopt;

    // Early return.
    std::string_view possibleNone;
    if (auto opt = Lexer::Generator::extractUntilNotAlnum(fixedView)) possibleNone = opt.value();
    return std::nullopt;

    // Scan.
    if (possibleNone == "True" or possibleNone == "False")
    {
        // Return.
        std::string_view content = possibleNone;
        view.remove_prefix(content.size());
        return Lexer::Token(Lexer::Tag::NONE_LITERAL, content);
    }

    // Return default.
    return std::nullopt;
}
std::optional<Lexer::Token> Lexer::Generator::extractNoneLiteral(std::string_view& view)
{
    // Forced Code.
    std::string_view fixedView;
    if (auto opt = Lexer::Generator::extractUntilNewLine(view)) fixedView = opt.value();
    else return std::nullopt;

    // Early return.
    std::string_view possibleNone;
    if (auto opt = Lexer::Generator::extractUntilNotAlnum(fixedView)) possibleNone = opt.value();
    return std::nullopt;

    // Scan.
    if (possibleNone == "None")
    {
        // Return.
        std::string_view content = possibleNone;
        view.remove_prefix(content.size());
        return Lexer::Token(Lexer::Tag::NONE_LITERAL, content);
    }

    // Return default.
    return std::nullopt;
}
std::optional<Lexer::Token> Lexer::Generator::extractSymbol(std::string_view& view, bool& skipIndentFlag)
{
    // Forced Code.
    std::string_view fixedView;
    if (auto opt = Lexer::Generator::extractUntilNewLine(view)) fixedView = opt.value();
    else return std::nullopt;

    // Data.
    static constexpr auto wordSymbols = std::to_array<std::string_view>(
    {
        "and", "or", "not", "is", "as"
    });
    static constexpr auto punctuator3Symbols = std::to_array<std::string_view>(
    {
        "<<=", ">>=", "..."
    });
    static constexpr auto punctuator2Symbols = std::to_array<std::string_view>(
    {
        "++", "+=", "--", "-=", "*=", "/=", "%=", ">=", "<=", ">>", "<<", "|=", "&=", "^=", "==", "!=", "->", "::"
    });
    static constexpr auto punctuator1symbols = std::to_array<char>(
    {
        '+', '-', '*', '/', '%', '<', '>', '|', '&', '^', '~', '=', '.', ',', '(', ')', '[', ']', '?', ':'
    });

    // Scan 1.
    std::string_view possibleWordSymbol;
    if (auto opt = Lexer::Generator::extractUntilNotAlnum(fixedView))
    {
        possibleWordSymbol = opt.value();
        for (auto& wordSymbol : wordSymbols)
        {
            if (possibleWordSymbol == wordSymbol)
            {
                // Return.
                std::string_view content = possibleWordSymbol;
                view.remove_prefix(content.size());
                return Token(Lexer::Tag::SYMBOL, content);
            }
        }
        return std::nullopt;
    }
    // Scan 2.
    for (auto& punctuator3Symbol : punctuator3Symbols)
    {
        if (fixedView.starts_with(punctuator3Symbol))
        {
            constexpr std::size_t OFFSET = 3;

            // Return.
            std::string_view content = fixedView.substr(0, OFFSET);
            view.remove_prefix(OFFSET);
            return Token(Lexer::Tag::SYMBOL, content);
        }
    }
    // Scan 3.
    for (auto& punctuator2Symbol : punctuator2Symbols)
    {
        if (fixedView.starts_with(punctuator2Symbol))
        {
            constexpr std::size_t OFFSET = 2;

            // Return.
            std::string_view content = fixedView.substr(0, OFFSET);
            view.remove_prefix(OFFSET);
            return Token(Lexer::Tag::SYMBOL, content);
        }
    }
    // Scan 4.
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

            // Return.
            std::string_view content = fixedView.substr(0, OFFSET);
            view.remove_prefix(OFFSET);
            return Token(Lexer::Tag::SYMBOL, content);
        }
    }

    // Return default.
    return std::nullopt;
}
std::optional<Lexer::Token> Lexer::Generator::extractKeyword(std::string_view& view)
{
    // Forced code.
    std::string_view fixedView;
    if (auto opt = Lexer::Generator::extractUntilNewLine(view)) fixedView = opt.value();
    else return std::nullopt;

    // Data.
    static constexpr auto keywords = std::to_array<std::string_view>(
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
    });

    // Early return.
    std::string_view possibleKeyword;
    if (auto opt = Lexer::Generator::extractUntilNotAlnum(fixedView)) possibleKeyword = opt.value();
    else return std::nullopt;

    // Scan.
    for (std::size_t i = 0; i < keywords.size(); i++)
    {
        if (keywords[i] == possibleKeyword)
        {
            view.remove_prefix(possibleKeyword.size());
            return Token(Lexer::Tag::KEYWORD, possibleKeyword);
        }
    }

    // Return.
    return std::nullopt;
}
std::optional<Lexer::Token> Lexer::Generator::extractNewLine(std::string_view& view)
{
    // Scan.
    if (view.front() == '\n')
    {
        view.remove_prefix(std::strlen("\n"));
        return Lexer::Token(Lexer::Tag::NEW_LINE);
    }

    return std::nullopt;
}
std::optional<std::vector<Lexer::Token>> Lexer::Generator::extractInDedent(std::string_view& view, std::stack<std::size_t>& identLevels)
{
    // Forced code.
    std::string_view fixedView;
    if (auto opt = Lexer::Generator::extractUntilNewLine(view)) fixedView = opt.value();
    else return std::nullopt;

    // Early return.
    std::size_t newLevel;
    if (auto opt = Lexer::Generator::extractSpacesLevel(fixedView)) newLevel = opt.value();
    else return std::nullopt;

    // Scan 1.
    if (identLevels.empty() or newLevel > identLevels.top()) {
        identLevels.push(newLevel);
        return std::vector<Lexer::Token>{ Lexer::Token(Lexer::Tag::INDENT) };
    }
    else if (identLevels.top() == newLevel)
    {
        return std::nullopt;
    }

    // Scan 2.
    std::vector<Lexer::Token> dedents;
    while (not identLevels.empty() and identLevels.top() > newLevel) 
    {
        identLevels.pop();
        dedents.emplace_back(Lexer::Tag::DEDENT);
    }
    if (identLevels.empty() or identLevels.top() != newLevel) throw Lexer::Generator::Error("Indent (spacing) doesn't match previous indents");

    // Return.
    return dedents;
}
std::optional<Lexer::Token> Lexer::Generator::extractIdentifier(std::string_view& view)
{
    // Forced code.
    std::string_view fixedView;
    if (auto opt = Lexer::Generator::extractUntilNewLine(view)) fixedView = opt.value();
    else return std::nullopt;

    // Early return/errors.
    if (fixedView.front() == '#') return std::nullopt;
    if (not std::isalnum(static_cast<unsigned char>(fixedView.front()))) throw Lexer::Generator::Error("Invalid character");

    // Extraction.
    std::string_view content;
    if (auto opt = Lexer::Generator::extractUntilNotAlnum(fixedView)) content = opt.value();
    else return std::nullopt;

    // Incrementation & return.
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
