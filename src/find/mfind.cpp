
#include "findsettings.h"
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <locale>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace {

struct Entry {
    std::filesystem::path path;
    bool isSub = false;
};

} // namespace

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

using ContainerT = std::map<std::filesystem::path, Entry>;

int main(int argc, char *argv[]) {
    auto settings = FindSettings{argc, argv};
    std::filesystem::path homeFolder = std::getenv("HOME");

    auto progFolder = std::filesystem::path{homeFolder} / "Prog";
    auto folders = std::vector<std::filesystem::path>{
        progFolder / "Projekt",
        progFolder / "Experiment",
    };

    auto paths = ContainerT{};

    auto queue = std::vector<Entry>{};
    for (auto &folder : folders) {
        for (auto &it : std::filesystem::directory_iterator{folder}) {
            queue.push_back({
                .path = it.path(),
                .isSub = false,
            });
        }
    }

    while (!queue.empty()) {
        auto it = queue.back();
        queue.pop_back();
        if (paths.find(it.path) != paths.end()) {
            continue;
        }
        paths[it.path] = it;
        findSubmodules(it.path,
                       [&it, &paths, &queue](std::filesystem::path path) {
                           auto entry = Entry{
                               .path = path,
                               .isSub = true,
                           };
                           // std::cout << "..." << path
                           queue.push_back(entry);
                       });
    }

    for (auto &path : paths) {
        if (settings.onlyPrintRoot && path.second.isSub) {
            continue;
        }
        auto comp = path.first.stem().string();
        for (auto &c : comp) {
            auto loc = std::locale{""};
            c = std::tolower(c, loc);
        }
        if (settings.name.empty() ||
            comp.find(settings.name) != std::string::npos) {
            std::cout
                << (path.second.isSub ? "\033[34m" : "")
                << std::filesystem::relative(path.first, homeFolder).string()
                << "\033[0m"
                << "\n";
        }
    }

    if (settings.name.empty()) {
        int countRoot = 0;
        int countAll = 0;
        for (auto &project : paths) {
            countRoot += !project.second.isSub;
            ++countAll;
        }

        std::cout << "Projects " << countRoot << "\n";
        std::cout << "Including subrepositories " << countAll << "\n";
    }

    return 0;
}
