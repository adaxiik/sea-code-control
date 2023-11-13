#pragma once
#include <set>
#include <cstddef>
namespace scc
{
    class Breakpoints
    {
    public:
        using BreakLineNumber = size_t;
        Breakpoints& add(BreakLineNumber line_number);
        Breakpoints& remove(BreakLineNumber line_number);
        bool contains(BreakLineNumber line_number) const;
        void clear();

    private:
        std::set<BreakLineNumber> m_breakpoints;
    };
}