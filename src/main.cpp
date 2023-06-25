#include <iostream>
#include <functional>
#include <string>
#include <tree_sitter/api.h>
#include <tree_sitter/parser.h>
#include <tree_sitter-c/parser-c.h>

#define UNUSED(x) (void)(x)

void print_tokens_as_puml(const TSNode node, const std::string &code)
{
    auto print_escaped = [](const std::string &str)
    {
        for (auto c : str)
        {
            switch (c)
            {
            case '\n':
                std::cout << "\\n";
                break;
            case '\t':
                std::cout << "\\t";
                break;
            case '\r':
                std::cout << "\\r";
                break;
            case '\b':
                std::cout << "\\b";
                break;
            case '\f':
                std::cout << "\\f";
                break;
            case '\\':
                std::cout << "\\\\";
                break;
            case '\"':
                std::cout << "\\\"";
                break;
            default:
                std::cout << c;
                break;
            }
        }
    };

    std::function<void(const TSNode)> print_tokens_as_json = [&](const TSNode node)
    {
        std::cout << "{\"node_name\": \"";

        if(ts_node_has_error(node))
            std::cout << "<color:red><b>";
  
        print_escaped(ts_node_type(node));
        std::cout << "\",\"value\": \"";
        print_escaped(code.substr(ts_node_start_byte(node), ts_node_end_byte(node) - ts_node_start_byte(node)));
        std::cout << "\"";
        
        uint32_t child_count = ts_node_child_count(node);
        if (child_count > 0)
        {
            std::cout << ",\"children\": [";
            for (uint32_t i = 0; i < child_count; i++)
            {
                print_tokens_as_json(ts_node_child(node, i));
                if (i != child_count - 1)
                    std::cout << ",";
            }
            std::cout << "]";
        }

        std::cout << "}";
    };

    std::cout << "@startjson" << std::endl;
    print_tokens_as_json(node);
    std::cout << std::endl
              << "@endjson" << std::endl;
}



int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    const std::string code = "#include <stdio.h>\nint main(int argc, char** argv)\n{\n\tprintf(\"%d\",sizeof(int);\n\treturn 0;\n}";
    // std::cout << code << std::endl;

    TSParser *parser = ts_parser_new();
    ts_parser_set_language(parser, tree_sitter_c());

    TSTree *tree = ts_parser_parse_string(
        parser,
        NULL,
        code.c_str(),
        code.size());

    TSNode root_node = ts_tree_root_node(tree);

    print_tokens_as_puml(root_node, code);

    ts_tree_delete(tree);
    ts_parser_delete(parser);

    return 0;
}
