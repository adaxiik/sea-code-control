# sea-code-control


# Used libraries
- [Tree sitter](https://github.com/tree-sitter/tree-sitter/)
- [Tree sitter C](https://github.com/tree-sitter/tree-sitter-c)
- [Linenoise](https://github.com/yhirose/cpp-linenoise)

# Dependencies
- bash (for build script, but can be done manually)
- cmake, compiler with c++17 support
- [emscripten](https://emscripten.org/) (only for wasm builds)
- [nodejs](https://nodejs.org/en/) (only for running wasm builds locally in terminal)
- [plantuml](https://plantuml.com/) (only for generating diagrams, not used by program itself)

# Quick start
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
$ ./build.sh wasm
$ python3 -m http.server 6969 # or any other http server 
$ chromium http://localhost:6969/build/wasm # or any other browser
# open console to see output
```


