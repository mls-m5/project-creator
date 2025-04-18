#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <string_view>
#include <vector>

inline const auto files = std::vector<std::pair<std::string_view, std::string>>{
    {"src/main.cpp",
     R"_(#include <iostream>

int main(int argc, char *argv[])
{
    std::cout << "hello there\n";
    return 0;
}
)_"},
    {"CMakeLists.txt", R"_(
cmake_minimum_required(VERSION 3.23)
project({name})

enable_testing()
add_subdirectory(lib)

add_executable(
    {name}
    src/main.cpp
    )

target_compile_features(
    {name}
    PRIVATE
    cxx_std_17
    )

find_package(Threads)
target_link_libraries(
    {name}
    PRIVATE
    ${CMAKE_THREAD_LIBS_INIT}
    )

add_subdirectory(test)

)_"},
    {"README.md", R"_(# {name}
)_"},
    {".clang-format", R"_(

Language: Cpp
BasedOnStyle: LLVM
IndentWidth: 4
SortIncludes: true
AccessModifierOffset: -4
AlwaysBreakTemplateDeclarations: true
AllowShortFunctionsOnASingleLine: Empty
BinPackArguments: false
BinPackParameters: false
BreakBeforeBraces: Custom
BraceWrapping:
  BeforeCatch: true
  BeforeElse: true

AlwaysBreakAfterReturnType: None
PenaltyReturnTypeOnItsOwnLine: 1000000
BreakConstructorInitializers: BeforeComma

)_"},
    {"lib/CMakeLists.txt", R"_()_"},
    {"test/CMakeLists.txt", R"_()_"},
};

inline void initProject(std::string name) {
    std::filesystem::create_directories("src");
    std::filesystem::create_directories("lib");
    std::filesystem::create_directories("test");

    auto re = std::regex{"\\{name\\}"};

    for (auto &f : files) {
        auto path = std::filesystem::path{f.first};
        if (std::filesystem::exists(path)) {
            std::cout << "skippig existing file " << path << std::endl;
            continue;
        }

        auto str = std::regex_replace(f.second, re, name);
        std::cout << "creating file " << path << std::endl;
        std::ofstream{path} << str;
    }
}
