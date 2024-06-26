#include "repl.hpp"
#include "debug.hpp"
#include "3rdparty/linenoise.hpp"
#include "overloaded.hpp"

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

    REPL::REPL() : REPL(std::cout)
    {
    }

    void REPL::print_result(const InterpreterResult &result)
    {
        Type::Value::print_as_type(m_output_stream, result.get_value().value.value, result.get_value().type);
    }

    void REPL::run()
    {
        auto running_interpreter = m_interpreter.interpret().value();

        std::string line;
        do
        {
            auto quit = linenoise::Readline(PROMPT, line);
            if (quit)
                goto repl_end;

            if (line.empty())
                continue;

            if (line == QUIT_COMMAND)
                goto repl_end;

            constexpr auto OPEN_BRACE = '{';
            constexpr auto CLOSE_BRACE = '}';

            if (line.back() == OPEN_BRACE)
            {
                auto indent = 1;
                std::string next_line;
                do
                {
                    std::string indent_str = std::string(indent * INDENTATION_SPACES, ' ');
                    std::string next_line_prompt = indent_str + PROMPT;

                    bool quit = linenoise::Readline(next_line_prompt.c_str(), next_line);
                    if (quit)
                        goto repl_end;

                    if (next_line.back() == OPEN_BRACE)
                        ++indent;
                    if (next_line.back() == CLOSE_BRACE)
                        --indent;

                    line += '\n';
                    line += next_line;

                    if (indent == 0)
                        break;
                } while (true);
            }

            linenoise::AddHistory(line.c_str());
            // =====================

            if (line.size() > 1 && line.front() == '#') // macro needs newline at the end for correct parsing
                line += '\n';

            auto parse_result = m_interpreter.parse(line);
            if (parse_result.has_error())
            {
                m_output_stream << RED << "Parse error" << RESET << std::endl;
                continue;
            }

            debug::ast_as_text_tree(m_output_stream, parse_result);

            auto bind_result = m_interpreter.bind(parse_result.root_node());
            if (bind_result.is_error())
            {
                m_output_stream << RED << "Bind error" << RESET << std::endl;
                m_output_stream << static_cast<int>(bind_result.get_error().kind) << std::endl;
                for (auto& diag : bind_result.get_diagnostics())
                {
                    m_output_stream << diag << std::endl;
                }

                continue;
            }

            debug::bound_ast_as_text_tree(m_output_stream, *bind_result.get_value());
            // debug::bound_ast_as_latex_dirtree(m_output_stream, *bind_result.get_value());

            auto lower_result = m_interpreter.lower(static_cast<binding::BoundBlockStatement*>(bind_result.get_value()));
            {
                std::vector<lowering::Instruction> lowered;
                std::transform(
                    lower_result.begin(),
                    lower_result.end(),
                    std::back_inserter(lowered),
                    [](auto& pair) { return pair.first; }
                );
                debug::instructions_as_text(std::cout, lowered);
            }

            running_interpreter.append_code(lower_result);

            auto result = running_interpreter.continue_execution();

            if (result.is_ok_and_has_value())
            {
                print_result(result);
                m_output_stream << " (" << result.get_value().type << ")" << std::endl;
                continue;
            }

            if (result.is_ok())
                continue;
            m_output_stream << RED;
            debug::interpreter_error_as_text(m_output_stream, result.get_error());
            m_output_stream << RESET << std::endl;
            debug::ast_as_text_tree(m_output_stream, parse_result);

        } while (true);

    repl_end:
        return;
    }

}
