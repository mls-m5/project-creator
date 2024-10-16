#pragma once

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

auto helpStr = R"_(
usage:

mfind [flagse] name

--only-root -r        do not search submodules
--only-sub -s         do not search root directories
--full-name -f        print the whole name, including home directory
--select -n [number]  select a result by number
--index -i            show result number
--count -c            show how many repositories there is

--help -h             print this string
)_";

struct FindSettings {
    std::string name;
    bool onlyPrintRoot = false;
    bool onlyPrintSub = false;
    bool shouldPrintFullName = false;
    bool shouldShowIndex = false;
    bool shouldCount = false;
    size_t selectNum = 0;

    [[noreturn]] void printHelp(int ret = 0) {
        std::cout << helpStr << std::endl;
        std::exit(ret);
    }

    FindSettings(int argc, char **argv) {
        auto args = std::vector<std::string>(argv + 1, argv + argc);
        // if (args.empty()) {
        //     printHelp();
        // }

        for (size_t i = 0; i < args.size(); ++i) {
            auto arg = args.at(i);

            if (arg == "--help" | arg == "-h") {
                printHelp();
            }
            else if (arg == "--only-root" || arg == "-r") {
                onlyPrintRoot = true;
            }
            else if (arg == "--only-sub" || arg == "-s") {
                onlyPrintSub = true;
            }
            else if (arg == "--full-name" || arg == "-f") {
                shouldPrintFullName = true;
            }
            else if (arg == "--select" || arg == "-n") {
                selectNum = std::stoul(args.at(++i));
            }
            else if (arg == "--index" || arg == "-i") {
                shouldShowIndex = true;
            }
            else if (arg == "--count" || arg == "-c") {
                shouldCount = true;
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
