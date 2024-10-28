#pragma once

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

auto helpStr = R"_(
usage:

create-project [flags]



--help -h           print this string
)_";

struct GitSettings {
    enum Action {
        Status,
    };

    Action action = Status;

    [[noreturn]] void printHelp(int ret = 0) {
        std::cout << helpStr << std::endl;
        std::exit(ret);
    }

    GitSettings(int argc, char **argv) {
        auto args = std::vector<std::string>(argv + 1, argv + argc);
        if (args.empty()) {
            printHelp();
        }

        for (size_t i = 0; i < args.size(); ++i) {
            auto arg = args.at(i);

            if (arg == "--help" | arg == "-h") {
                printHelp();
            }
            else {
                if (arg.front() == '-') {
                    std::cerr << "invalid argument " << arg << "\n";
                    std::exit(1);
                }

                // path = arg;
                std::cerr << "invalid argument " << arg << "\n";
                std::exit(1);
            }
        }
    }
};
