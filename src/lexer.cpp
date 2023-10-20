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

auto Dump(const Token& tok, std::ostringstream& str) -> std::ostringstream&
{
    if (std::holds_alternative<int64_t>(tok))
        str << "int64:" << std::get<int64_t>(tok);
    else if (std::holds_alternative<double>(tok))
        str << "double:" << std::get<double>(tok);
    else if (std::holds_alternative<float>(tok))
        str << "float:" << std::get<float>(tok);
    else if (std::holds_alternative<TokenType>(tok))
        str << "TokenType:" << std::get<TokenType>(tok);
    else if (std::holds_alternative<Identifier>(tok))
        str << "Identifier:" << std::get<Identifier>(tok).str;
    else
        assert(!"Foo");
    return str;
}

struct Lexer
{
    std::string str;
    LexPos start;
    LexPos current;
    std::vector<LexToken> tokens;
    std::string::iterator itr;

    auto Next() -> char
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

    auto Peek() -> char
    {
        if (itr == str.end())
        {
            return 0;
        }
        return *itr;
    }

    template <class T>
    auto Emit(T t)
    {
        tokens.push_back(LexToken{ .token = t, .start = start, .end = current });
        start = current;
    }

    auto EmitFloat(const std::string& val)
    {
        try
        {
            auto f = std::stof(val);
            Emit(f);
        }
        catch (std::exception& ex)
        {
            // TODO: Error
        }
    }

    auto EmitDouble(const std::string& val)
    {
        try
        {
            auto f = std::stod(val);
            Emit(f);
        }
        catch (std::exception& ex)
        {
            // TODO: Error
        }
    }

    auto Gather(std::string& str, const std::function<bool(char c)>& fnKeep) -> std::string
    {
        while (fnKeep(Peek()))
        {
            str.push_back(Peek());
            Next();
        }
        return str;
    }

    void Lex(const std::string& input)
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
        Emit(b);    \
        break;

        while (auto c = Next())
        {
            switch (c)
            {
                MATCH('(', TokenType::LeftParen);
                MATCH(')', TokenType::RightParen);
            case ' ':
            case '\n':
                start = current;
                break;

            default:
            {
                // Ignore white space
                if (std::isblank(c))
                {
                    continue;
                }
                else if (std::isdigit(c))
                {
                    std::string val(1, c);
                    Gather(val, [&](auto ch) { return std::isdigit(ch); });
                    if (Peek() == '.')
                    {
                        val.push_back('.');
                        Next();
                        Gather(val, [&](auto ch) { return std::isdigit(ch); });

                        switch (Peek())
                        {
                        case 'f':
                            Next();
                            EmitFloat(val);
                            break;
                        default:
                            EmitDouble(val);
                            break;
                        }
                    }
                    else
                    {
                        switch (Peek())
                        {
                        case 'f':
                            Next();
                            EmitFloat(val);
                            break;
                        default:
                            Emit(int64_t(std::stoll(val)));
                        }
                    }
                }
                else if (std::isalpha(c) || c == '_')
                {
                    // Collect the identifier
                    std::string ident(1, c);
                    Gather(ident, [&](auto ch) -> bool {
                        return std::isalnum(ch);
                    });
                    Emit(Identifier{ident});
                }
                else if (OPERATORS.find(c) != OPERATORS.end())
                {
                    char last = c;
                    std::string ops;
                    Gather(ops, [&](auto ch) -> bool {
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
    l.Lex(str);
    return std::move(l.tokens);
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
        Dump(tok.token, out);
    }
    return out.str();
}

} // namespace Lexer
