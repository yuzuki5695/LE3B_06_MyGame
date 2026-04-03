#pragma once
#include <filesystem>
#include <vector>
#include <RuleLoader.h>
#include <AssetInfoLoader.h>

class AssetGenerator {
public:
    void Run(int argc, char* argv[]);

private:
    void Initialize(int argc, char* argv[]);
    void Execute();
    void Finalize();

private:
    std::filesystem::path resourceRoot_;
    std::filesystem::path outputDir_;
    std::filesystem::path rulesJsonPath_;
    std::filesystem::path assetInfoJsonPath_; // 追加：ID定義用のJSONパス

    std::unique_ptr<RuleLoader> ruleLoader_;      // インスタンス管理に変更
    std::unique_ptr<AssetInfoLoader> assetLoader_; // 追加

    std::vector<ExportRule> rules_;
};