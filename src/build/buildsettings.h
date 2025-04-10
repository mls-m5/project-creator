#pragma once
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

auto helpStr = R"_(
usage
build [flags]

--release      build release
--test         run tests after build
--help -h      print this text

)_";

struct BuildSettings {
    bool isRelease = false;
    bool shouldTest = false;
    std::filesystem::path path;

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
            else {
                if (arg.front() == '-') {
                    std::cerr << "invalid argument " << arg << "\n";
                    printHelp(1);
                }

                path = arg;
            }
        }

        if (path.empty()) {
            path = "build/default";
        }
    }
};
