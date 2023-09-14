#include "cpp_compiler.hpp"
#include "lowering/binary_operation_instruction.hpp"
#include "interpreter_state.hpp"
namespace scc
{
    namespace lowering
    {
        InterpreterResult BinaryOperationInstruction::execute(InterpreterState &state) const
        {
            SCC_SUPPRESS_WARNING_PUSH_SIGN_COMPARISON

            if (state.result_stack.size() < 2)
                return InterpreterResult::error(InterpreterError::RuntimeError);

            InterpreterResult left_result{state.result_stack.top()};
            state.result_stack.pop();

            InterpreterResult right_result{state.result_stack.top()};
            state.result_stack.pop();

            if (left_result.get_value().type.is_pointer()
                && right_result.get_value().type.is_pointer())
            {
                return InterpreterResult::error(InterpreterError::InvalidOperationError);
            }

            if (left_result.get_value().type.kind == Type::Kind::Void
                || right_result.get_value().type.kind == Type::Kind::Void)
            {
                return InterpreterResult::error(InterpreterError::InvalidOperationError);
            }

            using Operator = binding::BoundBinaryExpression::OperatorKind;
        
            static_assert(static_cast<int>(Operator::COUNT) == 18, "Update this code");
            static_assert(static_cast<int>(Type::Kind::COUNT) == 13, "Update this code");

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


            switch (operator_kind)
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
    }
}