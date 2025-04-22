#include "buildsettings.h"
#include <filesystem>
#include <iostream>
#include <thread>

ProjectType projectType() {
    if (std::filesystem::exists("build.cpp")) {
        return Matmake4;
    }

    if (std::filesystem::exists("CMakeLists.txt")) {
        return CMake;
    }

    if (std::filesystem::exists("Matmakefile")) {
        return Matmake2;
    }

    std::cerr << "Could not find any build system in folder\n";
    std::terminate();
}

int buildCmake(const BuildSettings &settings) {
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

        if (!settings.isRelease) {
            command += " -DCMAKE_BUILD_TYPE=Debug";
        }

        std::cout << "running " << command << std::endl;

        if (auto r = std::system(command.c_str())) {
            exit(r);
        }
    }

    if (auto r =
            std::system(("cmake --build . -j " +
                         std::to_string(std::thread::hardware_concurrency()))
                            .c_str())) {
        return r;
    }

    if (settings.shouldTest) {
        if (auto r =
                std::system("ctest . --rerun-failed --output-on-failure")) {
            return r;
        }
    }
    return 0;
}

int buildMatmake4(const BuildSettings &settings) {
    auto command = std::string{"matmake4"};

    if (settings.shouldTest) {
        command += " --test";
    }

    if (!settings.isRelease) {
        command += " -t debug";
    }

    return std::system(command.c_str());
}

int buildMatmake2(const BuildSettings &settings) {
    auto test = std::string{settings.shouldTest ? " --test" : ""};
    if (settings.isRelease) {
        return std::system(("matmake2 -t gcc" + test).c_str());
    }
    else {
        return std::system(("matmake2 -t gcc-debug" + test).c_str());
    }
}

int main(int argc, char *argv[]) {
    const auto settings = BuildSettings{argc, argv};
    auto type = settings.type;
    if (type == Default) {
        type = projectType();
    }

    switch (type) {
    case CMake:
        return buildCmake(settings);
    case Matmake2:
        return buildMatmake2(settings);
    case Matmake4:
        return buildMatmake4(settings);
    case Default:
        std::cerr << "Could not find any build system\n";
        std::exit(1);
    }

    return 0;
}
