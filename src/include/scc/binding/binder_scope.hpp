#include "scope.hpp"
#include "type.hpp"
#include "binder_result.hpp"

namespace scc
{
    namespace binding
    {

        class BinderScopeStack : public ScopeStack<Scope<Type>>
        {
        public:
            BinderScopeStack() = default;
            virtual ~BinderScopeStack() = default;
            virtual void push() override
            {
                m_scopes.emplace_back(Scope<Type>{});
            }

            /**
             * @brief Creates a variable in the current scope
             * 
             * @param name 
             * @param type 
             * @return true on success
             * @return false on failure
             */
            bool create_variable(const std::string &name, Type type)
            {
                if (m_scopes.empty()) // This should be unreachable, but you never know
                    return false;

                Type* var{m_scopes.back().get_from_scope(name)};
                if (var != nullptr)
                    return false;
                
                m_scopes.back().ref_scopes().emplace(name, type);
                return true;
            }

            bool is_global_scope() const
            {
                return m_scopes.size() == 1;
            }
        };
    }
}
