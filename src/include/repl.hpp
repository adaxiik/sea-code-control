#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <functional>

#include "interpreter.hpp"
namespace scc
{
    constexpr auto QUIT_COMMAND = "q!";
    constexpr auto PROMPT = "> ";
    constexpr auto HISTORY_CAPACITY = 100;
    constexpr auto INDENTATION_SPACES = 4;


    class REPL
    {
        Interpreter m_interpreter;
        std::ostream &m_output_stream;

    public:
        REPL();
        REPL(std::ostream &output_stream);
        ~REPL() = default;

        void run();
    };
} // namespace scc
