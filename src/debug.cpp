#include "debug.hpp"
#include <functional>
#include <variant>
#include <any>
#include <iomanip>

#include "binding/bound_block_statement.hpp"
#include "binding/bound_literal_expression.hpp"
#include "binding/bound_binary_expression.hpp"
#include "binding/bound_expression_statement.hpp"


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
                static_assert(static_cast<int>(binding::BoundNodeKind::COUNT) == 4, "Update this switch statement");
                
                
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

                    std::visit(overloaded{
                        [&](const type::u8_type&){
                            static_assert(sizeof(char) == sizeof(uint8_t), "char is not 8 bits");
                            ss << "'" << std::any_cast<char>(literal_expression.value) << "'" << std::endl;
                        },
                        [&](const type::u16_type&){
                            static_assert(sizeof(unsigned short) == sizeof(uint16_t), "unsigned short is not 16 bits");
                            ss << std::any_cast<unsigned short>(literal_expression.value) << std::endl;
                        },
                        [&](const type::u32_type&){
                            static_assert(sizeof(unsigned int) == sizeof(uint32_t), "unsigned int is not 32 bits");
                            ss  << std::any_cast<unsigned int>(literal_expression.value)  << std::endl;
                        },
                        [&](const type::u64_type&){
                            static_assert(sizeof(unsigned long long) == sizeof(uint64_t), "unsigned long long is not 64 bits");
                            ss  << std::any_cast<unsigned long long>(literal_expression.value)  << std::endl;
                        },
                        [&](const type::i8_type&){
                            static_assert(sizeof(signed char) == sizeof(int8_t), "char is not 8 bits");
                            ss  << std::any_cast<char>(literal_expression.value)  << std::endl;
                        },
                        [&](const type::i16_type&){
                            static_assert(sizeof(short) == sizeof(int16_t), "short is not 16 bits");
                            ss  << std::any_cast<short>(literal_expression.value)  << std::endl;
                        },
                        [&](const type::i32_type&){
                            static_assert(sizeof(int) == sizeof(int32_t), "int is not 32 bits");
                            ss  << std::any_cast<int>(literal_expression.value)  << std::endl;
                        },
                        [&](const type::i64_type&){
                            static_assert(sizeof(long long) == sizeof(int64_t), "long long is not 64 bits");
                            ss  << std::any_cast<long long>(literal_expression.value)  << std::endl;
                        },
                        [&](const type::f32_type&){
                            static_assert(sizeof(float) == sizeof(float), "float is not 32 bits");
                            ss  << std::any_cast<float>(literal_expression.value)  << std::endl;
                        },
                        [&](const type::f64_type&){
                            static_assert(sizeof(double) == sizeof(double), "double is not 64 bits");
                            ss  << std::any_cast<double>(literal_expression.value)  << std::endl;
                        },
                        [&](const type::boolean_type&)
                        {
                            static_assert(sizeof(bool) == sizeof(bool), "bool is not 8 bit");
                            ss  << (std::any_cast<bool>(literal_expression.value) ? "true" : "false")  << std::endl;
                        },
                        [&](const type::ptr_type&)
                        {
                            ss << "TBD" << std::endl;
                        }
                    }, literal_expression.type.kind);
                    break;
                }

                case binding::BoundNodeKind::BinaryExpression:
                {
                    auto& binary_expression = static_cast<const binding::BoundBinaryExpression&>(node);
                    ss << "BinaryExpression" << std::endl;
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
