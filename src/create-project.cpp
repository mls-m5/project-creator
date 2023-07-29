#include "createsettings.h"
#include "files.h"
#include "qtuserfile.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <regex>
#include <set>
#include <unordered_set>

namespace {

namespace filesystem = std::filesystem;

bool isShouldInclude(filesystem::path path) {

    static const auto supportedExtensions = std::unordered_set<std::string>{
        ".cpp",
        ".cxx",
        ".c",
        ".cc",
        ".cppm",
        ".h",
        ".hpp",
        ".hxx",
        ".md",
        ".txt",
        ".html",
        ".js",
        ".png",
        ".jpg",
        ".bmp",
    };
    static const auto supportedFilenames = std::unordered_set<std::string>{
        ".clang-format",
        ".gitignore",
        "Matmakefile",
        "CMakeLists.txt",
        "Makefile",
        "WORKSPACE",
        "BUILD",
    };

    auto ext = path.extension();

    if (supportedExtensions.find(ext) != supportedExtensions.end()) {
        return true;
    }

    if (supportedFilenames.find(path.filename()) != supportedFilenames.end()) {
        return true;
    }

    return false;
}

bool doesGitignoreHasQtFiles(filesystem::path gitignore) {
    auto file = std::ifstream{gitignore};

    if (!file) {
        return false;
    }

    for (std::string line; getline(file, line);) {
        if (line.find(".creator") != std::string::npos) {
            return true;
        }
    }

    return false;
}

void addGitIgnore(filesystem::path path) {
    std::cout << "adding project files to git ignore\n";

    auto gitignore = path / ".gitignore";

    if (!doesGitignoreHasQtFiles(gitignore)) {
        auto file = std::ofstream{gitignore, std::ios::app};
        file << "\nbuild/";
        file << "\nbin/\n\n";
        file << "\n# qtcreator project files\n";
        for (auto str : {
                 "*.cflags",
                 "*.config",
                 "*.creator",
                 "*.creator.user",
                 "*.cxxflags",
                 "*.files",
                 "*.includes",
                 ".qtc_clangd/",
                 ".creator.user.",
             }) {
            file << str << "\n";
        }
    }
}

void createQtUserFile(std::string projectName) {
    auto filename = projectName + ".creator.user";
    if (filesystem::exists(filename)) {
        return;
    }

    auto str = std::string{qtUserFile};

    auto nameWithoutPeriod = projectName;
    nameWithoutPeriod.erase(0, 1);

    auto re = std::regex(std::regex{"\\{\\{projectName\\}\\}"});
    str = std::regex_replace(str, re, nameWithoutPeriod);

    std::ofstream{filename} << str;
}

void refreshFiles(std::filesystem::path projectName,
                  std::filesystem::path path) {
    auto isBuildPath = [](const std::filesystem::path name) {
        return name == "build" || name == "bin";
    };

    {
        auto filesFile = std::ofstream{projectName.string() + ".files"};

        for (auto it = filesystem::recursive_directory_iterator{path};
             it != decltype(it){};
             ++it) {
            if (isBuildPath(it->path().filename())) {
                it.disable_recursion_pending();
                continue;
            }
            if (isShouldInclude(it->path())) {
                filesFile << filesystem::relative(it->path(), path).string()
                          << "\n";
            }
        }
    }

    auto includeDirs = [isBuildPath, path]() {
        auto paths = std::vector<std::filesystem::path>{};
        for (auto it = filesystem::recursive_directory_iterator{path};
             it != decltype(it){};
             ++it) {
            if (isBuildPath(it->path().filename())) {
                it.disable_recursion_pending();
                continue;
            }
            if (filesystem::is_directory(it->path())) {
                if (it->path().filename() == "include" ||
                    it->path().filename() == "src") {
                    paths.push_back(filesystem::relative(it->path(), path));
                }
            }
        }
        return paths;
    };

    {
        auto dirs = includeDirs();
        auto includeFilePath = projectName.string() + ".includes";

        auto set = std::set<std::filesystem::path>{};

        for (auto &dir : dirs) {
            set.insert(dir);
        }

        {
            // Save includes from previous file
            auto inFile = std::ifstream{includeFilePath};
            for (auto line = std::string{}; std::getline(inFile, line);) {
                set.insert(line);
            }
        }

        auto includeFile = std::ofstream{includeFilePath};

        for (auto &dir : set) {
            includeFile << dir.string() << "\n";
        }
    }
}

void createQtCreatorFiles(bool shouldRefresh) {
    auto path = filesystem::absolute(filesystem::current_path());

    auto projectName = filesystem::path{"." + path.filename().string()};

    createQtUserFile(projectName.string());

    if (filesystem::exists(projectName.string() + ".creator")) {
        if (shouldRefresh) {
            std::cout << "refreshing qtcreator files" << std::endl;
            refreshFiles(projectName, path);
            return;
        }

        std::cerr << "found " << projectName.string() << ".creator"
                  << " the project seems to exist\n";
        exit(1);
    }

    std::cout << "init project " << projectName << "\n";

    std::ofstream{projectName.string() + ".creator"} << "[General]\n";

    std::ofstream{projectName.string() + ".cxxflags"} << "-std=c++17\n";

    {
        auto configFile = std::ofstream{projectName.string() + ".config"};
        configFile << "// add macros here like\n"
                      "#define X 43 // ;)\n";
    }

    refreshFiles(projectName, path);
}

} // namespace

int main(int argc, char **argv) {
    const auto settings = CreateSettings{argc, argv};

    if (!settings.path.empty()) {
        std::filesystem::create_directories(settings.path);
        std::filesystem::current_path(settings.path);
    }

    if (settings.shouldInitProject) {
        initProject(std::filesystem::current_path().filename());
        addGitIgnore(filesystem::absolute(filesystem::current_path()));
    }

    if (settings.shouldCreateQtCreatorFiles) {
        createQtCreatorFiles(settings.shouldRefresh);
    }

    if (settings.shouldInitGit) {
        return std::system("git init");
    }
}
