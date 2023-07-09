#define SCC_NEW_VARIABLE_IMPLEMENTATION
#include "instructions/new_variable.hpp"
#undef SCC_NEW_VARIABLE_IMPLEMENTATION

#define SCC_POP_SCOPE_IMPLEMENTATION
#include "instructions/pop_scope.hpp"
#undef SCC_POP_SCOPE_IMPLEMENTATION

#define SCC_PUSH_SCOPE_IMPLEMENTATION
#include "instructions/push_scope.hpp"
#undef SCC_PUSH_SCOPE_IMPLEMENTATION

#define SCC_LOAD_IMPLEMENTATION
#include "instructions/load.hpp"
#undef SCC_LOAD_IMPLEMENTATION

#define SCC_STORE_IMPLEMENTATION
#include "instructions/store.hpp"
#undef SCC_STORE_IMPLEMENTATION

#define SCC_PUSH_NUMBER_IMPLEMENTATION
#include "instructions/push_number.hpp"
#undef SCC_PUSH_NUMBER_IMPLEMENTATION

#define SCC_ADD_IMPLEMENTATION
#include "instructions/add.hpp"
#undef SCC_ADD_IMPLEMENTATION

#define SCC_GET_PTR_TO_VAR_IMPLEMENTATION
#include "instructions/get_ptr_to_var.hpp"
#undef SCC_GET_PTR_TO_VAR_IMPLEMENTATION

#define SCC_D_PRINT_IMPLEMENTATION
#include "instructions/d_print.hpp"
#undef SCC_D_PRINT_IMPLEMENTATION

#define SCC_CAST_IMPLEMENTATION
#include "instructions/cast.hpp"
#undef SCC_CAST_IMPLEMENTATION

#define SCC_DROP_IMPLEMENTATION
#include "instructions/drop.hpp"
#undef SCC_DROP_IMPLEMENTATION