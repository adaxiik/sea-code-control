#pragma once
#include "interpreter_result.hpp"
#include "interpreter_state.hpp"
#include "binding/bound_literal_expression.hpp"

namespace scc
{
    namespace lowering
    {
        struct PushLiteralInstruction
        {
            const binding::BoundLiteralExpression& bound_literal_expression;
            PushLiteralInstruction(const binding::BoundLiteralExpression& bound_literal_expression) : bound_literal_expression(bound_literal_expression){}
            InterpreterResult execute(InterpreterState &state) const;
        };
    }
} 
