#include "AssetGenerator.h"
#include <ResourceMapper.h>
#include <RuleLoader.h>
#include <iostream>

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

    if (argc > 1) {
        outputDir_ = fs::path(argv[1]);
    }

    std::cout << "ルール読み込み中: " << rulesJsonPath_.string() << std::endl;

    rules_ = RuleLoader::LoadFromJson(
        rulesJsonPath_,
        resourceRoot_,
        outputDir_);
}

void AssetGenerator::Execute() {
    std::cout << "スキャン開始: " << resourceRoot_.string() << std::endl;

    ResourceMapper mapper(resourceRoot_, outputDir_);
    mapper.UpdateByRules(rules_);
}

void AssetGenerator::Finalize() {
    std::cout << "処理が完了しました。" << std::endl;
}