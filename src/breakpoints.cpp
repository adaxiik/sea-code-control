#include "breakpoints.hpp"

namespace scc
{
    Breakpoints& Breakpoints::add(BreakLineNumber line_number)
    {
        m_breakpoints.insert(line_number);
        return *this;
    }

    Breakpoints& Breakpoints::remove(BreakLineNumber line_number)
    {
        m_breakpoints.erase(line_number);
        return *this;
    }

    bool Breakpoints::contains(BreakLineNumber line_number) const
    {
        return m_breakpoints.find(line_number) != m_breakpoints.end();
    }

    void Breakpoints::clear()
    {
        m_breakpoints.clear();
    }

    size_t Breakpoints::size() const
    {
        return m_breakpoints.size();
    }

}