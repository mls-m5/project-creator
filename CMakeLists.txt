
cmake_minimum_required(VERSION 3.23)
project(project-tools)

add_executable(
    create-project
    src/create-project.cpp
    )

target_compile_features(create-project PRIVATE cxx_std_17)

add_executable(
    build
    src/build-project.cpp
    )

target_compile_features(build PRIVATE cxx_std_17)

file(COPY install.sh DESTINATION .)
