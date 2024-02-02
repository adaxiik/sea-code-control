#include "binding/binder.hpp"

#include "binding/libc/stdio.h"
#include "binding/libc/stdlib.h"
#include "binding/libc/math.h"
#include "binding/libc/unistd.h"
#include "binding/libc/string.h"
#include "binding/libc/scc.h"

namespace scc
{

    /**
     * @brief Adds included files to the binder
     *
     * @param block_statement
     * @param node
     * @return true on success
     * @return false on error
     */
    bool Binder::prepoc_include(std::vector<std::unique_ptr<binding::BoundStatement>> & statements, const TreeNode &node)
    {
        SCC_ASSERT_NODE_SYMBOL(Parser::PREPROC_INCLUDE_SYMBOL);
        SCC_ASSERT_NAMED_CHILD_COUNT(node, 1);

        auto include_node = node.first_named_child();
        if (include_node.symbol() != Parser::SYSTEM_LIB_STRING_SYMBOL)
            return false;

        if (m_included_headers.find(include_node.value()) != m_included_headers.end())
            return true;

        const char* lib = nullptr;
        if (include_node.value() == BINDER_LIBC_STDIO_H)
            lib = binding::libc::stdio_h;
        else if (include_node.value() == BINDER_LIBC_STDLIB_H)
            lib = binding::libc::stdlib_h;
        else if (include_node.value() == BINDER_LIBC_MATH_H)
            lib = binding::libc::math_h;
        else if (include_node.value() == BINDER_LIBC_UNISTD_H)
            lib = binding::libc::unistd_h;
        else if (include_node.value() == BINDER_LIBC_STRING_H)
            lib = binding::libc::string_h;
        else if (include_node.value() == BINDER_LIBC_SCC_H)
            lib = binding::libc::scc_h;
        else
            return false;

        auto lib_tree = Parser().parse(lib, true);
        if (lib_tree.has_error())
            return false;

        auto lib_bind_result = bind(lib_tree.root_node());
        if (lib_bind_result.is_error())
            return false;

        std::unique_ptr<binding::BoundBlockStatement> lib_block_statement = std::unique_ptr<binding::BoundBlockStatement>(static_cast<binding::BoundBlockStatement*>(lib_bind_result.release_value().release()));
        for (auto& statement : lib_block_statement->statements)
            statements.push_back(std::move(statement));

        lib_block_statement->statements.clear();
        m_included_headers.insert(include_node.value());

        return true;
    }

    bool Binder::prepoc_define(const TreeNode &node)
    {
        SCC_ASSERT_NODE_SYMBOL(Parser::PREPROC_DEF_SYMBOL);
        // preproc_def ==>     #define a ...
        // ├── identifier ==>  a
        // └── preproc_arg ==>  5

        if (node.named_child_count() != 2)
            return false;

        std::string identifier = node.first_named_child().value();
        if (m_macros.find(identifier) != m_macros.end())
        {
            // redefinition
            return false;
        }
        std::string value = node.named_child(1).value();

        // for further parsing we need to add a semicolon
        if (not value.ends_with(';'))
            value += ';';

        m_macros.insert({identifier, value});
        return true;
    }
}