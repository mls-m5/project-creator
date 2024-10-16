
cmake_minimum_required(VERSION 3.23)
project(project-tools)

add_executable(
    create-project
    src/create/create-project.cpp
    )
target_compile_features(create-project PRIVATE cxx_std_20)

add_executable(
    build
    src/build/build-project.cpp
    )
target_compile_features(build PRIVATE cxx_std_20)

add_executable(
    mfind
    src/find/mfind.cpp
    )
target_compile_features(mfind PRIVATE cxx_std_20)

add_executable(
    mopen
    src/open/mopen.cpp
    )
target_compile_features(mopen PRIVATE cxx_std_20)

file(GLOB_RECURSE FILES_TO_COPY "script/*")

foreach(file ${FILES_TO_COPY})
    message("Copying ${file}")
    file(COPY ${file} DESTINATION .)
endforeach()
