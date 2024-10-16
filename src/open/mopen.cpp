#include <cstdlib>
#include <filesystem>
#include <iostream>

namespace {

void openWithNohup(std::string command) {
    std::system(("nohup " + command + " > /dev/null 2>&1&").c_str());
}

void openQtCreator(std::filesystem::path projectFile) {
    auto bin = std::filesystem::path{"/opt/Qt/Tools/QtCreator/bin/qtcreator"};
    auto command = bin.string() + " '" + projectFile.string() + "'";

    // openWithNohup(command);
    std::cout << projectFile << std::endl;
}

void openVsCode(std::filesystem::path path) {
    std::filesystem::current_path(path);
    std::system("code .");
}

void openKiCad(std::filesystem::path path) {
    openWithNohup("kicad '" + path.string() + "'");
}

bool tryOpenKiCad(std::filesystem::path path) {
    for (auto &path : std::filesystem::directory_iterator{path}) {
        if (path.path().extension() == ".kicad_pro") {
            openKiCad(path);
            return true;
        }
    }
    return false;
}

bool tryOpen(std::filesystem::path path) {
    for (auto &path : std::filesystem::directory_iterator{path}) {
        if (path.path().extension() == ".creator") {
            openQtCreator(path.path());
            return true;
        }
    }

    for (auto &path : std::filesystem::directory_iterator{path}) {
        if (path.path().filename() == "platformio.ini" ||
            path.path().filename() == "index.html" ||
            path.path().filename() == "index.php") {
            openVsCode(path.path().parent_path());
            return true;
        }
    }

    tryOpenKiCad(path);
    for (auto &path : std::filesystem::directory_iterator{path}) {
        if (path.is_directory()) {
            // The actual project is often in a subdirectory
            tryOpenKiCad(path);
        }
    }

    return false;
}

} // namespace

int main(int argc, char *argv[]) {
    for (auto path = std::filesystem::current_path();
         !path.empty() && path != "/";
         path = path.parent_path()) {

        if (tryOpen(path)) {
            return 1;
        }
    }

    return 0;
}
