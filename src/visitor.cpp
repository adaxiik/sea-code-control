#include "visitor.hpp"
#include "instructions.hpp"

namespace scc
{
    void OsTextVisitor::visit(const instructions::NewVariable& new_var)
    {
        os << "*" << new_var.m_name << " " << new_var.m_type;
    }

    void OsTextVisitor::visit(const instructions::PopScope& )
    {
        os << "}";
    }

    void OsTextVisitor::visit(const instructions::PushScope& )
    {
        os << "{";
    }

    void OsTextVisitor::visit(const instructions::Load& load)
    {
        os << "load " << static_cast<int>(load.m_size_bytes);
    }

    void OsTextVisitor::visit(const instructions::Store& store)
    {
        os << "store " << static_cast<int>(store.m_size_bytes);
    }

    void OsTextVisitor::visit(const instructions::PushNumber& push_number)
    {
        os << "push " << push_number.m_value;
    }

    void OsTextVisitor::visit(const instructions::Add& add)
    {
        os << "add " << add.m_result_type;
    }

    void OsTextVisitor::visit(const instructions::GetPtrToVar& get_ptr_to_var)
    {
        os << "&" << get_ptr_to_var.m_name;
    }

    void OsTextVisitor::visit(const instructions::DPrint& )
    {
        os << "call dprint";
    }

    void OsTextVisitor::visit(const instructions::Cast& cast)
    {
        os << "cast " << cast.m_from_type << " to " << cast.m_to_type;
    }
}