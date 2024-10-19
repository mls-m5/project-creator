#pragma once

#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

auto helpStr = R"_(
usage:

mopen [flags] [number]

[0-9...]         If mopen opens the wrong project. Select another.


--help -h             print this string
)_";

struct OpenSettings {
    std::string name;
    size_t skipNum = 0;

    [[noreturn]] static void printHelp(int ret = 0) {
        std::cout << helpStr << std::endl;
        std::exit(ret);
    }

    static bool isDigits(std::string_view str) {
        for (auto c : str) {
            if (c < '0' || c > '9') {
                return false;
            }
        }
        return true;
    }

    OpenSettings(int argc, char **argv) {
        auto args = std::vector<std::string>(argv + 1, argv + argc);

        for (size_t i = 0; i < args.size(); ++i) {
            auto arg = args.at(i);

            if (arg == "--help" | arg == "-h") {
                printHelp();
            }
            else if (isDigits(arg)) {
                skipNum = std::stoul(arg) - 1;
            }

            else {
                if (arg.front() == '-') {
                    std::cerr << "invalid argument " << arg << "\n";
                    std::exit(1);
                }

                name = arg;
            }
        }
    }
};
