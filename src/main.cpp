#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include <fmt/format.h>
#include <variant>
#include <memory>
#include <cassert>
#include <functional>

#include <iostream>

#include <morny/lexer.h>

using namespace lexer;
/*
enum class ObjectType
{
    None,
    Int,
    Double,
    String,
    Vector,
    Function
};

struct Object
{
    using ObjList = std::vector<std::shared_ptr<Object>>;
    using ObjFunction = std::function<Object(std::vector<Object>)>;

    operator bool() const
    {
        if (std::holds_alternative<int64_t>(var))
            return std::get<int64_t>(var) != 0;
        else if (std::holds_alternative<double>(var))
            return std::get<double>(var) != 0.0f;
        else if (std::holds_alternative<std::string>(var))
            return !std::get<std::string>(var).empty();
        else if (std::holds_alternative<ObjList>(var))
            return !std::get<ObjList>(var).empty();
        else if (std::holds_alternative<ObjFunction>(var))
            return std::get<ObjFunction>(var) != nullptr;
        throw std::logic_error("Unexpected type");
    }

    std::variant<int64_t, double, std::string, ObjList, ObjFunction> var;
};
*/

int main()
{
    lexer::Lexer l;
    l.lex("3.5346 34f 36 (36) ");
    std::cout << "TOKENS: " << l.dump();

    /*
    Object foo;
    foo.var = 6;
    std::cout << "is bool: " << bool(foo);
    foo.var = 0;
    std::cout << "is bool: " << bool(foo);
    return 0;
    */
}
