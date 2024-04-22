# sea-code-control

[sea-code-control](https://adaxiik.github.io/sea-code-control/)

[benchmark](/examples)

# used libraries
### for interpreter itself
- [Tree sitter](https://github.com/tree-sitter/tree-sitter/)
- [Tree sitter C](https://github.com/tree-sitter/tree-sitter-c) .. [this]( https://github.com/tree-sitter/tree-sitter-c/blob/82ed290fc3e53041eb0c64137b113a006a87c83b/grammar.js) particular grammar was used
- [Linenoise](https://github.com/yhirose/cpp-linenoise)
- [Doctest](https://github.com/doctest/doctest)
- [nlohmann/json](https://github.com/nlohmann/json)

### for web interface
- [Xtermjs](https://xtermjs.org/)
- [Ace](https://ace.c9.io/)
- [Golden layout](https://golden-layout.com/)
- [Jquery](https://jquery.com/)
- [anevrly/VisualizerBP](https://github.com/anevrly/VisualizerBP)
- [wasm-clang](https://github.com/binji/wasm-clang)

# used tools
- bash (for build script, but can be done manually)
- cmake, compiler with c++20 support
- [emscripten](https://emscripten.org/) (only for wasm builds)
- [nodejs](https://nodejs.org/en/) (only for running wasm builds locally in terminal)
- [plantuml](https://plantuml.com/) (only for generating diagrams, not used by program itself)
- [snitch](https://github.com/tsoding/snitch) (only for [generating todo list](other/todo.sh))

# quick start

```bash
$ git clone --recursive git@github.com:adaxiik/sea-code-control.git
$ cd sea-code-control
$ ./build.sh
```

I recommend you to export these environment variables before running build script:

```bash
export BUILD_THREADS=12 # default is 1
export BUILD_TYPE=Release # default is Debug
```


for native build:

```bash
$ ./build.sh run native examples/sum_array.c
```

for wasm build:

```bash
$ ./build.sh run wasm examples/sum_array.c
```

for wasm build on web:

```bash
$ ./build.sh run wasmserver
# default server command is: python3 -m http.server 6969 , but you can change it in build.sh
# open browser at http://localhost:6969
```

for REPL:

```bash
$ ./build.sh run native
```

for tests:

```bash
$ ./build.sh test native
```

just building:
```bash
$ ./build.sh <wasm|native|all>
```

or wasm build on web with docker
```bash
$ docker build -f Dockerfile -t scc .
$ docker run -p 6969:6969 -t scc
```

# what it can do:
- running natively external C file via `./build.sh run native src/wasm/examples-web/bf_jit.c`
- running natively in repl `./build.sh run native`
- probably run in wasm via `node`, but [wasm driver](src/wasm/wasm_driver.js) is not really maintained
- running in browser via wasm, with memory visualization, breakpoints, step by step execution... `./build.sh run wasmserver`
- export AST to plantuml format .. it does that automatically for some reason actually, but it can be disabled with `-mute` flag.. output is in `ast.puml`
    - it can be then rendered to png with `PLANTUML_LIMIT_SIZE=32000 plantuml out.puml`
- can be used as C++ library as well probably, you can find it in your build directory and use includes from [src/include](src/include)
- it can export memory state as json
- int literals `1`, `1l`, `1u`, `1ll`..
    - bin, oct, hex literals `0b1`, `01`, `0xDEADBEEF`
- float literals `1.0`, `1.0f`
- char literals `E`
- types
    - `char`
    - `bool`
    - `u8`, `u16`, `u32`, `u64`
    - `i8`, `i16`, `i32`, `i64`
    - `f32`, `f64`
    - `void` (only for function return type)
- binary operators

```cpp
enum class OperatorKind
{
    // arithmetic
    Addition,
    Subtraction,
    Multiplication,
    Division,
    Modulo,
    BitwiseAnd,
    BitwiseOr,
    BitwiseXor,
    BitwiseShiftLeft,
    BitwiseShiftRight,

    // logical
    LogicalAnd,
    LogicalOr,

    // comparison
    Equals,
    NotEquals,
    LessThan,
    GreaterThan,
    LessThanOrEqual,
    GreaterThanOrEqual,

    COUNT
};
```

- unary operators

```cpp
enum class OperatorKind
{
    BinaryNot,
    LogicalNot,
    COUNT
};
```
- post incement and decrement `a++`, `a--`
- pre incement and decrement `++a`, `--a`
- errors on zero division `1/0`, `1%0`
- errors for some invalid pointer arithmetic `ptr * ptr`..
- type arithmetic .. eg. `int + float = float`, `int + uint = uint`, `long + uint = long`, `short + short = int` .. etc
- type casting `(int)1.0`, `(float)1`, `(int)(long)(int*)1`
    - casting to pointers and structures as well
    - errors for casting as well
- correct parrenthesized comma operator evaluation `(1,2,3,69) == 69`
- variable declarations and initializations
    - `int a;`
    - `int* a;`
    - `int** a;`
    - `int a[10];`
    - `int a = 1;`
    - `int* a = 0;`
    - `int a[10] = {1,2,3,4,5,6,7,8,9,10};`
    - `int a[10] = {0};`
    - `int a[] = {1, 2, 3};`
- errors for invalid variable declarations
    - `int a[1] = {1, 2};` .. etc
- using variables of course..
- pointers and dereferencing
    - `int* b = &a;`
    - `int c = *b;
    - might not work for cases, where its like rvalue eg. `&(a)`, because `a` is already evaluated into value
- pointer arithmetic
    - `int* a = 0; a++; // a == 4`
- `sizeof()` operator (might not always parsed correctly, but should work with arrays, pointers, types, variables, structures)..
    - can be used only with parentheses
- array to pointer decay
- subscript operator `a[1]`
- errors for invalid pointer dereferencing (invalid memory, mismatched types)
- assignment (and operators)
    - `a = 1;`
    - `a += 1;`
    - `a -= 1;`
    - `a *= 1;`
    - `....`
- static and stack memory string literals
    - `const char* str = "Hello Sailor!";`
    - `char str[] = "Hello Sailor!";` .. and they are immutable
- escaping characters
    - `\n`, `\t`, `\\`, `\"`, `\'`
- memory allocations stack vs heap
- `malloc` and `free`
    - but allocations are not aligned
- local and global variables
- scopes (blocks) `{ int a = 1; }`
- `if`, `else if`, `else`
- `while`
- `for` .. even without initialization, condition, increment
    - `for(;;) {}`
    - `for(int i = 0; i < 10;) {}`
    - `for(int i = 0; ; i++) {}`
    - `for(; i < 10; i++) {}`
    - `for(int i = 0; i < 10; i++) {}`
- `do while`
- `break`, `continue`
- shadowing variables with same name `int a = 1; { int a = 2; }`
- error for redeclaration of variable `int a = 1; int a = 2;`
- function declarations and definitions
    - `int sum(int a, int b) { return a + b; }`
    - `int sum(int a, int b);`
- forward declarations
- function calls
    - `sum(1, 2);`
    - `sum(1, 2, 3);`
    - `sum();`
- `return` keyword
- `void` return type, all other types as well
- shadowing with global variables
- `#define` simple expressions
    - `#define PI 3.14`
    - `#define SQ_A a*a`
    - but not anything more complex
- `#include` for [inbuild headerfiles](src/include/scc/binding/libc/)
    - also can be only on top of file
- io (headers need to be included)
    - `write(1, "Hello Sailor!\n", 14);`
    - `puts("Hello Sailor!");`
- `printf` + some safety checks
    - `%%`, `%hhi`, `%hhu`, `%hi`, `%hu`, `%i`, `%u`, `%d`, `%s`, `%c`, `%x`, `%X`, `%p`, `%f` (casted to double as standard says)
- assertions with `_scc_asssert`
- inbuild `scc.h`
    - `void scc_puti(int x)` .. prints and integer
    - `int scc_eval(const char* s)` .. evaluates C source code and returns result as integer
    - `int scc_eval_len(const char* s, unsigned long len)` .. same as above, but does not require null terminated string
- structures.. only with typedefs
    - but array in structures are no well supported
    - there are no padding and alignment
    - `typedef struct { int a; int b; } A;`
    - direct initialization `A a = {1, 2};` .. (but might not work well with pointers)
    - nested initialization `OuterS outer_target = {{420, 69.69f, 'E'}, 0};`
    - designated initialization `OuterS outer_target = {.inner = {420, 69.69f, 'E'}, .b = 0};` .. mixed initialization is supported as well
    - access `outer_target.inner.a`
    - pointer to structure `OuterS* outer_ptr = &outer_target;`
    - pointer to structure access `outer_ptr->inner.a`

- you might want to check [benchmark](/examples) and [examples](src/wasm/examples-web/) for more :)


# what it can't do:
- comma operator.. it work only in parrenthesized form `(1,2,3,69) == 69`
- unions
- enums
- variadic functions
- function pointers
- `goto`
- `switch`
- const
- multiple files
- complex array declarations and initializations .. only parsing is not implemented, but type system supports it
    - `int a[1][2];`
    - `int **a[2][2];`
- `&(a)` .. because `a` is already evaluated into value
- command line arguments
- preprocesor
- input from user
- files
- a lot of other things

# other
- [todo](other/todo.txt)