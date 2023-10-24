#pragma once

#include <variant>
#include <memory>
#include <morny/lexer.h>

namespace Parser
{

enum class ExprType
{
    Null,
    Integer,
    Double,
    Float,
    Identifier,
    Lambda,
    Sequence
};

struct Expr;
using ExprPtr = std::shared_ptr<Expr>;

struct Expr
{
    static ExprPtr Create(ExprType type = ExprType::Null)
    {
        return std::make_shared<Expr>(type, int64_t(0));
    }

    template <class T>
    static ExprPtr Create(ExprType type, T val)
    {
        return std::make_shared<Expr>(type, val);
    }

    template <class T>
    explicit Expr(ExprType t, T val)
        : type(t)
        , value(val)
    {
    }

    ExprType type = ExprType::Null;

    std::variant<std::string, int64_t, float, double, ExprPtr, std::vector<ExprPtr>> value;
};

std::pair<ExprPtr,std::vector<std::string>> Parse(const std::vector<Lexer::LexToken>& tokens);

auto operator<<(std::ostream& str, const ExprPtr& expr) -> std::ostream&;

} // namespace Parser
