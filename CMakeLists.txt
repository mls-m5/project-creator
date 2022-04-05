
cmake_minimum_required(VERSION 3.23)
project(project-tools)

add_executable(
    create-project
    src/create-project.cpp
    )

target_compile_features(create-project PRIVATE cxx_std_17)
