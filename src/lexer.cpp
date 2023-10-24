#include <morny/lexer.h>

namespace Lexer
{

auto operator<<(std::ostream& str, const TokenType& type) -> std::ostream&
{
    switch (type)
    {
    case TokenType::String:
        str << "String";
        break;
    case TokenType::Int:
        str << "Int";
        break;
    case TokenType::Double:
        str << "Double";
        break;
    case TokenType::Float:
        str << "Float";
        break;
    case TokenType::Identifier:
        str << "Identifier";
        break;
    case TokenType::LeftParen:
        str << "{";
        break;
    case TokenType::RightParen:
        str << "}";
        break;
    case TokenType::LeftBracket:
        str << "(";
        break;
    case TokenType::RightBracket:
        str << ")";
        break;
    case TokenType::If:
        str << "If";
        break;
    case TokenType::Else:
        str << "Else";
        break;
    case TokenType::For:
        str << "For";
        break;
    default:
        str << "FIXME";
    }
    return str;
}

auto operator<<(std::ostream& str, const Token& tok) -> std::ostream&
{
    switch (tok.type)
    {
    case TokenType::String:
        str << tok.type << ": " << std::get<std::string>(tok.value);
        break;
    case TokenType::Int:
        str << tok.type << ": " << std::get<int64_t>(tok.value);
        break;
    case TokenType::Double:
        str << tok.type << ": " << std::get<double>(tok.value);
        break;
    case TokenType::Float:
        str << tok.type << ": " << std::get<float>(tok.value);
        break;
    case TokenType::Identifier:
        str << tok.type << ": " << std::get<Identifier>(tok.value).str;
        break;
    default:
        str << tok.type;
        break;
    }
    return str;
}

std::string Dump(Token& token)
{
    std::ostringstream str;
    str << token;
    return str.str();
}

std::string Dump(TokenType& tokenType)
{
    std::ostringstream str;
    str << tokenType;
    return str.str();
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

    auto Emit(TokenType t)
    {
        tokens.push_back(LexToken{ .token = Token{ t }, .start = start, .end = current });
        start = current;
    }

    auto EmitFloat(const std::string& val)
    {
        try
        {
            auto f = std::stof(val);
            Emit(Token{ TokenType::Float, f });
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
            Emit(Token{ TokenType::Double, f });
        }
        catch (std::exception& ex)
        {
            // TODO: Error
        }
    }

    auto Gather(std::string& str, const std::function<bool(char c)>& fnKeep) -> std::string
    {
        while (Peek() && fnKeep(Peek()))
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
            const std::string ops = "!$%&*+-./|~<=>";
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
                MATCH('{', TokenType::LeftParen);
                MATCH('}', TokenType::RightParen);
                MATCH('(', TokenType::LeftBracket);
                MATCH(')', TokenType::RightBracket);
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
                            Emit(Token{ TokenType::Int, int64_t(std::stoll(val)) });
                        }
                    }
                }
                else if (std::isalpha(c) || c == '_')
                {
                    // Collect the identifier
                    std::string ident(1, c);
                    Gather(ident, [&](auto ch) -> bool {
                        return std::isalnum(ch) || ch == '_';
                    });

#define MATCH_IDENT(a, t) \
    else if (ident == a)  \
    {                     \
        Emit(t);          \
    }
                    if (ident == "if")
                    {
                        Emit(TokenType::If);
                    }
                    MATCH_IDENT("if", TokenType::If)
                    MATCH_IDENT("else", TokenType::Else)
                    MATCH_IDENT("for", TokenType::For)
                    else
                    {
                        Emit(Token{ TokenType::Identifier, Identifier{ ident } });
                    }
                }
                else if (OPERATORS.find(c) != OPERATORS.end())
                {
                    // Collect the operators together
                    std::string ops(1, c);
                    Gather(ops, [&](auto ch) -> bool {
                        return OPERATORS.find(ch) != OPERATORS.end();
                    });
                    if (ops == "=")
                    {
                        Emit(Token{ TokenType::Assign });
                    }
                    else
                    {
                        Emit(Token{ TokenType::Identifier, Identifier{ ops } });
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
        out << tok.token;
    }
    return out.str();
}

} // namespace Lexer
