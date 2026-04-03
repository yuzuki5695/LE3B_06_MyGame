#pragma once
#include <BaseLoader.h>
#include <vector>

/// <summary>
/// アセットの書き出しルール
/// </summary>
struct ExportRule {
    std::filesystem::path dir;                  // スキャン対象
    std::vector<std::wstring> extensions;       // 拡張子
    std::filesystem::path output;               // 出力先
};

/// <summary>
/// rules.json を読み込み、ExportRuleとして保持するローダー
/// </summary>
class RuleLoader : public BaseLoader {
public:
    RuleLoader(const std::filesystem::path& resourceRoot, const std::filesystem::path& outputDir);

    /// <summary>
    /// rules.json を読み込み、ExportRuleの配列に変換する
    /// </summary>
    bool Load(const std::filesystem::path& jsonPath) override;

    /// <summary>
    /// 保持しているルールを破棄する
    /// </summary>
    void Clear() override;

    /// <summary>
    /// 読み込んだルールの一覧を取得する
    /// </summary>
    const std::vector<ExportRule>& GetRules() const { return rules_; }

private:
    std::vector<ExportRule> rules_;
    std::filesystem::path resourceRoot_;
    std::filesystem::path outputDir_;
};