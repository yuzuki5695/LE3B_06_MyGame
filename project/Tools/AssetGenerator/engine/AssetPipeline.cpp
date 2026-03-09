#include "AssetPipeline.h"
#include <PathManager.h>
#include <iostream>

void AssetPipeline::Run(int argc, char* argv[]) {
    Initialize(argc, argv);
    Execute();
    Finalize();
}

void AssetPipeline::Initialize(int argc, char* argv[]) {
    // コンソールの日本語表示対応
    std::locale::global(std::locale("japanese"));

    // パスマネージャを使用して Resources フォルダを特定
    resourcesRoot_ = PathManager::FindResourcesRoot(argc, argv);
    
    // エクスポーターの生成
    exporter_ = std::make_unique<JsonExporter>();

    if (resourcesRoot_.empty()) {
        throw std::runtime_error("Resources root directory could not be found.");
    }
}

void AssetPipeline::Execute() {
    // 出力ファイルパスの決定
    std::filesystem::path outputPath = resourcesRoot_ / L"manifest.json";

    // JSON の書き出し実行
    exporter_->Export(outputPath);

    // ログ出力（ワイド文字対応）
    std::wcout << L"[AssetPipeline] Generated: " << outputPath.wstring() << std::endl;
}

void AssetPipeline::Finalize() {
    // メンバのクリア
    exporter_.reset();
    std::wcout << L"[AssetPipeline] Finalize completed." << std::endl;
}