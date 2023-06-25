#include "debug.hpp"


namespace scc
{
    namespace debug
    {
        std::stringstream escape_string(const std::string &str)
        {
            std::stringstream ss;
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
            return ss;
        }


        std::stringstream ast_as_json(const TSNode& node, const std::string &code)
        {
            std::stringstream ss;
            ss << "{\"node_name\": \"";

            if (ts_node_has_error(node))
                ss << "<color:red><b>";

            ss << escape_string(ts_node_type(node)).str();
            ss << "\",\"value\": \"";
            ss << escape_string(code.substr(ts_node_start_byte(node), ts_node_end_byte(node) - ts_node_start_byte(node))).str();
            ss << "\"";

            uint32_t child_count = ts_node_child_count(node);
            if (child_count > 0)
            {
                ss << ",\"children\": [";
                for (uint32_t i = 0; i < child_count; i++)
                {
                    if (i > 0)
                        ss << ",";
                    ss << ast_as_json(ts_node_child(node, i), code).str();
                }
                ss << "]";
            }

            ss << "}";
            return ss;
        }

        std::stringstream ast_as_puml(const TSNode& node, const std::string &code)
        {
            std::stringstream ss;
            ss << "@startjson" << std::endl;
            ss << ast_as_json(node, code).rdbuf() << std::endl;
            ss << "@endjson" << std::endl;

            return ss;
        }
    } 
} 
