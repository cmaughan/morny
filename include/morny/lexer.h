#pragma once

#include <cassert>
#include <functional>
#include <sstream>
#include <string>
#include <unordered_set>
#include <variant>

namespace Lexer
{

struct LexPos
{
    size_t line;
    size_t column;
    size_t index;
};

enum class TokenType
{
    LeftParen,
    RightParen
};

using Token = std::variant<std::string, int64_t, float, double, TokenType>;

struct LexToken
{
    Token token;
    LexPos start;
    LexPos end;
};

std::vector<LexToken> Lex(const std::string& val);
std::string Dump(const std::vector<LexToken>& tokens);

} // namespace lexer