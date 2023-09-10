#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"


#include <scc/interpreter.hpp>
#include <scc/binding/binder.hpp>
#include <scc/memory.hpp>
#include <scc/type.hpp>


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
        auto result = interpreter.interpret(VALUE); \
        CHECK(result.is_error()); \
    }while(0)

#define SCC_TEST_INTERPRET_RESULT(CTYPE, EXPECTED_VALUE, CODE) \
    do{ \
        auto result = interpreter.interpret(CODE); \
        CHECK(result.is_ok_and_has_value()); \
        CHECK(result.get_value().type.kind == scc::Type::deduce_type<CTYPE>().kind); \
        CHECK(std::get<CTYPE>(result.get_value().value) == EXPECTED_VALUE); \
    }while(0)

#define SCC_TEST_IS_OK(CODE) \
    do{ \
        auto result = interpreter.interpret(CODE); \
        CHECK(result.is_ok()); \
    }while(0)

#define SCC_TEST_BINDER(CODE) \
    do{ \
        auto parse_result = interpreter.parse(CODE); \
        CHECK(!parse_result.has_error()); \
        auto binder = scc::Binder(); \
        auto binded {binder.bind(parse_result.root_node())}; \
        CHECK(binded.is_ok()); \
    }while(0)

#define SCC_TEST_BINDER_ERROR(CODE) \
    do{ \
        auto parse_result = interpreter.parse(CODE); \
        CHECK(!parse_result.has_error()); \
        auto binder = scc::Binder(); \
        auto binded {binder.bind(parse_result.root_node())}; \
        CHECK(binded.is_error()); \
    }while(0)

TEST_CASE("Single Expressions")
{
    auto interpreter = scc::Interpreter();

    SUBCASE("Failed to parse")
    {
        SCC_TEST_IS_ERROR("1 + ;");
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
        
        SCC_TEST_IS_ERROR("0x;");
        SCC_TEST_IS_ERROR("0b;");
        SCC_TEST_IS_ERROR("0o;");
        SCC_TEST_IS_ERROR("0b2;");
        SCC_TEST_IS_ERROR("0xg;");
        SCC_TEST_IS_ERROR("0o8;");

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

        SCC_TEST_IS_ERROR("1<<1.0;");
        SCC_TEST_IS_ERROR("1>>1.0;");
        SCC_TEST_IS_ERROR("1%1.0;");

        SCC_TEST_IS_ERROR("1/0;");
        SCC_TEST_IS_ERROR("1%0;");
        SCC_TEST_IS_ERROR("1/0.0;");
        SCC_TEST_IS_ERROR("1%0.0;");
        SCC_TEST_IS_ERROR("1.0/0;");
        SCC_TEST_IS_ERROR("1.0%0;");
        SCC_TEST_IS_ERROR("1.0/0.0;");
        SCC_TEST_IS_ERROR("1.0%0.0;");
        
        SCC_TEST_TYPE_PTR(I32, 1, (int*)1 + 2);
        SCC_TEST_TYPE_PTR(I32, 1, 2 + (int*)1);
        SCC_TEST_IS_ERROR("(int*)1 + (int*)1;");

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

// #define SCC_TEST_MEMORY_CHUNK(CHUNK_ID, BEGIN, END, SIZE, IS_FREE) do { \
//     CHECK(memory.get_chunk_begin(CHUNK_ID) == BEGIN); \
//     CHECK(memory.get_chunk_end(CHUNK_ID) == END); \
//     CHECK(memory.get_chunk_size(CHUNK_ID) == SIZE); \
//     CHECK(memory.is_chunk_free(CHUNK_ID) == IS_FREE); \
// } while(0)

#define SCC_TEST_MEMORY_CHUNK(ADDRESS, ADDRESS_START, ADDRESS_END, SIZE) do { \
    CHECK(ADDRESS_START == ADDRESS); \
    CHECK(memory.get_chunk_end(ADDRESS_START).value() == ADDRESS_END); \
    CHECK(memory.get_chunk_size(ADDRESS_START).value() == SIZE); \
} while(0)

#define SCC_TEST_MEMORY_FREE(ADDRESS, SUCCESS) CHECK(memory.free(ADDRESS) == SUCCESS)

#define SCC_TEST_MEMORY_EXPECT_CHUNK_COUNT(COUNT) CHECK(memory.allocated_chunk_count() == COUNT)

#define SCC_TEST_MEMORY_EXPECT_ALLOCATED_SIZE(SIZE) CHECK(memory.allocated_memory_size_bytes() == SIZE)

#define SCC_TEST_MEMORY_EXPECT_VALUE(ADDRESS, VALUE, TYPE) do { \
    auto value = memory.read<TYPE>(ADDRESS); \
    CHECK(value.has_value()); \
    CHECK(value.value() == VALUE); \
} while(0)

