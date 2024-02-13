#include "binding/binder.hpp"
#include <algorithm>
namespace scc
{

    struct DirectAccess { std::string identifier; };
    struct DerefAccess  { std::string identifier; };
    using Access = std::variant<DirectAccess, DerefAccess>;

    static const std::string& access_identifier(const Access& access)
    {
        if (std::holds_alternative<DirectAccess>(access))
            return std::get<DirectAccess>(access).identifier;
        else
            return std::get<DerefAccess>(access).identifier;
    }

    binding::BinderResult<binding::BoundDereferenceExpression> Binder::bind_field_expression(const TreeNode &node)
    {
        // NOTE:
        // původně to fungovalo jinak, ale už jsem moc líný to předělávat
        // takže se to prvně naparsuje a až pak konvertuje na bound expression
        // pár zbytečných kopii, ale co už
        SCC_BINDER_RESULT_TYPE(bind_field_expression);
        SCC_ASSERT_NODE_SYMBOL(Parser::FIELD_EXPRESSION_SYMBOL);
        // expression_statement ==>    a->first.s...
        // └── field_expression ==>    a->first.s...
        //     ├── field_expression ==>        a->first.s...
        //     │   ├── field_expression ==>    a->first
        //     │   │   ├── identifier ==>      a
        //     │   │   └── field_identifier ==>        first
        //     │   └── field_identifier ==>    second
        //     └── field_identifier ==>        third

        // its easier to parse it by hand
        std::string expression_string = node.value();
        std::string_view current_sv = expression_string;

        std::vector<Access> accesses;
        constexpr auto DIRECT_ACCESS = ".";
        constexpr auto DEREF_ACCESS = "->";

        size_t direct_access_index = current_sv.find(DIRECT_ACCESS);
        size_t deref_access_index = current_sv.find(DEREF_ACCESS);
        std::string_view base_identifier = current_sv.substr(0, std::min(direct_access_index, deref_access_index));
        std::string base_identifier_string = std::string(base_identifier);

        current_sv = current_sv.substr(base_identifier.size());

        while (true)
        {
            bool last_was_direct_access = false;
            bool is_deref = current_sv.starts_with(DEREF_ACCESS);
            if (is_deref)
            {
                current_sv = current_sv.substr(2);
                last_was_direct_access = false;
            }
            else
            {
                current_sv = current_sv.substr(1);
                last_was_direct_access = true;
            }

            size_t next_direct_access_index = current_sv.find(DIRECT_ACCESS);
            size_t next_deref_access_index = current_sv.find(DEREF_ACCESS);
            size_t next_access_index = std::min(next_direct_access_index, next_deref_access_index);
            bool is_last = next_access_index == std::string::npos;
            std::string_view next_access_sv = current_sv.substr(0, next_access_index);
            current_sv = current_sv.substr(next_access_sv.size());

            if (last_was_direct_access)
                accesses.push_back(DirectAccess {std::string(next_access_sv)});
            else
                accesses.push_back(DerefAccess {std::string(next_access_sv)});


            if (is_last)
                break;
        }


        Type *type_ptr = m_scope_stack.get_from_scopestack(base_identifier_string);
        if (not type_ptr)
        {
            auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::UnknownSymbolError, node));
            error.add_diagnostic("Unknown identifier: " + base_identifier_string);
            return error;
        }


        // base is always a pointer
        // &(base.first) == ((type*)((void*)base) + offsetof(first))
        // base->first == (*base).first
        // base->first ==  *((type*)((void*)base) + offsetof(first))

        // base.first->second == (*base.first).second
        // base.first->second == (*((type*)((void*)&base) + offsetof(first))) + offsetof(second)


        Type type = *type_ptr;
        std::string current_base = base_identifier_string;
        type.modifiers.push_back(Type::Pointer{});
        std::unique_ptr<binding::BoundExpression> current_base_ref = std::make_unique<binding::BoundReferenceExpression>(current_base, type);


        for (const auto& access: accesses)
        {
            const std::string& identifier = access_identifier(access);
            bool is_deref = std::holds_alternative<DerefAccess>(access);

            if (is_deref)
            {
                auto casted_to_type_ptr = std::make_unique<binding::BoundCastExpression>(std::move(current_base_ref), type);
                auto deref_expression = std::make_unique<binding::BoundDereferenceExpression>(std::move(casted_to_type_ptr), type);
                current_base_ref = std::move(deref_expression);
                type.modifiers.pop_back();
            }

            auto casted_to_void_ptr = std::make_unique<binding::BoundCastExpression>(std::move(current_base_ref), Type(Type::PrimitiveType::Char, {Type::Pointer{}}));
            // auto offset = get_offset(type, identifier);
            std::optional<size_t> offset = std::get<Type::StructType>(type.base_type).offset_of(identifier);
            if (not offset.has_value())
            {
                auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::UnknownSymbolError, node));
                error.add_diagnostic("Unknown field: " + identifier);
                return error;
            }
            auto offset_constant = std::make_unique<binding::BoundLiteralExpression>(
                Type::Value(Type(Type::PrimitiveType::U64), static_cast<Type::Primitive::U64>(offset.value()))
            );

            auto addition_expression = std::make_unique<binding::BoundBinaryExpression>(
                std::move(casted_to_void_ptr),
                std::move(offset_constant),
                Type(Type::PrimitiveType::Char, {Type::Pointer{}}),
                binding::BoundBinaryExpression::OperatorKind::Addition
            );

            current_base_ref = std::move(addition_expression);

            Type tt = std::get<Type::StructType>(type.base_type).type_of(identifier).value();
            tt.modifiers.push_back(Type::Pointer{});
            type = std::move(tt);
        }

        auto casted_to_target_type = std::make_unique<binding::BoundCastExpression>(std::move(current_base_ref), type);
        type.modifiers.pop_back();
        return std::make_unique<binding::BoundDereferenceExpression>(std::move(casted_to_target_type), type);

    }
}