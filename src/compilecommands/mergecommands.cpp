#include "fast-json/json.h"
#include "fast-json/jsonout.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

void findRecursive(std::filesystem::path path,
                   std::vector<std::filesystem::path> &ret,
                   bool ignoreInThisFolder = false) {
    // std::cout << "searching " << path << std::endl;
    if (path.stem() == ".git") {
        return;
    }

    if (!ignoreInThisFolder &&
        std::filesystem::exists(path / "compile_commands.json")) {
        std::cout << "found file in " << path << std::endl;
        ret.push_back(path / "compile_commands.json");
    }

    for (auto it : std::filesystem::directory_iterator{path}) {
        if (it.is_directory()) {
            findRecursive(it.path(), ret);
        }
    }
}

struct Settings {
    bool isVerbose = false;

    Settings(int argc, char *argv[]) {
        auto args = std::vector<std::string_view>{argv + 1, argv + argc};

        for (size_t i = 0; i < args.size(); ++i) {
            auto arg = args.at(i);

            if (arg == "--verbose") {
                isVerbose = true;
            }
            else if (arg.starts_with("-")) {
                std::cerr << "Invalid argument " << arg << "\n";
                std::exit(1);
            }
        }
    }
};

int main(int argc, char *argv[]) {
    const auto settings = Settings{argc, argv};
    auto list = std::vector<std::filesystem::path>{};
    findRecursive(std::filesystem::current_path(), list, true);

    if (list.empty()) {
        if (std::filesystem::exists("compile_commands.json")) {
            std::cerr << "there is only a compile_commands.json in the root "
                         "folder but nowhere else\n";
            return 0;
        }

        std::cerr << "No compile_commands.json found\n";
        return 1;
    }

    auto out = std::ofstream{"out-test.json"};

    auto outJson = json::JsonOut{out};

    for (auto l : list) {
        if (settings.isVerbose) {
            std::cout << "parsing " << l << std::endl;
        }
        auto file = std::ifstream{l};
        // std::cout << file.rdbuf();
        auto str = [&] {
            auto ss = std::stringstream{};
            ss << file.rdbuf();
            return ss.str();
        }();

        auto json = json::parse_json(str);

        for (auto element = json->children(); element;
             element = element->next()) {
            auto outElement = outJson.push_back();
            outElement["directory"] = element->at("directory");
            outElement["command"] = element->at("command");
            outElement["file"] = element->at("file");
            outElement["output"] = element->at("output");
        }

        // std::cout << "hello\n";
        // std::cout << l << std::endl;
        //
        // json::parse_json(std::string_view input) Json::load();
    }

    std::cout << "merged " << list.size() << " compilation database"
              << (list.size() > 1 ? "s" : "") << " into root folder\n";

    return 0;
}
