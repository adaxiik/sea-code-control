#include <iostream>
#include <fstream>
#include <functional>
#include <optional>
#include <sstream>
#include <string>

#define UNUSED(x) (void)(x)

// void print_tokens_as_puml(const TSNode& node, const std::string &code)
// {
//     auto print_escaped = [](const std::string &str)
//     {
//         for (auto c : str)
//         {
//             switch (c)
//             {
//             case '\n':
//                 std::cout << "\\n";
//                 break;
//             case '\t':
//                 std::cout << "\\t";
//                 break;
//             case '\r':
//                 std::cout << "\\r";
//                 break;
//             case '\b':
//                 std::cout << "\\b";
//                 break;
//             case '\f':
//                 std::cout << "\\f";
//                 break;
//             case '\\':
//                 std::cout << "\\\\";
//                 break;
//             case '\"':
//                 std::cout << "\\\"";
//                 break;
//             default:
//                 std::cout << c;
//                 break;
//             }
//         }
//     };

//     std::function<void(const TSNode)> print_tokens_as_json = [&](const TSNode node)
//     {
//         std::cout << "{\"node_name\": \"";

//         if (ts_node_has_error(node))
//             std::cout << "<color:red><b>";

//         print_escaped(ts_node_type(node));
//         std::cout << "\",\"value\": \"";
//         print_escaped(code.substr(ts_node_start_byte(node), ts_node_end_byte(node) - ts_node_start_byte(node)));
//         std::cout << "\"";

//         uint32_t child_count = ts_node_child_count(node);
//         if (child_count > 0)
//         {
//             std::cout << ",\"children\": [";
//             for (uint32_t i = 0; i < child_count; i++)
//             {
//                 print_tokens_as_json(ts_node_child(node, i));
//                 if (i != child_count - 1)
//                     std::cout << ",";
//             }
//             std::cout << "]";
//         }

//         std::cout << "}";
//     };

//     std::cout << "@startjson" << std::endl;
//     print_tokens_as_json(node);
//     std::cout << std::endl
//               << "@endjson" << std::endl;
// }

#include "debug.hpp"
#include "io.hpp"
#include "parser.hpp"

int main(int argc, char const *argv[])
{

    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    auto code = scc::io::slurp_file(argv[1]);
    if (!code)
    {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1;
    }


    scc::Parser parser(code.value());
    if (parser.has_error())
    {
        std::cerr << "Failed to parse file: " << argv[1] << std::endl;
        return 1;
    }


    std::cout << scc::debug::tokens_as_puml(parser.get_root_node(), code.value()).str() << std::endl;


    return 0;
}
