#include "interpreter.hpp"

#include <iostream>
#include <variant>
#include <numeric>

#include "debug.hpp"
#include "cpp_compiler.hpp"
#include "operation_result.hpp"
#include "overloaded.hpp"
#include "lowering/instruction.hpp"

#include "interpreter_io.hpp"

namespace scc
{
    // ParserResult Interpreter::parse(const std::string &code)
    // {
    //     return m_parser.parse(code);
    // }

    // InterpreterResult Interpreter::interpret(const std::string &code)
    // {  
    //     auto parse_result = m_parser.parse(code);
    //     return interpret(parse_result);
    // }

    // InterpreterResult Interpreter::interpret(const ParserResult &parse_result)
    // {
    //     TRACE();
    //     if (parse_result.has_error())
    //         return InterpreterError::ParseError;

    //     auto binded {m_binder.bind(parse_result.root_node())};
    //     if (binded.is_error())
    //     {
    //         std::cerr << static_cast<int>(binded.get_error().kind) << std::endl;
    //         std::cerr << binded.get_error().location.row << ":" << binded.get_error().location.column << std::endl;
    //         return InterpreterError::BindError;
    //     }

    //     debug::bound_ast_as_text_tree(std::cout, *binded.get_value());

    //     binding::BoundNode* root = binded.get_value();
    //     if (root->bound_node_kind() != binding::BoundNodeKind::BlockStatement)
    //         return InterpreterError::BindError;

    //     auto lowered_with_location = m_lowerer.lower(static_cast<binding::BoundBlockStatement*>(root));

    //     {
    //         std::vector<lowering::Instruction> lowered;
    //         std::transform(
    //             lowered_with_location.begin(),
    //             lowered_with_location.end(),
    //             std::back_inserter(lowered),
    //             [](auto& pair) { return pair.first; }
    //         );

    //         debug::instructions_as_text(std::cout, lowered);
    //     }


    //     size_t latest_program_size = m_program.size();
    //     std::copy(lowered_with_location.begin(), lowered_with_location.end(), std::back_inserter(m_program));

    //     for (size_t i = latest_program_size; i < m_program.size(); i++)
    //     {
    //         auto& instruction = m_program[i].first;
    //         if (std::holds_alternative<lowering::LabelInstruction>(instruction))
    //             m_state.labels[std::get<lowering::LabelInstruction>(instruction).label] = i;
    //         else if (std::holds_alternative<lowering::RegisterFunctionInstruction>(instruction))
    //             m_state.functions[std::get<lowering::RegisterFunctionInstruction>(instruction).function_name] = i;
    //     }

    //     m_state.instruction_pointer = latest_program_size;

    //     if (m_state.functions.find(MAIN_FUNCTION_NAME) != m_state.functions.end())
    //     {
    //         m_state.instruction_pointer = m_program.size(); // return address
    //         std::visit(
    //             lowering::InstructionExecuter(m_state),
    //             lowering::Instruction{lowering::CallInstruction(MAIN_FUNCTION_NAME, true)}
    //         );
    //     }

    //     for (; m_state.instruction_pointer < m_program.size(); m_state.instruction_pointer++)
    //     {
    //         const auto& [instruction, location] = m_program[m_state.instruction_pointer];
    //         if (location.has_value() and m_breakpoints.contains(location.value().row))
    //             return InterpreterError::BreakpointReachedError;
            
    //         auto result = std::visit(lowering::InstructionExecuter(m_state), instruction);
    //         if (result.is_error())
    //             return result;
            
    //         if (result.has_value())
    //             m_state.result_stack.push(result);

    //         if (m_state.instruction_pointer == m_program.size() - 1)
    //         {
    //             if (!m_state.result_stack.empty())
    //             {
    //                 auto top = m_state.result_stack.top();
    //                 m_state.result_stack.pop();
    //                 return top;
    //             }
    //         }
    //     }
        
    //     return InterpreterError::None;
    // }

    // Breakpoints& Interpreter::breakpoints()
    // {
    //     return m_breakpoints;
    // }

