#include "debug.hpp"
#include <functional>
#include <variant>
#include <iomanip>

#include "binding/bound_block_statement.hpp"
#include "binding/bound_literal_expression.hpp"
#include "binding/bound_binary_expression.hpp"
#include "binding/bound_expression_statement.hpp"
#include "binding/bound_cast_expression.hpp"
#include "binding/bound_parenthesized_expression.hpp"
#include "binding/bound_variable_declaration_statement.hpp"
#include "binding/bound_identifier_expression.hpp"
#include "binding/bound_assignment_expression.hpp"
#include "binding/bound_if_statement.hpp"
#include "binding/bound_while_statement.hpp"
#include "binding/bound_do_statement.hpp"
#include "binding/bound_function_statement.hpp"
#include "binding/bound_return_statement.hpp"
#include "binding/bound_call_expression.hpp"
#include "binding/bound_for_statement.hpp"
#include "binding/bound_pointer_expression.hpp"

#include "lowering/binary_operation_instruction.hpp"
#include "lowering/cast_instruction.hpp"
#include "lowering/drop_instruction.hpp"

#include "overloaded.hpp"

namespace scc
{
    namespace debug
    {
        void escape_string(std::ostream &ss, const std::string &str)
        {
            for (auto c : str)
            {
                switch (c)
                {
                case '\n':
                    ss << "\\\\n";
                    break; // for new line in puml
                case '\t':
                    ss << "\\t";
                    break;
                case '\r':
                    ss << "\\r";
                    break;
                case '\b':
                    ss << "\\b";
                    break;
                case '\f':
                    ss << "\\f";
                    break;
                case '\\':
                    ss << "\\\\";
                    break;
                case '\"':
                    ss << "\\\"";
                    break;
                default:
                    ss << c;
                    break;
                }
            }
        }

        void ast_as_json(std::ostream &ss, const ParserResult &parser_result)
        {
            std::function<void(TreeNode node)> ast_as_json_impl = [&](TreeNode node)
            {
                ss << "{\"node_name\": \"";

                if (node.has_error())
                    ss << "<color:red><b>";

                escape_string(ss, node.symbol_name());

                ss << "\",\"location\": \"" << node.location().row << ":" << node.location().column;

                ss << "\",\"value\": \"";
                escape_string(ss, node.value());
                ss << "\"";

                uint32_t child_count = node.named_child_count();
                if (child_count > 0)
                {
                    ss << ",\"children\": [";
                    for (uint32_t i = 0; i < child_count; i++)
                    {
                        if (i > 0)
                            ss << ",";

                        ast_as_json_impl(node.named_child(i));
                    }

                    ss << "]";
                }

                ss << "}";
            };

            ast_as_json_impl(parser_result.root_node());
        }

        void ast_as_puml(std::ostream &ss, const ParserResult &parser_result)
        {

            ss << "@startjson" << std::endl;
            ast_as_json(ss, parser_result);
            ss << std::endl;
            ss << "@endjson" << std::endl;
        }

        void ast_as_text_tree(std::ostream &ss, const ParserResult &parser_result)
        {

            std::function<void(TreeNode, int, bool, std::string)> ast_as_text_tree_impl = [&](TreeNode node, int depth, bool last, std::string prefix)
            {
                constexpr auto DOWN_PIPE  = "│   ";
                constexpr auto SPLIT_PIPE = "├── ";
                constexpr auto LAST_PIPE  = "└── ";
                constexpr auto SPACE      = "    ";

                if (depth > 0)
                {
                    ss << prefix;
                    if (last)
                        ss << LAST_PIPE;
                    else
                        ss << SPLIT_PIPE;
                }

                ss << node.symbol_name() << " ==>\t";
                auto value = node.value();
                constexpr auto MAX_VALUE_LENGTH = 10;
                
                if(value.length() > MAX_VALUE_LENGTH)
                    value = value.substr(0, MAX_VALUE_LENGTH) + "...";    
            
                escape_string(ss, value);
                ss << std::endl;



                uint32_t child_count = node.named_child_count();
                for (uint32_t i = 0; i < child_count; i++)
                {
                    if (depth > 0)
                        ast_as_text_tree_impl(node.named_child(i)
                                            , depth + 1
                                            , i == child_count - 1
                                            , prefix + (last ? SPACE : DOWN_PIPE));
                    else
                        ast_as_text_tree_impl(node.named_child(i)
                                            , depth + 1
                                            , i == child_count - 1
                                            , std::string());
                }
            };

            ast_as_text_tree_impl(parser_result.root_node()
                                , 0
                                , false
                                , std::string());
        }

