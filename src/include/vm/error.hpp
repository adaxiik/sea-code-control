#pragma once

namespace scc
{
    namespace vm
    {
        enum class Error
        {
            None,
            VariableAlreadyExists,
            VariableDoesNotExist,
            StackOverflow,
            StackUnderflow,
            IllegalMemoryAccess,
        };
    }
}
