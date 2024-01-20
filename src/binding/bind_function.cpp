#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundFunctionStatement> Binder::bind_function(const TreeNode &node)
    {
        SCC_ASSERT(node.symbol() == Parser::FUNCTION_DEFINITION_SYMBOL ||
                   node.symbol() == Parser::DECLARATION_SYMBOL);
        SCC_BINDER_RESULT_TYPE(bind_function);

        // int fn(int a, int b) {}

        // function_definition ==>     int fn(int...
        // ├── primitive_type ==>      int
        // ├── function_declarator ==> fn(int a, ...
        // │   ├── identifier ==>      fn
        // │   └── parameter_list ==>  (int a, in...
        // │       ├── parameter_declaration ==>       int a
        // │       │   ├── primitive_type ==>  int
        // │       │   └── identifier ==>      a
        // │       └── parameter_declaration ==>       int b
        // │           ├── primitive_type ==>  int
        // │           └── identifier ==>      b
        // └── compound_statement ==>  {}

        // int fn(int a, int b);
        // declaration ==>     int fn(int...
        // ├── primitive_type ==>      int
        // └── function_declarator ==> fn(int a, ...
        //     ├── identifier ==>      fn
        //     └── parameter_list ==>  (int a, in...
        //         ├── parameter_declaration ==>       int a
        //         │   ├── primitive_type ==>  int
        //         │   └── identifier ==>      a
        //         └── parameter_declaration ==>       int b
        //             ├── primitive_type ==>  int
        //             └── identifier ==>      b

        // int** a();
        // declaration ==>     int** a();
        // ├── primitive_type ==>      int
        // └── pointer_declarator ==>  ** a()
        //     └── pointer_declarator ==>      * a()
        //         └── function_declarator ==> a()
        //             ├── identifier ==>      a
        //             └── parameter_list ==>  ()

        int type_index = node.first_named_child().symbol() == Parser::STORAGE_CLASS_SPECIFIER_SYMBOL; // eg.: `static`
        
        // TODOO: custom types
        std::optional<Type> return_type_opt = Type::from_string(node.named_child(type_index).value());
        if (!return_type_opt.has_value())
        {
            auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::UnknownSymbolError, node));
            error.add_diagnostic("Unknown type: " + node.named_child(type_index).value());
            return error;
        }

        int declarator_index = type_index + 1;

        uint32_t pointer_depth = 0;
        if (node.named_child(declarator_index).symbol() == Parser::POINTER_DECLARATOR_SYMBOL)
        {
            auto current_node = node.named_child(declarator_index);
            while (current_node.symbol() == Parser::POINTER_DECLARATOR_SYMBOL)
            {
                pointer_depth++;
                current_node = current_node.first_named_child();
            }
        }

        auto return_type = return_type_opt.value();
        // return_type.pointer_depth = pointer_depth;
        return_type.modifiers = std::vector<Type::Modifier>(pointer_depth, Type::Pointer{});

        TreeNode declarator = node.named_child_with_symbol_BFS(Parser::FUNCTION_DECLARATOR_SYMBOL).value();

        std::string function_name = declarator.first_named_child().value();
        TreeNode parameter_list = declarator.named_child(1);
        
        
        std::vector<std::unique_ptr<binding::BoundVariableDeclarationStatement>> parameters;
        FunctionDeclaration fn_declaration = {return_type, std::vector<Type>(), false};
        
        m_scope_stack.push();
        for (size_t i = 0; i < parameter_list.named_child_count(); i++)
        {
            SCC_ASSERT(parameter_list.named_child(i).symbol() == Parser::PARAMETER_DECLARATION_SYMBOL);
            auto parameter_declaration = parameter_list.named_child(i);

            // TODOOO: It doesnt have to have a name, but idc for now
            auto bound_declaration = bind_variable_declaration(parameter_declaration);
            BUBBLE_ERROR(bound_declaration);
            m_scope_stack.create_variable(bound_declaration.get_value()->variable_name, bound_declaration.get_value()->type);
            fn_declaration.parameters.push_back(bound_declaration.get_value()->type);
            parameters.emplace_back(bound_declaration.release_value());
        }
        
        m_current_function = fn_declaration;
        if (m_functions.find(function_name) != m_functions.end())
        {
            auto& existing_fn_declaration = m_functions.at(function_name);
            if (existing_fn_declaration != fn_declaration)
            {
                auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::FunctionSignatureMismatchError, node));
                error.add_diagnostic("Function signature mismatch: " + function_name);
                return error;
            }

            if (existing_fn_declaration.is_defined)
            {
                auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::FunctionAlreadyDefinedError, node));
                error.add_diagnostic("Function redeclaration: " + function_name);
                return error;
            }
        }
        else
        {
            m_functions.insert({function_name, fn_declaration});
        }


        // TODOOOOOOOOO: check for missing return statement
        std::unique_ptr<binding::BoundBlockStatement> body = nullptr;
        if (node.last_named_child().symbol() == Parser::COMPOUND_STATEMENT_SYMBOL)
        {
            auto body_result = bind_impl(node.last_named_child());
            BUBBLE_ERROR(body_result);
            body = std::unique_ptr<binding::BoundBlockStatement>(static_cast<binding::BoundBlockStatement*>(body_result.release_value().release()));
            m_functions.at(function_name).is_defined = true;
        }
        m_scope_stack.pop();
        
       return std::make_unique<binding::BoundFunctionStatement>(return_type, std::move(function_name), std::move(parameters), std::move(body));
    }
}