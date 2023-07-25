#pragma once
#include <string>

namespace scc
{
    namespace binding
    {
        enum class BoundBinaryOpKind
        {
            Addition,
            Subtraction,
        };
        struct BoundBinaryOperator
        {
            BoundBinaryOpKind kind;
            
            // static BoundBinaryOperator bind(const std::string& op)
            // {

            // }
        };
    }
}
