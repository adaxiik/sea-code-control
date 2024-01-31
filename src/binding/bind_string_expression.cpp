#include "binding/binder.hpp"
#include <vector>
#include <string>
#include <memory>
namespace scc
{
    binding::BinderResult<binding::BoundStringExpression> Binder::bind_string_expression(const TreeNode &node)
    {
        std::vector<char> escaped_string = escape_string(node.value());
        std::string string_value(escaped_string.begin(), escaped_string.end()); // it creates it with a null terminator at the end
        return std::make_unique<binding::BoundStringExpression>(std::move(string_value));
    }
}