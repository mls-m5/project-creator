#include "gitsettings.h"
#include <cstdlib>
#include <iostream>
#include <string>

void run(std::string command) {
    if (int ret = std::system(command.c_str())) {
        std::cerr << "git returned value " << ret << "\n";
        std::exit(1);
    }
}

int main(int argc, char *argv[]) {
    const auto settings = GitSettings{argc, argv};

    std::cout << "hello\n";

    run("git status");

    return 0;
}
