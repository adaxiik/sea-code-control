#include "interpreter.hpp"

#include <iostream>
#include <variant>
#include "debug.hpp"
#include "cpp_compiler.hpp"
#include "operation_result.hpp"
#include "overloaded.hpp"
#include "lowering/instruction.hpp"

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

    InterpreterResult Interpreter::register_functions(binding::BoundBlockStatement &block_statement)
    {
        TRACE();

        // size_t index = 0;
        // while (index < block_statement.statements.size())
        // {
        //     std::unique_ptr<binding::BoundStatement>& current_statement = block_statement.statements[index];
        //     if (current_statement->bound_node_kind() != binding::BoundNodeKind::FunctionStatement)
        //     {
        //         index++;
        //         continue;
        //     }
        //     std::unique_ptr<binding::BoundFunctionStatement> function_statement {std::unique_ptr<binding::BoundFunctionStatement>(static_cast<binding::BoundFunctionStatement*>(current_statement.release()))} ;
        //     block_statement.statements.erase(block_statement.statements.begin() + index);

        //     bool already_exists = m_functions.find(function_statement->function_name) != m_functions.end();
        //     if (!already_exists)
        //     {
        //         m_functions[function_statement->function_name] = std::move(function_statement);
        //         continue;
        //     }

        //     auto& existing_function = m_functions[function_statement->function_name];

        //     if (*function_statement != *existing_function)
        //         return InterpreterError::IncosistentFunctionSignatureError;

        //     if (!function_statement->body)
        //             continue;
                
        //     if (existing_function->body)
        //         return InterpreterError::FunctionAlreadyDefinedError;
                
        //     existing_function->body = std::move(function_statement->body); // TODOOOO: might not be defined
                      

        // }

        return InterpreterError::None;
    }

    InterpreterResult Interpreter::interpret(const ParserResult &parse_result)
    {
        TRACE();
        if (parse_result.has_error())
            return InterpreterError::ParseError;

        auto binded {m_binder.bind(parse_result.root_node())};
        if (binded.is_error())
            return InterpreterError::BindError;

        binding::BoundNode* root = binded.get_value();
        if (root->bound_node_kind() != binding::BoundNodeKind::BlockStatement)
            return InterpreterError::BindError;

        auto lowered = m_lowerer.lower(static_cast<binding::BoundBlockStatement*>(root));
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

        for (m_state.instruction_pointer = latest_program_size; m_state.instruction_pointer < m_program.size(); m_state.instruction_pointer++)
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
