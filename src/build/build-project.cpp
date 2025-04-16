#include "buildsettings.h"
#include <filesystem>
#include <iostream>
#include <thread>

enum ProjectType {
    CMake,
    Matmake,
    Matmake4,
};

ProjectType projectType() {
    if (std::filesystem::exists("build.cpp")) {
        return Matmake4;
    }

    if (std::filesystem::exists("CMakeLists.txt")) {
        return CMake;
    }

    if (std::filesystem::exists("Matmakefile")) {
        return Matmake;
    }

    std::cerr << "Could not find any build system in folder\n";
    std::terminate();
}

void buildCmake(const BuildSettings &settings) {
    auto root = std::filesystem::absolute(std::filesystem::current_path());

    std::filesystem::create_directories(settings.path);
    std::filesystem::current_path(settings.path);

    std::cout << "building in " << std::filesystem::current_path() << std::endl;

    auto buildPath = std::filesystem::absolute(std::filesystem::current_path());
    auto relative = std::filesystem::relative(root, buildPath);

    if (!std::filesystem::exists("CMakeCache.txt")) {
        auto command =
            std::string{"cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \""} +
            relative.string() + "\"";

        std::cout << "running " << command << std::endl;

        if (auto r = std::system(command.c_str())) {
            exit(r);
        }
    }

    if (auto r =
            std::system(("cmake --build . -j " +
                         std::to_string(std::thread::hardware_concurrency()))
                            .c_str())) {
        exit(r);
    }

    if (settings.shouldTest) {
        if (auto r = std::system("ctest .")) {
            exit(r);
        }
    }
}

int buildMatmake4(const BuildSettings &settings) {
    auto command = std::string{"matmake4"};

    if (settings.shouldTest) {
        command += " --test";
    }

    return std::system(command.c_str());
}

int main(int argc, char *argv[]) {
    const auto settings = BuildSettings{argc, argv};
    auto type = projectType();

    switch (type) {
    case CMake:
        buildCmake(settings);
        break;
    case Matmake: {
        auto test = std::string{settings.shouldTest ? " --test" : ""};
        if (settings.isRelease) {
            return std::system(("matmake2 -t gcc" + test).c_str());
        }
        else {
            return std::system(("matmake2 -t gcc-debug" + test).c_str());
        }
    } break;
    case Matmake4:
        return buildMatmake4(settings);
    }

    return 0;
}
