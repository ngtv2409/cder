cmake_minimum_required(VERSION 3.16)
 
project(cder VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SUBMODULE_DIRS
    libs/CLI11
    libs/rapidjson
)

foreach(dir ${SUBMODULE_DIRS})
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${dir}/CMakeLists.txt")
        message(FATAL_ERROR
            "Missing submodule: ${dir}\n"
            "Run:" << std::endl
            "  git submodule update --init --recursive"
        )
    endif()
endforeach()
foreach(dir ${SUBMODULE_DIRS})
    include_directories(${dir}/include)
endforeach()

add_executable(
    cder
    src/main.cpp
    src/mark.cpp
    src/argparser.cpp
    src/db.cpp
    src/protocol.cpp
)
target_include_directories(cder PRIVATE include)
target_precompile_headers(cder 
    PRIVATE include/PCH/rapidjson_pch.hpp
    PRIVATE include/PCH/cli11_pch.hpp
    PRIVATE include/PCH/std_pch.hpp
)
