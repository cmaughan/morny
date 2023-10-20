#include <morny/lexer.h>

namespace Lexer
{

inline std::ostream& operator<<(std::ostream& str, const TokenType& type)
{
    switch (type)
    {
    case TokenType::LeftParen:
        str << "(";
        break;
    case TokenType::RightParen:
        str << ")";
        break;
    }
    return str;
}

auto dump(const Token& tok, std::ostringstream& str) -> std::ostringstream&
{
    if (std::holds_alternative<int64_t>(tok))
        str << "int64:" << std::get<int64_t>(tok);
    else if (std::holds_alternative<double>(tok))
        str << "double:" << std::get<double>(tok);
    else if (std::holds_alternative<float>(tok))
        str << "float:" << std::get<float>(tok);
    else if (std::holds_alternative<TokenType>(tok))
        str << "TokenType:" << std::get<TokenType>(tok);
    else
        assert(!"Foo");
    return str;
}

std::string Dump(const std::vector<LexToken>& tokens)
{
    std::ostringstream out;
    bool first = true;
    for (auto& tok : tokens)
    {
        if (!first)
        {
            out << ", ";
        }
        first = false;
        dump(tok.token, out);
    }
    return out.str();
}

struct Lexer
{
    std::string str;
    LexPos start;
    LexPos current;
    std::vector<LexToken> tokens;
    std::string::iterator itr;

    auto next() -> char
    {
        if (itr == str.end())
        {
            return 0;
        }
        auto c = *itr++;
        switch (c)
        {
        case '\n':
            current.line++;
            current.column = 1;
            current.index++;
        default:
            current.column++;
            current.index++;
        }
        return c;
    }

    auto peek() -> char
    {
        if (itr == str.end())
        {
            return 0;
        }
        return *itr;
    }

    template <class T>
    auto emit(T t)
    {
        tokens.push_back(LexToken{ .token = t, .start = start, .end = current });
        start = current;
    }

    auto emit_float(const std::string& val)
    {
        try
        {
            auto f = std::stof(val);
            emit(f);
        }
        catch (std::exception& ex)
        {
            // TODO: Error
        }
    }

    auto emit_double(const std::string& val)
    {
        try
        {
            auto f = std::stod(val);
            emit(f);
        }
        catch (std::exception& ex)
        {
            // TODO: Error
        }
    }
    auto gather(std::string& str, const std::function<bool(char c)>& fnKeep) -> std::string
    {
        while (fnKeep(peek()))
        {
            str.push_back(peek());
            next();
        }
        return str;
    }

    void lex(const std::string& input)
    {
        str = input;
        start = current = LexPos{
            .line = 1,
            .column = 1,
            .index = 0
        };
        itr = str.begin();

        static const std::unordered_set<char> OPERATORS = []() {
            const std::string ops = "!$%&*+-./|~";
            std::unordered_set<char> set{
                ops.begin(), ops.end()
            };
            return set;
        }();

#define MATCH(a, b) \
    case a:         \
        emit(b);    \
        break;

        while (auto c = next())
        {
            switch (c)
            {
                MATCH('(', TokenType::LeftParen);
                MATCH(')', TokenType::RightParen);

            default:
            {
                if (c == ' ')
                {
                    continue;
                }
                else if (std::isdigit(c))
                {
                    std::string val(1, c);
                    gather(val, [&](auto ch) { return std::isdigit(ch); });
                    if (peek() == '.')
                    {
                        val.push_back('.');
                        next();
                        gather(val, [&](auto ch) { return std::isdigit(ch); });

                        switch (peek())
                        {
                        case 'f':
                            next();
                            emit_float(val);
                            break;
                        default:
                            emit_double(val);
                            break;
                        }
                    }
                    else
                    {
                        switch (peek())
                        {
                        case 'f':
                            next();
                            emit_float(val);
                            break;
                        default:
                            emit(int64_t(std::stoll(val)));
                        }
                    }
                }
                else if (OPERATORS.find(c) != OPERATORS.end())
                {
                    char last = c;
                    std::string ops;
                    gather(ops, [&](auto ch) -> bool {
                        return OPERATORS.find(ch) != OPERATORS.end();
                    });
                    if (!ops.empty())
                    {
                        last = ops.back();
                    }
                }
                else
                {
                    // TODO
                    assert(!"UNKNOWN");
                }
            }
            break;
            }
        }
    }
};

std::vector<LexToken> Lex(const std::string& str)
{
    Lexer l;
    l.lex(str);
    return std::move(l.tokens);
}

} // namespace Lexer