TEST_CASE("Memory alloc and free")
{
    scc::Memory memory(0);
    // Chunk 0 [0, 9] (10)  used
    // Chunk 1 [10, 19] (10)  used
    // Chunk 2 [20, 29] (10)  used
    // Chunk 3 [30, 39] (10)  used
    // Chunk 4 [40, 49] (10)  used

    scc::Memory::address_t chunk0 = memory.allocate(10);
    scc::Memory::address_t chunk1 = memory.allocate(10);
    scc::Memory::address_t chunk2 = memory.allocate(10);
    scc::Memory::address_t chunk3 = memory.allocate(10);
    scc::Memory::address_t chunk4 = memory.allocate(10);
    SCC_TEST_MEMORY_CHUNK(chunk0, 0, 9, 10);
    SCC_TEST_MEMORY_CHUNK(chunk1, 10, 19, 10);
    SCC_TEST_MEMORY_CHUNK(chunk2, 20, 29, 10);
    SCC_TEST_MEMORY_CHUNK(chunk3, 30, 39, 10);
    SCC_TEST_MEMORY_CHUNK(chunk4, 40, 49, 10);
    SCC_TEST_MEMORY_EXPECT_CHUNK_COUNT(5);
    SCC_TEST_MEMORY_EXPECT_ALLOCATED_SIZE(50);

    // Chunk 0 [0, 9] (10)  used
    // Chunk 2 [20, 29] (10)  used
    // Chunk 4 [40, 49] (10)  used

    SCC_TEST_MEMORY_CHUNK(chunk0, 0, 9, 10);
    SCC_TEST_MEMORY_FREE(chunk1, true);
    SCC_TEST_MEMORY_CHUNK(chunk2, 20, 29, 10);
    SCC_TEST_MEMORY_FREE(chunk3, true);
    SCC_TEST_MEMORY_CHUNK(chunk4, 40, 49, 10);
    SCC_TEST_MEMORY_EXPECT_CHUNK_COUNT(3);
    SCC_TEST_MEMORY_EXPECT_ALLOCATED_SIZE(30);
    
    // Chunk 2 [20, 29] (10)  used
    // Chunk 4 [40, 49] (10)  used
    SCC_TEST_MEMORY_FREE(chunk0, true);
    SCC_TEST_MEMORY_CHUNK(chunk2, 20, 29, 10);
    SCC_TEST_MEMORY_CHUNK(chunk4, 40, 49, 10);
    SCC_TEST_MEMORY_EXPECT_CHUNK_COUNT(2);
    SCC_TEST_MEMORY_EXPECT_ALLOCATED_SIZE(20);


    // Chunk 4 [40, 49] (10)  used
    SCC_TEST_MEMORY_FREE(chunk2, true);
    SCC_TEST_MEMORY_CHUNK(chunk4, 40, 49, 10);
    SCC_TEST_MEMORY_EXPECT_CHUNK_COUNT(1);
    SCC_TEST_MEMORY_EXPECT_ALLOCATED_SIZE(10);

    // Chunk 4 [40, 49] (10)  used
    // Chunk 5 [50, 149] (100)  used
    scc::Memory::address_t chunk5 = memory.allocate(100);
    SCC_TEST_MEMORY_CHUNK(chunk5, 50, 149, 100);
    SCC_TEST_MEMORY_EXPECT_CHUNK_COUNT(2);
    SCC_TEST_MEMORY_EXPECT_ALLOCATED_SIZE(110);

    // Chunk 4 [40, 49] (10)  used
    SCC_TEST_MEMORY_FREE(chunk5, true);
    SCC_TEST_MEMORY_FREE(chunk5, false); // double free
    SCC_TEST_MEMORY_EXPECT_CHUNK_COUNT(1);
    SCC_TEST_MEMORY_EXPECT_ALLOCATED_SIZE(10);

    // Chunk 4 [40, 49] (10)  used
    CHECK(memory.write<uint64_t>(chunk4, 0xffeeddccbbaa9988));
    
    SCC_TEST_MEMORY_EXPECT_VALUE(chunk4, 0xffeeddccbbaa9988, uint64_t);
    SCC_TEST_MEMORY_EXPECT_VALUE(chunk4 + 0, 0x88, uint8_t);
    SCC_TEST_MEMORY_EXPECT_VALUE(chunk4 + 1, 0x99, uint8_t);
    SCC_TEST_MEMORY_EXPECT_VALUE(chunk4 + 2, 0xaa, uint8_t);
    SCC_TEST_MEMORY_EXPECT_VALUE(chunk4 + 3, 0xbb, uint8_t);
    SCC_TEST_MEMORY_EXPECT_VALUE(chunk4 + 4, 0xcc, uint8_t);
    SCC_TEST_MEMORY_EXPECT_VALUE(chunk4 + 5, 0xdd, uint8_t);
    SCC_TEST_MEMORY_EXPECT_VALUE(chunk4 + 6, 0xee, uint8_t);
    SCC_TEST_MEMORY_EXPECT_VALUE(chunk4 + 7, 0xff, uint8_t);

    // out of bounds
    // ... | 5 | 6 | 7 | 8 | 9 | - | - | - | ...
    //                   ^   ^   ^   ^

    CHECK(!memory.read<int32_t>(chunk4 + 8).has_value());
    CHECK(!memory.read<int32_t>(chunk4 + 7).has_value());
    CHECK( memory.read<int32_t>(chunk4 + 6).has_value());
    CHECK( memory.read<int32_t>(chunk4 + 5).has_value());

    CHECK(!memory.write<int32_t>(chunk4 + 8, 0xdeadbeef));
    CHECK(!memory.write<int32_t>(chunk4 + 7, 0xdeadbeef));
    CHECK( memory.write<int32_t>(chunk4 + 6, 0xdeadbeef));
    SCC_TEST_MEMORY_EXPECT_VALUE(chunk4 + 6, 0xdeadbeef, int32_t);
}