        void type_as_text(std::ostream &ss, const Type &type)
        {
            // dont tell anyone.. its for legacy reasons :D
            ss << type;
        }

        // TODOOOOOOOOOO0: convert it into multiple functions for each node type
        void bound_ast_as_text_tree(std::ostream &ss, const binding::BoundNode &bound_node)
        {  
            std::function<void(const binding::BoundNode&, int, bool, std::string)> bound_ast_as_text_tree_impl = [&](const binding::BoundNode& node, int depth, bool last, std::string prefix)
            {
                constexpr auto DOWN_PIPE  = "│   ";
                constexpr auto SPLIT_PIPE = "├── ";
                constexpr auto LAST_PIPE  = "└── ";
                constexpr auto SPACE      = "    ";

                if (depth > 0)
                {
                    ss << prefix;
                    if (last)
                        ss << LAST_PIPE;
                    else
                        ss << SPLIT_PIPE;
                }
                static_assert(static_cast<int>(binding::BoundNodeKind::COUNT) == 19, "Update this switch statement");
                
                
                if (node.location)
                    ss << node.location.value().row << ":" << node.location.value().column << " ";

                switch (node.bound_node_kind())
                {
                case binding::BoundNodeKind::BlockStatement:
                {
                    auto& block_statement = static_cast<const binding::BoundBlockStatement&>(node);
                    ss << "BlockStatement" << std::endl;
                    for(uint32_t i = 0; i < block_statement.statements.size(); i++)
                    {
                        if(depth > 0)
                            bound_ast_as_text_tree_impl(*block_statement.statements[i]
                                                        , depth + 1
                                                        , i == block_statement.statements.size() - 1
                                                        , prefix + (last ? SPACE : DOWN_PIPE));
                        else
                            bound_ast_as_text_tree_impl(*block_statement.statements[i]
                                                        , depth + 1
                                                        , i == block_statement.statements.size() - 1
                                                        , std::string());
                    }
                    break;
                }

                case binding::BoundNodeKind::LiteralExpression:
                {
                    auto& literal_expression = static_cast<const binding::BoundLiteralExpression&>(node);
                    ss << "LiteralExpression (" << literal_expression.type << ") ==> ";

                    type_as_text(ss, literal_expression.type); 
                    
                    ss << std::endl;
                    break;
                }

                case binding::BoundNodeKind::BinaryExpression:
                {
                    auto& binary_expression = static_cast<const binding::BoundBinaryExpression&>(node);
                    ss << "BinaryExpression(" << binary_expression.type << ") ==> ";
                    
                    #define CASE_OP_KIND(KIND, OP) \
                    case binding::BoundBinaryExpression::OperatorKind::KIND: \
                        ss << std::quoted(OP) << std::endl; \
                        break;

                    static_assert(static_cast<int>(binding::BoundBinaryExpression::OperatorKind::COUNT) == 18, "Update this switch statement");

                    switch(binary_expression.op_kind)
                    {
                        CASE_OP_KIND(Addition, "+")
                        CASE_OP_KIND(Subtraction, "-")
                        CASE_OP_KIND(Multiplication, "*")
                        CASE_OP_KIND(Division, "/")
                        CASE_OP_KIND(Modulo, "%")
                        CASE_OP_KIND(BitwiseAnd, "&")
                        CASE_OP_KIND(BitwiseOr, "|")
                        CASE_OP_KIND(BitwiseXor, "^")
                        CASE_OP_KIND(BitwiseShiftLeft, "<<")
                        CASE_OP_KIND(BitwiseShiftRight, ">>")
                        CASE_OP_KIND(LogicalAnd, "&&")
                        CASE_OP_KIND(LogicalOr, "||")
                        CASE_OP_KIND(Equals, "==")
                        CASE_OP_KIND(NotEquals, "!=")
                        CASE_OP_KIND(LessThan, "<")
                        CASE_OP_KIND(GreaterThan, ">")
                        CASE_OP_KIND(LessThanOrEqual, "<=")
                        CASE_OP_KIND(GreaterThanOrEqual, ">=")
                        default:
                            ss << "Unreachable " << __FILE__ << ":" << __LINE__ << std::endl;
                            break;
                    }

                    #undef CASE_OP_KIND

                    bound_ast_as_text_tree_impl(*binary_expression.left
                                                , depth + 1
                                                , false
                                                , prefix + (last ? SPACE : DOWN_PIPE));
                    bound_ast_as_text_tree_impl(*binary_expression.right 
                                                , depth + 1
                                                , true
                                                , prefix + (last ? SPACE : DOWN_PIPE));
                    break;
                }

                case binding::BoundNodeKind::CastExpression:
                {
                    auto& cast_expression = static_cast<const binding::BoundCastExpression&>(node);
                    ss << "CastExpression (" << cast_expression.type << ") ==> ";
                    type_as_text(ss, cast_expression.type);
                    ss << std::endl;
                    bound_ast_as_text_tree_impl(*cast_expression.expression
                                                , depth + 1
                                                , true
                                                , prefix + (last ? SPACE : DOWN_PIPE));
                    break;
                }

                case binding::BoundNodeKind::ExpressionStatement:
                {
                    auto& expression_statement = static_cast<const binding::BoundExpressionStatement&>(node);
                    ss << "ExpressionStatement" << std::endl;
                    bound_ast_as_text_tree_impl(*expression_statement.expression
                                                , depth + 1
                                                , true
                                                , prefix + (last ? SPACE : DOWN_PIPE));
                    break;
                }

                case binding::BoundNodeKind::ParenthesizedExpression:
                {
                    auto& parenthesized_expression = static_cast<const binding::BoundParenthesizedExpression&>(node);
                    ss << "ParenthesizedExpression" << std::endl;
                    
                    for(size_t i = 0; i < parenthesized_expression.expressions.size(); i++)
                    {
                        bound_ast_as_text_tree_impl(*parenthesized_expression.expressions[i]
                                                    , depth + 1
                                                    , i == parenthesized_expression.expressions.size() - 1
                                                    , prefix + (last ? SPACE : DOWN_PIPE));
                    }
                    break;
                }
                case binding::BoundNodeKind::IdentifierExpression:
                {
                    auto& identifier_expression = static_cast<const binding::BoundIdentifierExpression&>(node);
                    ss << "IdentifierExpression (" << identifier_expression.type << ") ==> " << identifier_expression.identifier << std::endl;
                    break;
                }
                case binding::BoundNodeKind::AssignmentExpression:
                {
                    auto& assignment_expression = static_cast<const binding::BoundAssignmentExpression&>(node);
                    ss << "AssignmentExpression (" << assignment_expression.type << ") ==> " << assignment_expression.identifier << std::endl;
                    bound_ast_as_text_tree_impl(*assignment_expression.expression
                                                , depth + 1
                                                , true
                                                , prefix + (last ? SPACE : DOWN_PIPE));
                    break;
                }
                case binding::BoundNodeKind::IfStatement:
                {
                    auto& if_statement = static_cast<const binding::BoundIfStatement&>(node);
                    ss << "IfStatement" << std::endl;
                    bound_ast_as_text_tree_impl(*if_statement.condition
                                                , depth + 1
                                                , false
                                                , prefix + (last ? SPACE : DOWN_PIPE));
                    bound_ast_as_text_tree_impl(*if_statement.then_statement
                                                , depth + 1
                                                , !if_statement.has_else()
                                                , prefix + (last ? SPACE : DOWN_PIPE));
                    if(if_statement.has_else())
                        bound_ast_as_text_tree_impl(*if_statement.else_statement
                                                    , depth + 1
                                                    , true
                                                    , prefix + (last ? SPACE : DOWN_PIPE));
                    break;
                }
                case binding::BoundNodeKind::WhileStatement:
                {
                    auto& while_statement = static_cast<const binding::BoundWhileStatement&>(node);
                    ss << "WhileStatement" << std::endl;
                    bound_ast_as_text_tree_impl(*while_statement.condition
                                                , depth + 1
                                                , !while_statement.body
                                                , prefix + (last ? SPACE : DOWN_PIPE));
                    if (while_statement.body)
                    {
                        bound_ast_as_text_tree_impl(*while_statement.body
                                                    , depth + 1
                                                    , true
                                                    , prefix + (last ? SPACE : DOWN_PIPE));
                    }
                    break;
                }
                case binding::BoundNodeKind::DoStatement:
                {
                    auto& do_statement = static_cast<const binding::BoundDoStatement&>(node);
                    ss << "DoStatement" << std::endl;
                    bound_ast_as_text_tree_impl(*do_statement.condition
                                                , depth + 1
                                                , false
                                                , prefix + (last ? SPACE : DOWN_PIPE));
                    if (do_statement.body)
                    {
                        bound_ast_as_text_tree_impl(*do_statement.body
                                                    , depth + 1
                                                    , true
                                                    , prefix + (last ? SPACE : DOWN_PIPE));
                    }
                    break;
                }
                case binding::BoundNodeKind::BreakStatement:
                {
                    ss << "BreakStatement" << std::endl;
                    break;
                }
                case binding::BoundNodeKind::ContinueStatement:
                {
                    ss << "ContinueStatement" << std::endl;
                    break;
                }
                case binding::BoundNodeKind::FunctionStatement:
                {
                    auto& function = static_cast<const binding::BoundFunctionStatement&>(node);
                    ss << "FunctionStatement (" << function.return_type << ") ==> " << function.function_name << "(";
                    for (size_t i = 0; i < function.parameters.size(); i++)
                    {
                        ss << function.parameters[i].get()->type << " " << function.parameters[i].get()->variable_name;
                        if (i != function.parameters.size() - 1)
                            ss << ", ";
                    } 
                    ss << ")" << std::endl;

                    if (function.body)
                    {
                        bound_ast_as_text_tree_impl(*function.body
                                                    , depth + 1
                                                    , true
                                                    , prefix + (last ? SPACE : DOWN_PIPE));
                    }
                    break;
                }
                case binding::BoundNodeKind::ReturnStatement:
                {
                    auto& return_statement = static_cast<const binding::BoundReturnStatement&>(node);
                    ss << "ReturnStatement" << std::endl;
                    if (return_statement.has_return_expression())
                    {
                        bound_ast_as_text_tree_impl(*return_statement.return_expression
                                                    , depth + 1
                                                    , true
                                                    , prefix + (last ? SPACE : DOWN_PIPE));
                    }
                    break;
                }
                case binding::BoundNodeKind::CallExpression:
                {
                    auto& call_expression = static_cast<const binding::BoundCallExpression&>(node);
                    ss << "CallExpression (" << call_expression.type << ") ==> " << call_expression.function_name << std::endl;
                    for (size_t i = 0; i < call_expression.arguments.size(); i++)
                    {
                        bound_ast_as_text_tree_impl(*call_expression.arguments[i]
                                                    , depth + 1
                                                    , i == call_expression.arguments.size() - 1
                                                    , prefix + (last ? SPACE : DOWN_PIPE));
                    }
                    break;
                }
                case binding::BoundNodeKind::ForStatement:
                {
                    auto& for_statement = static_cast<const binding::BoundForStatement&>(node);
                    ss << "ForStatement" << std::endl;
                    if (for_statement.initializer)
                    {
                        bound_ast_as_text_tree_impl(*for_statement.initializer
                                                    , depth + 1
                                                    , false
                                                    , prefix + (last ? SPACE : DOWN_PIPE));
                    }
                    if (for_statement.condition)
                    {
                        bound_ast_as_text_tree_impl(*for_statement.condition
                                                    , depth + 1
                                                    , false
                                                    , prefix + (last ? SPACE : DOWN_PIPE));
                    }
                    if (for_statement.increment)
                    {
                        bound_ast_as_text_tree_impl(*for_statement.increment
                                                    , depth + 1
                                                    , false
                                                    , prefix + (last ? SPACE : DOWN_PIPE));
                    }
                    if (for_statement.body)
                    {
                        bound_ast_as_text_tree_impl(*for_statement.body
                                                    , depth + 1
                                                    , true
                                                    , prefix + (last ? SPACE : DOWN_PIPE));
                    }
                    break;
                }
                case binding::BoundNodeKind::PointerExpression:
                {
                    auto& pointer_expression = static_cast<const binding::BoundPointerExpression&>(node);
                    ss << "PointerExpression (" << pointer_expression.type << ") ==> " << pointer_expression.identifier << std::endl;
                    break;
                }
                case binding::BoundNodeKind::VariableDeclarationStatement:
                {
                    auto& variable_declaration_statement = static_cast<const binding::BoundVariableDeclarationStatement&>(node);
                    switch(variable_declaration_statement.variable_declaration_statement_kind())
                    {
                        using Kind = binding::BoundVariableDeclarationStatement::VariableDeclarationStatementKind;
                        static_assert(static_cast<int>(Kind::COUNT) == 3, "Update this switch statement");
                        case Kind::Value:
                        {
                            ss << "VariableValueDeclarationStatement (" << (variable_declaration_statement.is_constant ? "const " : "")  << variable_declaration_statement.type << " " ;
                            ss << variable_declaration_statement.variable_name << ")" << std::endl;

                            auto& vvds = static_cast<const binding::BoundVariableValueDeclarationStatement&>(variable_declaration_statement);
                            if (vvds.initializer)
                            {
                                bound_ast_as_text_tree_impl(*vvds.initializer
                                                            , depth + 1
                                                            , true
                                                            , prefix + (last ? SPACE : DOWN_PIPE));
                            }
                            break;
                        }
                        case Kind::Pointer:
                        {
                            ss << "VariablePointerDeclarationStatement (" << (variable_declaration_statement.is_constant ? "const " : "") <<  variable_declaration_statement.type << " " ;
                            ss << variable_declaration_statement.variable_name << ")" << std::endl;

                            auto& vpds = static_cast<const binding::BoundVariablePointerDeclarationStatement&>(variable_declaration_statement);
                            if (vpds.initializer)
                            {
                                bound_ast_as_text_tree_impl(*vpds.initializer
                                                            , depth + 1
                                                            , true
                                                            , prefix + (last ? SPACE : DOWN_PIPE));
                            }
                            break;
                        }
                        case Kind::StaticArray:
                        {
                            ss << "VariableStaticArrayDeclarationStatement (" << (variable_declaration_statement.is_constant ? "const " : "") << variable_declaration_statement.type << " " ;
                            auto& vsads = static_cast<const binding::BoundVariableStaticArrayDeclarationStatement&>(variable_declaration_statement);
                            ss << variable_declaration_statement.variable_name << " [" << vsads.array_size << "])" << std::endl;

                            for(size_t i = 0; i < vsads.initializers.size(); i++)
                            {
                                if(depth > 0)
                                    bound_ast_as_text_tree_impl(*vsads.initializers[i]
                                                                , depth + 1
                                                                , i == vsads.initializers.size() - 1
                                                                , prefix + (last ? SPACE : DOWN_PIPE));
                                else
                                    bound_ast_as_text_tree_impl(*vsads.initializers[i]
                                                                , depth + 1
                                                                , i == vsads.initializers.size() - 1
                                                                , std::string());
                            }
                            break;
                        }
                        default:
                            ss << "Unreachable " << __FILE__ << ":" << __LINE__ << std::endl;
                            break;
                    }
                    break;
                }
                default:
                    ss << "Unreachable " << __FILE__ << ":" << __LINE__ << std::endl;
                    break;
                }
            };

            bound_ast_as_text_tree_impl(bound_node
                                , 0
                                , false
                                , std::string());

        }
    
