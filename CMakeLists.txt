
cmake_minimum_required(VERSION 3.23)
project(project-tools)

add_subdirectory(lib)

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

add_executable(
    mgit
    src/git/mgit.cpp
    )
target_compile_features(mgit PRIVATE cxx_std_20)

add_executable(
    project-overview
    src/overview/overview.cpp
    )
target_compile_features(project-overview PRIVATE cxx_std_20)

add_executable(
    mergecommands
    src/compilecommands/mergecommands.cpp
    )
target_compile_features(mergecommands PRIVATE cxx_std_20)

target_link_libraries(
    mergecommands
    PRIVATE
    FastJson::FastJson
)

file(GLOB_RECURSE FILES_TO_COPY "script/*")

foreach(file ${FILES_TO_COPY})
    message("Copying ${file}")
    file(COPY ${file} DESTINATION .)
endforeach()
