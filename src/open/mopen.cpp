#include "opensettings.h"
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

    openWithNohup(command);
    std::cout << "opening with qtcreator: " << projectFile << std::endl;
}

void openVsCode(std::filesystem::path path) {
    std::filesystem::current_path(path);
    std::system("code .");
}

void openKiCad(std::filesystem::path path) {
    openWithNohup("kicad '" + path.string() + "'");
}

bool tryOpenKiCad(int &skipNum, std::filesystem::path path) {
    for (auto &path : std::filesystem::directory_iterator{path}) {
        if (path.path().extension() == ".kicad_pro") {
            if (skipNum > 0) {
                --skipNum;
                std::cout << "skipping " << path.path() << "\n";
                continue;
            }
            openKiCad(path);
            return true;
        }
    }
    return false;
}

bool tryOpenGoLang(int &skipNum, std::filesystem::path dir) {
    for (auto &path : std::filesystem::directory_iterator{dir}) {
        if (path.path().extension() == ".go") {
            if (skipNum > 0) {
                --skipNum;
                std::cout << "skipping " << path.path() << "\n";
                continue;
            }
            openVsCode(dir);
            return true;
        }
    }

    return false;
}

bool tryOpen(int &skipNum,
             const OpenSettings &settings,
             std::filesystem::path path) {
    for (auto &path : std::filesystem::directory_iterator{path}) {
        if (path.path().extension() == ".creator") {
            if (skipNum > 0) {
                --skipNum;
                std::cout << "skipping " << path.path() << "\n";
                continue;
            }
            openQtCreator(path.path());
            return true;
        }
    }

    for (auto &path : std::filesystem::directory_iterator{path}) {
        if (path.path().filename() == "platformio.ini" ||
            path.path().filename() == "index.html" ||
            path.path().filename() == "index.php") {

            if (skipNum > 0) {
                --skipNum;
                std::cout << "skipping " << path.path() << "\n";
                continue;
            }
            openVsCode(path.path().parent_path());
            return true;
        }
    }

    if (tryOpenKiCad(skipNum, path)) {
        return true;
    }
    for (auto &path : std::filesystem::directory_iterator{path}) {
        if (path.is_directory()) {
            // The actual project is often in a subdirectory
            if (tryOpenKiCad(skipNum, path)) {
                return true;
            }
        }
    }

    if (tryOpenGoLang(skipNum, path)) {
        return true;
    }

    return false;
}

} // namespace

int main(int argc, char *argv[]) {
    auto settings = OpenSettings{argc, argv};
    int skipNum = settings.skipNum;

    for (auto path = std::filesystem::current_path();
         !path.empty() && path != "/";
         path = path.parent_path()) {

        if (tryOpen(skipNum, settings, path)) {
            return 0;
        }
    }

    std::cerr << "could not find any viable candidate to open\n";

    return 1;
}