#define SCC_GET_PTR_FROM_RESULT(INTERPRETER_RESULT) \
    std::get<scc::Type::Primitive::U64>(INTERPRETER_RESULT.get_value().value)


TEST_CASE("Scopes")
{
    auto interpreter = scc::Interpreter();
    auto a = SCC_GET_PTR_FROM_RESULT(interpreter.interpret("int a;"));
    auto b = SCC_GET_PTR_FROM_RESULT(interpreter.interpret("int b;"));

    CHECK(a != b);
    CHECK(a == b + sizeof(int));
    CHECK(interpreter.interpret("int a;").is_error());
    CHECK(interpreter.interpret("int b;").is_error());

    auto c1 = SCC_GET_PTR_FROM_RESULT(interpreter.interpret("{int c;}"));
    auto c2 = SCC_GET_PTR_FROM_RESULT(interpreter.interpret("{int c;}"));
    CHECK(c1 == c2);
    CHECK(b == c1 + sizeof(int));
}

TEST_CASE("Assignments")
{
    auto interpreter = scc::Interpreter();
    SCC_TEST_IS_ERROR("a;"); // undeclared
    SCC_TEST_IS_OK("int a;");
    SCC_TEST_IS_ERROR("a;"); // not initialized
    SCC_TEST_IS_ERROR("a = a;"); // not initialized
    SCC_TEST_INTERPRET_RESULT(int, 1, "a = 1;");
    SCC_TEST_INTERPRET_RESULT(int, 1, "a;");
    SCC_TEST_INTERPRET_RESULT(int, 34, "a = 34;");
    SCC_TEST_INTERPRET_RESULT(int, 69, "a += 35;");
    SCC_TEST_IS_OK("double b = 1.5;");
    SCC_TEST_INTERPRET_RESULT(double, 1.5, "b;");
    SCC_TEST_INTERPRET_RESULT(double, 1.5 + 1.6, "b += 1.6;");
    SCC_TEST_INTERPRET_RESULT(double, 1.5 + 1.6, "b;");    
    SCC_TEST_INTERPRET_RESULT(int, 3, "a = b;"); // implicit cast
    SCC_TEST_INTERPRET_RESULT(int, 3, "a;"); // implicit cast

    SCC_TEST_IS_OK("int c = a + b;"); // 3 + 3.1 = 6.1 -> 6
    SCC_TEST_INTERPRET_RESULT(int, 6, "c;");
}
TEST_CASE("If statement")
{
    SUBCASE("Simple")
    {
        auto interpreter = scc::Interpreter();
        SCC_TEST_IS_OK("int a = 0;");
        SCC_TEST_IS_OK("int b = 0;");

        SCC_TEST_IS_OK("if (0) { b = 1; }");       
        SCC_TEST_INTERPRET_RESULT(int, 0, "a;");
        SCC_TEST_INTERPRET_RESULT(int, 0, "b;");

        SCC_TEST_IS_OK("if (1) { b = 1; }");
        SCC_TEST_INTERPRET_RESULT(int, 0, "a;");
        SCC_TEST_INTERPRET_RESULT(int, 1, "b;");

        auto statement = ""
        "if (a == 0) {"
        "   b = 10;"
        "   a = 1;"
        "}"
        "else if (a == 1) {"
        "   b = 20;"
        "   a = 2;"
        "}"
        "else {"
        "   b = 30;"
        "   a = 3;"
        "}";

        SCC_TEST_IS_OK(statement);
        SCC_TEST_INTERPRET_RESULT(int, 1, "a;");
        SCC_TEST_INTERPRET_RESULT(int, 10, "b;");
        SCC_TEST_IS_OK(statement);
        SCC_TEST_INTERPRET_RESULT(int, 2, "a;");
        SCC_TEST_INTERPRET_RESULT(int, 20, "b;");
        SCC_TEST_IS_OK(statement);
        SCC_TEST_INTERPRET_RESULT(int, 3, "a;");
        SCC_TEST_INTERPRET_RESULT(int, 30, "b;");
        SCC_TEST_IS_OK(statement);
        SCC_TEST_INTERPRET_RESULT(int, 3, "a;");
        SCC_TEST_INTERPRET_RESULT(int, 30, "b;");
    }

    SUBCASE("Shadowing")
    {
        auto interpreter = scc::Interpreter();
        SCC_TEST_IS_OK("int a = 10;");
        SCC_TEST_IS_OK("if (1) { int a = 20; }");
        SCC_TEST_INTERPRET_RESULT(int, 10, "a;");
    }
}

