#include "lowering/printf_instruction.hpp"
#include <optional>
#include <sstream>
#include "interpreter_io.hpp"
#include "overloaded.hpp"

namespace scc
{
    namespace lowering
    {
        template <typename T>
        static std::optional<T> size_cast(InterpreterState &state, bool is_pointer = false)
        {
            if (state.result_stack.size() < 1)
                return std::nullopt;

            auto result = state.result_stack.top().get_value();
            state.result_stack.pop();


            if (not std::holds_alternative<Type::PrimitiveValue>(result.value.value))
                return std::nullopt;

            if (is_pointer)
            {
                std::optional<Type::Primitive::U64> v;
                std::visit(overloaded{

                    [&](Type::Primitive::I64 value) { v = static_cast<T>(value); },
                    [&](Type::Primitive::U64 value) { v = static_cast<T>(value); },
                    [&](auto) { v = std::nullopt; }
                }, std::get<Type::PrimitiveValue>(result.value.value));
                return v;
            }

            return std::visit(overloaded{
                [](Type::Primitive::I8 value) -> T { return static_cast<T>(value); },
                [](Type::Primitive::U8 value) -> T { return static_cast<T>(value); },
                [](Type::Primitive::I16 value) -> T { return static_cast<T>(value); },
                [](Type::Primitive::U16 value) -> T { return static_cast<T>(value); },
                [](Type::Primitive::I32 value) -> T { return static_cast<T>(value); },
                [](Type::Primitive::U32 value) -> T { return static_cast<T>(value); },
                [](Type::Primitive::I64 value) -> T { return static_cast<T>(value); },
                [](Type::Primitive::U64 value) -> T { return static_cast<T>(value); },
                [](Type::Primitive::F32 value) -> T { return static_cast<T>(value); },
                [](Type::Primitive::F64 value) -> T { return static_cast<T>(value); },
                [](Type::Primitive::Bool value) -> T { return static_cast<T>(value); },
                [](Type::Primitive::Char value) -> T { return static_cast<T>(value); },
                [](auto) -> T { return T(); }
            }, std::get<Type::PrimitiveValue>(result.value.value));

        }

