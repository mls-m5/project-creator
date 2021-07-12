

#include <filesystem>
#include <fstream>
#include <iostream>

namespace filesystem = std::filesystem;

bool isShouldInclude(filesystem::path path) {
    auto ext = path.extension();

    if (ext == ".cpp" || ext == ".cxx" || ext == ".c") {
        return true;
    }

    if (ext == ".h" || ext == ".hpp" || ext == ".hxx") {
        return true;
    }

    if (path.filename() == ".clang-format" || path.filename() == ".gitignore") {
        return true;
    }

    if (path.filename() == "Matmakefile" ||
        path.filename() == "CMakeLists.txt" || path.filename() == "Makefile" ||
        path.filename() == "WORKSPACE" || path.filename() == "BUILD") {
        return true;
    }

    return false;
}

int main(int argc, char **argv) {
    auto path = filesystem::absolute(filesystem::current_path());

    auto projectName = filesystem::path{"." + path.filename().string()};

    std::cout << "init project " << projectName << "\n";

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

        for (auto it : filesystem::recursive_directory_iterator{path}) {
            if (isShouldInclude(it.path())) {
                filesFile << filesystem::relative(it.path(), path).string()
                          << "\n";
            }
        }
    }

    {
        auto includeFile = std::ofstream{projectName.string() + ".includes"};

        for (auto it : filesystem::recursive_directory_iterator{path}) {
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
