#include "scope.hpp"
#include "type.hpp"

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
        };
    }
}
