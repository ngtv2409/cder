#!/bin/bash

export CDER_DB_PATH="$(realpath ./cder_db)"

mkdir build
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug

export PATH="$PATH:$(realpath ./build/)"
