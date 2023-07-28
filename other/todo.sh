#!/bin/sh

# must be run from the root of the project

set -xe

SCRIPT_LOCATION=$(dirname $0)
OUTPUT_FILE_NAME="todo.txt"

OUTPUT_FILE="$SCRIPT_LOCATION/$OUTPUT_FILE_NAME"

echo "Sorted TODO list:\n" > "$OUTPUT_FILE"
snitch list | grep -v "3rdparty\|doctest.h" >> "$OUTPUT_FILE"