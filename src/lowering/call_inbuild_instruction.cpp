#include "lowering/call_inbuild_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        InterpreterResult CallInbuildInstruction::execute(InterpreterState &state) const
        {
            return fn(state);
        }
    }
}