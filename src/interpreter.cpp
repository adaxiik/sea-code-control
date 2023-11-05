#include "interpreter.hpp"

#include <iostream>
#include <variant>
#include "debug.hpp"
#include "cpp_compiler.hpp"
#include "operation_result.hpp"
#include "overloaded.hpp"
#include "lowering/instruction.hpp"

#include "interpreter_io.hpp"

namespace scc
{
    ParserResult Interpreter::parse(const std::string &code)
    {
        return m_parser.parse(code);
    }

    InterpreterResult Interpreter::interpret(const std::string &code)
    {  
        auto parse_result = m_parser.parse(code);
        return interpret(parse_result);
    }

    InterpreterResult Interpreter::interpret(const ParserResult &parse_result)
    {
        TRACE();
        if (parse_result.has_error())
            return InterpreterError::ParseError;

        auto binded {m_binder.bind(parse_result.root_node())};
        if (binded.is_error())
        {
            std::cerr << static_cast<int>(binded.get_error().kind) << std::endl;
            std::cerr << binded.get_error().location.row << ":" << binded.get_error().location.column << std::endl;
            return InterpreterError::BindError;
        }

        debug::bound_ast_as_text_tree(std::cout, *binded.get_value());

        binding::BoundNode* root = binded.get_value();
        if (root->bound_node_kind() != binding::BoundNodeKind::BlockStatement)
            return InterpreterError::BindError;

        auto lowered_with_location = m_lowerer.lower(static_cast<binding::BoundBlockStatement*>(root));

        std::vector<lowering::Instruction> lowered;
        std::transform(
            lowered_with_location.begin(),
            lowered_with_location.end(),
            std::back_inserter(lowered),
            [](auto& pair) { return pair.first; }
        );

        debug::instructions_as_text(std::cout, lowered);

        size_t latest_program_size = m_program.size();
        std::copy(lowered.begin(), lowered.end(), std::back_inserter(m_program));

        for (size_t i = latest_program_size; i < m_program.size(); i++)
        {
            if (std::holds_alternative<lowering::LabelInstruction>(m_program[i]))
                m_state.labels[std::get<lowering::LabelInstruction>(m_program[i]).label] = i;
            else if (std::holds_alternative<lowering::RegisterFunctionInstruction>(m_program[i]))
                m_state.functions[std::get<lowering::RegisterFunctionInstruction>(m_program[i]).function_name] = i;
        }

        m_state.instruction_pointer = latest_program_size;

        if (m_state.functions.find(MAIN_FUNCTION_NAME) != m_state.functions.end())
        {
            m_state.instruction_pointer = m_program.size(); // return address
            std::visit(
                lowering::InstructionExecuter(m_state),
                lowering::Instruction{lowering::CallInstruction(MAIN_FUNCTION_NAME, true)}
            );
        }

        for (; m_state.instruction_pointer < m_program.size(); m_state.instruction_pointer++)
        {
            auto& instruction = m_program[m_state.instruction_pointer];
            auto result = std::visit(lowering::InstructionExecuter(m_state), instruction);
            if (result.is_error())
                return result;
            
            if (result.has_value())
                m_state.result_stack.push(result);

            if (m_state.instruction_pointer == m_program.size() - 1)
            {
                if (!m_state.result_stack.empty())
                {
                    auto top = m_state.result_stack.top();
                    m_state.result_stack.pop();
                    return top;
                }
            }
        }
        
        return InterpreterError::None;
    }
}
