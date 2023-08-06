#pragma once
#include <any>
#include <optional>
#include "type.hpp"

namespace scc
{
    enum class InterpreterError
    {
        None,
        ParseError,
        BindError,
        RuntimeError,
        ReachedUnreachableError, 
        InvalidOperationError,
        DivisionByZeroError,
        VariableAlreadyExistsError,
        VariableDoesntExistError,
        COUNT
    };

    struct ResultValue
    {
        std::any value;
        Type type;

        template <typename T>
        ResultValue(T value, Type type)
            : value(value), type(type) {}
        
        template <typename T>
        ResultValue(T value)
            : value(value), type(Type::deduce_type<T>()) {}
      
    };

    class InterpreterResult
    {
        std::optional<ResultValue> value;
        InterpreterError interpreter_result;

        InterpreterResult(std::optional<ResultValue> value, InterpreterError interpreter_result)
            : value(value), interpreter_result(interpreter_result){}

    public:
        static InterpreterResult ok(ResultValue value) { return InterpreterResult(value, InterpreterError::None); }
        static InterpreterResult error(InterpreterError interpreter_result) { return InterpreterResult(std::nullopt, interpreter_result); }

        // Ill left them implicit
        InterpreterResult(std::optional<ResultValue> value)
            : value(value), interpreter_result(InterpreterError::None) {}
        
        InterpreterResult(InterpreterError interpreter_result)
            : value(std::nullopt), interpreter_result(interpreter_result) {}

        bool is_ok() const { return interpreter_result == InterpreterError::None; }
        bool is_error() const { return !is_ok(); }
        bool has_value() const { return value.has_value(); }
        bool is_ok_and_has_value() const { return is_ok() && has_value(); }
        ResultValue get_value() const { return *value; }
        InterpreterError get_error() const { return interpreter_result; }
    };
}