        static InterpreterResult end_printf(const std::stringstream &output)
        {
            std::string output_str = output.str();
            InterpreterIO::write_stdout(output_str);
            return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::I32>(output_str.size())));
        }

        static InterpreterResult format_and_output(InterpreterState &state, const std::string &fmt, size_t arg_count)
        {
            std::stringstream output;
            size_t fmt_index = fmt.find('%');
            if (fmt_index == std::string::npos)
            {
                if (arg_count != 0)
                    return InterpreterError::PrintfHasTooManyArgumentsError;

                output << fmt;
                return end_printf(output);
            }

            std::string_view until_first_fmt(fmt.data(), fmt_index);
            output << until_first_fmt;

            while (fmt_index < fmt.size())
            {
                fmt_index++;
                arg_count--;
                std::string_view fmt_view(fmt.data() + fmt_index, fmt.size() - fmt_index);

                if (fmt_view.starts_with('%'))
                {
                    output << '%';
                    fmt_index += 1;
                    arg_count++; // bit hackish, but %% is only case where we don't consume an argument
                }
                else if (fmt_view.starts_with("hhi") or fmt_view.starts_with("hhd"))
                {
                    auto value = size_cast<Type::Primitive::I8>(state);
                    if (not value.has_value())
                        return InterpreterError::PrintfFormatError;

                    output << static_cast<int>(value.value());
                    fmt_index += 3;
                }
                else if (fmt_view.starts_with("hhu"))
                {
                    auto value = size_cast<Type::Primitive::U8>(state);
                    if (not value.has_value())
                        return InterpreterError::PrintfFormatError;

                    output << static_cast<int>(value.value());
                    fmt_index += 3;
                }
                else if (fmt_view.starts_with("hi") or fmt_view.starts_with("hd"))
                {
                    auto value = size_cast<Type::Primitive::I16>(state);
                    if (not value.has_value())
                        return InterpreterError::PrintfFormatError;

                    output << value.value();
                    fmt_index += 2;
                }
                else if (fmt_view.starts_with("hu"))
                {
                    auto value = size_cast<Type::Primitive::U16>(state);
                    if (not value.has_value())
                        return InterpreterError::PrintfFormatError;

                    output << value.value();
                    fmt_index += 2;
                }
                else if (fmt_view.starts_with("u"))
                {
                    auto value = size_cast<Type::Primitive::U32>(state);
                    if (not value.has_value())
                        return InterpreterError::PrintfFormatError;

                    output << value.value();
                    fmt_index += 1;
                }
                else if (fmt_view.starts_with("d") or fmt_view.starts_with("i"))
                {
                    auto value = size_cast<Type::Primitive::I32>(state);
                    if (not value.has_value())
                        return InterpreterError::PrintfFormatError;

                    output << value.value();
                    fmt_index += 1;
                }
                else if (fmt_view.starts_with("s"))
                {
                    auto value = size_cast<Type::Primitive::U64>(state, true);
                    if (not value.has_value())
                        return InterpreterError::PrintfFormatError;

                    size_t str_size = state.memory.find_first_byte_value_offset_in_chunk(value.value(), 0).value_or(0);
                    if (str_size == 0)
                        return InterpreterError::MemoryError;

                    std::string str(str_size, '\0');
                    if (not state.memory.read_buffer(value.value(), str.data(), str_size))
                        return InterpreterError::MemoryError;

                    output << str;
                    fmt_index += 1;
                }
                else if (fmt_view.starts_with("c"))
                {
                    auto value = size_cast<Type::Primitive::Char>(state);

                    output << static_cast<char>(value.value());
                    fmt_index += 1;
                }
                else if (fmt_view.starts_with("x"))
                {
                    auto value = size_cast<Type::Primitive::U32>(state);
                    if (not value.has_value())
                        return InterpreterError::PrintfFormatError;

                    output << std::hex << value.value() << std::dec;
                    fmt_index += 1;
                }
                else if (fmt_view.starts_with("X"))
                {
                    auto value = size_cast<Type::Primitive::U32>(state);
                    if (not value.has_value())
                        return InterpreterError::PrintfFormatError;

                    output << std::uppercase << std::hex << value.value() << std::nouppercase << std::dec;
                    fmt_index += 1;
                }
                else if (fmt_view.starts_with("p"))
                {
                    auto value = size_cast<Type::Primitive::U64>(state, true);
                    if (not value.has_value())
                        return InterpreterError::PrintfFormatError;

                    output << "0x" << std::hex << value.value() << std::dec;
                    fmt_index += 1;
                }
                else if (fmt_view.starts_with("f"))
                {
                    auto value = size_cast<Type::Primitive::F64>(state);
                    if (not value.has_value())
                        return InterpreterError::PrintfFormatError;

                    output << value.value();

                    fmt_index += 1;
                }
                else
                {
                    return InterpreterError::PrintfUnsuportedFormatError;
                }


                size_t new_fmt_index = fmt.find('%', fmt_index);
                if (new_fmt_index == std::string::npos)
                    break;

                std::string_view between_fmts(fmt.data() + fmt_index, new_fmt_index - fmt_index);
                output << between_fmts;

                fmt_index = new_fmt_index;
            }

            if (arg_count != 0)
                return InterpreterError::PrintMissingArgumentsError;

            std::string_view to_end(fmt.data() + fmt_index, fmt.size() - fmt_index);
            output << to_end;
            return end_printf(output);
        }

        InterpreterResult PrintfInstruction::execute(InterpreterState &state) const
        {
            if (state.result_stack.size() < argument_count_without_fmt + 1)
                return InterpreterError::RuntimeError; // should never happen :D

            auto fmt_result_value = state.result_stack.top().get_value();
            state.result_stack.pop();

            if (fmt_result_value.type.primitive_type().value_or(Type::PrimitiveType::COUNT) != Type::PrimitiveType::Char
                and fmt_result_value.type.pointer_depth() != 1)
            {
                return InterpreterError::RuntimeError; // should never happen as well
            }

            Type::Primitive::U64 fmt_address = std::get<Type::Primitive::U64>(fmt_result_value.value.primitive_value().value_or(0));

            size_t fmt_size = state.memory.find_first_byte_value_offset_in_chunk(fmt_address, 0).value_or(0);
            if (fmt_size == 0)
                return InterpreterError::MemoryError;

            std::string fmt(fmt_size, '\0');
            if (not state.memory.read_buffer(fmt_address, fmt.data(), fmt_size))
                return InterpreterError::MemoryError;


            // return InterpreterResult::ok(InterpreterResultValue(fmt_size));
            return format_and_output(state, fmt, argument_count_without_fmt);
        }
    }
}