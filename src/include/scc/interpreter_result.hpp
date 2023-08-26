#pragma once
#include <optional>
#include "type.hpp"

namespace scc
{
    enum class InterpreterError
    {
        None = 0,
        ParseError,
        BindError,
        RuntimeError,
        ReachedUnreachableError, 
        InvalidOperationError,
        DivisionByZeroError,
        VariableAlreadyExistsError,
        VariableDoesntExistError,
        VariableNotInitializedError,
        COUNT
    };

    struct InterpreterResultValue
    {
        Type::Value value;
        Type type;

        constexpr InterpreterResultValue(Type::Value value)
            : value(value), type(Type::from_value(value)) {}
        
        template <typename T>
        constexpr InterpreterResultValue(T value)
            : value(Type::Value(value)), type(Type::from_value(value)) {}

        template <typename T>
        constexpr InterpreterResultValue(T value, Type type)
            : value(Type::Value(value)), type(type) {}

        
        constexpr InterpreterResultValue(Type::Value value, Type type)
            : value(value), type(type) {}
        
        
    };

    class InterpreterResult
    {
        std::optional<InterpreterResultValue> value;
        InterpreterError interpreter_result;

        InterpreterResult(std::optional<InterpreterResultValue> value, InterpreterError interpreter_result)
            : value(value), interpreter_result(interpreter_result){}

    public:
        static InterpreterResult ok(InterpreterResultValue value) { return InterpreterResult(value, InterpreterError::None); }
        static InterpreterResult error(InterpreterError interpreter_result) { return InterpreterResult(std::nullopt, interpreter_result); }

        // Ill left them implicit
        InterpreterResult(std::optional<InterpreterResultValue> value)
            : value(value), interpreter_result(InterpreterError::None) {}
        
        InterpreterResult(InterpreterError interpreter_result)
            : value(std::nullopt), interpreter_result(interpreter_result) {}

        bool is_ok() const { return interpreter_result == InterpreterError::None; }
        bool is_error() const { return !is_ok(); }
        bool has_value() const { return value.has_value(); }
        bool is_ok_and_has_value() const { return is_ok() && has_value(); }
        InterpreterResultValue get_value() const { return *value; }
        InterpreterError get_error() const { return interpreter_result; }
    };
}