        void memory_chunks_as_json(std::ostream& ss, const Memory& memory)
        {
            ss << "[";
            const std::map<Memory::address_t, Memory::MemoryChunk>& chunks = memory.get_chunks();
            
            bool first = true;
            for (auto& [address, chunk] : chunks)
            {
                ss << (first ? "" : ", ") << std::endl;
                first = false;
                ss << "{";
                ss << "\"start\": " << address << ", ";
                ss << "\"end\": "   << memory.get_chunk_end(address).value() << ", ";
                ss << "\"size\": "  << memory.get_chunk_size(address).value() << ", ";
                ss << "}";
            }

            ss << "]";
        }

        void memory_chunks_as_puml(std::ostream& ss, const Memory& memory)
        {
            ss << "@startjson" << std::endl;
            memory_chunks_as_json(ss, memory);
            ss << std::endl;
            ss << "@endjson" << std::endl;
        }

        void memory_chunks_as_text(std::ostream &ss, const Memory& memory)
        {
            const std::map<Memory::address_t, Memory::MemoryChunk>& chunks = memory.get_chunks();
            
            for (auto& [address, chunk] : chunks)
                ss << "Chunk " << address << " [" << address << ", " << memory.get_chunk_end(address).value() << "] (" << memory.get_chunk_size(address).value() << ") " << std::endl;
        }

