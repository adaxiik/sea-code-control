#pragma once
#include "type.hpp"
#include "memory.hpp"
#include <string>
namespace scc
{
    struct Variable
    {        
        Type type;
        Memory::address_t address;
        bool is_constant;  

        Variable(Type type, Memory::address_t address, bool is_constant = false) : type(type), address(address), is_constant(is_constant) {}
    };  
} 
