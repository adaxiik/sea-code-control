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
        UnhandeledSignalError,
        MissingMainFunctionError,
        FunctionAlreadyDefinedError,
        COUNT
    };

    enum class InterpreterSignal
    {
        None = 0,
        Break,
        Continue,
        
        COUNT
    };

    struct InterpreterResultValue
    {
        Type::Value value;
        Type type;

        constexpr InterpreterResultValue(Type::Value value)
            : value(value), type(Type::from_value(value)) {}
        
        template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
        constexpr InterpreterResultValue(T value)
            : value(Type::Value(value)), type(Type::from_value(value)) {}

        // fallback error constructor.. should be never actually called
        // its here only for clang typechecking..
        template <typename T, typename = std::enable_if_t<!std::is_arithmetic_v<T>>>
        constexpr InterpreterResultValue(const T&)
            : type(Type(Type::Kind::I32)) {}

        template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
        constexpr InterpreterResultValue(T value, Type type)
            : value(Type::Value(value)), type(type) {}

        // same story here
        template <typename T, typename = std::enable_if_t<!std::is_arithmetic_v<T>>>
        constexpr InterpreterResultValue(const T& , Type type)
            : type(type) {}
        
        constexpr InterpreterResultValue(Type::Value value, Type type)
            : value(value), type(type) {}
    
    };

    class InterpreterResult
    {
        std::optional<InterpreterResultValue> value;
        InterpreterError interpreter_result;
        InterpreterSignal signal;


        InterpreterResult(std::optional<InterpreterResultValue> value, InterpreterError interpreter_result)
            : value(value), interpreter_result(interpreter_result), signal(InterpreterSignal::None) {}

    public:
        static InterpreterResult ok(InterpreterResultValue value) { return InterpreterResult(value, InterpreterError::None); }
        static InterpreterResult error(InterpreterError interpreter_result) { return InterpreterResult(std::nullopt, interpreter_result); }

        // Ill left them implicit
        InterpreterResult(std::optional<InterpreterResultValue> value)
            : value(value), interpreter_result(InterpreterError::None), signal(InterpreterSignal::None) {}
        
        InterpreterResult(InterpreterError interpreter_result)
            : value(std::nullopt), interpreter_result(interpreter_result), signal(InterpreterSignal::None) {}
        
        InterpreterResult(InterpreterSignal signal)
            : value(std::nullopt), interpreter_result(InterpreterError::None), signal(signal) {}

        bool is_ok() const { return interpreter_result == InterpreterError::None; }
        bool is_error() const { return !is_ok(); }
        bool has_value() const { return value.has_value(); }
        bool has_signal() const { return signal != InterpreterSignal::None; }
        bool is_ok_and_has_value() const { return is_ok() && has_value(); }
        InterpreterResultValue get_value() const { return *value; }
        InterpreterSignal get_signal() const { return signal; }
        InterpreterError get_error() const { return interpreter_result; }
    };
}