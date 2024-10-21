
#include "findsettings.h"
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <locale>
#include <map>
#include <string>
#include <unistd.h>
#include <vector>

namespace {

struct Entry {
    std::filesystem::path path;
    bool isSub = false;
};

bool isTerminal() {
    return isatty(fileno(stdout));
}

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

ContainerT findAllPaths(FindSettings &settings) {
    auto paths = ContainerT{};

    auto queue = std::vector<Entry>{};
    for (auto &folder : settings.folders) {
        if (!std::filesystem::exists(folder)) {
            continue;
        }

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
                           queue.push_back(entry);
                       });
    }

    return paths;
}

std::vector<std::filesystem::path> printPaths(const FindSettings &settings,
                                              const ContainerT &paths) {
    bool isTerminal = ::isTerminal();
    size_t index = 1;
    auto ret = std::vector<std::filesystem::path>{};

    for (auto &path : paths) {
        if (settings.onlyPrintRoot && path.second.isSub) {
            continue;
        }
        if (settings.onlyPrintSub && !path.second.isSub) {
            continue;
        }
        auto comp = path.first.stem().string();
        for (auto &c : comp) {
            auto loc = std::locale{""};
            c = std::tolower(c, loc);
        }
        if (settings.name.empty() ||
            comp.find(settings.name) != std::string::npos) {

            if (settings.selectNum) {
                if (index < settings.selectNum) {
                    ++index;
                    continue;
                }

                if (index > settings.selectNum) {
                    break;
                }
            }

            if (settings.shouldShowIndex) {
                std::cout << index << ": ";
            }

            std::cout << (path.second.isSub && isTerminal ? "\033[34m" : "");
            if (settings.shouldPrintFullName) {
                std::cout << path.first.string();
            }
            else {
                std::cout << std::filesystem::relative(path.first,
                                                       settings.homeFolder)
                                 .string();
            }
            ret.push_back(path.second.path);
            std::cout << (isTerminal ? "\033[0m" : "") << "\n";
            ++index;
        }
    }

    return ret;
}

ContainerT getFavorites(const FindSettings &settings) {
    auto ret = ContainerT{};
    if (!std::filesystem::exists(settings.favoritesPath)) {
        return ret;
    }

    auto file = std::ifstream{settings.favoritesPath};
    for (std::string line; std::getline(file, line);) {
        if (line.empty()) {
            continue;
        }
        ret[line] = Entry{
            .path = line,
        };
    }

    return ret;
}

void saveFavorites(const FindSettings &settings, const ContainerT &paths) {
    auto file = std::ofstream{settings.favoritesPath};

    for (auto &it : paths) {
        file << it.second.path.string() << "\n";
    }
}

int main(int argc, char *argv[]) {
    auto settings = FindSettings{argc, argv};

    auto paths = settings.shouldOnlyListFavorites ? getFavorites(settings)
                                                  : findAllPaths(settings);

    if (settings.shouldOnlyListFavorites && paths.empty()) {
        std::cerr << "No favorites found. Add with --add-favorite\n";
        return 1;
    }

    auto selectedPaths = printPaths(settings, paths);

    if (settings.shouldAddFavorite) {
        if (selectedPaths.empty()) {
            std::cerr << "No path selected: No match\n";
            return 1;
        }
        else if (selectedPaths.size() > 1) {
            std::cerr << "No path selected: To many matches, select a single\n";
        }
        auto favorites = getFavorites(settings);
        favorites[selectedPaths.front()] = {
            .path = selectedPaths.front(),
        };

        saveFavorites(settings, favorites);
        std::cout << "Added " << selectedPaths.front() << " to favorites\n";
    }
    else if (settings.shouldRemoveFavorite) {
        if (selectedPaths.empty()) {
            std::cerr << "No path selected: No match\n";
            return 1;
        }
        else if (selectedPaths.size() > 1) {
            std::cerr << "No path selected: To many matches, select a single\n";
            return 1;
        }
        auto favorites = getFavorites(settings);

        favorites.erase(selectedPaths.front());

        saveFavorites(settings, favorites);
        std::cout << "Removed " << selectedPaths.front() << " to favorites\n";
    }

    if (settings.shouldCount) {
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
