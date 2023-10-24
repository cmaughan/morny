#include <cstdint>
#include <cstdio>
#include <iostream>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include <fmt/format.h>

#include <morny/parser.h>

using namespace Lexer;

namespace Parser
{

// For console debug
auto operator<<(std::ostream& str, const ExprPtr& expr) -> std::ostream&
{
    switch (expr->type)
    {
    case ExprType::Null:
        str << "Null";
        break;
    case ExprType::Sequence:
    {
        str << "[";
        bool first = true;
        for (auto& pEx : std::get<std::vector<ExprPtr>>(expr->value))
        {
            if (!first)
                str << ", ";
            first = false;
            str << pEx;
        }
        str << "]";
    }
    break;
    case ExprType::Double:
        str << "Double:" << std::get<double>(expr->value);
        break;
    case ExprType::Float:
        str << "Float:" << std::get<float>(expr->value);
        break;
    case ExprType::Integer:
        str << "Integer:" << std::get<int64_t>(expr->value);
        break;
    case ExprType::Identifier:
        str << "Identifier:" << std::get<std::string>(expr->value);
        break;
    case ExprType::Lambda:
        break;
    default:
        break;
    }
    return str;
}

struct Parser
{
    std::vector<LexToken> tokens;
    uint32_t index;
    std::vector<std::string> messages;

    auto Error(const std::string& str)
    {
        index = tokens.size();
        messages.push_back(str);
    }

    auto Peek() -> LexToken*
    {
        if (index >= tokens.size())
        {
            return nullptr;
        }
        return &tokens[index];
    }

    auto Advance() -> void
    {
        index++;
    }

    auto Require(TokenType type) -> bool
    {
        auto v = Peek();
        if (v->token.type != type)
        {
            Error(fmt::format("Required: {}, found {}", Dump(type), Dump(v->token)));
            return false;
        }
        Advance();
        return true;
    }

    auto Atom() -> ExprPtr
    {
        auto t = Peek();
        if (!t)
        {
            Error("Invalid end of parse");
            return nullptr;
        }
        switch (t->token.type)
        {
        case TokenType::Int:
        {
            Advance();
            return Expr::Create(ExprType::Integer, std::get<int64_t>(t->token.value));
        }
        break;
        case TokenType::Double:
        {
            Advance();
            return Expr::Create(ExprType::Double, std::get<double>(t->token.value));
        }
        break;
        case TokenType::Float:
        {
            Advance();
            return Expr::Create(ExprType::Double, std::get<float>(t->token.value));
        }
        break;
        case TokenType::Identifier:
        {
            Advance();
            return Expr::Create(ExprType::Identifier, std::get<Identifier>(t->token.value).str);
        }
        break;
        case TokenType::LeftParen:
        {
            Advance();
            auto expr = Expression(true);
            std::cout << expr;
            Require(TokenType::RightParen);
            return expr;
        }
        break;
        case TokenType::LeftBracket:
        {
            Advance();
            // TODO: Collect list
            assert(!"Not done");
            Require(TokenType::RightBracket);
            return ExprPtr{};
        }
        break;
        case TokenType::RightBracket:
        case TokenType::RightParen:
        {
            Error("Unexpected parenthesis");
        }
        break;
        }
        Error("Unexpected atom");
        return nullptr;
    }

    auto Operand() -> ExprPtr
    {
        auto atom = Atom();
        return atom;
    }

    auto Chain() -> ExprPtr
    {
        auto op = Operand();
        return op;
    }

    auto CommaSeparated() -> std::vector<ExprPtr>
    {
        auto chain = Chain();
        if (!chain)
        {
            return std::vector<ExprPtr>();
        }
        return std::vector<ExprPtr>{ chain };
    }

    auto Pattern() -> ExprPtr
    {
        auto sep = CommaSeparated();
        if (sep.empty())
        {
            return nullptr;
        }
        return sep[0];
    }

    auto Assignment() -> ExprPtr
    {
        auto pattern = Pattern();
        if (!pattern)
        {
            return nullptr;
        }
        return pattern;
    }

    auto Expression(bool required = false) -> ExprPtr
    {

        std::vector<ExprPtr> expressions;
        while (auto expr = Assignment())
        {
            expressions.push_back(expr);
        }

        if (required && expressions.empty())
        {
            Error("Expected expression");
            return nullptr;
        }
        return Expr::Create(ExprType::Sequence, expressions);
    }
};

std::pair<ExprPtr, std::vector<std::string>> Parse(const std::vector<LexToken>& tokens)
{
    Parser p{ .tokens = tokens };
    return std::make_pair(p.Expression(), p.messages);
}

} // namespace Parser