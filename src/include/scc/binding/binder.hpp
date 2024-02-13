#pragma once
#include <optional>
#include <set>
#include "treenode.hpp"
#include "parser.hpp"
#include "binding/binder_scope.hpp"

#include "binding/binder_result.hpp"
#include "binding/bound_node.hpp"
#include "binding/bound_block_statement.hpp"
#include "binding/bound_expression.hpp"
#include "binding/bound_expression_statement.hpp"
#include "binding/bound_binary_expression.hpp"
#include "binding/bound_literal_expression.hpp"
#include "binding/bound_cast_expression.hpp"
#include "binding/bound_parenthesized_expression.hpp"
#include "binding/bound_variable_declaration_statement.hpp"
#include "binding/bound_identifier_expression.hpp"
#include "binding/bound_assignment_expression.hpp"
#include "binding/bound_reference_expression.hpp"
#include "binding/bound_dereference_expression.hpp"
#include "binding/bound_pointer_assignment_expression.hpp"
#include "binding/bound_if_statement.hpp"
#include "binding/bound_while_statement.hpp"
#include "binding/bound_do_statement.hpp"
#include "binding/bound_break_statement.hpp"
#include "binding/bound_continue_statement.hpp"
#include "binding/bound_function_statement.hpp"
#include "binding/bound_return_statement.hpp"
#include "binding/bound_call_expression.hpp"
#include "binding/bound_for_statement.hpp"
#include "binding/bound_unary_expression.hpp"
#include "binding/bound_string_expression.hpp"
#include "binding/bound_printf_expression.hpp"

namespace scc
{
    constexpr auto BINDER_LIBC_STDIO_H = "<stdio.h>";
    constexpr auto BINDER_LIBC_STDLIB_H = "<stdlib.h>";
    constexpr auto BINDER_LIBC_STRING_H = "<string.h>";
    constexpr auto BINDER_LIBC_STDDEF_H = "<stddef.h>";
    constexpr auto BINDER_LIBC_STDBOOL_H = "<stdbool.h>";
    constexpr auto BINDER_LIBC_MATH_H = "<math.h>";
    constexpr auto BINDER_LIBC_UNISTD_H = "<unistd.h>";
    constexpr auto BINDER_LIBC_SCC_H = "<scc.h>";

    class Binder
    {

        static_assert(static_cast<int>(binding::BoundNodeKind::COUNT) == 24, "Update this code");
        static_assert(std::is_base_of_v<binding::BoundStatement, binding::BoundExpressionStatement>, "ExpressionStatement must be derived from BoundStatement");
        static_assert(std::is_base_of_v<binding::BoundStatement, binding::BoundBlockStatement>, "BlockStatement must be derived from BoundStatement");
        static_assert(std::is_base_of_v<binding::BoundExpression, binding::BoundBinaryExpression>, "BinaryExpression must be derived from BoundExpression");
        static_assert(std::is_base_of_v<binding::BoundExpression, binding::BoundLiteralExpression>, "LiteralExpression must be derived from BoundExpression");
        static_assert(std::is_base_of_v<binding::BoundExpression, binding::BoundCastExpression>, "CastExpression must be derived from BoundExpression");
        static_assert(std::is_base_of_v<binding::BoundExpression, binding::BoundParenthesizedExpression>, "ParenthesizedExpression must be derived from BoundExpression");
        static_assert(std::is_base_of_v<binding::BoundStatement, binding::BoundVariableDeclarationStatement>, "VariableDeclaration must be derived from BoundStatement");
        static_assert(std::is_base_of_v<binding::BoundExpression, binding::BoundIdentifierExpression>, "IdentifierExpression must be derived from BoundExpression");
        static_assert(std::is_base_of_v<binding::BoundExpression, binding::BoundAssignmentExpression>, "AssignmentExpression must be derived from BoundExpression");
        static_assert(std::is_base_of_v<binding::BoundStatement, binding::BoundIfStatement>, "IfStatement must be derived from BoundStatement");
        static_assert(std::is_base_of_v<binding::BoundStatement, binding::BoundWhileStatement>, "WhileStatement must be derived from BoundStatement");
        static_assert(std::is_base_of_v<binding::BoundStatement, binding::BoundDoStatement>, "DoStatement must be derived from BoundStatement");
        static_assert(std::is_base_of_v<binding::BoundStatement, binding::BoundBreakStatement>, "BreakStatement must be derived from BoundStatement");
        static_assert(std::is_base_of_v<binding::BoundStatement, binding::BoundContinueStatement>, "ContinueStatement must be derived from BoundStatement");
        static_assert(std::is_base_of_v<binding::BoundNode, binding::BoundFunctionStatement>, "FunctionStatement must be derived from BoundStatement");
        static_assert(std::is_base_of_v<binding::BoundExpression, binding::BoundCallExpression>, "CallExpression must be derived from BoundExpression");
        static_assert(std::is_base_of_v<binding::BoundStatement, binding::BoundReturnStatement>, "ReturnStatement must be derived from BoundStatement");
        static_assert(std::is_base_of_v<binding::BoundStatement, binding::BoundForStatement>, "ForStatement must be derived from BoundStatement");
        static_assert(std::is_base_of_v<binding::BoundExpression, binding::BoundReferenceExpression>, "ReferenceExpression must be derived from BoundExpression");
        static_assert(std::is_base_of_v<binding::BoundExpression, binding::BoundDereferenceExpression>, "DereferenceExpression must be derived from BoundExpression");
        static_assert(std::is_base_of_v<binding::BoundExpression, binding::BoundPointerAssignmentExpression>, "PointerAssignmentExpression must be derived from BoundExpression");
        static_assert(std::is_base_of_v<binding::BoundExpression, binding::BoundUnaryExpression>, "UnaryExpression must be derived from BoundExpression");
        static_assert(std::is_base_of_v<binding::BoundExpression, binding::BoundStringExpression>, "StringExpression must be derived from BoundExpression");
        static_assert(std::is_base_of_v<binding::BoundExpression, binding::BoundPrintfExpression>, "PrintfExpression must be derived from BoundExpression");

