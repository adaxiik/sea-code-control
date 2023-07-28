#pragma once
#include "cpp_compiler.hpp"
#include <optional>
namespace scc
{
    namespace OperationResult
    {
        struct InvalidOperation{};
    
        #define CREATE_OP_RESULT(LOWER_NAME, CAPITAL_NAME, OP,IS_DIVISION) \
        template <class T, class U> \
        struct CAPITAL_NAME ## OperationResult \
        { \
            template <class X, class Y> \
            static auto test(X x, Y y) -> decltype(x OP y); \
            static InvalidOperation test(...); \
            using type = decltype(test(std::declval<T>(), std::declval<U>())); \
        }; \
        template <class T, class U> \
        static auto perform_ ## LOWER_NAME (T a, U b) ->std::optional<typename CAPITAL_NAME ## OperationResult<T, U>::type> \
        {   \
            if constexpr (std::is_same_v<typename CAPITAL_NAME ## OperationResult<T, U>::type, InvalidOperation>) \
            { \
                return InvalidOperation{}; \
            } \
            else \
            { \
                if constexpr (IS_DIVISION) \
                { \
                    if (b == 0) \
                    { \
                        return std::nullopt; \
                    } \
                } \
                return a OP b; \
            }  \
        }

        SCC_SUPPRESS_WARNING_PUSH_SIGN_COMPARISON

        CREATE_OP_RESULT(addition, Addition, +, false)
        CREATE_OP_RESULT(subtraction, Subtraction, -, false)
        CREATE_OP_RESULT(multiplication, Multiplication, *, false)
        CREATE_OP_RESULT(division, Division, /, true)
        CREATE_OP_RESULT(modulo, Modulo, %, true)
        CREATE_OP_RESULT(bitwiseand, BitwiseAnd, &, false)
        CREATE_OP_RESULT(bitwiseor, BitwiseOr, |, false)
        CREATE_OP_RESULT(bitwisexor, BitwiseXor, ^, false)
        CREATE_OP_RESULT(bitwiseshiftleft, BitwiseShiftLeft, <<, false)
        CREATE_OP_RESULT(bitwiseshiftright, BitwiseShiftRight, >>, false)
        CREATE_OP_RESULT(logicaland, LogicalAnd, &&, false)
        CREATE_OP_RESULT(logicalor, LogicalOr, ||, false)
        CREATE_OP_RESULT(equals, Equals, ==, false)
        CREATE_OP_RESULT(notequals, NotEquals, !=, false)
        CREATE_OP_RESULT(lessthan, LessThan, <, false)
        CREATE_OP_RESULT(greaterthan, GreaterThan, >, false)
        CREATE_OP_RESULT(lessthanorequal, LessThanOrEqual, <=, false)
        CREATE_OP_RESULT(greaterthanorequal, GreaterThanOrEqual, >=, false)

        SCC_SUPPRESS_WARNING_POP


    }
}