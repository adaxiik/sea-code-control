#include "debug.hpp"
#include <functional>
#include <variant>

namespace scc
{
    namespace debug
    {
        void escape_string(std::ostream & ss, const std::string &str)
        {
            for (auto c : str)
            {
                switch (c)
                {
                case '\n':
                    ss << "\\n";
                    break;
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


        void ast_as_json(std::ostream & ss, const Parser& parser)
        {
            std::function<void(TSNode node)> ast_as_json_impl = [&](TSNode node)  {
                ss << "{\"node_name\": \"";

                if (ts_node_has_error(node))
                    ss << "<color:red><b>";

                escape_string(ss, ts_node_type(node));

                ss << "\",\"value\": \"";
                escape_string(ss, parser.get_code().substr(ts_node_start_byte(node), ts_node_end_byte(node) - ts_node_start_byte(node)));
                ss << "\"";

                uint32_t child_count = ts_node_child_count(node);
                if (child_count > 0)
                {
                    ss << ",\"children\": [";
                    for (uint32_t i = 0; i < child_count; i++)
                    {
                        if (i > 0)
                            ss << ",";
                        ast_as_json_impl(ts_node_child(node, i));
                    }
                    ss << "]";
                }

                ss << "}";
            };

            ast_as_json_impl(parser.get_root_node());
        }

        void  ast_as_puml(std::ostream &ss, const Parser &parser)
        {

            ss << "@startjson" << std::endl;
            ast_as_json(ss, parser);
            ss << std::endl;
            ss << "@endjson" << std::endl;
        }

        void vars_as_json(std::ostream &ss, const vm::VM& vm)
        {
            ss << "{\"variables\": [";

            bool first = true;
            for (const auto& scope : vm.ref_scope_stack().ref_scopes())
            {
                for (auto& var : scope.ref_variables())
                {
                    if (!first)
                        ss << ",";
                    first = false;

                    ss << "{\"name\": \"";
                    escape_string(ss, var.first);
                    ss << "\",";
                    ss << "\"type\": \"" << var.second.type << "\",";
                    ss << "\"address\": 0x" << std::hex << var.second.pointer << std::dec << ",";
                    ss << "\" value\": \"";

                    // temporary :)
                    if(std::holds_alternative<scc::type::I32>(var.second.type.kind))
                        ss << *reinterpret_cast<const int32_t*>(vm.get_stack() + var.second.pointer);
                    else if (std::holds_alternative<scc::type::F32>(var.second.type.kind))
                        ss << *reinterpret_cast<const float*>(vm.get_stack() + var.second.pointer);
                    else
                        ss << "unknown";

                    ss << "}";
                }
            }

            ss << "]}";
        }

    } 
} 