    // void Interpreter::continue_execution()
    // {
    //     for (; m_state.instruction_pointer < m_program.size(); m_state.instruction_pointer++)
    //     {
    //         const auto& [instruction, location] = m_program[m_state.instruction_pointer];
    //         if (location.has_value() and m_breakpoints.contains(location.value().row))
    //             return;
            
    //         auto result = std::visit(lowering::InstructionExecuter(m_state), instruction);
    //         if (result.is_error())
    //             return;
            
    //         if (result.has_value())
    //             m_state.result_stack.push(result);

    //         if (m_state.instruction_pointer == m_program.size() - 1)
    //         {
    //             if (!m_state.result_stack.empty())
    //             {
    //                 auto top = m_state.result_stack.top();
    //                 m_state.result_stack.pop();
    //                 return;
    //             }
    //         }
    //     }
    // }

    // void Interpreter::step_into()
    // {
    //     auto location = m_program[m_state.instruction_pointer].second;
    // }


    // ***************
    // * Interpreter *
    // ***************

    ParserResult Interpreter::parse(const std::string &code)
    {
        return m_parser.parse(code);
    }

    binding::BinderResult<binding::BoundNode> Interpreter::bind(const TreeNode& root_node)
    {
        return m_binder.bind(root_node);
    }

    LocationAnotatedProgram Interpreter::lower(const binding::BoundBlockStatement* bind_result)
    {
        return m_lowerer.lower(bind_result);
    }



    Interpreter::RunningInterpreterCreationResult Interpreter::interpret(const std::string &code)
    {
        return interpret(m_parser.parse(code));
    }

    Interpreter::RunningInterpreterCreationResult Interpreter::interpret(const ParserResult &parse_result)
    {
        return interpret(m_binder.bind(parse_result.root_node()));
    }

