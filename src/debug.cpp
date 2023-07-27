#include "debug.hpp"
#include <functional>
#include <variant>
#include <any>
#include <iomanip>

#include "binding/bound_block_statement.hpp"
#include "binding/bound_literal_expression.hpp"
#include "binding/bound_binary_expression.hpp"
#include "binding/bound_expression_statement.hpp"
#include "binding/bound_cast_expression.hpp"


// https://en.cppreference.com/w/cpp/utility/variant/visit
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;


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
            static_assert(static_cast<int>(Type::Kind::COUNT) == 12, "Update this code");
            switch (type.kind)
            {
                case Type::Kind::Char: ss << "char"; break;
                case Type::Kind::U8: ss << "unsigned char"; break;
                case Type::Kind::U16: ss << "unsigned short"; break;
                case Type::Kind::U32: ss << "unsigned int"; break;
                case Type::Kind::U64: ss << "unsigned long"; break;
                case Type::Kind::I8: ss << "signed char"; break;
                case Type::Kind::I16: ss << "short"; break;
                case Type::Kind::I32: ss << "int"; break;
                case Type::Kind::I64: ss << "long"; break;
                case Type::Kind::F32: ss << "float"; break;
                case Type::Kind::F64: ss << "double"; break;
                case Type::Kind::Bool: ss << "bool"; break;
                default: ss << "Unreachable " << __FILE__ << ":" << __LINE__; std::exit(1); break;
            }
        }

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
                static_assert(static_cast<int>(binding::BoundNodeKind::COUNT) == 5, "Update this switch statement");
                
                
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
                    
                    /* #define VISIT_LITERAL_EXPRESSION_TYPE(KIND, C_TYPE, EXPECTED_SIZE) \
                    // [&](const KIND&){ \
                    //     static_assert(sizeof(C_TYPE) == EXPECTED_SIZE, "sizeof(" #C_TYPE ") is not " #EXPECTED_SIZE); \
                    //     ss << std::any_cast<C_TYPE>(literal_expression.value) << std::endl; \
                     }*/

                    // std::visit(overloaded{
                    //     [&](const char_type&){
                    //         static_assert(sizeof(char) == sizeof(uint8_t), "char is not 8 bits");
                    //         ss << "'" << std::any_cast<char>(literal_expression.value) << "'" << std::endl;
                    //     },
                    //     VISIT_LITERAL_EXPRESSION_TYPE(u8_type, unsigned char, sizeof(uint8_t)),
                    //     VISIT_LITERAL_EXPRESSION_TYPE(u16_type, unsigned short, sizeof(uint16_t)),
                    //     VISIT_LITERAL_EXPRESSION_TYPE(u32_type, unsigned int, sizeof(uint32_t)),
                    //     VISIT_LITERAL_EXPRESSION_TYPE(u64_type, unsigned long long, sizeof(uint64_t)),
                    //     VISIT_LITERAL_EXPRESSION_TYPE(i8_type, signed char, sizeof(int8_t)),
                    //     VISIT_LITERAL_EXPRESSION_TYPE(i16_type, short, sizeof(int16_t)),
                    //     VISIT_LITERAL_EXPRESSION_TYPE(i32_type, int, sizeof(int32_t)),
                    //     VISIT_LITERAL_EXPRESSION_TYPE(i64_type, long long, sizeof(int64_t)),
                    //     VISIT_LITERAL_EXPRESSION_TYPE(f32_type, float, sizeof(float)),
                    //     VISIT_LITERAL_EXPRESSION_TYPE(f64_type, double, sizeof(double)),
                    //     [&](const boolean_type&)
                    //     {
                    //         static_assert(sizeof(bool) == sizeof(bool), "bool is not 8 bit");
                    //         ss  << (std::any_cast<bool>(literal_expression.value) ? "true" : "false")  << std::endl;
                    //     },
                    //     [&](const ptr_type&)
                    //     {
                    //         ss << "TBD" << std::endl;
                    //     }
                    // }, literal_expression.type.kind);
                    // break;
                    // #undef VISIT_LITERAL_EXPRESSION_TYPE
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
    }
}