        Binder(const Binder &) = delete;
        Binder(Binder &&) = delete;
        Binder &operator=(const Binder &) = delete;
        Binder &operator=(Binder &&) = delete;

        binding::BinderScopeStack m_scope_stack;

        struct FunctionDeclaration
        {
            Type return_type;
            std::vector<Type> parameters;
            bool is_defined;

            bool operator==(const FunctionDeclaration &other) const
            {
                if (return_type != other.return_type)
                    return false;

                if (parameters.size() != other.parameters.size())
                    return false;

                for (size_t i = 0; i < parameters.size(); i++)
                {
                    if (parameters[i] != other.parameters[i])
                        return false;
                }

                return true;
            }

            bool operator!=(const FunctionDeclaration &other) const
            {
                return !(*this == other);
            }
        };

        std::map<std::string, FunctionDeclaration> m_functions;
        std::map<std::string, std::string> m_macros;
        std::set<std::string> m_included_headers;
        std::optional<std::reference_wrapper<const FunctionDeclaration>> m_current_function;
        std::map<std::string, Type> m_typedefs;

        binding::BinderResult<binding::BoundBlockStatement> bind_block_statement(const TreeNode &node);
        binding::BinderResult<binding::BoundNode> bind_impl(const TreeNode &node);
        binding::BinderResult<binding::BoundExpressionStatement> bind_expression_statement(const TreeNode &node);
        binding::BinderResult<binding::BoundExpression> bind_expression(const TreeNode &node);
        binding::BinderResult<binding::BoundBinaryExpression> bind_binary_expression(const TreeNode &node);
        binding::BinderResult<binding::BoundCastExpression> bind_cast_expression(const TreeNode &node);
        binding::BinderResult<binding::BoundParenthesizedExpression> bind_parenthesized_expression(const TreeNode &node);
        binding::BinderResult<binding::BoundLiteralExpression> bind_literal_expression(const TreeNode &node);
        binding::BinderResult<binding::BoundLiteralExpression> bind_number_literal(const TreeNode &node);
        binding::BinderResult<binding::BoundVariableDeclarationStatement> bind_variable_declaration(const TreeNode &node);
        // might return a BoundIdentifierExpression or macro value
        binding::BinderResult<binding::BoundExpression> bind_identifier_expression(const TreeNode &node);
        binding::BinderResult<binding::BoundAssignmentExpression> bind_assignment_expression(const TreeNode &node);
        binding::BinderResult<binding::BoundIfStatement> bind_if_statement(const TreeNode &node);
        binding::BinderResult<binding::BoundWhileStatement> bind_while_statement(const TreeNode &node);
        binding::BinderResult<binding::BoundDoStatement> bind_do_statement(const TreeNode &node);
        binding::BinderResult<binding::BoundBreakStatement> bind_break_statement(const TreeNode &node);
        binding::BinderResult<binding::BoundContinueStatement> bind_continue_statement(const TreeNode &node);
        binding::BinderResult<binding::BoundNode> bind_declaration(const TreeNode &node);
        binding::BinderResult<binding::BoundFunctionStatement> bind_function(const TreeNode &node);
        binding::BinderResult<binding::BoundReturnStatement> bind_return_statement(const TreeNode &node);
        binding::BinderResult<binding::BoundExpression> bind_call_expression(const TreeNode &node);
        binding::BinderResult<binding::BoundForStatement> bind_for_statement(const TreeNode &node);
        binding::BinderResult<binding::BoundReferenceExpression> bind_reference_expression(const TreeNode &node);
        binding::BinderResult<binding::BoundDereferenceExpression> bind_dereference_expression(const TreeNode &node);
        binding::BinderResult<binding::BoundDereferenceExpression> bind_subscript_expression(const TreeNode &node);
        binding::BinderResult<binding::BoundPointerAssignmentExpression> bind_pointer_assignment_expression(const TreeNode &node);
        binding::BinderResult<binding::BoundLiteralExpression> bind_sizeof_expression(const TreeNode &node);
        binding::BinderResult<binding::BoundPointerAssignmentExpression> bind_subscript_assignment_expression(const TreeNode &node);
        binding::BinderResult<binding::BoundExpression> bind_update_expression(const TreeNode &node);
        binding::BinderResult<binding::BoundUnaryExpression> bind_unary_expression(const TreeNode &node);
        binding::BinderResult<binding::BoundStringExpression> bind_string_expression(const TreeNode &node);
        binding::BinderResult<binding::BoundPrintfExpression> bind_printf_expression(const TreeNode &node);
        binding::BinderResult<binding::BoundDereferenceExpression> bind_field_expression(const TreeNode &node);
        binding::BinderResult<binding::BoundPointerAssignmentExpression> bind_field_assignment_expression(const TreeNode &node);

