#include "repl.hpp"
#include "debug.hpp"
#include "3rdparty/linenoise.hpp"

constexpr auto RED = "\033[0;31m";
constexpr auto GREEN = "\033[0;32m";
constexpr auto RESET = "\033[0m";

// https://en.cppreference.com/w/cpp/utility/variant/visit
template <class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

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
/* #define VISIT_RESULT(KIND, C_TYPE)                                      \
//     [&](const KIND &) {                                                                  \
//         m_output_stream << std::any_cast<C_TYPE>(result.get_value().value) << std::endl; \
     }*/

//         std::visit(overloaded{[&](const type::char_type &)
//                               {
//                                   static_assert(sizeof(char) == sizeof(uint8_t), "char is not 8 bits");
//                                   m_output_stream << "'" << std::any_cast<char>(result.get_value().value) << "'" << std::endl;
//                               },
//                               VISIT_RESULT(type::u8_type, unsigned char),
//                               VISIT_RESULT(type::u16_type, unsigned short),
//                               VISIT_RESULT(type::u32_type, unsigned int),
//                               VISIT_RESULT(type::u64_type, unsigned long long),
//                               VISIT_RESULT(type::i8_type, signed char),
//                               VISIT_RESULT(type::i16_type, short),
//                               VISIT_RESULT(type::i32_type, int),
//                               VISIT_RESULT(type::i64_type, long long),
//                               VISIT_RESULT(type::f32_type, float),
//                               VISIT_RESULT(type::f64_type, double),
//                               [&](const type::boolean_type &)
//                               {
//                                   static_assert(sizeof(bool) == sizeof(bool), "bool is not 8 bit");
//                                   m_output_stream << (std::any_cast<bool>(result.get_value().value) ? "true" : "false") << std::endl;
//                               },
//                               [&](const type::ptr_type &)
//                               {
//                                   m_output_stream << "TBD" << std::endl;
//                               }},
//                    result.get_value().type.kind);



// #undef VISIT_RESULT
        #define VISIT_CASE(KIND, C_TYPE) \
            case Type::Kind::KIND: \
                m_output_stream << std::any_cast<C_TYPE>(result.get_value().value) ; \
                break;
        static_assert(static_cast<int>(Type::Kind::COUNT) == 12, "Edit this code");
        switch (result.get_value().type.kind)
        {
            case Type::Kind::Char:
                m_output_stream << "'" << std::any_cast<char>(result.get_value().value) << "'" ;
                break;
            VISIT_CASE(U8, unsigned char)
            VISIT_CASE(U16, unsigned short)
            VISIT_CASE(U32, unsigned int)
            VISIT_CASE(U64, unsigned long long)
            VISIT_CASE(I8, signed char)
            VISIT_CASE(I16, short)
            VISIT_CASE(I32, int)
            VISIT_CASE(I64, long long)
            VISIT_CASE(F32, float)
            VISIT_CASE(F64, double)
            case Type::Kind::Bool:
                m_output_stream << (std::any_cast<bool>(result.get_value().value) ? "true" : "false") ;
                break;
            default:
                m_output_stream << "UNREACHABLE at " << __FILE__ << ":" << __LINE__ ;
                std::exit(1);
                break;
        }
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
                m_output_stream << " (";
                debug::type_as_text(m_output_stream, result.get_value().type);
                m_output_stream << ")" << std::endl;
                continue;
            }

            if (result.is_error())
            {
                m_output_stream << RED << "Interpreter error" << RESET << std::endl;
                debug::ast_as_text_tree(m_output_stream, parse_result);
                continue;
            }

        } while (true);

    repl_end:
        return;
    }

}
