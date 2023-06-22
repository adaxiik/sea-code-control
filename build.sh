#!/bin/sh

set -e


BUILD_TYPE="Debug"

PROJECT_PATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
CMAKE_PATH="$PROJECT_PATH/CMakeLists.txt"

BUILD_FOLDER="$PROJECT_PATH/build"
BUILD_WASM_FOLDER="$BUILD_FOLDER/wasm"
BUILD_NATIVE_FOLDER="$BUILD_FOLDER/native"

EXECUTABLE_NAME="scc"

build_wasm() {
    if [ ! -d "$BUILD_WASM_FOLDER" ]; then
        mkdir -p "$BUILD_WASM_FOLDER"
        cd "$BUILD_WASM_FOLDER"
        emcmake cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE $PROJECT_PATH
    fi

    cd "$BUILD_WASM_FOLDER"
    make
}

build_native() {
    if [ ! -d "$BUILD_NATIVE_FOLDER" ]; then
        mkdir -p "$BUILD_NATIVE_FOLDER"
        cd "$BUILD_NATIVE_FOLDER"
        cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE $PROJECT_PATH
    fi

    cd "$BUILD_NATIVE_FOLDER"
    make
}

clean() {
    ARG_COUNT=$#

    if [ $ARG_COUNT -lt 2 ]; then
        usage
    fi

    if [ "$2" = "wasm" ]; then
        rm -r $BUILD_WASM_FOLDER
    elif [ "$2" = "native" ]; then
        rm -r $BUILD_NATIVE_FOLDER
    elif [ "$2" = "all" ]; then
        rm -r $BUILD_FOLDER
    else
        usage
    fi
}

run()
{
    ARG_COUNT=$#
    if [ $ARG_COUNT -lt 2 ]; then
        usage
    fi

    if [ "$2" = "native" ]; then
        build_native
        $BUILD_NATIVE_FOLDER/$EXECUTABLE_NAME
    elif [ "$2" = "wasm" ]; then
        build_wasm
        node $BUILD_WASM_FOLDER/$EXECUTABLE_NAME.js # TODOO: something better than node?
    else
        usage
    fi
}

usage() {
    echo "Usage: $0 [wasm|native|all|clean|run]"
    echo "  wasm: build wasm"
    echo "  native: build native"
    echo "  all: build wasm and native"
    echo "  clean [wasm|native|all]: clean build folder"
    echo "  run [wasm|native]: run executable"
    exit 0
}

print_green() {
    echo "\033[0;32m$1\033[0m"
}
print_red() {
    echo "\033[0;31m$1\033[0m"
}

if [ "$1" = "wasm" ]; then
    build_wasm
elif [ "$1" = "native" ]; then
    build_native
elif [ "$1" = "all" ]; then
    build_wasm
    build_native
elif [ "$1" = "clean" ]; then
    clean $@
elif [ "$1" = "run" ]; then
    run $@
else
    print_green "Building native"
    build_native
    print_green "Building wasm"
    build_wasm
fi
