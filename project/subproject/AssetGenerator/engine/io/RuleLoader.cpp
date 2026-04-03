#include "RuleLoader.h"
#include <fstream>
#include <json.hpp>

using namespace AssetGen;

namespace fs = std::filesystem;

RuleLoader::RuleLoader(const fs::path& resourceRoot, const fs::path& outputDir)
    : resourceRoot_(resourceRoot), outputDir_(outputDir) {}

bool RuleLoader::Load(const fs::path& jsonPath) {
    Clear();

    std::ifstream ifs(jsonPath);
    if (!ifs) {
        return false; // ファイルが開けない場合は失敗を返す
    }

    try {
        nlohmann::json j;
        ifs >> j;

        for (const nlohmann::json& item : j["rules"]) {
            ExportRule rule;

            // ディレクトリの絶対パスを構築
            std::string dir = item["dir"];
            rule.dir = resourceRoot_ / dir;

            // 出力先の絶対パスを構築
            std::string output = item["output"];
            rule.output = outputDir_ / output;

            // 拡張子の変換 (std::string -> std::wstring)
            for (const nlohmann::json& ext : item["ext"]) {
                std::string e = ext;
                rule.extensions.push_back(std::wstring(e.begin(), e.end()));
            }

            rules_.push_back(rule);
        }
    }
    catch (...) {
        // パースエラー等が発生した場合
        return false;
    }

    return true;
}

void RuleLoader::Clear() {
    rules_.clear();
}