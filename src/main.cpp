#include <iostream>
#include <tree_sitter/api.h>
#include <tree_sitter/parser.h>
#include <tree_sitter-c/parser-c.h>

#define UNUSED(x) (void)(x)

void print_tokens(const TSNode node, const std::string &code)
{
    if (ts_node_is_named(node))
    {
        std::cout << "Token: " << ts_node_type(node) << " - "
                  << code.substr(ts_node_start_byte(node), ts_node_end_byte(node) - ts_node_start_byte(node) + 1)
                  << std::endl;
    }

    for (uint32_t i = 0; i < ts_node_child_count(node); ++i)
        print_tokens(ts_node_child(node, i), code);
}

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    const std::string code = "#include <stdio.h>\nint main()\n{\n\treturn 0;\n}";
    std::cout << code << std::endl;

    TSParser *parser = ts_parser_new();
    ts_parser_set_language(parser, tree_sitter_c());

    TSTree *tree = ts_parser_parse_string(
        parser,
        NULL,
        code.c_str(),
        code.size());

    TSNode root_node = ts_tree_root_node(tree);
    print_tokens(root_node, code);

    ts_tree_delete(tree);
    ts_parser_delete(parser);

    return 0;
}
