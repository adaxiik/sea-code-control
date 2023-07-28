#pragma once
#include "cpp_compiler.hpp"
namespace scc
{
    namespace OperationResult
    {
        struct InvalidOperation{};
    
        #define CREATE_OP_RESULT(LOWER_NAME, CAPITAL_NAME, OP) \
        template <class T, class U> \
        struct CAPITAL_NAME ## OperationResult \
        { \
            template <class X, class Y> \
            static auto test(X x, Y y) -> decltype(x OP y); \
            static InvalidOperation test(...); \
            using type = decltype(test(std::declval<T>(), std::declval<U>())); \
        }; \
        template <class T, class U> \
        static auto perform_ ## LOWER_NAME (T a, U b) -> typename CAPITAL_NAME ## OperationResult<T, U>::type \
        {   \
            if constexpr (std::is_same_v<typename CAPITAL_NAME ## OperationResult<T, U>::type, InvalidOperation>) \
            { \
                return InvalidOperation{}; \
            } \
            else \
            { \
                return a OP b; \
            }  \
        }

        SCC_SUPPRESS_WARNING_PUSH_SIGN_COMPARISON

        CREATE_OP_RESULT(addition, Addition, +)
        CREATE_OP_RESULT(subtraction, Subtraction, -)
        CREATE_OP_RESULT(multiplication, Multiplication, *)
        CREATE_OP_RESULT(division, Division, /)
        CREATE_OP_RESULT(modulo, Modulo, %)
        CREATE_OP_RESULT(bitwiseand, BitwiseAnd, &)
        CREATE_OP_RESULT(bitwiseor, BitwiseOr, |)
        CREATE_OP_RESULT(bitwisexor, BitwiseXor, ^)
        CREATE_OP_RESULT(bitwiseshiftleft, BitwiseShiftLeft, <<)
        CREATE_OP_RESULT(bitwiseshiftright, BitwiseShiftRight, >>)
        CREATE_OP_RESULT(logicaland, LogicalAnd, &&)
        CREATE_OP_RESULT(logicalor, LogicalOr, ||)
        CREATE_OP_RESULT(equals, Equals, ==)
        CREATE_OP_RESULT(notequals, NotEquals, !=)
        CREATE_OP_RESULT(lessthan, LessThan, <)
        CREATE_OP_RESULT(greaterthan, GreaterThan, >)
        CREATE_OP_RESULT(lessthanorequal, LessThanOrEqual, <=)
        CREATE_OP_RESULT(greaterthanorequal, GreaterThanOrEqual, >=)

        SCC_SUPPRESS_WARNING_POP


    }
}