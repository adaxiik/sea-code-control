#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundVariableDeclarationStatement> Binder::bind_variable_declaration(const TreeNode &node)
    {
        SCC_BINDER_RESULT_TYPE(bind_variable_declaration);

        // translation_unit ==>    int a;
        // └── declaration ==>     int a;
        //     ├── primitive_type ==>      int
        //     └── identifier ==>  a

        SCC_ASSERT(node.symbol() == Parser::DECLARATION_SYMBOL
                || node.symbol() == Parser::PARAMETER_DECLARATION_SYMBOL);
        // 3 if have qualifier (eg.: `const`)
        // TODOOO: int a, b, c;
        SCC_ASSERT(node.named_child_count() == 2 || node.named_child_count() == 3);


        bool has_qualifier = node.named_child_count() == 3;
        int type_index = static_cast<int>(has_qualifier);
        int identifier_index = type_index + 1;


        bool is_constant = has_qualifier && node.first_named_child().value() == "const";
        bool is_global = m_scope_stack.is_global_scope();
        auto type_descriptor = node.named_child(type_index).value();
        auto type = Type::from_string(type_descriptor);
        if(not type.has_value())
        {
            // TODOOOOOOOOOOOOO: custom types
            // std::cerr << "Unknown type: " << type_descriptor << std::endl;
            // return nullptr;
            SCC_BINDER_RESULT_TYPE(bind_variable_declaration);
            auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::UnknownSymbolError, node));
            error.add_diagnostic("Unknown type: " + type_descriptor);
            return error;
        }


        // TODOOO: Investigate further mixing of pointer and array declarators.. and also with initializer
        // multidimensional arrays and multidimensional initializer..
        auto& type_scope = m_scope_stack;
        auto resolve_declarator = [&type, is_constant, &type_scope, is_global, this](const TreeNode &node, int identifier_index, bool has_initializer = false)
        ->  binding::BinderResult<binding::BoundVariableDeclarationStatement>
        {
            switch (node.named_child(identifier_index).symbol())
            {
                case Parser::IDENTIFIER_SYMBOL:
                {
                    std::string identifier = node.named_child(identifier_index).value();
                    if(not type_scope.create_variable(identifier, type.value()))
                        return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::FailedToCreateVariableError, node));

                    return static_cast<std::unique_ptr<binding::BoundVariableDeclarationStatement>>(
                         std::make_unique<binding::BoundVariableValueDeclarationStatement>(identifier
                    , type.value()
                    , is_constant
                    , is_global));
                    break;
                }
                case Parser::ARRAY_DECLARATOR_SYMBOL:
                {
                    // declaration ==>     int x[1];
                    // ├── primitive_type ==>      int
                    // └── array_declarator ==>    x[1]
                    //     ├── identifier ==>      x
                    //     └── number_literal ==>  1    //might be an identifier too


                    // TODOO: int x[1][2];  multi dimensional arrays

                    // int x[]; is error
                    // int x[] = {1,2,3}; is ok and this node wont have a number_literal child
                    if (node.named_child(identifier_index).named_child_count() != 2
                        and not has_initializer)
                        return static_cast<std::unique_ptr<binding::BoundVariableDeclarationStatement>>(
                            std::unique_ptr<binding::BoundVariableStaticArrayDeclarationStatement>(nullptr));

                    bool we_know_the_size = node.named_child(identifier_index).named_child_count() == 2;

                    std::string array_size_str = "0";
                    if(we_know_the_size)
                    {
                        // we dont want to support vla?

                        bool is_number_literal = node.named_child(identifier_index).named_child(1).symbol() == Parser::NUMBER_LITERAL_SYMBOL;
                        std::string node_value = node.named_child(identifier_index).named_child(1).value();
                        bool is_macro_identifier = m_macros.find(node_value) != m_macros.end();

                        if (is_number_literal)
                            array_size_str = node_value;
                        else if (is_macro_identifier)
                            array_size_str = m_macros[node_value];
                        else
                            SCC_NOT_IMPLEMENTED("vla");

                    }

                    // now we know its a number, or unknown size
                    size_t array_size = we_know_the_size ? std::stoll(array_size_str) : 0;

                    SCC_ASSERT_EQ(node.named_child(identifier_index).first_named_child().symbol(), Parser::IDENTIFIER_SYMBOL);
                    std::string identifier = node.named_child(identifier_index).first_named_child().value();
                    // type.value().pointer_depth = 1;
                    type.value().modifiers.push_back(Type::Array{array_size});

                    if(not type_scope.create_variable(identifier, type.value()))
                        return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::FailedToCreateVariableError, node));

                    return static_cast<std::unique_ptr<binding::BoundVariableDeclarationStatement>>(std::make_unique<binding::BoundVariableStaticArrayDeclarationStatement>(identifier
                    , type.value()
                    , array_size
                    , is_constant
                    , is_global));
                    break;
                }
                case Parser::POINTER_DECLARATOR_SYMBOL:
                {
                    auto current_node = node.named_child(identifier_index);
                    uint32_t pointer_depth = 0;
                    while (current_node.symbol() == Parser::POINTER_DECLARATOR_SYMBOL)
                    {
                        pointer_depth++;
                        current_node = current_node.first_named_child();
                    }

                    std::string identifier = current_node.value();
                    // type.value().pointer_depth = pointer_depth;
                    type.value().modifiers = std::vector<Type::Modifier>(pointer_depth, Type::Pointer{});

                    if(not type_scope.create_variable(identifier, type.value()))
                        return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::FailedToCreateVariableError, node));

                    return  static_cast<std::unique_ptr<binding::BoundVariableDeclarationStatement>>(std::make_unique<binding::BoundVariablePointerDeclarationStatement>(identifier
                    , type.value()
                    , is_constant
                    , is_global));

                    break;
                }
                default:
                    std::cerr << "Unknown identifier symbol: " << node.named_child(identifier_index).symbol_name() << std::endl;
                    SCC_UNREACHABLE();
            }
        };

        switch (node.named_child(identifier_index).symbol())
        {
            case Parser::IDENTIFIER_SYMBOL:
            case Parser::ARRAY_DECLARATOR_SYMBOL:
            case Parser::POINTER_DECLARATOR_SYMBOL:
                return resolve_declarator(node, identifier_index);
            case Parser::INIT_DECLARATOR_SYMBOL:
            {
                // declaration ==>     int a[] = ...
                // ├── primitive_type ==>      int
                // └── init_declarator ==>     a[] = {1,2...
                //     ├── array_declarator ==>        a[]
                //     │   └── identifier ==>  a
                //     └── initializer_list ==>        {1,2}
                //         ├── number_literal ==>      1
                //         └── number_literal ==>      2

                // declaration ==>     int a = 0;
                // ├── primitive_type ==>      int
                // └── init_declarator ==>     a = 0
                //     ├── identifier ==>      a
                //     └── number_literal ==>  0

                // declaration ==>     char x[] =...
                // ├── primitive_type ==>      char
                // └── init_declarator ==>     x[] = \"abc...
                //     ├── array_declarator ==>        x[]
                //     │   └── identifier ==>  x
                //     └── string_literal ==>  \"abc\"

                auto init_declarator_node = node.named_child(identifier_index);
                // first is declaration and second is initializer
                auto declaration = resolve_declarator(init_declarator_node, 0, true);
                BUBBLE_ERROR(declaration);
                auto initializer_node = init_declarator_node.named_child(1);
                if (initializer_node.symbol() == Parser::INITIALIZER_LIST_SYMBOL)
                {

                    switch (declaration.get_value()->variable_declaration_statement_kind())
                    {
                        using DeclarationKind = binding::BoundVariableDeclarationStatement::VariableDeclarationStatementKind;
                        case DeclarationKind::Value:
                        {
                            // int x = {1}; is ok, but int x = {1,2}; is not
                            // int x = {}; is not valid C99
                            if (initializer_node.named_child_count() != 1 )
                                return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::InvalidInitializerError, initializer_node));

                            auto initializer = bind_expression(initializer_node.first_named_child());
                            BUBBLE_ERROR(initializer);
                            auto casted_initializer = std::make_unique<binding::BoundCastExpression>(initializer.release_value(), type.value());

                            static_cast<binding::BoundVariableValueDeclarationStatement*>(declaration.get_value())->initializer = std::move(casted_initializer);
                            return declaration;
                        }
                        case DeclarationKind::Pointer:
                        {
                            if (initializer_node.named_child_count() != 1 )
                                return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::InvalidInitializerError, initializer_node));


                            auto initializer = bind_expression(initializer_node.first_named_child());
                            BUBBLE_ERROR(initializer);
                            auto casted_initializer = std::make_unique<binding::BoundCastExpression>(initializer.release_value(), type.value());

                            static_cast<binding::BoundVariablePointerDeclarationStatement*>(declaration.get_value())->initializer = std::move(casted_initializer);
                            return declaration;
                        }
                        case DeclarationKind::StaticArray:
                        {
                                // int x[2] = {}; is not valid C99
                            if (initializer_node.named_child_count() == 0)
                                return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::InvalidInitializerError, initializer_node));

                            auto array_declaration = static_cast<binding::BoundVariableStaticArrayDeclarationStatement*>(declaration.get_value());
                            bool forced_size = array_declaration->array_size != 0;
                            if (forced_size && initializer_node.named_child_count() > array_declaration->array_size)
                            {
                                // initializer list is bigger than array size
                                // int x[2] = {1,2,3};
                                // but
                                // int x[10] = {1,2,3}; is ok, and rest of the elements are 0
                                return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::InvalidInitializerError, initializer_node));
                            }

                            if (not forced_size)
                            {
                                array_declaration->array_size = initializer_node.named_child_count();
                                array_declaration->type.modifiers.back() = Type::Array{array_declaration->array_size};
                                *type_scope.get_from_scopestack(array_declaration->variable_name) = array_declaration->type;
                            }

                            for (uint32_t i = 0; i < array_declaration->array_size; i++)
                            {
                                if (i < initializer_node.named_child_count())
                                {
                                    auto initializer = bind_expression(initializer_node.named_child(i));
                                    BUBBLE_ERROR(initializer);

                                    Type initializer_type = type.value();
                                    initializer_type.modifiers.pop_back();
                                    auto casted_initializer = std::make_unique<binding::BoundCastExpression>(initializer.release_value(), std::move(initializer_type));

                                    array_declaration->initializers.push_back(std::move(casted_initializer));
                                }
                                else
                                {
                                    // TODOOOOO: what if its custom type? investigate or @help

                                    // we just do nothing.. interpreter will memset the rest of the array to 0
                                }
                            }

                            return declaration;
                        }
                        default:
                            SCC_UNREACHABLE();
                    }

                }

                // probably just an expression?
                // TODOOO: investigate other initializers
                using DeclarationKind = binding::BoundVariableDeclarationStatement::VariableDeclarationStatementKind;
                static_assert(static_cast<int>(DeclarationKind::COUNT) == 3, "Update this switch");
                switch (declaration.get_value()->variable_declaration_statement_kind())
                {
                    case DeclarationKind::Value:
                    {
                        auto initializer = bind_expression(initializer_node);
                        BUBBLE_ERROR(initializer);
                        auto casted_initializer = std::make_unique<binding::BoundCastExpression>(initializer.release_value(), type.value());
                        static_cast<binding::BoundVariableValueDeclarationStatement*>(declaration.get_value())->initializer = std::move(casted_initializer);
                        return declaration;
                    }
                    case DeclarationKind::Pointer:
                    {
                        auto initializer = bind_expression(initializer_node);
                        BUBBLE_ERROR(initializer);
                        auto casted_initializer = std::make_unique<binding::BoundCastExpression>(initializer.release_value(), type.value());
                        static_cast<binding::BoundVariablePointerDeclarationStatement*>(declaration.get_value())->initializer = std::move(casted_initializer);
                        return declaration;
                    }
                    case DeclarationKind::StaticArray:
                    {
                        // int x[] = 1;
                        // int x[2] = 1;
                        // both are invalid
                        // char x[] = "abc"; is only valid for static arrays?
                        if (initializer_node.symbol() != Parser::STRING_LITERAL_SYMBOL)
                            return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::InvalidInitializerError, initializer_node));

                        auto array_declaration = static_cast<binding::BoundVariableStaticArrayDeclarationStatement*>(declaration.get_value());
                        bool forced_size = array_declaration->array_size != 0;
                        std::vector<char> escaped_string = escape_string(initializer_node.value());

                        if (forced_size and escaped_string.size() > array_declaration->array_size)
                        {
                            // initializer list is bigger than array size
                            // int x[2] = "abc";
                            // but
                            // int x[10] = "abc"; is ok, and rest of the elements are 0
                            return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::InvalidInitializerError, initializer_node));
                        }

                        if (not forced_size)
                        {
                            array_declaration->array_size = escaped_string.size();
                            array_declaration->type.modifiers.back() = Type::Array{array_declaration->array_size};
                            *type_scope.get_from_scopestack(array_declaration->variable_name) = array_declaration->type;
                        }

                        Type initializer_type = type.value();
                        initializer_type.modifiers.pop_back();

                        for (uint32_t i = 0; i < escaped_string.size(); i++)
                        {
                            auto initializer = std::make_unique<binding::BoundLiteralExpression>(static_cast<Type::Primitive::Char>(escaped_string[i]));
                            auto casted_initializer = std::make_unique<binding::BoundCastExpression>(std::move(initializer), std::move(initializer_type));

                            array_declaration->initializers.push_back(std::move(casted_initializer));
                        }

                        for (uint32_t i = escaped_string.size(); i < array_declaration->array_size; i++)
                        {
                            auto initializer = std::make_unique<binding::BoundLiteralExpression>(static_cast<Type::Primitive::Char>(0));
                            auto casted_initializer = std::make_unique<binding::BoundCastExpression>(std::move(initializer), std::move(initializer_type));

                            array_declaration->initializers.push_back(std::move(casted_initializer));
                        }

                        return declaration;
                    }
                    default:
                        SCC_UNREACHABLE();
                }
            }
        }
        return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::ReachedUnreachableError, node));
    }
}