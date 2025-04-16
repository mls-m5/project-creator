
#include "matmake4/matmake.h"

void build() {
    // subdirectory("lib");

    auto &l = lib("common") //
                  .flags("-std=c++23")
                  .copy("README.md")
                  .copy("script/*", ".")
                  .include("include")
                  .type(TargetType::Interface);

    executable("create-project") //
        .src("src/create/*.cpp")
        .include("include")
        .in(l);

    executable("build") //
        .src("src/build/*.cpp")
        .in(l);

    executable("mfind") //
        .src("src/find/*.cpp")
        .in(l)
        .include("include");

    executable("mopen") //
        .src("src/open/*.cpp")
        .in(l);

    executable("project-overview") //
        .src("src/overview/*.cpp")
        .in(l);

    executable("mergecommand") //
        .src("src/compilecommands/*.cpp")
        .include("./lib/fast-json/include/")
        .in(l);
}
