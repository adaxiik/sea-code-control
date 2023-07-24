#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <functional>

#include "interpreter.hpp"
namespace scc
{
    constexpr auto QUIT_COMMAND = "q!";
    constexpr auto PROMPT = ">> ";
    constexpr auto HISTORY_CAPACITY = 100;


    class REPL
    {
        Interpreter m_interpreter;
        std::istream &m_input_stream;
        std::ostream &m_output_stream;

    public:
        REPL();
        REPL(std::istream &input_stream, std::ostream &output_stream);
        ~REPL() = default;

        void run();
    };
} // namespace scc
