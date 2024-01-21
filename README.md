# sea-code-control

[sea-code-control](https://adaxiik.github.io/sea-code-control/)

# Used libraries
### For interpreter itself
- [Tree sitter](https://github.com/tree-sitter/tree-sitter/)
- [Tree sitter C](https://github.com/tree-sitter/tree-sitter-c)
- [Linenoise](https://github.com/yhirose/cpp-linenoise)
- [Doctest](https://github.com/doctest/doctest)
- [nlohmann/json](https://github.com/nlohmann/json)

### For web interface
- [Xtermjs](https://xtermjs.org/)
- [Ace](https://ace.c9.io/)
- [Golden layout](https://golden-layout.com/)
- [Jquery](https://jquery.com/)
- [anevrly/VisualizerBP](https://github.com/anevrly/VisualizerBP)
- [wasm-clang](https://github.com/binji/wasm-clang)

# Used tools
- bash (for build script, but can be done manually)
- cmake, compiler with c++20 support
- [emscripten](https://emscripten.org/) (only for wasm builds)
- [nodejs](https://nodejs.org/en/) (only for running wasm builds locally in terminal)
- [plantuml](https://plantuml.com/) (only for generating diagrams, not used by program itself)
- [snitch](https://github.com/tsoding/snitch) (only for [generating todo list](other/todo.sh))

# Quick start

```bash
$ git clone --recursive git@github.com:adaxiik/sea-code-control.git
$ cd sea-code-control
$ ./build.sh
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
# check the developer console
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

# Other
- see [todo](other/todo.txt) or [plan](other/PLAN.md) for more