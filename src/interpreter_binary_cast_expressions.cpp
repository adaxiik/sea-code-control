#include "interpreter_result.hpp"
#include "binding/bound_binary_expression.hpp"
#include "binding/bound_cast_expression.hpp"
#include "cpp_compiler.hpp"
#include "type.hpp"

namespace scc
{
    // These 2 functions expand into 26k lines of code.. it's slow to compile and slow to analyze with my ide
    // so I moved them to another file.. Im not sure if this is legal in cpp, but it works I guess
    // the rest of Interpreter class is in src/interpreter.hpp
    // (which is not included in this file, because I edit it very often and I don't want to recompile this file every time I change something)
    struct Interpreter
    {
        InterpreterResult eval(const binding::BoundExpression &expression);
        InterpreterResult eval(const binding::BoundBinaryExpression& binary_expression);
        InterpreterResult eval(const binding::BoundCastExpression &cast_expression);
    };


    InterpreterResult Interpreter::eval(const binding::BoundBinaryExpression& binary_expression)
    {
        SCC_SUPPRESS_WARNING_PUSH_SIGN_COMPARISON


        TRACE();
        auto left_result{eval(*binary_expression.left)};
        if (left_result.is_error())
            return left_result;

        auto right_result{eval(*binary_expression.right)};
        if (right_result.is_error())
            return right_result;


        if (left_result.get_value().type.is_pointer()
             && right_result.get_value().type.is_pointer())
        {
            return InterpreterResult::error(InterpreterError::InvalidOperationError);
        }

        using Operator = binding::BoundBinaryExpression::OperatorKind;
      
        static_assert(static_cast<int>(Operator::COUNT) == 18, "Update this code");
        static_assert(static_cast<int>(Type::Kind::COUNT) == 12, "Update this code");

        // I'm so sorry
        
        #define DO_CASTED_OP(FN_NAME, STRUCT_NAME, LEFT_CAST, RIGHT_CAST, LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH) do{ \
           if constexpr (std::is_same_v<typename OperationResult::STRUCT_NAME ## OperationResult <LEFT_CAST,RIGHT_CAST>::type, OperationResult::InvalidOperation>) \
           { \
                return InterpreterResult::error(InterpreterError::InvalidOperationError); \
           } \
           else \
           { \
                if (LEFT_PTR_DEPTH > 0) \
                { \
                    auto result {OperationResult::perform_ ## FN_NAME (std::get<Type::Primitive::U64>(left_result.get_value().value), std::get<RIGHT_CAST>(right_result.get_value().value))}; \
                    if (!result.has_value()) \
                        return InterpreterResult::error(InterpreterError::DivisionByZeroError); \
                    else \
                    { \
                        auto result_type {Type::deduce_type<LEFT_CAST>()}; \
                        result_type.pointer_depth = LEFT_PTR_DEPTH; \
                        return InterpreterResult::ok(InterpreterResultValue(result.value(), result_type)); \
                    } \
                } \
                else if (RIGHT_PTR_DEPTH > 0) \
                { \
                    auto result {OperationResult::perform_ ## FN_NAME (std::get<LEFT_CAST>(left_result.get_value().value), std::get<Type::Primitive::U64>(right_result.get_value().value))}; \
                    if (!result.has_value()) \
                        return InterpreterResult::error(InterpreterError::DivisionByZeroError); \
                    else \
                    { \
                        auto result_type {Type::deduce_type<RIGHT_CAST>()}; \
                        result_type.pointer_depth = RIGHT_PTR_DEPTH; \
                        return InterpreterResult::ok(InterpreterResultValue(result.value(), result_type)); \
                    } \
                } \
                else \
                { \
                    auto result {OperationResult::perform_ ## FN_NAME (std::get<LEFT_CAST>(left_result.get_value().value), std::get<RIGHT_CAST>(right_result.get_value().value))}; \
                    if (!result.has_value()) \
                        return InterpreterResult::error(InterpreterError::DivisionByZeroError); \
                    else \
                    { \
                        return InterpreterResult::ok(InterpreterResultValue(result.value())); \
                    } \
                }\
           } \
        }while(0)

        #define RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,TYPE,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH) case Type::Kind::TYPE: DO_CASTED_OP(OP,STRUCT_NAME,LEFT_CAST,Type::Primitive::TYPE,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH)

        #define DO_OP_RIGHT(OP,STRUCT_NAME,LEFT_CAST,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH) do{   \
            switch (right_result.get_value().type.kind)                                    \
            {                                                                              \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,Char,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH);  \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,U8,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH);    \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,I8,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH);    \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,U32,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH);   \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,I32,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH);   \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,U64,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH);   \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,I64,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH);   \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,F32,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH);   \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,F64,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH);   \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,Bool,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH);  \
                default:                                         \
                    return InterpreterResult::error(InterpreterError::ReachedUnreachableError); \
            }                                                    \
        }while(0)

        #define LEFT_CASE(OP,STRUCT_NAME,TYPE,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH) case Type::Kind::TYPE: DO_OP_RIGHT(OP,STRUCT_NAME,Type::Primitive::TYPE,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH)

        #define DO_OP_LEFT(OP,STRUCT_NAME,LEFT_PTR_DEPTH,RIGHT_PTR_DEPT) do{ \
            switch (left_result.get_value().type.kind) \
            { \
                LEFT_CASE(OP,STRUCT_NAME,Char,LEFT_PTR_DEPTH,RIGHT_PTR_DEPT); \
                LEFT_CASE(OP,STRUCT_NAME,U8,LEFT_PTR_DEPTH,RIGHT_PTR_DEPT); \
                LEFT_CASE(OP,STRUCT_NAME,I8,LEFT_PTR_DEPTH,RIGHT_PTR_DEPT); \
                LEFT_CASE(OP,STRUCT_NAME,U32,LEFT_PTR_DEPTH,RIGHT_PTR_DEPT); \
                LEFT_CASE(OP,STRUCT_NAME,I32,LEFT_PTR_DEPTH,RIGHT_PTR_DEPT); \
                LEFT_CASE(OP,STRUCT_NAME,U64,LEFT_PTR_DEPTH,RIGHT_PTR_DEPT); \
                LEFT_CASE(OP,STRUCT_NAME,I64,LEFT_PTR_DEPTH,RIGHT_PTR_DEPT); \
                LEFT_CASE(OP,STRUCT_NAME,F32,LEFT_PTR_DEPTH,RIGHT_PTR_DEPT); \
                LEFT_CASE(OP,STRUCT_NAME,F64,LEFT_PTR_DEPTH,RIGHT_PTR_DEPT); \
                LEFT_CASE(OP,STRUCT_NAME,Bool,LEFT_PTR_DEPTH,RIGHT_PTR_DEPT); \
                default: \
                    return InterpreterResult::error(InterpreterError::ReachedUnreachableError); \
            } \
        }while(0)


        switch (binary_expression.op_kind)
        {
            case Operator::Addition:           DO_OP_LEFT(addition,Addition,left_result.get_value().type.pointer_depth, right_result.get_value().type.pointer_depth);
            case Operator::Subtraction:        DO_OP_LEFT(subtraction,Subtraction,left_result.get_value().type.pointer_depth, right_result.get_value().type.pointer_depth);
            case Operator::Multiplication:     DO_OP_LEFT(multiplication,Multiplication,left_result.get_value().type.pointer_depth, right_result.get_value().type.pointer_depth);
            case Operator::Division:           DO_OP_LEFT(division,Division,left_result.get_value().type.pointer_depth, right_result.get_value().type.pointer_depth);
            case Operator::Modulo:             DO_OP_LEFT(modulo,Modulo,left_result.get_value().type.pointer_depth, right_result.get_value().type.pointer_depth);
            case Operator::BitwiseAnd:         DO_OP_LEFT(bitwiseand,BitwiseAnd,left_result.get_value().type.pointer_depth, right_result.get_value().type.pointer_depth);
            case Operator::BitwiseOr:          DO_OP_LEFT(bitwiseor,BitwiseOr,left_result.get_value().type.pointer_depth, right_result.get_value().type.pointer_depth);
            case Operator::BitwiseXor:         DO_OP_LEFT(bitwisexor,BitwiseXor,left_result.get_value().type.pointer_depth, right_result.get_value().type.pointer_depth);
            case Operator::BitwiseShiftLeft:   DO_OP_LEFT(bitwiseshiftleft,BitwiseShiftLeft,left_result.get_value().type.pointer_depth, right_result.get_value().type.pointer_depth);
            case Operator::BitwiseShiftRight:  DO_OP_LEFT(bitwiseshiftright,BitwiseShiftRight,left_result.get_value().type.pointer_depth, right_result.get_value().type.pointer_depth);
            case Operator::LogicalAnd:         DO_OP_LEFT(logicaland,LogicalAnd,left_result.get_value().type.pointer_depth, right_result.get_value().type.pointer_depth);
            case Operator::LogicalOr:          DO_OP_LEFT(logicalor,LogicalOr,left_result.get_value().type.pointer_depth, right_result.get_value().type.pointer_depth);
            case Operator::Equals:             DO_OP_LEFT(equals,Equals,left_result.get_value().type.pointer_depth, right_result.get_value().type.pointer_depth);
            case Operator::NotEquals:          DO_OP_LEFT(notequals,NotEquals,left_result.get_value().type.pointer_depth, right_result.get_value().type.pointer_depth);
            case Operator::LessThan:           DO_OP_LEFT(lessthan,LessThan,left_result.get_value().type.pointer_depth, right_result.get_value().type.pointer_depth);
            case Operator::GreaterThan:        DO_OP_LEFT(greaterthan,GreaterThan,left_result.get_value().type.pointer_depth, right_result.get_value().type.pointer_depth);
            case Operator::LessThanOrEqual:    DO_OP_LEFT(lessthanorequal,LessThanOrEqual,left_result.get_value().type.pointer_depth, right_result.get_value().type.pointer_depth);
            case Operator::GreaterThanOrEqual: DO_OP_LEFT(greaterthanorequal,GreaterThanOrEqual,left_result.get_value().type.pointer_depth, right_result.get_value().type.pointer_depth);

            default:
                // UNREACHABLE
                return InterpreterResult::error(InterpreterError::ReachedUnreachableError);
        }
        #undef DO_OP_LEFT
        #undef LEFT_CASE
        #undef DO_OP_RIGHT
        #undef RIGHT_CASE
        #undef DO_CASTED_OP

        SCC_SUPPRESS_WARNING_POP
        return InterpreterError::ReachedUnreachableError;
    }


    InterpreterResult Interpreter::eval(const binding::BoundCastExpression &cast_expression)
    {
        TRACE();
        auto result{eval(*cast_expression.expression)};
        if (result.is_error())
            return result;

        auto target_type{cast_expression.type};


        if (target_type == result.get_value().type)
            return result;

        if (target_type.pointer_depth > 0)
        {
            switch (result.get_value().type.kind)
            {
                case Type::Kind::Char:
                    return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::Char>(result.get_value().value)), target_type));
                case Type::Kind::U8:
                    return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::U8>(result.get_value().value)), target_type));
                case Type::Kind::I8:
                    return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::I8>(result.get_value().value)), target_type));
                case Type::Kind::U16:
                    return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::U16>(result.get_value().value)), target_type));
                case Type::Kind::I16:
                    return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::I16>(result.get_value().value)), target_type));
                case Type::Kind::U32:
                    return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::U32>(result.get_value().value)), target_type));
                case Type::Kind::I32:
                    return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::I32>(result.get_value().value)), target_type));
                case Type::Kind::U64:
                    return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::U64>(result.get_value().value)), target_type));
                case Type::Kind::I64:
                    return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::U64>(result.get_value().value)), target_type));
                case Type::Kind::F32:
                    return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::F32>(result.get_value().value)), target_type));
                case Type::Kind::F64:
                    return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::F64>(result.get_value().value)), target_type));
                case Type::Kind::Bool:
                    return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::Bool>(result.get_value().value)), target_type));
                default:
                    return InterpreterResult::error(InterpreterError::ReachedUnreachableError);
            }
        }

        static_assert(static_cast<int>(Type::Kind::COUNT) == 12, "Update this code");


        #define CAST_CASE(KIND_TYPE, TARGET_TYPE) case Type::Kind::KIND_TYPE: \
            return InterpreterResult::ok(InterpreterResultValue(static_cast<TARGET_TYPE>(std::get<Type::Primitive::KIND_TYPE>(result.get_value().value))))
        
        #define CAST_ORIGINAL(TARGET_TYPE) do{ \
            switch(result.get_value().type.kind) \
            { \
                CAST_CASE(Char, TARGET_TYPE); \
                CAST_CASE(U8, TARGET_TYPE); \
                CAST_CASE(I8, TARGET_TYPE); \
                CAST_CASE(U32, TARGET_TYPE); \
                CAST_CASE(I32, TARGET_TYPE); \
                CAST_CASE(U64, TARGET_TYPE); \
                CAST_CASE(I64, TARGET_TYPE); \
                CAST_CASE(F32, TARGET_TYPE); \
                CAST_CASE(F64, TARGET_TYPE); \
                CAST_CASE(Bool, TARGET_TYPE); \
                default: \
                    return InterpreterResult::error(InterpreterError::ReachedUnreachableError); \
            }}while(0)

        switch(target_type.kind)
        {
            case Type::Kind::Char: CAST_ORIGINAL(Type::Primitive::Char);
            case Type::Kind::U8: CAST_ORIGINAL(Type::Primitive::U8);
            case Type::Kind::I8: CAST_ORIGINAL(Type::Primitive::I8);
            case Type::Kind::U16: CAST_ORIGINAL(Type::Primitive::U16);
            case Type::Kind::I16: CAST_ORIGINAL(Type::Primitive::I16);
            case Type::Kind::U32: CAST_ORIGINAL(Type::Primitive::U32);
            case Type::Kind::I32: CAST_ORIGINAL(Type::Primitive::I32);
            case Type::Kind::U64: CAST_ORIGINAL(Type::Primitive::U64);
            case Type::Kind::I64: CAST_ORIGINAL(Type::Primitive::I64);
            case Type::Kind::F32: CAST_ORIGINAL(Type::Primitive::F32);
            case Type::Kind::F64: CAST_ORIGINAL(Type::Primitive::F64);
            case Type::Kind::Bool: CAST_ORIGINAL(Type::Primitive::Bool);
            default:
                return InterpreterResult::error(InterpreterError::ReachedUnreachableError); 
        }
        #undef CAST_CASE
        #undef CAST_ORIGINAL
    }


}
