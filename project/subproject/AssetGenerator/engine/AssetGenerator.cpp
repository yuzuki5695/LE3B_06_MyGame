#include "AssetGenerator.h"
#include <ResourceMapper.h>
#include <RuleLoader.h>
#include <iostream>
#include <CodeGenerator.h>

namespace fs = std::filesystem;

void AssetGenerator::Run(int argc, char* argv[]) {
    try {
        Initialize(argc, argv);
        Execute();
        Finalize();
    }
    catch (const std::exception& e) {
        std::cerr << "エラー: " << e.what() << std::endl;
    }
}

void AssetGenerator::Initialize(int argc, char* argv[]) {
    fs::path current = fs::current_path();

    resourceRoot_ = current / "../../Resources";
    outputDir_    = current / "../../Resources/Data/Resource";
    rulesJsonPath_ = outputDir_ / "Rules.json";
    assetInfoJsonPath_ = outputDir_ / "AssetInfo.json";
    // ローダーの生成
    ruleLoader_  = std::make_unique<RuleLoader>(resourceRoot_, outputDir_);
    assetLoader_ = std::make_unique<AssetInfoLoader>();
}

void AssetGenerator::Execute() {
    // 1. ルールの読み込み
    if (!ruleLoader_->Load(rulesJsonPath_)) {
        std::cerr << "Failed to load rules." << std::endl;
        return;
    }

    // 2. リソースID定義の読み込み
    if (!assetLoader_->Load(assetInfoJsonPath_)) {
        std::cout << "AssetInfo.json not found. Skipping ID generation." << std::endl;
    }

    // 3. 従来のリソースマッピング（ディレクトリ走査）処理
    ResourceMapper mapper(resourceRoot_, outputDir_);
    mapper.UpdateByRules(ruleLoader_->GetRules());

    // 4. C++ヘッダの自動生成
    CodeGenerator::GenerateResourceIDHeader(assetLoader_->GetAssets(), outputDir_ / "LoadResourceID.h");
}

void AssetGenerator::Finalize() {
    std::cout << "処理が完了しました。" << std::endl;
}