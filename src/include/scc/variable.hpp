#pragma once
#include "type.hpp"
#include <string>
namespace scc
{
    struct Variable
    {        
        Type type;
        bool is_constant;  

        Variable(Type type, bool is_constant = false)
            : type(type), is_constant(is_constant) {}      
        
        Variable(bool is_constant = false) 
            : type(Type::Kind::COUNT), is_constant(is_constant) {}
    };  
} 
