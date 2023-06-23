cmake_minimum_required(VERSION 3.0.0)
project(sea-code-control VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(BUILDING_WASM "Build for wasm" OFF) 


set(3RD_PARTY_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/src/3rdparty)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra ")

if(BUILDING_WASM)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O1") # wasm with O0 takes too much memory for some reason
    message("Building for wasm enabled")
else()
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -fsanitize=address")
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -pedantic -flto ")


# tree-sitter
set(TREE_SITTER_PATH ${3RD_PARTY_PATH}/tree-sitter)
set(TREE_SITTER_FILES ${TREE_SITTER_PATH}/src/lib.c)
set(TREE_SITTER_INCLUDE_DIRS_PUBLIC ${TREE_SITTER_PATH}/include)
set(TREE_SITTER_INCLUDE_DIRS_PRIVATE ${TREE_SITTER_PATH}/src)
set(TREE_SITTER_INCLUDE_DIRS ${TREE_SITTER_INCLUDE_DIRS_PUBLIC} ${TREE_SITTER_INCLUDE_DIRS_PRIVATE})

add_library(tree-sitter STATIC ${TREE_SITTER_FILES} )
target_include_directories(tree-sitter PUBLIC ${TREE_SITTER_INCLUDE_DIRS_PUBLIC})
target_include_directories(tree-sitter PRIVATE ${TREE_SITTER_INCLUDE_DIRS_PRIVATE})
set_target_properties(tree-sitter PROPERTIES LINKER_LANGUAGE C)

# tree-sitter-c 
set(TREE_SITTER_C_PATH ${3RD_PARTY_PATH}/tree-sitter-c)
set(TREE_SITTER_C_INCLUDE_DIRS ${TREE_SITTER_C_PATH}/include)

add_library(tree-sitter-c STATIC ${TREE_SITTER_C_PATH}/parser.c)
target_include_directories(tree-sitter-c PRIVATE ${TREE_SITTER_INCLUDE_DIRS_PUBLIC})
target_include_directories(tree-sitter-c PUBLIC ${TREE_SITTER_C_INCLUDE_DIRS})
set_target_properties(tree-sitter-c PROPERTIES LINKER_LANGUAGE C)



add_executable(scc src/main.cpp)
target_include_directories(scc PUBLIC ${TREE_SITTER_INCLUDE_DIRS_PUBLIC} ${TREE_SITTER_C_INCLUDE_DIRS})
target_link_libraries(scc tree-sitter tree-sitter-c)