        void interpreter_error_as_text(std::ostream &ss, InterpreterError error)
        {
            static_assert(static_cast<int>(InterpreterError::COUNT) == 20, "Edit this code");
            switch (error)
            {
                case InterpreterError::None:
                    ss << "None";
                    break;
                case InterpreterError::ParseError:
                    ss << "Parse error";
                    break;
                case InterpreterError::BindError:
                    ss << "Bind error";
                    break;
                case InterpreterError::RuntimeError:
                    ss << "Runtime error";
                    break;
                case InterpreterError::InvalidOperationError:
                    ss << "Invalid operation error";
                    break;
                case InterpreterError::ReachedUnreachableError:
                    ss << "Reached unreachable error";
                    break;
                case InterpreterError::DivisionByZeroError:
                    ss << "Division by zero error";
                    break;
                case InterpreterError::VariableAlreadyExistsError:
                    ss << "Variable already exists error";
                    break;
                case InterpreterError::VariableDoesntExistError:
                    ss << "Variable doesn't exist error";
                    break;
                case InterpreterError::VariableNotInitializedError:
                    ss << "Variable not initialized error";
                    break;
                case InterpreterError::UnhandeledSignalError:
                    ss << "Unhandeled signal error";
                    break;
                case InterpreterError::MissingMainFunctionError:
                    ss << "Missing main function error";
                    break;
                case InterpreterError::FunctionAlreadyDefinedError:
                    ss << "Function already defined error";
                    break;
                case InterpreterError::IncosistentFunctionSignatureError:
                    ss << "Incosistent function signature error";
                    break;
                case InterpreterError::FunctionArgumentCountMismatchError:
                    ss << "Function argument count mismatch error";
                    break;
                case InterpreterError::FunctionNotDeclaredError:
                    ss << "Function not declared error";
                    break;
                case InterpreterError::FunctionNotDefinedError:
                    ss << "Function not defined error";
                    break;
                case InterpreterError::MissingReturnStatementError:
                    ss << "Missing return statement error";
                    break;
                case InterpreterError::MissingReturnValueError:
                    ss << "Missing return value error";
                    break;
                case InterpreterError::BreakpointReachedError:
                    ss << "Breakpoint reached error";
                    break;
                default:
                    ss << "Unknown error";
                    break;
            }
        }
    
