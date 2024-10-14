
#include "findsettings.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <set>
#include <string>
#include <vector>

// namespace {

// struct Entry {
//     std::filesystem::path path;
// };

// }

bool isGit(std::filesystem::path path) {
    return std::filesystem::exists(path) &&
           std::filesystem::exists(path / ".git");
}

void parseSubmoduleFile(std::filesystem::path path,
                        std::function<void(std::filesystem::path)> cb) {
    std::string_view matchStr = "path = ";

    auto file = std::ifstream{path};

    for (std::string line; std::getline(file, line);) {
        if (auto f = line.find(matchStr) != std::string::npos) {
            cb(path.parent_path() / line.substr(f + matchStr.size()));
        }
    }
}

void findSubmodules(std::filesystem::path path,
                    std::function<void(std::filesystem::path)> cb) {
    auto submoduleFile = path / ".gitmodules";
    if (!std::filesystem::exists(submoduleFile)) {
        return;
    }
    parseSubmoduleFile(submoduleFile, cb);
}

using ContainerT = std::set<std::filesystem::path>;

int main(int argc, char *argv[]) {
    auto settings = FindSettings{argc, argv};
    std::filesystem::path homeFolder = std::getenv("HOME");

    auto progFolder = std::filesystem::path{homeFolder} / "Prog";
    auto folders = std::vector<std::filesystem::path>{
        progFolder / "Projekt",
        progFolder / "Experiment",
    };

    auto paths = ContainerT{};

    for (auto &folder : folders) {
        for (auto &it : std::filesystem::directory_iterator{folder}) {
            paths.insert(it.path());
        }
    }

    for (auto it = paths.begin(); it != paths.end(); ++it) {
        findSubmodules(*it, [&paths](std::filesystem::path path) {
            paths.insert(path); //
        });
    }

    for (auto &path : paths) {
        if ((path.stem()).string().find(settings.name) != std::string::npos) {
            std::cout << std::filesystem::relative(path, homeFolder).string()
                      << "\n";
        }
    }

    return 0;
}
