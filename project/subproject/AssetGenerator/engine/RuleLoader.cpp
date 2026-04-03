#include "RuleLoader.h"
#include <fstream>
#include <stdexcept>
#include <json.hpp>

namespace fs = std::filesystem;

std::vector<ExportRule> RuleLoader::LoadFromJson(const fs::path& jsonPath, const fs::path& resourceRoot, const fs::path& outputDir) {
    std::vector<ExportRule> rules;

    std::ifstream ifs(jsonPath);
    if (!ifs) {
        throw std::runtime_error("rules.json が開けない");
    }

    nlohmann::json j;
    ifs >> j;

    for (const nlohmann::json& item : j["rules"]) {

        ExportRule rule;

        // ディレクトリ
        std::string dir = item["dir"];
        rule.dir = resourceRoot / dir;

        // 出力
        std::string output = item["output"];
        rule.output = outputDir / output;

        // 拡張子
        for (const nlohmann::json& ext : item["ext"]) {
            std::string e = ext;
            rule.extensions.push_back(
                std::wstring(e.begin(), e.end())
            );
        }

        rules.push_back(rule);
    }

    return rules;
}