#pragma once
#include <filesystem>
#include <vector>
#include <string>

struct ExportRule {
    std::filesystem::path dir;                 // スキャン対象
    std::vector<std::wstring> extensions;      // 拡張子
    std::filesystem::path output;              // 出力先
};

class RuleLoader {
public: // メンバ関数
    /// <summary>
    /// rules.json を読み込み、ExportRuleの配列に変換する
    /// </summary>
    static std::vector<ExportRule> LoadFromJson(const std::filesystem::path& jsonPath, const std::filesystem::path& resourceRoot, const std::filesystem::path& outputDir);

};