#pragma once
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

static constexpr auto helpStr = R"_(
usage
build [flags]

--release      build release
--test         run tests after build
--help -h      print this text
--dir -C       run in specified directory
--type -t      Type. Could be cmake, matmake2 or matmake4

)_";

enum ProjectType {
    Default,
    CMake,
    Matmake2,
    Matmake4,
};

inline ProjectType typeFromString(std::string_view name) {
    if (name == "cmake") {
        return CMake;
    }
    if (name == "matmake2") {
        return Matmake2;
    }
    if (name == "matmake4") {
        return Matmake4;
    }
    std::cerr << "no such build system " << name << "\n";
    std::exit(1);
}

struct BuildSettings {
    bool isRelease = false;
    bool shouldTest = false;
    std::filesystem::path path;
    ProjectType type = ProjectType::Default;

    [[noreturn]] void printHelp(int ret = 0) {
        std::cout << helpStr << std::endl;
        std::exit(ret);
    }

    BuildSettings(int argc, char **argv) {
        auto args = std::vector<std::string>(argv + 1, argv + argc);

        for (size_t i = 0; i < args.size(); ++i) {
            auto arg = args.at(i);

            if (arg == "--release") {
                isRelease = true;
                if (path.empty()) {
                    path = "build/release";
                }
            }
            else if (arg == "--test") {
                shouldTest = true;
            }
            else if (arg == "--help" || arg == "-h") {
                printHelp(0);
            }
            else if (arg == "--dir" || arg == "-C") {
                std::filesystem::current_path(args.at(++i));
            }
            else if (arg == "--type" || arg == "-t") {
                type = typeFromString(args.at(++i));
            }
            else {
                if (arg.front() == '-') {
                    std::cerr << "invalid argument " << arg << "\n";
                    printHelp(1);
                }

                path = arg;
            }
        }

        if (path.empty()) {
            path = "build/debug";
        }
    }
};
