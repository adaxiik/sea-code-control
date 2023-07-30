#include "interpreter_result.hpp"
#include "binding/bound_binary_expression.hpp"
#include "binding/bound_cast_expression.hpp"
#include "cpp_compiler.hpp"

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

        
        
        using Operator = binding::BoundBinaryExpression::OperatorKind;
        static_assert(static_cast<int>(Operator::COUNT) == 18, "Update this code");
        static_assert(static_cast<int>(Type::Kind::COUNT) == 12, "Update this code");

        // I'm so sorry
        
        #define DO_CASTED_OP(FN_NAME, STRUCT_NAME, LEFT_CAST, RIGHT_CAST) do{ \
           if constexpr (std::is_same_v<typename OperationResult::STRUCT_NAME ## OperationResult <LEFT_CAST,RIGHT_CAST>::type, OperationResult::InvalidOperation>) \
           { \
                return InterpreterResult::error(InterpreterError::InvalidOperationError); \
           } \
           else \
           { \
                auto result {OperationResult::perform_ ## FN_NAME (std::any_cast<LEFT_CAST>(left_result.get_value().value), std::any_cast<RIGHT_CAST>(right_result.get_value().value))}; \
                if (!result.has_value()) \
                    return InterpreterResult::error(InterpreterError::DivisionByZeroError); \
                else \
                    return InterpreterResult::ok(ResultValue(result.value())); \
           } \
        }while(0)

        #define RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,TYPE,CTYPE) case Type::Kind::TYPE: DO_CASTED_OP(OP,STRUCT_NAME,LEFT_CAST,CTYPE)

        #define DO_OP_RIGHT(OP,STRUCT_NAME,LEFT_CAST) do{        \
            switch (right_result.get_value().type.kind)          \
            {                                                    \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,Char,char);              \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,U8,unsigned char);       \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,I8,signed char);         \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,U32,unsigned int);       \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,I32,int);                \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,U64,unsigned long long); \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,I64,long long);          \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,F32,float);              \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,F64,double);             \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,Bool,bool);              \
                default:                                         \
                    return InterpreterResult::error(InterpreterError::ReachedUnreachableError); \
            }                                                    \
        }while(0)

        #define LEFT_CASE(OP,STRUCT_NAME,TYPE,CTYPE) case Type::Kind::TYPE: DO_OP_RIGHT(OP,STRUCT_NAME,CTYPE)

        #define DO_OP_LEFT(OP,STRUCT_NAME) do{ \
            switch (left_result.get_value().type.kind) \
            { \
                LEFT_CASE(OP,STRUCT_NAME,Char,char); \
                LEFT_CASE(OP,STRUCT_NAME,U8,unsigned char); \
                LEFT_CASE(OP,STRUCT_NAME,I8,signed char); \
                LEFT_CASE(OP,STRUCT_NAME,U32,unsigned int); \
                LEFT_CASE(OP,STRUCT_NAME,I32,int); \
                LEFT_CASE(OP,STRUCT_NAME,U64,unsigned long long); \
                LEFT_CASE(OP,STRUCT_NAME,I64,long long); \
                LEFT_CASE(OP,STRUCT_NAME,F32,float); \
                LEFT_CASE(OP,STRUCT_NAME,F64,double); \
                LEFT_CASE(OP,STRUCT_NAME,Bool,bool); \
                default: \
                    return InterpreterResult::error(InterpreterError::ReachedUnreachableError); \
            } \
        }while(0)

        switch (binary_expression.op_kind)
        {
            case Operator::Addition: DO_OP_LEFT(addition,Addition);
            case Operator::Subtraction: DO_OP_LEFT(subtraction,Subtraction);
            case Operator::Multiplication: DO_OP_LEFT(multiplication,Multiplication);
            case Operator::Division: DO_OP_LEFT(division,Division);
            case Operator::Modulo: DO_OP_LEFT(modulo,Modulo);
            case Operator::BitwiseAnd: DO_OP_LEFT(bitwiseand,BitwiseAnd);
            case Operator::BitwiseOr: DO_OP_LEFT(bitwiseor,BitwiseOr);
            case Operator::BitwiseXor: DO_OP_LEFT(bitwisexor,BitwiseXor);
            case Operator::BitwiseShiftLeft: DO_OP_LEFT(bitwiseshiftleft,BitwiseShiftLeft);
            case Operator::BitwiseShiftRight: DO_OP_LEFT(bitwiseshiftright,BitwiseShiftRight);
            case Operator::LogicalAnd: DO_OP_LEFT(logicaland,LogicalAnd);
            case Operator::LogicalOr: DO_OP_LEFT(logicalor,LogicalOr);
            case Operator::Equals: DO_OP_LEFT(equals,Equals);
            case Operator::NotEquals: DO_OP_LEFT(notequals,NotEquals);
            case Operator::LessThan: DO_OP_LEFT(lessthan,LessThan);
            case Operator::GreaterThan: DO_OP_LEFT(greaterthan,GreaterThan);
            case Operator::LessThanOrEqual: DO_OP_LEFT(lessthanorequal,LessThanOrEqual);
            case Operator::GreaterThanOrEqual: DO_OP_LEFT(greaterthanorequal,GreaterThanOrEqual);

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
        return InterpreterResult::ok(1);
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

        static_assert(static_cast<int>(Type::Kind::COUNT) == 12, "Update this code");


        #define CAST_CASE(KIND_TYPE, TARGET_TYPE,ORIGINAL_TYPE) case Type::Kind::KIND_TYPE: \
            return InterpreterResult::ok(ResultValue(static_cast<TARGET_TYPE>(std::any_cast<ORIGINAL_TYPE>(result.get_value().value))));
        
        #define CAST_ORIGINAL(TARGET_TYPE) do{ \
            switch(result.get_value().type.kind) \
            { \
                CAST_CASE(Char, TARGET_TYPE,char); \
                CAST_CASE(U8, TARGET_TYPE,unsigned char); \
                CAST_CASE(I8, TARGET_TYPE,signed char); \
                CAST_CASE(U32, TARGET_TYPE,unsigned int); \
                CAST_CASE(I32, TARGET_TYPE,int); \
                CAST_CASE(U64, TARGET_TYPE,unsigned long long); \
                CAST_CASE(I64, TARGET_TYPE,long long); \
                CAST_CASE(F32, TARGET_TYPE,float); \
                CAST_CASE(F64, TARGET_TYPE,double); \
                CAST_CASE(Bool, TARGET_TYPE,bool); \
                default: \
                    return InterpreterResult::error(InterpreterError::ReachedUnreachableError); \
            }}while(0)

        switch(target_type.kind)
        {
            case Type::Kind::Char: CAST_ORIGINAL(char);
            case Type::Kind::U8: CAST_ORIGINAL(unsigned char);
            case Type::Kind::I8: CAST_ORIGINAL(signed char);
            case Type::Kind::U16: CAST_ORIGINAL(unsigned short);
            case Type::Kind::I16: CAST_ORIGINAL(signed short);
            case Type::Kind::U32: CAST_ORIGINAL(unsigned int);
            case Type::Kind::I32: CAST_ORIGINAL(int);
            case Type::Kind::U64: CAST_ORIGINAL(unsigned long long);
            case Type::Kind::I64: CAST_ORIGINAL(long long);
            case Type::Kind::F32: CAST_ORIGINAL(float);
            case Type::Kind::F64: CAST_ORIGINAL(double);
            case Type::Kind::Bool: CAST_ORIGINAL(bool);
            default:
                return InterpreterResult::error(InterpreterError::ReachedUnreachableError); 
        }
        #undef CAST_CASE
        #undef CAST_ORIGINAL
    }


}
