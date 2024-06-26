#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "3rdparty/doctest.h"


#include <scc/interpreter.hpp>
#include <scc/binding/binder.hpp>
#include <scc/memory.hpp>
#include <scc/type.hpp>

auto scc_append_and_continue(scc::Interpreter& interpreter, scc::RunningInterpreter& running_interpreter, const std::string& code)
{
    auto parsed = interpreter.parse(code);
    if (parsed.has_error())
        return scc::InterpreterResult::error(scc::InterpreterError::ParseError);
    auto binded = interpreter.bind(parsed.root_node());
    if (binded.is_error())
        return scc::InterpreterResult::error(scc::InterpreterError::BindError);
    auto lowered = interpreter.lower(static_cast<scc::binding::BoundBlockStatement*>(binded.get_value()));
    return running_interpreter.append_code(lowered).continue_execution();
}

#define SCC_APPEND_AND_CONTINUE(CODE) scc_append_and_continue(interpreter, running_interpreter, CODE)

#define SCC_TEST_TYPE(TYPE, VALUE) \
    do \
    {  \
        auto result = SCC_APPEND_AND_CONTINUE(#VALUE ";"); \
        CHECK(result.is_ok_and_has_value()); \
        CHECK(result.get_value().type.primitive_type().value_or(scc::Type::PrimitiveType::COUNT) == scc::Type::PrimitiveType::TYPE); \
    }while(0)

#define SCC_TEST_TYPE_PTR(TYPE,PTR_DEPTH, VALUE) \
    do \
    {  \
        auto result = SCC_APPEND_AND_CONTINUE(#VALUE ";"); \
        CHECK(result.is_ok_and_has_value()); \
        CHECK(result.get_value().type.primitive_type().value_or(scc::Type::PrimitiveType::COUNT) == scc::Type::PrimitiveType::TYPE); \
        CHECK(result.get_value().type.pointer_depth() == PTR_DEPTH); \
    }while(0)

#define SCC_TEST_TYPE_AUTO(VALUE) \
    do \
    {  \
        auto result = SCC_APPEND_AND_CONTINUE(#VALUE ";"); \
        CHECK(result.is_ok_and_has_value()); \
        CHECK(result.get_value().type.primitive_type().value_or(scc::Type::PrimitiveType::COUNT) == scc::Type::deduce_type<decltype(VALUE)>().primitive_type().value_or(scc::Type::PrimitiveType::COUNT)); \
    }while(0)

#define SCC_TEST_IS_ERROR(VALUE) \
    do \
    {  \
        auto result = SCC_APPEND_AND_CONTINUE(VALUE); \
        CHECK(result.is_error()); \
    }while(0)

#define SCC_TEST_INTERPRET_RESULT(CTYPE, EXPECTED_VALUE, CODE) \
    do{ \
        auto result = SCC_APPEND_AND_CONTINUE(CODE); \
        CHECK(result.is_ok_and_has_value()); \
        CHECK(result.get_value().type.primitive_type().value_or(scc::Type::PrimitiveType::COUNT) == scc::Type::deduce_type<CTYPE>().primitive_type().value_or(scc::Type::PrimitiveType::COUNT)); \
        CHECK(std::get<CTYPE>(result.get_value().value.primitive_value().value() ) == EXPECTED_VALUE); \
    }while(0)

#define SCC_TEST_IS_OK(CODE) \
    do{ \
        auto result = SCC_APPEND_AND_CONTINUE(CODE); \
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
    auto running_interpreter = scc::RunningInterpreter({});



    SUBCASE("Failed to parse")
    {
        SCC_TEST_IS_ERROR("1 + ;");
        CHECK(SCC_APPEND_AND_CONTINUE("1").is_error());
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

        SCC_TEST_TYPE_PTR(I32, 1, (int*)0 + 2);
        SCC_TEST_TYPE_PTR(I32, 1, 2 + (int*)0);
        SCC_TEST_TYPE_PTR(Char, 1, (char*)2 - 1);
        SCC_TEST_TYPE_PTR(Char, 1, 2 - (char*)1);
        SCC_TEST_TYPE_PTR(F64, 1, (double*)0 + 2);
        SCC_TEST_TYPE_PTR(F64, 1, 2 + (double*)0);
        SCC_TEST_IS_ERROR("(int*)1 + (int*)1;");
        SCC_TEST_INTERPRET_RESULT(bool, true, "(int*)1 == (int*)1;");
        SCC_TEST_INTERPRET_RESULT(bool, false,"(int*)1 != (int*)1;");
        SCC_TEST_INTERPRET_RESULT(bool, true, "(int*)1 <  (int*)2;");
        SCC_TEST_INTERPRET_RESULT(bool, false,"(int*)2 <  (int*)1;");
        SCC_TEST_INTERPRET_RESULT(bool, true, "(int*)1 <= (int*)1;");

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

        // int32_t x = (int32_t)(int64_t)(int32_t*)1;  // ok
        // int32_t y = (int32_t)(int32_t*)1;           // error

        SCC_TEST_INTERPRET_RESULT(int, 1, "(int)(long)(int*)1;");
        SCC_TEST_IS_ERROR("(int)(int*)1;");

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
    std::get<scc::Type::Primitive::PTR>(INTERPRETER_RESULT.get_value().value.primitive_value().value())


TEST_CASE("Scopes")
{
    auto interpreter = scc::Interpreter();
    auto running_interpreter = scc::RunningInterpreter({});

    auto a = SCC_GET_PTR_FROM_RESULT(SCC_APPEND_AND_CONTINUE("int a;"));
    auto b = SCC_GET_PTR_FROM_RESULT(SCC_APPEND_AND_CONTINUE("int b;"));

    CHECK(a != b);
    CHECK(a == b + sizeof(int));
    CHECK(SCC_APPEND_AND_CONTINUE("int a;").is_error());
    CHECK(SCC_APPEND_AND_CONTINUE("int b;").is_error());

    SCC_TEST_IS_ERROR("int c; int c;");
    SCC_TEST_IS_ERROR("int d; int d = 1;");
    SCC_TEST_IS_ERROR("int e = 1; int e;");

    // TODOOO: hook on assert
    // auto c1 = SCC_GET_PTR_FROM_RESULT(SCC_APPEND_AND_CONTINUE("{int c;}"));
    // auto c2 = SCC_GET_PTR_FROM_RESULT(SCC_APPEND_AND_CONTINUE("{int c;}"));
    // CHECK(c1 == c2);
    // CHECK(b == c1 + sizeof(int));
}

TEST_CASE("Assignments")
{
    auto interpreter = scc::Interpreter();
    auto running_interpreter = scc::RunningInterpreter({});

    SCC_TEST_IS_ERROR("a;"); // undeclared
    SCC_TEST_IS_OK("int a;");
    // SCC_TEST_IS_ERROR("a;"); // not initialized          EDIT: global variables are initialized to 0, local variables tests are later :)
    // SCC_TEST_IS_ERROR("a = a;"); // not initialized
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
        auto running_interpreter = scc::RunningInterpreter({});

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
        auto running_interpreter = scc::RunningInterpreter({});

        SCC_TEST_IS_OK("int a = 10;");
        SCC_TEST_IS_OK("if (1) { int a = 20; }");
        SCC_TEST_INTERPRET_RESULT(int, 10, "a;");
    }
}

TEST_CASE("While and Do statements")
{
    auto interpreter = scc::Interpreter();
    auto running_interpreter = scc::RunningInterpreter({});

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
    auto running_interpreter = scc::RunningInterpreter({});

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
    SCC_TEST_INTERPRET_RESULT(int, 120, "factorial(5);");

    SCC_TEST_IS_OK("int var = 420;");
    SCC_TEST_IS_OK("int fn_e() { int var = 1; return var; }");
    SCC_TEST_INTERPRET_RESULT(int, 420, "var;");
    SCC_TEST_INTERPRET_RESULT(int, 1, "fn_e();");
    SCC_TEST_INTERPRET_RESULT(int, 420, "var;");

    SCC_TEST_IS_OK("int fn_f() { return var; }");
    SCC_TEST_INTERPRET_RESULT(int, 420, "fn_f();");

    SCC_TEST_IS_ERROR("int fn_g() { return local; int local = 1; } fn_g();");
    // SCC_TEST_IS_ERROR("void fn_h() { int x; x; } fn_h();"); // local variable not initialized
    SCC_TEST_IS_ERROR("void fn_i() { int x = x; } fn_i();"); // illegal stuff

    // TODOOOOOO: fix no return statement error..
    // SCC_TEST_IS_ERROR("int fn_j() { int x = 1; } fn_j();"); // no return statement

}
#include "interpreter_io.hpp"
TEST_CASE("stdout and stderr")
{
    auto interpreter = scc::Interpreter();
    auto running_interpreter = scc::RunningInterpreter({});

    std::stringstream ss;
    std::stringstream ss_err;
    scc::InterpreterIO::set_stdout_callback([&ss](const char* str){ ss << str; });
    scc::InterpreterIO::set_stderr_callback([&ss_err](const char* str){ ss_err << str; });

    SCC_TEST_IS_OK("void _scc_puti(int i);");
    SCC_TEST_IS_OK("void _scc_putc(char c);");

    SCC_TEST_IS_OK("_scc_puti(1);");
    SCC_TEST_IS_OK("_scc_puti(2);");

    SCC_TEST_IS_OK("_scc_putc('a');");
    SCC_TEST_IS_OK("_scc_putc('b');");
    SCC_TEST_IS_OK("_scc_putc('\\n');");

    CHECK(ss.str() == "1\n2\nab\n");
    CHECK(ss_err.str() == "");

    SCC_TEST_IS_OK("int a = 34;");
    SCC_TEST_IS_OK("_scc_puti(a);");
    SCC_TEST_IS_OK("a += 35;");
    SCC_TEST_IS_OK("_scc_puti(a);");

    CHECK(ss.str() == "1\n2\nab\n34\n69\n");
    CHECK(ss_err.str() == "");
}

TEST_CASE("Global variables")
{
    auto interpreter = scc::Interpreter();
    auto running_interpreter = scc::RunningInterpreter({});

    std::stringstream ss;
    std::stringstream ss_err;
    scc::InterpreterIO::set_stdout_callback([&ss](const char* str){ ss << str; });
    scc::InterpreterIO::set_stderr_callback([&ss_err](const char* str){ ss_err << str; });


    SCC_TEST_IS_OK(R"(
        void _scc_puti(int i);
        int global = 123;
        int main()
        {
            int a = 1;
            _scc_puti(a);
            _scc_puti(global);
            return 0;
        }
    )");

    CHECK(ss.str() == "1\n123\n");
}

TEST_CASE("For statements")
{
    auto interpreter = scc::Interpreter();
    auto running_interpreter = scc::RunningInterpreter({});

    std::stringstream ss;
    std::stringstream ss_err;
    scc::InterpreterIO::set_stdout_callback([&ss](const char* str){ ss << str; });
    scc::InterpreterIO::set_stderr_callback([&ss_err](const char* str){ ss_err << str; });

    SCC_TEST_IS_OK("void _scc_puti(int i);");
    SCC_TEST_IS_OK("void print_num(int i) { _scc_puti(i);}");

    SCC_TEST_IS_OK("for (int i = 0; i < 10; i += 1) { print_num(i); }");
    CHECK(ss.str() == "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n");
    CHECK(ss_err.str() == "");

    SCC_TEST_IS_OK("for (;;) {print_num(69); break;}");
    CHECK(ss.str() == "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n69\n");
    CHECK(ss_err.str() == "");

    ss.str("");
    ss_err.str("");
    SCC_TEST_IS_OK("for (int i = 0; i < 10; i += 1) { if (i == 5) { continue; } print_num(i); }");
    CHECK(ss.str() == "0\n1\n2\n3\n4\n6\n7\n8\n9\n");
    CHECK(ss_err.str() == "");

    ss.str("");
    ss_err.str("");
    SCC_TEST_IS_OK("int x = 0;");
    SCC_TEST_IS_OK("for (x = 5; x < 10; x += 2) { print_num(x); }");
    CHECK(ss.str() == "5\n7\n9\n");
    CHECK(ss_err.str() == "");

    ss.str("");
    ss_err.str("");

    SCC_TEST_IS_OK("for (int i = 0; i < 10; i += 1) { if (i == 5 || i == 6) { int i = 90; continue; } print_num(i); }");
    CHECK(ss.str() == "0\n1\n2\n3\n4\n7\n8\n9\n");
    CHECK(ss_err.str() == "");

    ss.str("");
    ss_err.str("");

    SCC_TEST_IS_OK("for (int i = 0; i < 4; i += 1) { for (int j = 0; j < 4; j += 1) { print_num(i * 4 + j); } }");
    CHECK(ss.str() == "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n");


}

TEST_CASE("Breakpoints")
{
    auto interpreter = scc::Interpreter();
    auto running_interpreter = scc::RunningInterpreter({});

    std::stringstream ss;
    std::stringstream ss_err;
    scc::InterpreterIO::set_stdout_callback([&ss](const char* str){ ss << str; });
    scc::InterpreterIO::set_stderr_callback([&ss_err](const char* str){ ss_err << str; });

    std::string code = R"(
        void _scc_puti(int i);      // 1
        void print(int a, int b) {  // 2
            _scc_puti(a);           // 3
            _scc_puti(b);           // 4
        }                           // 5
        int a = 34;                 // 6
        int b = 0;                  // 7
        print(a, b);                // 8
        a += 35;                    // 9
        print(a, b);                // 10
        b = a;                      // 11
        print(a, b);                // 12
    )";

    SUBCASE("Without breakpoints")
    {
        SCC_TEST_IS_OK(code);
        CHECK(ss.str() == "34\n0\n69\n0\n69\n69\n");
        CHECK(ss_err.str() == "");
    }

    SUBCASE("With breakpoints")
    {
        running_interpreter.breakpoints().add(8);
        running_interpreter.breakpoints().add(10);
        running_interpreter.breakpoints().add(12);

        SCC_TEST_IS_ERROR(code); // breakpoint reached error
        CHECK(ss.str() == "");
        CHECK(ss_err.str() == "");

        auto result = running_interpreter.continue_execution();
        CHECK(result.is_error());
        CHECK(result.get_error() == scc::InterpreterError::BreakpointReachedError);
        CHECK(ss.str() == "34\n0\n");
        CHECK(ss_err.str() == "");

        result = running_interpreter.continue_execution();
        CHECK(result.is_error());
        CHECK(result.get_error() == scc::InterpreterError::BreakpointReachedError);
        CHECK(ss.str() == "34\n0\n69\n0\n");
        CHECK(ss_err.str() == "");

        result = running_interpreter.continue_execution();
        CHECK(result.is_ok());
        CHECK(ss.str() == "34\n0\n69\n0\n69\n69\n");
        CHECK(ss_err.str() == "");
    }

    SUBCASE("Stepping")
    {
        running_interpreter.breakpoints().add(3);
        SCC_TEST_IS_ERROR(code); // breakpoint reached error
        CHECK(ss.str() == "");
        CHECK(ss_err.str() == "");

        auto result = running_interpreter.next();
        CHECK(result.is_ok());
        CHECK(ss.str() == "34\n");
        CHECK(ss_err.str() == "");

        result = running_interpreter.continue_execution();
        REQUIRE(result.is_error());
        CHECK(result.get_error() == scc::InterpreterError::BreakpointReachedError);
        CHECK(ss.str() == "34\n0\n"); // it breaks again in the function
        CHECK(ss_err.str() == "");
    }
}

TEST_CASE("Assertions")
{
    auto interpreter = scc::Interpreter();
    auto running_interpreter = scc::RunningInterpreter({});

    SCC_TEST_IS_OK("void _scc_assert(bool b);");
    SCC_TEST_IS_OK("_scc_assert(1);");
    SCC_TEST_IS_ERROR("_scc_assert(0);");
}

TEST_CASE("Pointers")
{
    auto interpreter = scc::Interpreter();
    auto running_interpreter = scc::RunningInterpreter({});

    std::stringstream ss;
    std::stringstream ss_err;
    scc::InterpreterIO::set_stdout_callback([&ss](const char* str){ ss << str; });
    scc::InterpreterIO::set_stderr_callback([&ss_err](const char* str){ ss_err << str; });

    SCC_TEST_IS_OK("void _scc_assert(bool b);");

    SCC_TEST_IS_OK("int* a_ptr;");
    SCC_TEST_IS_OK("int* b_ptr;");
    SCC_TEST_IS_OK("int a_val = 1;");
    SCC_TEST_IS_OK("int b_val = 2;");
    SCC_TEST_IS_OK("a_ptr = &a_val;");
    SCC_TEST_IS_OK("b_ptr = &b_val;");
    SCC_TEST_IS_OK("_scc_assert(a_ptr == &a_val);");
    SCC_TEST_IS_OK("_scc_assert(b_ptr == &b_val);");
    SCC_TEST_IS_OK("_scc_assert(a_ptr != b_ptr);");
    SCC_TEST_IS_OK("_scc_assert(*a_ptr == 1);");
    SCC_TEST_IS_OK("_scc_assert(*b_ptr == 2);");
    SCC_TEST_IS_OK("_scc_assert(*(&a_val) == 1);");
    SCC_TEST_IS_ERROR("*(int*)0;"); // invalid memory access
    SCC_TEST_IS_OK("*a_ptr = 3;");
    SCC_TEST_IS_OK("_scc_assert(*a_ptr == 3);");
    SCC_TEST_IS_OK("_scc_assert(a_val == 3);");
    SCC_TEST_IS_OK("_scc_assert(*(&a_val) == 3);");
    SCC_TEST_IS_OK("_scc_assert(*b_ptr == 2);");
    SCC_TEST_IS_OK("_scc_assert(b_val == 2);");
    SCC_TEST_IS_OK("*(&b_val) = 4;");
    SCC_TEST_IS_OK("_scc_assert(*b_ptr == 4);");
    SCC_TEST_IS_OK("_scc_assert(b_val == 4);");
    SCC_TEST_IS_OK("_scc_assert(*(&b_val) == 4);");

    SCC_TEST_IS_OK("int* c_ptr = a_ptr;");
    SCC_TEST_IS_OK("_scc_assert(c_ptr == a_ptr);");
    SCC_TEST_IS_OK("_scc_assert(c_ptr == &a_val);");
    SCC_TEST_IS_OK("_scc_assert(*c_ptr == 3);");
    SCC_TEST_IS_OK("_scc_assert(*c_ptr == a_val);");
    SCC_TEST_IS_OK("_scc_assert(*c_ptr == *a_ptr);");

    SCC_TEST_IS_OK("float* d_ptr;");
    SCC_TEST_IS_OK("float d_val = 1.5f;");
    SCC_TEST_IS_OK("d_ptr = &d_val;");
    SCC_TEST_IS_OK("_scc_assert(d_ptr == &d_val);");
    SCC_TEST_IS_OK("_scc_assert(*d_ptr == 1.5f);");

    SCC_TEST_IS_OK("long e_val = 123456;");
    SCC_TEST_IS_OK("long* e_ptr = &e_val;");
    SCC_TEST_IS_OK("_scc_assert(e_ptr == &e_val);");
    SCC_TEST_IS_OK("_scc_assert(*e_ptr == 123456);");

    SCC_TEST_IS_OK("long long_value = 123456;");
    SCC_TEST_IS_OK("long* long_ptr = &long_value;");
    SCC_TEST_IS_OK("_scc_assert(long_ptr == &long_value);");
    SCC_TEST_IS_OK("_scc_assert(*long_ptr == 123456);");
    SCC_TEST_IS_OK("*long_ptr += 1;");
    SCC_TEST_IS_OK("_scc_assert(*long_ptr == 123457);");
    SCC_TEST_IS_OK("_scc_assert(long_value == 123457);");
    SCC_TEST_IS_OK("*long_ptr -= 1;");
    SCC_TEST_IS_OK("_scc_assert(*long_ptr == 123456);");
    SCC_TEST_IS_OK("*long_ptr *= 2;");
    SCC_TEST_IS_OK("_scc_assert(*long_ptr == 246912);");
    SCC_TEST_IS_OK("_scc_assert(long_value == 246912);");
    SCC_TEST_IS_OK("*long_ptr /= 2;");
    SCC_TEST_IS_OK("_scc_assert(*long_ptr == 123456);");
    SCC_TEST_IS_OK("_scc_assert(long_value == 123456);");
    SCC_TEST_IS_OK("*long_ptr %= 2;");
    SCC_TEST_IS_OK("_scc_assert(*long_ptr == 0);");
    SCC_TEST_IS_OK("_scc_assert(long_value == 0);");

    SCC_TEST_IS_OK("float float_value = 1.5f;");
    SCC_TEST_IS_OK("float* float_ptr = &float_value;");
    SCC_TEST_IS_OK("_scc_assert(float_ptr == &float_value);");
    SCC_TEST_IS_OK("_scc_assert(*float_ptr == 1.5f);");
    SCC_TEST_IS_OK("*float_ptr += 1.5f;");
    SCC_TEST_IS_OK("_scc_assert(*float_ptr == 3.0f);");
    SCC_TEST_IS_OK("_scc_assert(float_value == 3.0f);");
    SCC_TEST_IS_OK("*float_ptr -= 1.5f;");
    SCC_TEST_IS_OK("_scc_assert(*float_ptr == 1.5f);");
    SCC_TEST_IS_OK("*float_ptr *= 2.0f;");
    SCC_TEST_IS_OK("_scc_assert(*float_ptr == 1.5f * 2.0f);");
    SCC_TEST_IS_OK("_scc_assert(float_value == 1.5f * 2.0f);");


    SCC_TEST_IS_OK("long long_value1 = 1;");
    SCC_TEST_IS_OK("long long_value2 = 2;");
    SCC_TEST_IS_OK("long* long_ptr1 = &long_value1;");
    SCC_TEST_IS_OK("long* long_ptr2 = &long_value2;");
    SCC_TEST_IS_OK("long** long_ptr_ptr = &long_ptr1;");
    SCC_TEST_IS_OK("_scc_assert(long_ptr_ptr == &long_ptr1);");
    SCC_TEST_IS_OK("_scc_assert(*long_ptr_ptr == long_ptr1);");
    SCC_TEST_IS_OK("_scc_assert(**long_ptr_ptr == 1);");
    SCC_TEST_IS_OK("_scc_assert(*long_ptr_ptr == &long_value1);");
    SCC_TEST_IS_OK("long_ptr_ptr = &long_ptr2;");
    SCC_TEST_IS_OK("_scc_assert(long_ptr_ptr == &long_ptr2);");
    SCC_TEST_IS_OK("_scc_assert(*long_ptr_ptr == long_ptr2);");
    SCC_TEST_IS_OK("_scc_assert(**long_ptr_ptr == 2);");
    SCC_TEST_IS_OK("**long_ptr_ptr = 3;");
    SCC_TEST_IS_OK("_scc_assert(**long_ptr_ptr == 3);");
    SCC_TEST_IS_OK("_scc_assert(long_value2 == 3);");


    SUBCASE("Pointer Arithmetic")
    {
        SCC_TEST_IS_OK("_scc_assert((int*)0 + 1 == 4);");
        SCC_TEST_IS_OK("_scc_assert((long*)0 + 1 == 8);");
        SCC_TEST_IS_OK("_scc_assert((double*)0 + 1 == 8);");
        SCC_TEST_IS_OK("_scc_assert(1 + (int*)0 == 4);");
        SCC_TEST_IS_OK("_scc_assert(1 + (long*)0 == 8);");
        SCC_TEST_IS_OK("_scc_assert(1 + (double*)0 == 8);");
        SCC_TEST_IS_OK("_scc_assert((int*)8 - 1 == 4);");
        SCC_TEST_IS_OK("_scc_assert((long*)16 - 1 == 8);");
        SCC_TEST_IS_OK("_scc_assert((double*)16 - 1 == 8);");
        SCC_TEST_IS_OK("_scc_assert(1 + (short*)8 == 10);");
    }

}

TEST_CASE("Include libc"){
    auto interpreter = scc::Interpreter();
    auto running_interpreter = scc::RunningInterpreter({});
    std::stringstream ss;
    std::stringstream ss_err;
    scc::InterpreterIO::set_stdout_callback([&ss](const char* str){ ss << str; });
    scc::InterpreterIO::set_stderr_callback([&ss_err](const char* str){ ss_err << str; });


    SCC_TEST_IS_OK("#include <math.h>\n");
    SCC_TEST_IS_OK("M_PI;");

    SCC_TEST_IS_OK("#include <stdlib.h>\n");
    SCC_TEST_IS_OK("#include <unistd.h>\n");

    SCC_TEST_IS_OK("char* data = malloc(4);");
    SCC_TEST_IS_OK("*(data + 0) = 'l';");
    SCC_TEST_IS_OK("*(data + 1) = 'o';");
    SCC_TEST_IS_OK("*(data + 2) = 'l';");
    SCC_TEST_IS_OK("*(data + 3) = '\\n';");
    SCC_TEST_IS_OK("write(1, data, 4);");
    SCC_TEST_IS_OK("free(data);");

}


TEST_CASE("Define") {
    auto interpreter = scc::Interpreter();
    auto running_interpreter = scc::RunningInterpreter({});

    SCC_TEST_IS_OK("void _scc_assert(bool b);");

    SCC_TEST_IS_OK("#define A 1\n");
    SCC_TEST_IS_OK("int b = A;");
    SCC_TEST_IS_OK("_scc_assert(b == 1);");

    SCC_TEST_IS_OK("#define TRIPLE_A A+A+A\n");
    SCC_TEST_IS_OK("_scc_assert(TRIPLE_A == 3);");

    SCC_TEST_IS_OK("#define POW3_A TRIPLE_A*TRIPLE_A*TRIPLE_A\n");
    SCC_TEST_IS_OK("_scc_assert(POW3_A == 27);");
}

TEST_CASE("sizeof") {
    auto interpreter = scc::Interpreter();
    auto running_interpreter = scc::RunningInterpreter({});

    SCC_TEST_IS_OK("void _scc_assert(bool b);");
    using SizeOfType = scc::Type::Primitive::U64;

    SCC_TEST_INTERPRET_RESULT(SizeOfType, 4, "sizeof(int);");

    SCC_TEST_IS_OK("_scc_assert(sizeof(int) == 4);");
    SCC_TEST_IS_OK("_scc_assert(sizeof(long) == 8);");
    SCC_TEST_IS_OK("_scc_assert(sizeof(float) == 4);");
    SCC_TEST_IS_OK("_scc_assert(sizeof(double) == 8);");
    SCC_TEST_IS_OK("_scc_assert(sizeof(short) == 2);");
    SCC_TEST_IS_OK("_scc_assert(sizeof(char) == 1);");
    SCC_TEST_IS_OK("_scc_assert(sizeof(bool) == 1);");
    SCC_TEST_IS_OK("_scc_assert(sizeof(34+35) == 4);");
    SCC_TEST_IS_OK("_scc_assert(sizeof(69.69f) == 4);");
    SCC_TEST_IS_OK("_scc_assert(sizeof(420.0) == 8);");
    SCC_TEST_IS_OK("_scc_assert(sizeof(123ll) == 8);");
    SCC_TEST_IS_OK("_scc_assert(sizeof(123456l) == 8);");

    SCC_TEST_IS_OK("int a_int;");
    SCC_TEST_IS_OK("_scc_assert(sizeof(a_int) == 4);");
    SCC_TEST_IS_OK("long a_long;");
    SCC_TEST_IS_OK("_scc_assert(sizeof(a_long) == 8);");

    SCC_TEST_IS_OK("#define A 1l\n");
    SCC_TEST_IS_OK("_scc_assert(sizeof(A) == 8);");

    // SCC_TEST_IS_OK("_scc_assert(sizeof(int*) == 8);"); TODOO: sizeof pointer
}

TEST_CASE("Subscript expression")
{
    auto interpreter = scc::Interpreter();
    auto running_interpreter = scc::RunningInterpreter({});

    SCC_TEST_IS_OK("void _scc_assert(bool b);");

    SCC_TEST_IS_OK("#include <stdlib.h>\n");

    SCC_TEST_IS_OK("char* data = malloc(4);");
    SCC_TEST_IS_OK("*(data + 0) = 'a';");
    SCC_TEST_IS_OK("_scc_assert(data[0] == 'a');");
    SCC_TEST_IS_OK("data[1] = 'b';");
    SCC_TEST_IS_OK("_scc_assert(data[1] == 'b');");
    SCC_TEST_IS_OK("_scc_assert(*(data + 1) == 'b');");

    SCC_TEST_IS_OK("_scc_assert(1[data] == 'b');");
    SCC_TEST_IS_OK("_scc_assert(0[data] == 'a');");

    SCC_TEST_IS_OK("data[0] += 1;");
    SCC_TEST_IS_OK("_scc_assert(data[0] == 'b');");
    SCC_TEST_IS_OK("_scc_assert(*data == 'b');");
    SCC_TEST_IS_OK("_scc_assert(data[1] == 'b');");
}

TEST_CASE("Update expression")
{
    auto interpreter = scc::Interpreter();
    auto running_interpreter = scc::RunningInterpreter({});

    SCC_TEST_IS_OK("void _scc_assert(bool b);");

    SCC_TEST_IS_OK("#include <stdlib.h>\n");

    SCC_TEST_IS_OK("int a = 1;");
    SCC_TEST_IS_OK("_scc_assert(a == 1);");
    SCC_TEST_IS_OK("a++;");
    SCC_TEST_IS_OK("_scc_assert(a == 2);");
    SCC_TEST_IS_OK("a--;");
    SCC_TEST_IS_OK("_scc_assert(a == 1);");
    SCC_TEST_IS_OK("++a;");
    SCC_TEST_IS_OK("_scc_assert(a == 2);");
    SCC_TEST_IS_OK("--a;");
    SCC_TEST_IS_OK("_scc_assert(a == 1);");

    SCC_TEST_IS_OK("_scc_assert(a++ == 1);");
    SCC_TEST_IS_OK("_scc_assert(a == 2);");
    SCC_TEST_IS_OK("_scc_assert(a-- == 2);");
    SCC_TEST_IS_OK("_scc_assert(a == 1);");
    SCC_TEST_IS_OK("_scc_assert(++a == 2);");
    SCC_TEST_IS_OK("_scc_assert(a == 2);");
    SCC_TEST_IS_OK("_scc_assert(--a == 1);");
    SCC_TEST_IS_OK("_scc_assert(a == 1);");

    SCC_TEST_IS_OK("int* p = &a;");
    SCC_TEST_IS_OK("_scc_assert(*p == 1);");
    SCC_TEST_IS_OK("_scc_assert(++*p == 2);");
    SCC_TEST_IS_OK("_scc_assert(a == 2);");
    SCC_TEST_IS_OK("_scc_assert(--*p == 1);");
    SCC_TEST_IS_OK("_scc_assert(a == 1);");

    SCC_TEST_IS_OK("_scc_assert(p[0] == 1);");
    SCC_TEST_IS_OK("_scc_assert(++p[0] == 2);");
    SCC_TEST_IS_OK("_scc_assert(a == 2);");
    SCC_TEST_IS_OK("_scc_assert(--p[0] == 1);");
    SCC_TEST_IS_OK("_scc_assert(a == 1);");
    SCC_TEST_IS_OK("_scc_assert(p[0]++ == 1);");
    SCC_TEST_IS_OK("_scc_assert(a == 2);");
    SCC_TEST_IS_OK("_scc_assert(p[0]-- == 2);");
    SCC_TEST_IS_OK("_scc_assert(a == 1);");
}

TEST_CASE("Static arrays")
{
    auto interpreter = scc::Interpreter();
    auto running_interpreter = scc::RunningInterpreter({});

    SCC_TEST_IS_OK("void _scc_assert(bool b);");

    SCC_TEST_IS_OK("int a[4];");
    SCC_TEST_IS_OK("_scc_assert(sizeof(a) == 4 * sizeof(int));");
    SCC_TEST_IS_OK("_scc_assert(sizeof(a) == 16);");

    for (int i = 0; i < 4; i++)
    {
        // its global so its initialized to 0
        SCC_TEST_IS_OK("_scc_assert(a[" + std::to_string(i) + "] == 0);");
    }

    SCC_TEST_IS_OK("a[3] = 1;");
    SCC_TEST_IS_OK("_scc_assert(a[3] == 1);");
    SCC_TEST_IS_OK("_scc_assert(a[2] == 0);");

    SCC_TEST_IS_OK("int b[4] = {1, 2, 3, 4};");

    for (int i = 0; i < 4; i++)
    {
        SCC_TEST_IS_OK("_scc_assert(b[" + std::to_string(i) + "] == " + std::to_string(i + 1) + ");");
    }

    SCC_TEST_IS_OK("int c[10] = {1, 2};");

    for (int i = 0; i < 10; i++)
    {
        int expected_value = i < 2 ? i + 1 : 0;
        SCC_TEST_IS_OK("_scc_assert(c[" + std::to_string(i) + "] == " + std::to_string(expected_value) + ");");
    }

    SCC_TEST_IS_OK("int d[] = {1, 2, 3, 4, 5};");
    SCC_TEST_IS_OK("_scc_assert(sizeof(d) == 5 * sizeof(int));");
    SCC_TEST_IS_OK("_scc_assert(sizeof(d) == 20);");

    for (int i = 0; i < 5; i++)
    {
        SCC_TEST_IS_OK("_scc_assert(d[" + std::to_string(i) + "] == " + std::to_string(i + 1) + ");");
    }

    SCC_TEST_IS_OK(R"(
        {
            // local static array
            int e[4] = {0};
            _scc_assert(sizeof(e) == 4 * sizeof(int));
            _scc_assert(sizeof(e) == 16);
            for (int i = 0; i < 4; i++)
            {
                _scc_assert(e[i] == 0);
            }
        }
    )");

}

TEST_CASE("Strings")
{
    auto interpreter = scc::Interpreter();
    auto running_interpreter = scc::RunningInterpreter({});

    SCC_TEST_IS_OK("void _scc_assert(bool b);");

    SUBCASE("Stack strings")
    {
        SCC_TEST_IS_OK(R"(char a[] = "abcdefg";)");
        SCC_TEST_IS_OK("_scc_assert(sizeof(a) == 8);");
        for (int i = 0; i < 7; i++)
        {
            SCC_TEST_IS_OK("_scc_assert(a[" + std::to_string(i) + "] == '" + std::string(1, 'a' + i) + "');");
        }
        SCC_TEST_IS_OK("_scc_assert(a[7] == '\\0');");

        SCC_TEST_IS_OK(R"(char b[10] = "abcdefg";)");
        SCC_TEST_IS_OK("_scc_assert(sizeof(b) == 10);");
        for (int i = 0; i < 7; i++)
        {
            SCC_TEST_IS_OK("_scc_assert(b[" + std::to_string(i) + "] == '" + std::string(1, 'a' + i) + "');");
        }
        SCC_TEST_IS_OK("_scc_assert(b[7] == '\\0');");
        SCC_TEST_IS_OK("_scc_assert(b[8] == '\\0');");
        SCC_TEST_IS_OK("_scc_assert(b[9] == '\\0');");

        SCC_TEST_IS_ERROR(R"(char c[3] = "abcdefg";)");
    }
}

TEST_CASE("Unary expressions")
{
    auto interpreter = scc::Interpreter();
    auto running_interpreter = scc::RunningInterpreter({});

    SCC_TEST_IS_OK("void _scc_assert(bool b);");
    SCC_TEST_IS_OK("_scc_assert(!0);");
    SCC_TEST_IS_OK("_scc_assert(!1 == 0);");
    SCC_TEST_IS_OK("_scc_assert(!2 == 0);");
    SCC_TEST_IS_OK("_scc_assert(!!0 == 0);");
    SCC_TEST_IS_OK("_scc_assert(!!1);");
    SCC_TEST_IS_OK("_scc_assert(!1LL == 0);");
    SCC_TEST_IS_OK("_scc_assert(!1.0f == 0);");
    SCC_TEST_IS_OK("_scc_assert(!1.0 == 0);");
    SCC_TEST_IS_OK("_scc_assert(!'a' == 0);");
    SCC_TEST_IS_OK("_scc_assert(!'\\0');");

    SCC_TEST_IS_OK("_scc_assert(~0 == -1);");
    SCC_TEST_IS_OK("_scc_assert(~1 == -2);");
    SCC_TEST_IS_OK("_scc_assert(~2 == -3);");
    SCC_TEST_IS_OK("_scc_assert(~-1 == 0);");
    SCC_TEST_IS_OK("_scc_assert(~(unsigned int)0 == (unsigned int)0xFFFFFFFF);");
    SCC_TEST_IS_OK("_scc_assert(~(unsigned int)1 == (unsigned int)0xFFFFFFFE);");

    SCC_TEST_IS_ERROR("~1.0f);");
    SCC_TEST_IS_ERROR("~1.0);");
    SCC_TEST_IS_ERROR("~(int*)0);");
}

TEST_CASE("Read-only string literals")
{
    auto interpreter = scc::Interpreter();
    auto running_interpreter = scc::RunningInterpreter({});

    SCC_TEST_IS_OK("void _scc_assert(bool b);");

    SCC_TEST_IS_OK("_scc_assert(\"abc\"[0] == 'a');");
    SCC_TEST_IS_OK("_scc_assert(\"abc\"[1] == 'b');");
    SCC_TEST_IS_OK("_scc_assert(\"abc\"[2] == 'c');");
    SCC_TEST_IS_OK("_scc_assert(\"abc\"[3] == '\\0');");
    SCC_TEST_IS_OK("_scc_assert(\"abc\"[4] == '\\0');");

    SCC_TEST_IS_OK("_scc_assert(\"abc\" == \"abc\");");
    SCC_TEST_IS_OK("_scc_assert(\"abc\" != \"def\");");
    SCC_TEST_IS_OK("_scc_assert(sizeof(\"abc\") == 4);");

    SCC_TEST_IS_ERROR("*\"abc\" = 'd';");
}

TEST_CASE("printf")
{
    auto interpreter = scc::Interpreter();
    auto running_interpreter = scc::RunningInterpreter({});

    std::stringstream ss;
    std::stringstream ss_err;

    scc::InterpreterIO::set_stdout_callback([&ss](const char* str){ ss << str; });
    scc::InterpreterIO::set_stderr_callback([&ss_err](const char* str){ ss_err << str; });

    SCC_TEST_IS_ERROR(R"(printf("no args\n");)");

    SCC_TEST_IS_OK("#include <stdio.h>\n");

    SCC_TEST_IS_ERROR(R"(printf("no args %d \n");)");
    SCC_TEST_IS_ERROR(R"(printf("not enough args %d %d \n", 1);)");
    SCC_TEST_IS_ERROR(R"(printf("too many args %d \n", 1, 2);)");

    ss.str("");

    SCC_TEST_IS_OK(R"(printf("no args\n");)");
    CHECK(ss.str() == "no args\n");
    ss.str("");

    SCC_TEST_IS_OK(R"(printf("one arg %d\n", 1);)");
    CHECK(ss.str() == "one arg 1\n");
    ss.str("");

    SCC_TEST_IS_OK(R"(printf("two args %d %d\n", 1, 2);)");
    CHECK(ss.str() == "two args 1 2\n");
    ss.str("");

    SCC_TEST_IS_OK(R"(printf("chars %c %c %c %c\n", 'a', 69, 'c', 'd');)");
    CHECK(ss.str() == "chars a E c d\n");
    ss.str("");

    SCC_TEST_IS_OK(R"(printf("float %f\n", 1.5f);)");
    CHECK(ss.str() == "float 1.5\n");
    ss.str("");


    SCC_TEST_IS_OK(R"(printf("string %s\n", "abc");)");
    CHECK(ss.str() == "string abc\n");
    ss.str("");

    SCC_TEST_IS_OK(R"(printf("string %s %s\n", "abc", "def");)");
    CHECK(ss.str() == "string abc def\n");
    ss.str("");

    SCC_TEST_IS_OK(R"(char stack_str[] = "stack string";)");
    SCC_TEST_IS_OK(R"(printf("stack string %s\n", stack_str);)");
    CHECK(ss.str() == "stack string stack string\n");
    ss.str("");

    SCC_TEST_IS_OK(R"(printf("%%\n");)");
    CHECK(ss.str() == "%\n");

}

TEST_CASE("Structures")
{
    auto interpreter = scc::Interpreter();
    auto running_interpreter = scc::RunningInterpreter({});

    SCC_TEST_IS_OK("void _scc_assert(bool b);");

    SCC_TEST_IS_OK("typedef struct { int a; float b; char c; } S;");
    SCC_TEST_IS_OK("S s;");
    SCC_TEST_IS_OK("s.a = 1;");
    SCC_TEST_IS_OK("s.b = 4.20f;");
    SCC_TEST_IS_OK("s.c = 'E';");
    SCC_TEST_IS_OK("_scc_assert(s.a == 1);");
    SCC_TEST_IS_OK("_scc_assert(s.b == 4.20f);");
    SCC_TEST_IS_OK("_scc_assert(s.c == 'E');");
    SCC_TEST_IS_OK("s.a += 1;");
    SCC_TEST_IS_OK("_scc_assert(s.a == 2);");
    SCC_TEST_IS_OK("s.a -= 1;");
    SCC_TEST_IS_OK("_scc_assert(s.a == 1);");
    SCC_TEST_IS_OK("S* s_ptr = &s;");
    SCC_TEST_IS_OK("_scc_assert(s_ptr->a == 1);");
    SCC_TEST_IS_OK("_scc_assert(s_ptr->b == 4.20f);");
    SCC_TEST_IS_OK("_scc_assert(s_ptr->c == 'E');");
    SCC_TEST_IS_OK("s_ptr->a += 1;");
    SCC_TEST_IS_OK("_scc_assert(s_ptr->a == 2);");
    SCC_TEST_IS_OK("s_ptr->a -= 1;");
    SCC_TEST_IS_OK("_scc_assert(s_ptr->a == 1);");
    SCC_TEST_IS_OK("s_ptr->a = 2;");
    SCC_TEST_IS_OK("s_ptr->b = 3.14f;");
    SCC_TEST_IS_OK("s_ptr->c = 'F';");
    SCC_TEST_IS_OK("_scc_assert(s.a == 2);");
    SCC_TEST_IS_OK("_scc_assert(s.b == 3.14f);");
    SCC_TEST_IS_OK("_scc_assert(s.c == 'F');");
    SCC_TEST_IS_OK("S s2 = {1.11, 2.22f, 'C'};");
    SCC_TEST_IS_OK("_scc_assert(s2.a == 1);");
    SCC_TEST_IS_OK("_scc_assert(s2.b == 2.22f);");
    SCC_TEST_IS_OK("_scc_assert(s2.c == 'C');");

    SCC_TEST_IS_OK("S s3 = s2;");
    SCC_TEST_IS_OK("_scc_assert(s3.a == 1);");
    SCC_TEST_IS_OK("_scc_assert(s3.b == 2.22f);");
    SCC_TEST_IS_OK("_scc_assert(s3.c == 'C');");
    SCC_TEST_IS_OK("s3.a = 3;");
    SCC_TEST_IS_OK("_scc_assert(s3.a == 3);");
    SCC_TEST_IS_OK("_scc_assert(s2.a == 1);");

    SCC_TEST_IS_OK("S s4 = {.c = 'D', .a = 4, .b = 5.55f};");
    SCC_TEST_IS_OK("_scc_assert(s4.a == 4);");
    SCC_TEST_IS_OK("_scc_assert(s4.b == 5.55f);");
    SCC_TEST_IS_OK("_scc_assert(s4.c == 'D');");

    SCC_TEST_IS_OK("S s5 = {.a = 1};");
    SCC_TEST_IS_OK("_scc_assert(s5.a == 1);");
    SCC_TEST_IS_OK("_scc_assert(s5.b == 0.0f);");
    SCC_TEST_IS_OK("_scc_assert(s5.c == '\\0');");

    SCC_TEST_IS_OK("S s6 = {1};");
    SCC_TEST_IS_OK("_scc_assert(s6.a == 1);");
    SCC_TEST_IS_OK("_scc_assert(s6.b == 0.0f);");
    SCC_TEST_IS_OK("_scc_assert(s6.c == '\\0');");

    SCC_TEST_IS_OK("S s7 = {1, 2.2f};");
    SCC_TEST_IS_OK("_scc_assert(s7.a == 1);");
    SCC_TEST_IS_OK("_scc_assert(s7.b == 2.2f);");

    SCC_TEST_IS_ERROR("S s8 = {1, 2.2f, 'C', 4};");

    SCC_TEST_IS_OK("typedef struct { int a; float b; char c; } InnerS;");
    SCC_TEST_IS_OK("typedef struct { InnerS inner; InnerS* inner_ptr; } OuterS;");

    SCC_TEST_IS_OK("InnerS inner_target = {1, 2.2f, 'C'};");
    SCC_TEST_IS_OK("OuterS outer_target = {{420, 69.69f, 'E'}, 0};");
    SCC_TEST_IS_OK("outer_target.inner_ptr = &inner_target;");
    SCC_TEST_IS_OK("_scc_assert(outer_target.inner.a == 420);");
    SCC_TEST_IS_OK("_scc_assert(outer_target.inner.b == 69.69f);");
    SCC_TEST_IS_OK("_scc_assert(outer_target.inner.c == 'E');");
    SCC_TEST_IS_OK("_scc_assert(outer_target.inner_ptr->a == 1);");
    SCC_TEST_IS_OK("_scc_assert(outer_target.inner_ptr->b == 2.2f);");
    SCC_TEST_IS_OK("_scc_assert(outer_target.inner_ptr->c == 'C');");

    SCC_TEST_IS_OK("OuterS* outer_target_ptr = &outer_target;");
    SCC_TEST_IS_OK("_scc_assert(outer_target_ptr->inner.a == 420);");
    SCC_TEST_IS_OK("_scc_assert(outer_target_ptr->inner.b == 69.69f);");
    SCC_TEST_IS_OK("_scc_assert(outer_target_ptr->inner.c == 'E');");
    SCC_TEST_IS_OK("_scc_assert(outer_target_ptr->inner_ptr->a == 1);");
    SCC_TEST_IS_OK("_scc_assert(outer_target_ptr->inner_ptr->b == 2.2f);");
    SCC_TEST_IS_OK("_scc_assert(outer_target_ptr->inner_ptr->c == 'C');");

    SCC_TEST_IS_OK("OuterS outer_designated = {.inner = {.a = 420, .b = 69.69f, .c = 'E'}, .inner_ptr = 123};");
    SCC_TEST_IS_OK("_scc_assert(outer_designated.inner.a == 420);");
    SCC_TEST_IS_OK("_scc_assert(outer_designated.inner.b == 69.69f);");
    SCC_TEST_IS_OK("_scc_assert(outer_designated.inner.c == 'E');");
    SCC_TEST_IS_OK("_scc_assert(outer_designated.inner_ptr == (InnerS*)123);");

    SCC_TEST_IS_OK("typedef struct { int data;} FromFnS;");
    SCC_TEST_IS_OK("FromFnS from_fn(int data) { FromFnS s; s.data = data; return s; }");
    SCC_TEST_IS_OK("FromFnS from_fn_result = from_fn(420);");
    SCC_TEST_IS_OK("_scc_assert(from_fn_result.data == 420);");

    SCC_TEST_IS_OK("typedef struct { int data;} ToFnS;");
    SCC_TEST_IS_OK("int to_fn(ToFnS s) { return s.data; }");
    SCC_TEST_IS_OK("ToFnS to_fn_arg = {420};");
    SCC_TEST_IS_OK("_scc_assert(to_fn(to_fn_arg) == 420);");

    SCC_TEST_IS_OK("typedef struct { int data;} ToFnPtrS;");
    SCC_TEST_IS_OK("int to_fn_ptr(ToFnPtrS* s) { return s->data; }");
    SCC_TEST_IS_OK("ToFnPtrS to_fn_ptr_arg = {420};");
    SCC_TEST_IS_OK("_scc_assert(to_fn_ptr(&to_fn_ptr_arg) == 420);");

}