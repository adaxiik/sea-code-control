#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"


#include <scc/interpreter.hpp>
#include <scc/binding/binder.hpp>
#include <scc/memory.hpp>

#define SCC_TEST_TYPE(TYPE, VALUE) \
    do \
    {  \
        auto result = interpreter.interpret(#VALUE ";"); \
        CHECK(result.is_ok_and_has_value()); \
        CHECK(result.get_value().type.kind == scc::Type::Kind::TYPE); \
    }while(0)

#define SCC_TEST_TYPE_PTR(TYPE,PTR_DEPTH, VALUE) \
    do \
    {  \
        auto result = interpreter.interpret(#VALUE ";"); \
        CHECK(result.is_ok_and_has_value()); \
        CHECK(result.get_value().type.kind == scc::Type::Kind::TYPE); \
        CHECK(result.get_value().type.pointer_depth == PTR_DEPTH); \
    }while(0)

#define SCC_TEST_TYPE_AUTO(VALUE) \
    do \
    {  \
        auto result = interpreter.interpret(#VALUE ";"); \
        CHECK(result.is_ok_and_has_value()); \
        CHECK(result.get_value().type.kind == scc::Type::deduce_type<decltype(VALUE)>().kind); \
    }while(0)

#define SCC_TEST_IS_ERROR(VALUE) \
    do \
    {  \
        auto result = interpreter.interpret(#VALUE ";"); \
        CHECK(result.is_error()); \
    }while(0)

#define SCC_TEST_INTERPRET_RESULT(CTYPE, EXPECTED_VALUE, CODE) \
    do{ \
        auto result = interpreter.interpret(CODE); \
        CHECK(result.is_ok_and_has_value()); \
        CHECK(result.get_value().type.kind == scc::Type::deduce_type<CTYPE>().kind); \
        CHECK(std::any_cast<CTYPE>(result.get_value().value) == EXPECTED_VALUE); \
    }while(0)

#define SCC_TEST_BINDER(CODE) \
    do{ \
        auto parse_result = interpreter.parse(CODE); \
        CHECK(!parse_result.has_error()); \
        auto binded {scc::Binder::bind(parse_result.root_node())}; \
        CHECK(binded.is_ok()); \
    }while(0)

#define SCC_TEST_BINDER_ERROR(CODE) \
    do{ \
        auto parse_result = interpreter.parse(CODE); \
        CHECK(!parse_result.has_error()); \
        auto binded {scc::Binder::bind(parse_result.root_node())}; \
        CHECK(binded.is_error()); \
    }while(0)

TEST_CASE("Single Expressions")
{
    auto interpreter = scc::Interpreter();

    SUBCASE("Failed to parse")
    {
        SCC_TEST_IS_ERROR(1 + );
        CHECK(interpreter.interpret("1").is_error());
    }

    SUBCASE("Literals")
    {
        SCC_TEST_TYPE(I32, 1);
        SCC_TEST_TYPE(F32, 1.0f);
        SCC_TEST_TYPE(F64, 1.0);
        SCC_TEST_TYPE(I64, 1l);
        SCC_TEST_TYPE(I64, 1L);
        SCC_TEST_TYPE(I64, 1ll);
        SCC_TEST_TYPE(I64, 1LL);
        SCC_TEST_TYPE(U32, 1u);
        SCC_TEST_TYPE(U32, 1U);
        SCC_TEST_TYPE(U64, 1ul);
        SCC_TEST_TYPE(U64, 1UL);
        SCC_TEST_TYPE(U64, 1ull);
        SCC_TEST_TYPE(U64, 1ULL);

        SCC_TEST_TYPE_AUTO(1);
        SCC_TEST_TYPE_AUTO(1.0f);
        SCC_TEST_TYPE_AUTO(1.0);
        SCC_TEST_TYPE_AUTO(1l);
        SCC_TEST_TYPE_AUTO(1L);
        SCC_TEST_TYPE_AUTO(1ll);
        SCC_TEST_TYPE_AUTO(1LL);
        SCC_TEST_TYPE_AUTO(1u);
        SCC_TEST_TYPE_AUTO(1U);
        SCC_TEST_TYPE_AUTO(1ul);
        SCC_TEST_TYPE_AUTO(1UL);
        SCC_TEST_TYPE_AUTO(1ull);
        SCC_TEST_TYPE_AUTO(1ULL);

        SCC_TEST_TYPE_AUTO(0x1);
        SCC_TEST_TYPE_AUTO(0x1u);
        SCC_TEST_TYPE_AUTO(0x1l);
        SCC_TEST_TYPE_AUTO(0x1ul);
        SCC_TEST_TYPE_AUTO(0x1ll);
        SCC_TEST_TYPE_AUTO(0x1ull);

        SCC_TEST_TYPE_AUTO(0b1);
        SCC_TEST_TYPE_AUTO(0b1u);
        SCC_TEST_TYPE_AUTO(0b1l);
        SCC_TEST_TYPE_AUTO(0b1ul);
        SCC_TEST_TYPE_AUTO(0b1ll);
        SCC_TEST_TYPE_AUTO(0b1ull);

        SCC_TEST_TYPE_AUTO(0777);
        SCC_TEST_TYPE_AUTO(0777u);
        SCC_TEST_TYPE_AUTO(0777l);
        SCC_TEST_TYPE_AUTO(0777ul);
        SCC_TEST_TYPE_AUTO(0777ll);
        SCC_TEST_TYPE_AUTO(0777ull);
        
        SCC_TEST_IS_ERROR(0x);
        SCC_TEST_IS_ERROR(0b);
        SCC_TEST_IS_ERROR(0o);
        SCC_TEST_IS_ERROR(0b2);
        SCC_TEST_IS_ERROR(0xg);
        SCC_TEST_IS_ERROR(0o8);

        SCC_TEST_INTERPRET_RESULT(int, 7, "0b111;");
        // this raises an ParseError, but on tree-sitter level
        // if I really want to, I could probably bypass it, but I don't think it's worth it right now :)
        // Im not sure if this syntax is actually valid in C99 (but it is in C++17)
        // @help
        // SCC_TEST_INTERPRET_RESULT(int, 7, "0B111;"); 
        SCC_TEST_INTERPRET_RESULT(int, 255, "0xFF;");
        // SCC_TEST_INTERPRET_RESULT(int, 255, "0Xff;");


    }

    SUBCASE("Binary Expressions")
    {
        SCC_TEST_TYPE(I32, 1 + 1);
        SCC_TEST_TYPE(I32, 1 - 1);

        SCC_TEST_TYPE(U32, 1 + 1u);
        SCC_TEST_TYPE(I64, 1ll + 1u);

        SCC_TEST_TYPE_AUTO(1 + 1);
        SCC_TEST_TYPE_AUTO(1 - 1);
        SCC_TEST_TYPE_AUTO(1 + 1u);
        SCC_TEST_TYPE_AUTO(1ll + 1u);

        SCC_TEST_TYPE(Bool, 1 == 1);
        SCC_TEST_TYPE(Bool, 1 != 1);
        SCC_TEST_TYPE(Bool, 1 < 1);

        SCC_TEST_IS_ERROR(1<<1.0);
        SCC_TEST_IS_ERROR(1>>1.0);
        SCC_TEST_IS_ERROR(1%1.0);

        SCC_TEST_IS_ERROR(1/0);
        SCC_TEST_IS_ERROR(1%0);
        SCC_TEST_IS_ERROR(1/0.0);
        SCC_TEST_IS_ERROR(1%0.0);
        SCC_TEST_IS_ERROR(1.0/0);
        SCC_TEST_IS_ERROR(1.0%0);
        SCC_TEST_IS_ERROR(1.0/0.0);
        SCC_TEST_IS_ERROR(1.0%0.0);
        
        SCC_TEST_TYPE_PTR(I32, 1, (int*)1 + 2);
        SCC_TEST_TYPE_PTR(I32, 1, 2 + (int*)1);
        SCC_TEST_IS_ERROR((int*)1 + (int*)1);

    }

    SUBCASE("Cast Expressions")
    {
        SCC_TEST_TYPE(I32, (int)1);
        SCC_TEST_TYPE(I32, (int)1.0f);
        SCC_TEST_TYPE(I32, (int)1.0);
        SCC_TEST_TYPE(I32, (int)1l);
        SCC_TEST_TYPE(I32, (int)1L);
        SCC_TEST_TYPE(I32, (int)1ll);
        SCC_TEST_TYPE(I32, (int)1LL);
        SCC_TEST_TYPE(I32, (int)1u);
        SCC_TEST_TYPE(I32, (int)1U);
        SCC_TEST_TYPE(I32, (int)1ul);
        SCC_TEST_TYPE(I32, (int)1UL);
        SCC_TEST_TYPE(I32, (int)1ull);
        SCC_TEST_TYPE(I32, (int)1ULL);

        SCC_TEST_TYPE(Char, (char)69);

        
        SCC_TEST_INTERPRET_RESULT(char, 'E', "(char)69;");
        SCC_TEST_INTERPRET_RESULT(int, 420, "(int)420.69f;");
        SCC_TEST_INTERPRET_RESULT(short, 420, "(short)420.69f;");

        SCC_TEST_TYPE_PTR(I32, 1, (int*)1);
        SCC_TEST_TYPE_PTR(Char, 1, (char*)1);
        SCC_TEST_TYPE_PTR(I32, 2, (int**)2);
    }

    SUBCASE("Parenthesized Expression")
    {
        SCC_TEST_INTERPRET_RESULT(int, 69 , "(1,2,3,69);");
        SCC_TEST_INTERPRET_RESULT(int, 420 , "2 * (105 + 105);");
        SCC_TEST_INTERPRET_RESULT(char, 'E' , "(char)(65 + (1 + (1 + (1 + 1))));");
        SCC_TEST_INTERPRET_RESULT(short, 100, "(1,2,3,(short)100);");
    }

    SUBCASE("Declaration")
    {
        SCC_TEST_BINDER("int a;");
        SCC_TEST_BINDER("int* a;");
        // TODOO: multiple declarations
        // SCC_TEST_BINDER("int a, b;"); 
        SCC_TEST_BINDER("int a = 1;");
        // TODOO: multiple initializers
        // SCC_TEST_BINDER("int a = 1, b = 2;"); 
        SCC_TEST_BINDER("int a[1];");
        // TODO: 2d arrays 
        // SCC_TEST_BINDER("int a[1][2];");
        // TODOO: complex types
        // SCC_TEST_BINDER("int **a[2][2];");
        SCC_TEST_BINDER("int a[1] = {1};");
        SCC_TEST_BINDER_ERROR("int a[1] = {1, 2};"); 
        SCC_TEST_BINDER("int a = {1};");
        SCC_TEST_BINDER_ERROR("int a = {1, 2};"); 

    }
}

#define SCC_TEST_MEMORY_CHUNK(CHUNK_ID, BEGIN, END, SIZE, IS_FREE) do { \
    CHECK(memory.get_chunk_begin(CHUNK_ID) == BEGIN); \
    CHECK(memory.get_chunk_end(CHUNK_ID) == END); \
    CHECK(memory.get_chunk_size(CHUNK_ID) == SIZE); \
    CHECK(memory.is_chunk_free(CHUNK_ID) == IS_FREE); \
} while(0)


TEST_CASE("Memory alloc and free")
{
    scc::Memory memory;
    // Chunk 0 [0, 9] (10)  used
    // Chunk 1 [10, 19] (10)  used
    // Chunk 2 [20, 29] (10)  used
    // Chunk 3 [30, 39] (10)  used
    // Chunk 4 [40, 49] (10)  used

    scc::Memory::MemoryChunkId chunk0 = memory.allocate(10);
    SCC_TEST_MEMORY_CHUNK(chunk0, 0, 9, 10, false);

    scc::Memory::MemoryChunkId chunk1 = memory.allocate(10);
    scc::Memory::MemoryChunkId chunk2 = memory.allocate(10);
    scc::Memory::MemoryChunkId chunk3 = memory.allocate(10);
    scc::Memory::MemoryChunkId chunk4 = memory.allocate(10);
    SCC_TEST_MEMORY_CHUNK(chunk1, 10, 19, 10, false);
    SCC_TEST_MEMORY_CHUNK(chunk2, 20, 29, 10, false);
    SCC_TEST_MEMORY_CHUNK(chunk3, 30, 39, 10, false);
    SCC_TEST_MEMORY_CHUNK(chunk4, 40, 49, 10, false);

    // Chunk 0 [0, 9] (10)  used
    // Chunk 1 [10, 19] (10)  free
    // Chunk 2 [20, 29] (10)  used
    // Chunk 3 [30, 39] (10)  free
    // Chunk 4 [40, 49] (10)  used

    memory.free(chunk1);
    memory.free(chunk3);
    SCC_TEST_MEMORY_CHUNK(chunk0, 0, 9, 10, false);
    SCC_TEST_MEMORY_CHUNK(chunk1, 10, 19, 10, true);
    SCC_TEST_MEMORY_CHUNK(chunk2, 20, 29, 10, false);
    SCC_TEST_MEMORY_CHUNK(chunk3, 30, 39, 10, true);
    SCC_TEST_MEMORY_CHUNK(chunk4, 40, 49, 10, false);
    
    // Chunk 0 [0, 19] (20)  free
    // Chunk 2 [20, 29] (10)  used
    // Chunk 3 [30, 39] (10)  free
    // Chunk 4 [40, 49] (10)  used
    memory.free(chunk0);
    SCC_TEST_MEMORY_CHUNK(chunk0, 0, 19, 20, true); // 0 and 1 merged into 0
    SCC_TEST_MEMORY_CHUNK(chunk2, 20, 29, 10, false);
    SCC_TEST_MEMORY_CHUNK(chunk3, 30, 39, 10, true);
    SCC_TEST_MEMORY_CHUNK(chunk4, 40, 49, 10, false);

    // Chunk 2 [0, 39] (40)  free
    // Chunk 4 [40, 49] (10)  used
    memory.free(chunk2);
    SCC_TEST_MEMORY_CHUNK(chunk2, 0, 39, 40, true); // 0 and 3 merged into 2
    SCC_TEST_MEMORY_CHUNK(chunk4, 40, 49, 10, false);

    // Chunk 2 [0, 29] (30)  used       // again returned id 2
    // Chunk 4 [40, 49] (10)  used
    // Chunk 5 [30, 39] (10)  free      // rest of the memory is free and assigned to new chunk
    scc::Memory::MemoryChunkId chunk2_new = memory.allocate(30);
    SCC_TEST_MEMORY_CHUNK(chunk2_new, 0, 29, 30, false);
    SCC_TEST_MEMORY_CHUNK(chunk4, 40, 49, 10, false);

    // Chunk 2 [0, 29] (30)  used
    // Chunk 4 [40, 49] (10)  used
    // Chunk 5 [30, 39] (10)  free
    // Chunk 6 [50, 79] (30)  used
    scc::Memory::MemoryChunkId chunk6 = memory.allocate(30);
    SCC_TEST_MEMORY_CHUNK(chunk2_new, 0, 29, 30, false);
    SCC_TEST_MEMORY_CHUNK(chunk4, 40, 49, 10, false);
    SCC_TEST_MEMORY_CHUNK(chunk6, 50, 79, 30, false);
}
