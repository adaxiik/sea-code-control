#include "debug.hpp"
#include <functional>
#include <variant>

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

        // void vars_as_json(std::ostream &ss, const vm::VM& vm)
        // {
        //     ss << "{\"variables\": [";

        //     bool first = true;
        //     for (const auto& scope : vm.ref_scope_stack().ref_scopes())
        //     {
        //         for (auto& var : scope.ref_variables())
        //         {
        //             if (!first)
        //                 ss << ",";
        //             first = false;

        //             ss << "{\"name\": \"";
        //             escape_string(ss, var.first);
        //             ss << "\",";
        //             ss << "\"type\": \"" << var.second.type << "\",";
        //             ss << "\"address\": \"0x" << std::hex << var.second.pointer << std::dec << "\",";
        //             ss << "\" value\": \"";

        //             static_assert(std::variant_size_v<scc::type::Type::Kind> == 4, "Not all types are implemented");
        //             // temporary :)
        //             if(std::holds_alternative<scc::type::int_type>(var.second.type.kind))
        //                 ss << *reinterpret_cast<const int32_t*>(vm.get_memory() + var.second.pointer);
        //             else if (std::holds_alternative<scc::type::float_type>(var.second.type.kind))
        //                 ss << *reinterpret_cast<const float*>(vm.get_memory() + var.second.pointer);
        //             else if (std::holds_alternative<scc::type::double_type>(var.second.type.kind))
        //                 ss << *reinterpret_cast<const double*>(vm.get_memory() + var.second.pointer);
        //             else if (std::holds_alternative<scc::type::ptr_type>(var.second.type.kind))
        //                 ss << "0x" << std::hex << *reinterpret_cast<const uint64_t*>(vm.get_memory() + var.second.pointer) << std::dec;
        //             else
        //                 ss << "unknown";

        //             ss << "\"}";
        //         }
        //     }

        //     ss << "]}";
        // }
    }
}
