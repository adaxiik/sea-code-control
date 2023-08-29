#!/bin/bash

set -e


BUILD_TYPE="${BUILD_TYPE=Debug}"

PROJECT_PATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
CMAKE_PATH="$PROJECT_PATH/CMakeLists.txt"

BUILD_FOLDER="$PROJECT_PATH/build"
BUILD_WASM_FOLDER="$BUILD_FOLDER/wasm"
BUILD_NATIVE_FOLDER="$BUILD_FOLDER/native"

EXECUTABLE_NAME="scc"
WASM_DRIVER_NAME="wasm_driver"
WASM_SERVER_PORT="6969"
SERVER_COMMAND="python3 -m http.server $WASM_SERVER_PORT --directory $BUILD_WASM_FOLDER"

BUILD_THREADS="${BUILD_THREADS=1}"



RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
RESET='\033[0m' # No Color

print_green() {
    echo -e "${GREEN}$1${RESET}"
}
print_red() {
    echo -e "${RED}$1${RESET}"
}
print_yellow() {
    echo -e "${YELLOW}$1${RESET}"
}

print_build_type() {
    if [ "$BUILD_TYPE" = "Debug" ]; then
        print_yellow "Debug build"
    else
        print_yellow "Release build"
    fi
}

print_threads() {
    if [ "$BUILD_THREADS" -gt 1 ]; then
        print_yellow "Building with $BUILD_THREADS threads"
    else
        print_yellow "Building with $BUILD_THREADS thread"
    fi
}

print_build_info() {
    print_build_type
    print_threads
}

build_wasm() {
    print_build_info

    if [ ! -d "$BUILD_WASM_FOLDER" ]; then
        mkdir -p "$BUILD_WASM_FOLDER"
        cd "$BUILD_WASM_FOLDER"
        emcmake cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DBUILDING_WASM=ON $PROJECT_PATH
    fi

    cd "$BUILD_WASM_FOLDER"
    make -j$BUILD_THREADS 
    cd "$PROJECT_PATH"
}

build_native() {
    print_build_info

    if [ ! -d "$BUILD_NATIVE_FOLDER" ]; then
        mkdir -p "$BUILD_NATIVE_FOLDER"
        cd "$BUILD_NATIVE_FOLDER"
        cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE $PROJECT_PATH
    fi

    cd "$BUILD_NATIVE_FOLDER"
    make -j$BUILD_THREADS 
    cd "$PROJECT_PATH"
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
        $BUILD_NATIVE_FOLDER/$EXECUTABLE_NAME ${@:3}
    elif [ "$2" = "wasm" ]; then
        build_wasm
        node $BUILD_WASM_FOLDER/$WASM_DRIVER_NAME.js ${@:3} 
    elif [ "$2" = "wasmserver" ]; then
        build_wasm
        $SERVER_COMMAND
    else
        usage
    fi
}

test()
{
    ARG_COUNT=$#
    if [ $ARG_COUNT -lt 2 ]; then
        usage
    fi

    if [ "$2" = "native" ]; then
        build_native 
        $BUILD_NATIVE_FOLDER/tests
    # TODOOO:
    # elif [ "$2" = "wasm" ]; then
    #     build_wasm 
    #     node $BUILD_WASM_FOLDER/$WASM_DRIVER_NAME.js ${@:3} 
    else
        usage
    fi
}

usage() {
    echo "Usage: $0 [wasm|native|all|clean|run|test]"
    echo "  wasm: build wasm"
    echo "  native: build native"
    echo "  all: build wasm and native"
    echo "  clean [wasm|native|all]: clean build folder"
    echo "  run [wasm|wasmserver|native]: run executable"
    echo "  test [native]: run executable"
    exit 0
}

if [ "$1" = "wasm" ]; then
    if [ $# -eq 2 ]; then
        BUILD_TYPE=$2
    fi
    build_wasm
elif [ "$1" = "native" ]; then
    if [ $# -eq 2 ]; then
        BUILD_TYPE=$2
    fi
    build_native
elif [ "$1" = "all" ]; then
    build_wasm
    build_native
elif [ "$1" = "clean" ]; then
    clean $@
elif [ "$1" = "run" ]; then
    run $@
elif [ "$1" = "test" ]; then
    test $@
else
    usage
fi
