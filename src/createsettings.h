#pragma once

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

auto helpStr = R"_(
usage:

create-project [flagse] [path]

--all -a            enable all flags to create a full project
                    ie  --creator --git
--init              create default project files
--creator           add qt creator files
--git               init git
--help -h           print this string
)_";

struct CreateSettings {
    bool shouldCreateQtCreatorFiles = false;
    bool shouldInitGit = false;
    bool shouldInitProject = false;
    std::filesystem::path path;

    [[noreturn]] void printHelp(int ret = 0) {
        std::cout << helpStr << std::endl;
        std::exit(ret);
    }

    CreateSettings(int argc, char **argv) {
        auto args = std::vector<std::string>(argv + 1, argv + argc);
        if (args.empty()) {
            printHelp();
        }

        for (size_t i = 0; i < args.size(); ++i) {
            auto arg = args.at(i);

            if (arg == "--creator" || arg == "--qtcreator") {
                shouldCreateQtCreatorFiles = true;
            }
            else if (arg == "--git") {
                shouldInitGit = true;
            }
            else if (arg == "--help" | arg == "-h") {
                printHelp();
            }
            else if (arg == "--all" | arg == "-a") {
                shouldCreateQtCreatorFiles = true;
                shouldInitProject = true;
                shouldInitGit = true;
            }
            else if (arg == "--init") {
                shouldInitProject = true;
            }
            else {
                if (arg.front() == '-') {
                    std::cerr << "invalid argument " << arg << "\n";
                    std::exit(1);
                }

                path = arg;
            }
        }

        if (!(shouldCreateQtCreatorFiles | shouldInitGit | shouldInitProject)) {
            printHelp(1);
        }
    }
};
