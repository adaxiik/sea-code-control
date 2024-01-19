#include "cpp_compiler.hpp"
#include "lowering/binary_operation_instruction.hpp"
#include "interpreter_state.hpp"

#include  <type_traits>

// #pragma GCC diagnostic error "-Wframe-larger-than=50000"
namespace scc
{
    namespace lowering
    {
        template <class LEFT_CAST, class RIGHT_CAST, bool multiply_ptr, bool inherit_ptr_type, class PERFORM_OP>
        static InterpreterResult binary_operation_result(
            size_t left_pointer_depth,
            size_t right_pointer_depth,
            PERFORM_OP perform_op,
            const InterpreterResultValue& left_result,
            const InterpreterResultValue& right_result
        )
        {

            SCC_SUPPRESS_WARNING_PUSH_INT_IN_BOOL_CONTEXT
            // int* + 1 = int* + (1 * sizeof(int))
            if (left_pointer_depth > 0)
            {
                auto r_val = std::get<RIGHT_CAST>(right_result.value.primitive_value().value());
                if constexpr (multiply_ptr and inherit_ptr_type)
                {
                    Type underlying_type = left_result.type;
                    underlying_type.modifiers.pop_back();
                    r_val *= underlying_type.size_bytes();
                }

                auto result {perform_op(std::get<Type::Primitive::PTR>(left_result.value.primitive_value().value()), r_val)};
                if (not result.has_value())
                    return InterpreterResult::error(InterpreterError::DivisionByZeroError);
                
                if constexpr (not inherit_ptr_type)
                    return InterpreterResult::ok(InterpreterResultValue(result.value()));

                auto result_type = Type::deduce_type<LEFT_CAST>();
                result_type.modifiers = std::vector<Type::Modifier>(left_pointer_depth, Type::Pointer{});
                return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::PTR>(result.value()), result_type));
            }
            else if (right_pointer_depth > 0)
            {
                auto l_val = std::get<LEFT_CAST>(left_result.value.primitive_value().value());
                if constexpr (multiply_ptr and inherit_ptr_type)
                {
                    Type underlying_type = right_result.type;
                    underlying_type.modifiers.pop_back();
                    l_val *= underlying_type.size_bytes();
                }
                auto result {perform_op(l_val, std::get<Type::Primitive::PTR>(right_result.value.primitive_value().value()))};
                if (not result.has_value())
                    return InterpreterResult::error(InterpreterError::DivisionByZeroError);
                
                if constexpr (not inherit_ptr_type)
                    return InterpreterResult::ok(InterpreterResultValue(result.value()));
                
                auto result_type = Type::deduce_type<RIGHT_CAST>();
                result_type.modifiers = std::vector<Type::Modifier>(right_pointer_depth, Type::Pointer{});
                return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::PTR>(result.value()), result_type));

            }
            else
            {
                auto result {perform_op(std::get<LEFT_CAST>(left_result.value.primitive_value().value()), std::get<RIGHT_CAST>(right_result.value.primitive_value().value()))};
                if (not result.has_value())
                    return InterpreterResult::error(InterpreterError::DivisionByZeroError);
                else
                    return InterpreterResult::ok(InterpreterResultValue(result.value()));
            }
            return InterpreterResult::error(InterpreterError::InvalidOperationError);
            SCC_SUPPRESS_WARNING_POP_INT_IN_BOOL_CONTEXT
        }

        InterpreterResult BinaryOperationInstruction::execute(InterpreterState &state) const
        {
            SCC_SUPPRESS_WARNING_PUSH_SIGN_COMPARISON

            if (state.result_stack.size() < 2)
                return InterpreterResult::error(InterpreterError::RuntimeError);

            InterpreterResultValue left_result{state.result_stack.top().get_value()};
            state.result_stack.pop();

            InterpreterResultValue right_result{state.result_stack.top().get_value()};
            state.result_stack.pop();
            using Operator = binding::BoundBinaryExpression::OperatorKind;
            if (left_result.type.is_pointer()
                and right_result.type.is_pointer())
            {
                #define DO_OP_PTR(OP) do {                                                     \
                    return InterpreterResult::ok(                                              \
                            InterpreterResultValue(                                            \
                                std::get<Type::Primitive::PTR>(                                \
                                    left_result.value.primitive_value().value()    \
                                )                                                              \
                                OP                                                             \
                                std::get<Type::Primitive::PTR>(                                \
                                    right_result.value.primitive_value().value()   \
                                )                                                              \
                            )                                                                  \
                        );                                                                     \
                } while(0)

                switch (operator_kind)
                {
                    case Operator::Equals: DO_OP_PTR(==);
                    case Operator::NotEquals: DO_OP_PTR(!=);
                    case Operator::LessThan: DO_OP_PTR(<);
                    case Operator::GreaterThan: DO_OP_PTR(>);
                    case Operator::LessThanOrEqual: DO_OP_PTR(<=);
                    case Operator::GreaterThanOrEqual: DO_OP_PTR(>=);
                    default:
                        return InterpreterResult::error(InterpreterError::InvalidOperationError);
                        
                }
            }
            

            if (not left_result.type.is_primitive() or not right_result.type.is_primitive())
            {
                return InterpreterResult::error(InterpreterError::InvalidOperationError);
            }

            if (left_result.type.primitive_type().value() == Type::PrimitiveType::Void
                or right_result.type.primitive_type().value() == Type::PrimitiveType::Void)
            {
                return InterpreterResult::error(InterpreterError::InvalidOperationError);
            }


            static_assert(static_cast<int>(Operator::COUNT) == 18, "Update this code");
            static_assert(static_cast<int>(Type::PrimitiveType::COUNT) == 13, "Update this code");

            // I'm so sorry

            #define DO_CASTED_OP(FN_NAME, STRUCT_NAME, LEFT_CAST, RIGHT_CAST, LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH, MULT_PTR, INHERIT_PTR_TYPE) do{                                                                                                     \
            if constexpr (std::is_same_v<typename OperationResult::STRUCT_NAME ## OperationResult <LEFT_CAST,RIGHT_CAST>::type, OperationResult::InvalidOperation>)                                                   \
            {                                                                                                                                                                                                         \
                return InterpreterResult::error(InterpreterError::InvalidOperationError);                                                                                                                             \
            }                                                                                                                                                                                                         \
            else                                                                                                                                                                                                      \
            {                                                                                                                                                                                                         \
                if (LEFT_PTR_DEPTH > 0 and RIGHT_PTR_DEPTH > 0)                                                                                                                                                       \
                    return binary_operation_result<LEFT_CAST, RIGHT_CAST, MULT_PTR, INHERIT_PTR_TYPE>(LEFT_PTR_DEPTH, RIGHT_PTR_DEPTH, OperationResult::perform_ ## FN_NAME<Type::Primitive::PTR,Type::Primitive::PTR>, left_result, right_result);\
                else if (LEFT_PTR_DEPTH > 0)                                                                                                                                                                          \
                    return binary_operation_result<LEFT_CAST, RIGHT_CAST, MULT_PTR, INHERIT_PTR_TYPE>(LEFT_PTR_DEPTH, RIGHT_PTR_DEPTH, OperationResult::perform_ ## FN_NAME<Type::Primitive::PTR,RIGHT_CAST>, left_result, right_result);          \
                else if (RIGHT_PTR_DEPTH > 0)                                                                                                                                                                         \
                    return binary_operation_result<LEFT_CAST, RIGHT_CAST, MULT_PTR, INHERIT_PTR_TYPE>(LEFT_PTR_DEPTH, RIGHT_PTR_DEPTH, OperationResult::perform_ ## FN_NAME<LEFT_CAST,Type::Primitive::PTR>, left_result, right_result);           \
                else                                                                                                                                                                                                  \
                    return binary_operation_result<LEFT_CAST, RIGHT_CAST, MULT_PTR, INHERIT_PTR_TYPE>(LEFT_PTR_DEPTH, RIGHT_PTR_DEPTH, OperationResult::perform_ ## FN_NAME<LEFT_CAST,RIGHT_CAST>, left_result, right_result);                     \
            }                                                                                                                                                                                                         \
            }while(0)

            #define RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,TYPE,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH,MULT_PTR,INHERIT_PTR_TYPE) case Type::PrimitiveType::TYPE: DO_CASTED_OP(OP,STRUCT_NAME,LEFT_CAST,Type::Primitive::TYPE,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH, MULT_PTR,INHERIT_PTR_TYPE)

            #define DO_OP_RIGHT(OP,STRUCT_NAME,LEFT_CAST,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH,MULT_PTR,INHERIT_PTR_TYPE) do{   \
                switch (right_result.type.primitive_type().value())                \
                {                                                                              \
                    RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,Char,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH,MULT_PTR,INHERIT_PTR_TYPE);  \
                    RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,U8,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH,MULT_PTR,INHERIT_PTR_TYPE);    \
                    RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,I8,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH,MULT_PTR,INHERIT_PTR_TYPE);    \
                    RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,U16,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH,MULT_PTR,INHERIT_PTR_TYPE);    \
                    RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,I16,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH,MULT_PTR,INHERIT_PTR_TYPE);    \
                    RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,U32,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH,MULT_PTR,INHERIT_PTR_TYPE);   \
                    RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,I32,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH,MULT_PTR,INHERIT_PTR_TYPE);   \
                    RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,U64,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH,MULT_PTR,INHERIT_PTR_TYPE);   \
                    RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,I64,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH,MULT_PTR,INHERIT_PTR_TYPE);   \
                    RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,F32,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH,MULT_PTR,INHERIT_PTR_TYPE);   \
                    RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,F64,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH,MULT_PTR,INHERIT_PTR_TYPE);   \
                    RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,Bool,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH,MULT_PTR,INHERIT_PTR_TYPE);  \
                    default:                                         \
                        return InterpreterResult::error(InterpreterError::ReachedUnreachableError); \
                }                                                    \
            }while(0)

            #define LEFT_CASE(OP,STRUCT_NAME,TYPE,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH,MULT_PTR,INHERIT_PTR_TYPE) case Type::PrimitiveType::TYPE: DO_OP_RIGHT(OP,STRUCT_NAME,Type::Primitive::TYPE,LEFT_PTR_DEPTH,RIGHT_PTR_DEPTH,MULT_PTR,INHERIT_PTR_TYPE)

            #define DO_OP_LEFT(OP,STRUCT_NAME,LEFT_PTR_DEPTH,RIGHT_PTR_DEPT,MULT_PTR,INHERIT_PTR_TYPE) do{ \
                switch (left_result.type.primitive_type().value()) \
                { \
                    LEFT_CASE(OP,STRUCT_NAME,Char,LEFT_PTR_DEPTH,RIGHT_PTR_DEPT,MULT_PTR,INHERIT_PTR_TYPE); \
                    LEFT_CASE(OP,STRUCT_NAME,U8,LEFT_PTR_DEPTH,RIGHT_PTR_DEPT,MULT_PTR,INHERIT_PTR_TYPE); \
                    LEFT_CASE(OP,STRUCT_NAME,I8,LEFT_PTR_DEPTH,RIGHT_PTR_DEPT,MULT_PTR,INHERIT_PTR_TYPE); \
                    LEFT_CASE(OP,STRUCT_NAME,U16,LEFT_PTR_DEPTH,RIGHT_PTR_DEPT,MULT_PTR,INHERIT_PTR_TYPE); \
                    LEFT_CASE(OP,STRUCT_NAME,I16,LEFT_PTR_DEPTH,RIGHT_PTR_DEPT,MULT_PTR,INHERIT_PTR_TYPE); \
                    LEFT_CASE(OP,STRUCT_NAME,U32,LEFT_PTR_DEPTH,RIGHT_PTR_DEPT,MULT_PTR,INHERIT_PTR_TYPE); \
                    LEFT_CASE(OP,STRUCT_NAME,I32,LEFT_PTR_DEPTH,RIGHT_PTR_DEPT,MULT_PTR,INHERIT_PTR_TYPE); \
                    LEFT_CASE(OP,STRUCT_NAME,U64,LEFT_PTR_DEPTH,RIGHT_PTR_DEPT,MULT_PTR,INHERIT_PTR_TYPE); \
                    LEFT_CASE(OP,STRUCT_NAME,I64,LEFT_PTR_DEPTH,RIGHT_PTR_DEPT,MULT_PTR,INHERIT_PTR_TYPE); \
                    LEFT_CASE(OP,STRUCT_NAME,F32,LEFT_PTR_DEPTH,RIGHT_PTR_DEPT,MULT_PTR,INHERIT_PTR_TYPE); \
                    LEFT_CASE(OP,STRUCT_NAME,F64,LEFT_PTR_DEPTH,RIGHT_PTR_DEPT,MULT_PTR,INHERIT_PTR_TYPE); \
                    LEFT_CASE(OP,STRUCT_NAME,Bool,LEFT_PTR_DEPTH,RIGHT_PTR_DEPT,MULT_PTR,INHERIT_PTR_TYPE); \
                    default: \
                        return InterpreterResult::error(InterpreterError::ReachedUnreachableError); \
                } \
            }while(0)


            switch (operator_kind)
            {
                case Operator::Addition:           DO_OP_LEFT(addition,Addition,left_result.type.pointer_depth(), right_result.type.pointer_depth(), true, true);
                case Operator::Subtraction:        DO_OP_LEFT(subtraction,Subtraction,left_result.type.pointer_depth(), right_result.type.pointer_depth(), true, true);
                case Operator::Multiplication:     DO_OP_LEFT(multiplication,Multiplication,left_result.type.pointer_depth(), right_result.type.pointer_depth(), false, true);
                case Operator::Division:           DO_OP_LEFT(division,Division,left_result.type.pointer_depth(), right_result.type.pointer_depth(), false, true);
                case Operator::Modulo:             DO_OP_LEFT(modulo,Modulo,left_result.type.pointer_depth(), right_result.type.pointer_depth(), false, true);
                case Operator::BitwiseAnd:         DO_OP_LEFT(bitwiseand,BitwiseAnd,left_result.type.pointer_depth(), right_result.type.pointer_depth(), false, true);
                case Operator::BitwiseOr:          DO_OP_LEFT(bitwiseor,BitwiseOr,left_result.type.pointer_depth(), right_result.type.pointer_depth(), false, true);
                case Operator::BitwiseXor:         DO_OP_LEFT(bitwisexor,BitwiseXor,left_result.type.pointer_depth(), right_result.type.pointer_depth(), false, true);
                case Operator::BitwiseShiftLeft:   DO_OP_LEFT(bitwiseshiftleft,BitwiseShiftLeft,left_result.type.pointer_depth(), right_result.type.pointer_depth(), false, true);
                case Operator::BitwiseShiftRight:  DO_OP_LEFT(bitwiseshiftright,BitwiseShiftRight,left_result.type.pointer_depth(), right_result.type.pointer_depth(), false, true);
                case Operator::LogicalAnd:         DO_OP_LEFT(logicaland,LogicalAnd,left_result.type.pointer_depth(), right_result.type.pointer_depth(), false, false);
                case Operator::LogicalOr:          DO_OP_LEFT(logicalor,LogicalOr,left_result.type.pointer_depth(), right_result.type.pointer_depth(), false, false);
                case Operator::Equals:             DO_OP_LEFT(equals,Equals,left_result.type.pointer_depth(), right_result.type.pointer_depth(), false, false);
                case Operator::NotEquals:          DO_OP_LEFT(notequals,NotEquals,left_result.type.pointer_depth(), right_result.type.pointer_depth(), false, false);
                case Operator::LessThan:           DO_OP_LEFT(lessthan,LessThan,left_result.type.pointer_depth(), right_result.type.pointer_depth(), false, false);
                case Operator::GreaterThan:        DO_OP_LEFT(greaterthan,GreaterThan,left_result.type.pointer_depth(), right_result.type.pointer_depth(), false, false);
                case Operator::LessThanOrEqual:    DO_OP_LEFT(lessthanorequal,LessThanOrEqual,left_result.type.pointer_depth(), right_result.type.pointer_depth(), false, false);
                case Operator::GreaterThanOrEqual: DO_OP_LEFT(greaterthanorequal,GreaterThanOrEqual,left_result.type.pointer_depth(), right_result.type.pointer_depth(), false, false);

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