    Interpreter::RunningInterpreterCreationResult Interpreter::interpret(const binding::BinderResult<binding::BoundNode>& bind_result)
    {
        if (bind_result.is_error())
        {
            return RunningInterpreterCreationResult{
                std::nullopt,
                bind_result.get_error()
            };
        }

        {
            debug::bound_ast_as_text_tree(std::cout, *bind_result.get_value());
        }

        binding::BoundNode* root = bind_result.get_value();
        if (root->bound_node_kind() != binding::BoundNodeKind::BlockStatement)
        {
            // Im too lazy to handle it.. I mean.. I cant handle anything anymore lol
            throw std::runtime_error("Reached unreachable at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
        }

        auto lowered_with_location = m_lowerer.lower(static_cast<binding::BoundBlockStatement*>(root));

        {
            // std::vector<lowering::Instruction> lowered;
            // std::transform(
            //     lowered_with_location.begin(),
            //     lowered_with_location.end(),
            //     std::back_inserter(lowered),
            //     [](auto& pair) { return pair.first; }
            // );

            // debug::instructions_as_text(std::cout, lowered);
            debug::instructions_as_text(std::cout, lowered_with_location);
        }
        
        // this doesnt have copy constructor
        
        return RunningInterpreterCreationResult{
            RunningInterpreter(lowered_with_location),
            std::nullopt
        };
    }

    Interpreter::RunningInterpreterCreationResult Interpreter::interpret(const LocationAnotatedProgram& program)
    {
        return RunningInterpreterCreationResult{
            RunningInterpreter(program),
            std::nullopt
        };
    }


    std::optional<LocationAnotatedProgram> Interpreter::compile_or_nothing(const std::string& code)
    {
        auto parse_result = m_parser.parse(code);
        if (parse_result.has_error())
            return std::nullopt;

        auto binded {m_binder.bind(parse_result.root_node())};
        if (binded.is_error())
            return std::nullopt;

        // debug::bound_ast_as_text_tree(std::cout, *binded.get_value());

        binding::BoundNode* root = binded.get_value();
        if (root->bound_node_kind() != binding::BoundNodeKind::BlockStatement)
            return std::nullopt;

        auto lowered_with_location = m_lowerer.lower(static_cast<binding::BoundBlockStatement*>(root));

        // {
        //     std::vector<lowering::Instruction> lowered;
        //     std::transform(
        //         lowered_with_location.begin(),
        //         lowered_with_location.end(),
        //         std::back_inserter(lowered),
        //         [](auto& pair) { return pair.first; }
        //     );

        //     debug::instructions_as_text(std::cout, lowered);
        // }

        return lowered_with_location;
    }

    Interpreter::RunningInterpreterCreationResult Interpreter::interpret()
    {
        return RunningInterpreterCreationResult{
            RunningInterpreter({}),
            std::nullopt
        };
    }

    

    // **********************
    // * RunningInterpreter *
    // **********************

    RunningInterpreter::RunningInterpreter(const LocationAnotatedProgram& program)
        : m_state(InterpreterState(0, STACK_SIZE, GLOBAL_SCOPE_SIZE))
        , m_current_location{0, 0}
        , m_continuing_after_breakpoint(false)
    {
        append_code(program);
    }

    Location RunningInterpreter::current_location() const
    {
        return m_current_location;
    }

    RunningInterpreter& RunningInterpreter::append_code(const LocationAnotatedProgram& program)
    {
        size_t latest_program_size = m_program.size();
        std::copy(program.begin(), program.end(), std::back_inserter(m_program));

        for (size_t i = latest_program_size; i < m_program.size(); i++)
        {
            auto& instruction = m_program[i].first;
            if (std::holds_alternative<lowering::LabelInstruction>(instruction))
                m_state.labels[std::get<lowering::LabelInstruction>(instruction).label] = i;
            else if (std::holds_alternative<lowering::RegisterFunctionInstruction>(instruction))
                m_state.functions[std::get<lowering::RegisterFunctionInstruction>(instruction).function_name] = i;
        }

        return *this;
    }

    InterpreterResult RunningInterpreter::continue_execution()
    {

        if (m_state.functions.find(MAIN_FUNCTION_NAME) != m_state.functions.end() and (m_state.instruction_pointer == 0))
        {
            m_state.instruction_pointer = m_program.size(); // return address
            std::visit(
                lowering::InstructionExecuter(m_state),
                lowering::Instruction{lowering::CallInstruction(MAIN_FUNCTION_NAME, true)}
            );
        }

        // auto call_stack = m_state.call_stack;
        // auto global_scope = m_state.global_scope;

        for (; m_state.instruction_pointer < m_program.size(); )
        {
            const auto& [instruction, location] = m_program[m_state.instruction_pointer];  

            bool should_break = false;
            if (location.has_value())
            {
                should_break = m_current_location.row != location.value().row;
                m_current_location = location.value();
            }

            if (
                m_breakpoints.contains(m_current_location.row) 
                and not m_continuing_after_breakpoint
                and should_break
            ){
                m_continuing_after_breakpoint = true;
                return InterpreterError::BreakpointReachedError;
            }

            m_continuing_after_breakpoint = false;
            m_state.instruction_pointer++;

            auto result = std::visit(lowering::InstructionExecuter(m_state), instruction);
            if (result.is_error())
            {
                m_state.instruction_pointer = m_program.size();
                // m_state.call_stack = call_stack; // literally unwinding xd
                // m_state.global_scope = global_scope;
                return result;
            }
            
            if (result.has_value())
                m_state.result_stack.push(result);

            if (m_state.instruction_pointer == m_program.size())
            {
                if (not m_state.result_stack.empty())
                {
                    auto top = m_state.result_stack.top();
                    m_state.result_stack.pop();
                    return top;
                }
            }

        }
        
        return InterpreterError::None;
    }

    InterpreterResult RunningInterpreter::next()
    {

        auto last_location = m_current_location;
        while (true)
        {
            const auto& [instruction, location] = m_program[m_state.instruction_pointer];
            if (location.has_value())
                m_current_location = location.value();

            if (m_current_location.row != last_location.row)
                break;

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

            m_state.instruction_pointer++;
        }

       
        return InterpreterError::None;
    }

}
