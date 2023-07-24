#include "repl.hpp"
#include "debug.hpp"
#include "3rdparty/linenoise.hpp"

namespace scc
{
    REPL::REPL(std::istream &input_stream, std::ostream &output_stream)
    : m_input_stream(input_stream)
    , m_output_stream(output_stream)
    {
        linenoise::SetMultiLine(true);
        linenoise::SetHistoryMaxLen(HISTORY_CAPACITY);
    }

    REPL::REPL() : REPL(std::cin, std::cout)
    {
    }



    void REPL::run()
    {
        std::string line;
        do
        {
            auto quit = linenoise::Readline(PROMPT, line);
            if(quit)
                break;
            
            if (line.empty())
                continue;
            
            if (line == QUIT_COMMAND)
                break;
            
            linenoise::AddHistory(line.c_str());



            // =====================

            auto parse_result = m_interpreter.parse(line);
            if (parse_result.has_error())
            {
                m_output_stream << "Parse error" << std::endl;
                continue;
            }

            auto result = m_interpreter.interpret(parse_result);

            if (result != InterpreterResult::Ok)
                m_output_stream << "Error: " << static_cast<int>(result) << std::endl;
            else
                debug::ast_as_text_tree(m_output_stream, parse_result);

            m_output_stream << std::endl;

            
        } while (true);
    }


} 