        void instruction_as_text(std::ostream &ss, const lowering::Instruction& instruction)
        {
            static_assert(lowering::InstructionCount == 18, "Update this switch statement");
            std::visit(overloaded{
                [&](lowering::BinaryOperationInstruction binary_operation) { 
                    ss << "BinaryOperation ==> ";
                    #define CASE_OP_KIND(KIND, OP) \
                        case binding::BoundBinaryExpression::OperatorKind::KIND: \
                            ss << std::quoted(OP); \
                            break;

                        static_assert(static_cast<int>(binding::BoundBinaryExpression::OperatorKind::COUNT) == 18, "Update this switch statement");

                        switch(binary_operation.operator_kind)
                        {
                            CASE_OP_KIND(Addition, "+")
                            CASE_OP_KIND(Subtraction, "-")
                            CASE_OP_KIND(Multiplication, "*")
                            CASE_OP_KIND(Division, "/")
                            CASE_OP_KIND(Modulo, "%")
                            CASE_OP_KIND(BitwiseAnd, "&")
                            CASE_OP_KIND(BitwiseOr, "|")
                            CASE_OP_KIND(BitwiseXor, "^")
                            CASE_OP_KIND(BitwiseShiftLeft, "<<")
                            CASE_OP_KIND(BitwiseShiftRight, ">>")
                            CASE_OP_KIND(LogicalAnd, "&&")
                            CASE_OP_KIND(LogicalOr, "||")
                            CASE_OP_KIND(Equals, "==")
                            CASE_OP_KIND(NotEquals, "!=")
                            CASE_OP_KIND(LessThan, "<")
                            CASE_OP_KIND(GreaterThan, ">")
                            CASE_OP_KIND(LessThanOrEqual, "<=")
                            CASE_OP_KIND(GreaterThanOrEqual, ">=")
                            default:
                                ss << "Unreachable " << __FILE__ << ":" << __LINE__ << std::endl;
                                break;
                        }

                        #undef CASE_OP_KIND
                },
                [&](lowering::CastInstruction cast) {
                    ss << "Cast ==> " << cast.type;
                },
                [&](lowering::DropInstruction drop) {
                    ss << "Drop ==> " << drop.count;
                },
                [&](lowering::PopScopeInstruction) {
                    ss << "PopScope";
                },
                [&](lowering::PushLiteralInstruction) {
                    ss << "PushLiteral"; // TODOO: Get value
                },
                [&](lowering::PushScopeInstruction) {
                    ss << "PushScope";
                },
                [&](lowering::CreateValueVariableInstruction create_value_variable) {
                    using CVFlags = lowering::CreateValueVariableInstruction::Flags;
                    ss << "CreateValueVariable ==> "<< (create_value_variable.flags & CVFlags::IsGlobal ? "global " : "") \
                    << (create_value_variable.flags & CVFlags::IsConst ? "const " : "") \
                    << create_value_variable.variable_type \
                    << " " << create_value_variable.variable_name;
                },
                [&](lowering::IdentifierInstruction identifer) {
                    ss << "Identifer ==> " << identifer.type << " " << identifer.identifier;
                },
                [&](lowering::AssignmentInstruction assignment) {
                    ss << "Assignment ==> " << assignment.variable_name;
                },
                [&](lowering::LabelInstruction label) {
                    ss << "Label ==> " << label.label;
                },
                [&](lowering::GotoTrueInstruction goto_true) {
                    ss << "GotoTrue ==> " << goto_true.label;
                },
                [&](lowering::GotoFalseInstruction goto_false) {
                    ss << "GotoFalse ==> " << goto_false.label;
                },[&](lowering::GotoInstruction _goto) {
                    ss << "Goto ==> " << _goto.label;
                },
                [&](lowering::ReturnInstruction _return) {
                    ss << "Return" << (_return.has_return_expression ? " value" : "");
                },
                [&](lowering::CallInstruction call) {
                    ss << "Call ==> " << call.function_name;
                },
                [&](lowering::CallInbuildInstruction call) {
                    ss << "CallInbuild ==> " << call.function_name;
                },
                [&](lowering::RegisterFunctionInstruction register_function) {
                    ss << "RegisterFunction ==> " << register_function.function_name;
                },
                [&](lowering::ReferenceInstruction reference) {
                    ss << "Reference ==> " << reference.identifier;
                }
            }, instruction);
        }

        void instructions_as_text(std::ostream &ss, const std::vector<lowering::Instruction>& instructions)
        {
            for (size_t i = 0; i < instructions.size(); i++)
            {
                ss << i << ": ";
                instruction_as_text(ss, instructions[i]);
                ss << std::endl;
            }
        }

        void instructions_as_text(std::ostream &ss, const LocationAnotatedProgram& instructions)
        {
            for (size_t i = 0; i < instructions.size(); i++)
            {
                ss << i << ": ";
                if (instructions[i].second.has_value())
                    ss << instructions[i].second.value().row << ":" << instructions[i].second.value().column << " ";
                instruction_as_text(ss, instructions[i].first);
                ss << std::endl;
            }
        }
    }
}
