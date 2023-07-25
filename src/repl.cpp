#include "repl.hpp"
#include "debug.hpp"
#include "3rdparty/linenoise.hpp"

constexpr auto RED = "\033[0;31m";
// constexpr auto GREEN = "\033[0;32m";
constexpr auto RESET = "\033[0m";

namespace scc
{
    REPL::REPL(std::ostream &output_stream)
    : m_output_stream(output_stream)
    {
        linenoise::SetMultiLine(true);
        linenoise::SetHistoryMaxLen(HISTORY_CAPACITY);
    }

    REPL::REPL() : REPL( std::cout)
    {
    }

    void REPL::run()
    {
        std::string line;
        do
        {
            auto quit = linenoise::Readline(PROMPT, line);
            if(quit)
                goto repl_end;
            
            if (line.empty())
                continue;
            
            if (line == QUIT_COMMAND)
                goto repl_end;
            
            constexpr auto OPEN_BRACE = '{';
            constexpr auto CLOSE_BRACE = '}';

            if(line.back() == OPEN_BRACE)
            {
                auto indent = 1;
                std::string next_line;
                do
                {
                    std::string indent_str = std::string(indent * INDENTATION_SPACES, ' ');
                    std::string next_line_prompt = indent_str + PROMPT;

                    bool quit = linenoise::Readline(next_line_prompt.c_str(), next_line);
                    if(quit)
                        goto repl_end;
                    
                    if(next_line.back() == OPEN_BRACE)
                        ++indent;
                    if(next_line.back() == CLOSE_BRACE)
                        --indent;
                
                    line += '\n';
                    line += next_line;

                    if(indent == 0)
                        break;
                } while (true);
            }


            linenoise::AddHistory(line.c_str());
            // =====================

            if(line.size() > 1 && line.front() == '#') // macro needs newline at the end for correct parsing 
                line+= '\n';

            auto parse_result = m_interpreter.parse(line);
            if (parse_result.has_error())
            {
                m_output_stream << RED << "Parse error" << RESET << std::endl;
                continue;
            }

            auto result = m_interpreter.interpret(parse_result);

            switch (result)
            {
                case InterpreterResult::BindError:
                    m_output_stream << RED << "Bind error" << RESET << std::endl;
                    debug::ast_as_text_tree(m_output_stream, parse_result);
                    break;
                default:
                    break;
            }

            m_output_stream << std::endl;
        } while (true);
        
        repl_end:
        return;
    }


} 