TEST_CASE("While and Do statements")
{
    auto interpreter = scc::Interpreter();
    SCC_TEST_IS_OK("int a = 0;");
    SCC_TEST_IS_OK("int b = 0;");

    SCC_TEST_IS_OK("while (0) { b = 1; }");       
    SCC_TEST_INTERPRET_RESULT(int, 0, "a;");
    SCC_TEST_INTERPRET_RESULT(int, 0, "b;");

    SCC_TEST_IS_OK("a = 1;");
    SCC_TEST_IS_OK("while (a+=1) { b = 1; break;}");
    SCC_TEST_INTERPRET_RESULT(int, 2, "a;");
    SCC_TEST_INTERPRET_RESULT(int, 1, "b;");

    SCC_TEST_IS_OK("a = 1;");
    SCC_TEST_IS_OK("b = 0;");

    SCC_TEST_IS_OK("do { b = 1; break;} while (a+=1);");
    SCC_TEST_INTERPRET_RESULT(int, 1, "a;");
    SCC_TEST_INTERPRET_RESULT(int, 1, "b;");

    SCC_TEST_IS_OK("a = 0;");
    SCC_TEST_IS_OK("b = 0;");
    SCC_TEST_IS_OK(""
    "while (a < 20) {"
    "   a += 1;"
    "   if (a == 10) {"
    "       continue;"
    "   }"
    "   b += 1;"
    "}"
    );

    SCC_TEST_INTERPRET_RESULT(int, 20, "a;");
    SCC_TEST_INTERPRET_RESULT(int, 19, "b;");

    SCC_TEST_IS_OK("a = 0;");
    SCC_TEST_IS_OK("b = 0;");
    SCC_TEST_IS_OK(""
    "while (a < 20) {"
    "   a += 1;"
    "   if (a == 10) {"
    "       break;"
    "   }"
    "   b += 1;"
    "}"
    );

    SCC_TEST_INTERPRET_RESULT(int, 10, "a;");
    SCC_TEST_INTERPRET_RESULT(int, 9, "b;");
}

