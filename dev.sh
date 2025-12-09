#!/bin/bash

mkdir build
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
