cmake_minimum_required(VERSION 3.20)

project(visu-alloc LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(visu-alloc
  src/main.cpp
)

add_custom_target(run
  COMMAND visu-alloc
  DEPENDS visu-alloc
  WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)
