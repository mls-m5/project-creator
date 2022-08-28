#include "createsettings.h"
#include "files.h"
#include "qtuserfile.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <regex>

namespace {

namespace filesystem = std::filesystem;

bool isShouldInclude(filesystem::path path) {
    auto ext = path.extension();

    if (ext == ".cpp" || ext == ".cxx" || ext == ".c" || ext == ".cc") {
        return true;
    }

    if (ext == ".h" || ext == ".hpp" || ext == ".hxx") {
        return true;
    }

    if (path.filename() == ".clang-format" || path.filename() == ".gitignore") {
        return true;
    }

    if (ext == ".md") {
        return true;
    }

    if (path.filename() == "Matmakefile" ||
        path.filename() == "CMakeLists.txt" || path.filename() == "Makefile" ||
        path.filename() == "WORKSPACE" || path.filename() == "BUILD") {
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

void createQtCreatorFiles() {
    auto path = filesystem::absolute(filesystem::current_path());

    auto projectName = filesystem::path{"." + path.filename().string()};

    std::cout << "init project " << projectName << "\n";

    createQtUserFile(projectName.string());

    if (filesystem::exists(projectName.string() + ".creator")) {
        std::cerr << "found " << projectName.string() << ".creator"
                  << " the project seems to exist\n";
        exit(1);
    }

    std::ofstream{projectName.string() + ".creator"} << "[General]\n";

    std::ofstream{projectName.string() + ".cxxflags"} << "-std=c++17\n";

    {
        auto configFile = std::ofstream{projectName.string() + ".config"};
        configFile << "// add macros here like\n"
                      "#define X 43 // ;)\n";
    }

    {
        auto filesFile = std::ofstream{projectName.string() + ".files"};

        for (auto &it : filesystem::recursive_directory_iterator{path}) {
            if (isShouldInclude(it.path())) {
                filesFile << filesystem::relative(it.path(), path).string()
                          << "\n";
            }
        }
    }

    {
        auto includeFile = std::ofstream{projectName.string() + ".includes"};

        for (auto &it : filesystem::recursive_directory_iterator{path}) {
            if (filesystem::is_directory(it.path())) {
                if (it.path().filename() == "include" ||
                    it.path().filename() == "src") {
                    includeFile
                        << filesystem::relative(it.path(), path).string()
                        << "\n";
                }
            }
        }
    }
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
        createQtCreatorFiles();
    }

    if (settings.shouldInitGit) {
        return std::system("git init");
    }
}
