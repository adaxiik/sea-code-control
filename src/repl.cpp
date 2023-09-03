#include "repl.hpp"
#include "debug.hpp"
#include "3rdparty/linenoise.hpp"
#include "overloaded.hpp"

constexpr auto RED = "\033[0;31m";
constexpr auto GREEN = "\033[0;32m";
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
        if (result.get_value().type.pointer_depth > 0)
        {
            m_output_stream << "0x" << std::hex << std::get<Memory::address_t>(result.get_value().value) << std::dec; 
            return;
        }

        std::visit(overloaded{
            [&](const Type::Primitive::Char& value) { m_output_stream << "'" << value << "'" ; },
            [&](const Type::Primitive::U8& value) { m_output_stream << value; },
            [&](const Type::Primitive::I8& value) { m_output_stream << value; },
            [&](const Type::Primitive::U16& value) { m_output_stream << value; },
            [&](const Type::Primitive::I16& value) { m_output_stream << value; },
            [&](const Type::Primitive::U32& value) { m_output_stream << value; },
            [&](const Type::Primitive::I32& value) { m_output_stream << value; },
            [&](const Type::Primitive::U64& value) { m_output_stream << value; },
            [&](const Type::Primitive::I64& value) { m_output_stream << value; },
            [&](const Type::Primitive::F32& value) { m_output_stream << value; },
            [&](const Type::Primitive::F64& value) { m_output_stream << value; },
            [&](const Type::Primitive::Bool& value) { m_output_stream << (value ? "true" : "false") ; }
        }, result.get_value().value);
    }

    void REPL::run()
    {
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

            auto result = m_interpreter.interpret(parse_result);

            if (result.is_ok_and_has_value())
            {
                print_result(result);
                m_output_stream << " (" << result.get_value().type << ")" << std::endl;
                continue;
            }

            if (result.is_ok())
                continue;

            // TOOOOO: Move to debug
            static_assert(static_cast<int>(InterpreterError::COUNT) == 13, "Edit this code");
            switch (result.get_error())
            {
                case InterpreterError::BindError:
                    m_output_stream << RED << "Bind error" << RESET << std::endl;
                    break;
                case InterpreterError::RuntimeError:
                    m_output_stream << RED << "Runtime error" << RESET << std::endl;
                    break;
                case InterpreterError::InvalidOperationError:
                    m_output_stream << RED << "Invalid operation error" << RESET << std::endl;
                    break;
                case InterpreterError::ReachedUnreachableError:
                    m_output_stream << RED << "Reached unreachable error" << RESET << std::endl;
                    break;
                case InterpreterError::DivisionByZeroError:
                    m_output_stream << RED << "Division by zero error" << RESET << std::endl;
                    break;
                case InterpreterError::VariableAlreadyExistsError:
                    m_output_stream << RED << "Variable already exists error" << RESET << std::endl;
                    break;
                case InterpreterError::VariableDoesntExistError:
                    m_output_stream << RED << "Variable doesn't exist error" << RESET << std::endl;
                    break;
                case InterpreterError::VariableNotInitializedError:
                    m_output_stream << RED << "Variable not initialized error" << RESET << std::endl;
                    break;
                case InterpreterError::UnhandeledSignalError:
                    m_output_stream << RED << "Unhandeled signal error" << RESET << std::endl;
                    break;
                case InterpreterError::MissingMainFunctionError:
                    m_output_stream << RED << "Missing main function error" << RESET << std::endl;
                    break;
                case InterpreterError::FunctionAlreadyDefinedError:
                    m_output_stream << RED << "Function already defined error" << RESET << std::endl;
                    break;
                default:
                    m_output_stream << RED << "Unknown error" << RESET << std::endl;
                    break;
            }
            debug::ast_as_text_tree(m_output_stream, parse_result);

        } while (true);

    repl_end:
        return;
    }

}
