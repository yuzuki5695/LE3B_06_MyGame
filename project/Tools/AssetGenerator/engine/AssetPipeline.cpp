#include "AssetPipeline.h"
#include <PathManager.h>
#include <iostream>
#include <ResourceScanner.h>

void AssetPipeline::Run(int argc, char* argv[]) {
	Initialize(argc, argv); // 初期化処理
	Execute();              // 出力処理
	Finalize();             // 解放処理
}

void AssetPipeline::Initialize(int argc, char* argv[]) {
    // コンソールの日本語表示対応
    std::locale::global(std::locale("japanese"));

    // パスマネージャを使用して Resources フォルダを特定
    resourcesRoot_ = PathManager::FindResourcesRoot(argc, argv);    
    // エクスポーターの生成
    exporter_ = std::make_unique<JsonExporter>();
    // 出力ファイル名の初期化
    manifestFileName_ = L"manifest.json";
    // フォルダが見つからない場合は致命的エラーとして停止
    if (resourcesRoot_.empty()) {
        throw std::runtime_error("Resources root directory could not be found.");
    }
}

void AssetPipeline::Execute() {
    // 処理時間の計測開始
    const std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    // Resources 内の全ファイルを再帰的にスキャン
    ResourceScanner scanner;
    std::vector<AssetEntry> assets = scanner.Scan(resourcesRoot_, manifestFileName_);

    // 実行日の取得 (YYYY/MM/DD)
    const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm;
    localtime_s(&now_tm, &now_c);

    std::stringstream dateStream;
    dateStream << std::put_time(&now_tm, "%Y/%m/%d");

    // 処理時間の計測終了
    const std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;

    // 取得したデータを manifest.json に書き出し
    std::filesystem::path outputPath = resourcesRoot_ / manifestFileName_;
    exporter_->Export(outputPath, dateStream.str(), diff.count(), assets);

    // 完了ログを表示
    std::wcout << L"[AssetPipeline] Generated " << manifestFileName_ << L" for " << assets.size() << L" assets." << std::endl;
}

void AssetPipeline::Finalize() {
    // インスタンスの破棄と終了通知
    exporter_.reset();
    std::wcout << L"[AssetPipeline] Finalize completed." << std::endl;
}