TEST_CASE("Functions")
{
    auto interpreter = scc::Interpreter();
    SCC_TEST_IS_OK("int fn_a();");
    SCC_TEST_IS_ERROR("fn_a();");
    SCC_TEST_IS_OK("int fn_a() { return 1; }");
    SCC_TEST_INTERPRET_RESULT(int, 1, "fn_a();");

    SCC_TEST_IS_OK("int fn_b(int a) { return a; }");
    SCC_TEST_INTERPRET_RESULT(int, 1, "fn_b(1);");
    SCC_TEST_INTERPRET_RESULT(int, 2, "fn_b(2);");
    SCC_TEST_INTERPRET_RESULT(int, 3, "fn_b(3);");
    SCC_TEST_IS_OK("int a = 0;");
    SCC_TEST_INTERPRET_RESULT(int, 0, "a;");
    SCC_TEST_INTERPRET_RESULT(int, 1, "fn_b(1);");
    SCC_TEST_INTERPRET_RESULT(int, 0, "a;");
    SCC_TEST_IS_OK("a = fn_b(1);");
    SCC_TEST_INTERPRET_RESULT(int, 1, "a;");

    SCC_TEST_IS_OK("int fn_c(double a, double b) { return a + b; }");
    SCC_TEST_INTERPRET_RESULT(int, 4, "fn_c(1.6, 2.6);");

    SCC_TEST_IS_OK("int fn_d(int a, int b) { return fn_c(a, b); }");
    SCC_TEST_INTERPRET_RESULT(int, 4, "fn_d(1, 3);");

    SCC_TEST_IS_OK("int factorial(int n) { if (n <= 0) { return 1; } return n * factorial(n - 1); }");
    SCC_TEST_INTERPRET_RESULT(int, 1, "factorial(0);");
    SCC_TEST_INTERPRET_RESULT(int, 1, "factorial(1);");
    SCC_TEST_INTERPRET_RESULT(int, 2, "factorial(2);");
    SCC_TEST_INTERPRET_RESULT(int, 6, "factorial(3);");
    SCC_TEST_INTERPRET_RESULT(int, 24, "factorial(4);");
    // SCC_TEST_INTERPRET_RESULT(int, 120, "factorial(5);"); // TODOO: fix stack overflow

    SCC_TEST_IS_OK("int var = 420;");
    SCC_TEST_IS_OK("int fn_e() { int var = 1; return var; }");
    SCC_TEST_INTERPRET_RESULT(int, 420, "var;");
    SCC_TEST_INTERPRET_RESULT(int, 1, "fn_e();");
    SCC_TEST_INTERPRET_RESULT(int, 420, "var;");

    SCC_TEST_IS_OK("int fn_f() { return var; }");
    SCC_TEST_INTERPRET_RESULT(int, 420, "fn_f();");
}