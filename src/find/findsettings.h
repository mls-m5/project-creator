#pragma once

#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

auto helpStr = R"_(
usage:

mfind [flags] name

--only-root -r        do not search submodules
--only-sub -s         do not search root directories
--full-name -f        print the whole name, including home directory
--select -n [number]  select a result by number
--index -i            show result number
--count -c            show how many repositories there is

--favorites -o        only list or search favorites
--add-favorite -a     save a project to quickly access
--remove-favorite -x  remove saved project from list

--help -h             print this string
)_";

struct FindSettings {
    std::string name;
    bool onlyPrintRoot = false;
    bool onlyPrintSub = false;
    bool shouldPrintFullName = false;
    bool shouldShowIndex = false;
    bool shouldCount = false;
    bool shouldRemoveFavorite = false;
    bool shouldAddFavorite = false;
    bool shouldOnlyListFavorites;
    size_t selectNum = 0;

    std::filesystem::path homeFolder = std::getenv("HOME");

    std::filesystem::path progFolder =
        std::filesystem::path{homeFolder} / "Prog";
    std::vector<std::filesystem::path> folders =
        std::vector<std::filesystem::path>{
            progFolder / "Projekt",
            progFolder / "Experiment",
            progFolder / "Other",
        };
    std::filesystem::path favoritesPath = homeFolder / ".mfind_favorites";

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

    FindSettings(int argc, char **argv) {
        auto args = std::vector<std::string>(argv + 1, argv + argc);

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
            else if (arg.starts_with("-n")) {
                selectNum = std::stoul(arg.substr(2));
            }
            else if (isDigits(arg)) {
                selectNum = std::stoul(arg);
            }
            else if (arg == "--index" || arg == "-i") {
                shouldShowIndex = true;
            }
            else if (arg == "--count" || arg == "-c") {
                shouldCount = true;
            }
            else if (arg == "--add-favorite" || arg == "-a") {
                shouldAddFavorite = true;
            }
            else if (arg == "--remove-favorite" || arg == "-x") {
                shouldRemoveFavorite = true;
            }
            else if (arg == "--favorites" || arg == "--favorite" ||
                     arg == "-o") {
                shouldOnlyListFavorites = true;
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
