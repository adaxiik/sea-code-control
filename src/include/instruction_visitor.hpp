#pragma once
#include <ostream>
namespace scc
{
    namespace instructions
    {
        struct NewVariable;
        struct PopScope;
        struct PushScope;
        struct Load;
        struct Store;
        struct PushNumber;
        struct Add;
        struct GetPtrToVar;
        struct DPrint;
        struct Cast;
        struct Drop;
    }
    
    struct InstructionVisitor
    {
        virtual ~InstructionVisitor() = default;
        virtual void visit(const scc::instructions::NewVariable& new_var) = 0;
        virtual void visit(const scc::instructions::PopScope& pop_scope) = 0;
        virtual void visit(const scc::instructions::PushScope& push_scope) = 0;
        virtual void visit(const scc::instructions::Load& load) = 0;
        virtual void visit(const scc::instructions::Store& store) = 0;
        virtual void visit(const scc::instructions::PushNumber& push_number) = 0;
        virtual void visit(const scc::instructions::Add& add) = 0;
        virtual void visit(const scc::instructions::GetPtrToVar& get_ptr_to_var) = 0;
        virtual void visit(const scc::instructions::DPrint& d_print) = 0;
        virtual void visit(const scc::instructions::Cast& cast) = 0;
        virtual void visit(const scc::instructions::Drop& drop) = 0;
    };

    struct OsTextVisitor : public InstructionVisitor
    {
        std::ostream& os;
        OsTextVisitor(std::ostream& os) : os(os) {}

        virtual void visit(const scc::instructions::NewVariable& new_var) override;
        virtual void visit(const scc::instructions::PopScope& pop_scope) override;
        virtual void visit(const scc::instructions::PushScope& push_scope) override;
        virtual void visit(const scc::instructions::Load& load) override;
        virtual void visit(const scc::instructions::Store& store) override;
        virtual void visit(const scc::instructions::PushNumber& push_number) override;
        virtual void visit(const scc::instructions::Add& add) override;
        virtual void visit(const scc::instructions::GetPtrToVar& get_ptr_to_var) override;
        virtual void visit(const scc::instructions::DPrint& d_print) override;
        virtual void visit(const scc::instructions::Cast& cast) override;
        virtual void visit(const scc::instructions::Drop& drop) override;
    };
     
} 