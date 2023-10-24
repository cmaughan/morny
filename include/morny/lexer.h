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
    String,
    Identifier,
    Int,
    Float,
    Double,

    // Operators
    LeftParen,
    RightParen,
    LeftBracket,
    RightBracket,
    Semicolon,
    If,
    Else,
    For,
    Assign
};

struct Identifier
{
    std::string str;
};

struct Token
{
    TokenType type;
    std::variant<std::monostate, std::string, Identifier, int64_t, float, double> value;
};

struct LexToken
{
    Token token;
    LexPos start;
    LexPos end;
};

std::vector<LexToken> Lex(const std::string& val);
std::string Dump(const std::vector<LexToken>& tokens);
std::string Dump(Token& token);
std::string Dump(TokenType& tokenType);

} // namespace lexer