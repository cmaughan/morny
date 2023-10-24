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
#include <morny/parser.h>

using namespace Lexer;
using namespace Parser;

int main()
{
    //auto vals = Lex("3.5346 34f 36 (36) foobar __hello MyName !32 &*64 if a == 3 else for");
    std::string input = "{3 + 3}";
    std::cout << "INPUT: " << input << std::endl;

    auto l = Lex(input);
    std::cout << "LEXER: " << Dump(l) << std::endl;

    auto [p, messages] = Parse(l);
    std::cout << "PARSER: " << p << std::endl;

    for (auto& msg : messages)
    {
        std::cout << "MSG: " << msg << std::endl;
    }
}