        std::optional<binding::BoundBinaryExpression::OperatorKind> operation_kind_from_string(const std::string op_kind);
        binding::BinderResult<binding::BoundLiteralExpression> bind_struct_initializer_list(const TreeNode &node, const Type& type);

        bool prepoc_include(std::vector<std::unique_ptr<binding::BoundStatement>> & statements, const TreeNode &node);
        bool prepoc_define(const TreeNode &node);
        static std::vector<char> escape_string(const std::string& str);
        bool handle_typedef(const TreeNode &node);

    public:
        Binder(): m_current_function(std::nullopt) { m_scope_stack.push();}
        ~Binder() = default;
        binding::BinderResult<binding::BoundNode> bind(const TreeNode &node);
        std::optional<Type> deduce_type_from_node(const TreeNode &node);
    };
}

#include <iomanip>


#define SCC_ASSERT(x)                                                        \
    do                                                                       \
    {                                                                        \
        if (!(x))                                                            \
        {                                                                    \
            std::cerr << "==============================" << std::endl;      \
            std::cerr << "Assertion failed: " << #x << std::endl;            \
            std::cerr << "At: " << __FILE__ << ":" << __LINE__ << std::endl; \
            std::cerr << "==============================" << std::endl;      \
            std::exit(1);                                             \
        }                                                                    \
    } while (0)

#define SCC_ASSERT_EQ(x, y) SCC_ASSERT((x) == (y))
#define SCC_ASSERT_NAMED_CHILD_COUNT(x, y) SCC_ASSERT_EQ((x).named_child_count(), (y))
#define SCC_ASSERT_CHILD_COUNT(x, y) SCC_ASSERT_EQ((x).child_count(), (y))

#define SCC_UNIMPLEMENTED()                                                         \
    do                                                                              \
    {                                                                               \
        std::cerr << "==============================" << std::endl;                 \
        std::cerr << "Unimplemented: " << __FILE__ << ":" << __LINE__ << std::endl; \
        std::cerr << "==============================" << std::endl;                 \
        std::exit(1);                                                               \
    } while (0)

#define SCC_ASSERT_NODE_SYMBOL(x) SCC_ASSERT_EQ(node.symbol(), (x))

#define SCC_NOT_IMPLEMENTED(x)                                           \
    do                                                                   \
    {                                                                    \
        std::cerr << "==============================" << std::endl;      \
        std::cerr << "Not implemented: " << std::quoted(x) << std::endl; \
        std::cerr << "At: " << __FILE__ << ":" << __LINE__ << std::endl; \
        std::cerr << "==============================" << std::endl;      \
        std::exit(1);                                             \
    } while (0)

#define SCC_NOT_IMPLEMENTED_WARN(x)                                      \
    do                                                                   \
    {                                                                    \
        std::cerr << "==============================" << std::endl;      \
        std::cerr << "Not implemented: " << std::quoted(x) << std::endl; \
        std::cerr << "At: " << __FILE__ << ":" << __LINE__ << std::endl; \
        std::cerr << "==============================" << std::endl;      \
    } while (0)
#define SCC_UNREACHABLE() SCC_ASSERT(false)

#define SCC_BINDER_RESULT_TYPE(x) using ResultType = decltype(x(std::declval<const TreeNode&>()))::type

constexpr bool TRACE_BINDER = false;

// #define BUBBLE_ERROR(x) if ((x).is_error()) return (x)

#define SOURCE_LOCATION_STR std::string("at ") + std::string( __FILE__) + ":" + std::to_string(__LINE__)

#define BUBBLE_ERROR(x) do{ \
    if ((x).is_error()) \
    { \
        if constexpr (TRACE_BINDER) \
            (x).add_diagnostic(std::string(__func__) + " " + SOURCE_LOCATION_STR); \
        return (x); \
    } \
} while(0)

using ErrorKind = scc::binding::BinderError::BinderErrorKind;
