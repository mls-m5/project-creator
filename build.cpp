
#include "matmake4/matmake.h"

void build() {
    add_subdirectory("lib");

    executable("create-project") //
        .src("src/create/*.cpp")
        .include("include")
        .copy("README.md");

    executable("build") //
        .src("src/build/*.cpp")
        .include("include");

    executable("mfind") //
        .src("src/find/*.cpp")
        .flags("-std=c++23")
        .include("include");

    executable("mopen") //
        .src("src/open/*.cpp")
        .flags("-std=c++23")
        .include("include");

    executable("project-overview") //
        .src("src/overview/*.cpp")
        .flags("-std=c++23")
        .include("include");
}
