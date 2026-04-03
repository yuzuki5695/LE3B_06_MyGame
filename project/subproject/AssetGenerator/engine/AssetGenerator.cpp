#include "AssetGenerator.h"
#include <ResourceMapper.h>
#include <RuleLoader.h>
#include <iostream>
#include <CodeGenerator.h>
#include <clocale>

using namespace AssetGen;

namespace fs = std::filesystem;

void AssetGenerator::Run(int argc, char* argv[]) {
    try {
        // 例外をキャッチできるように一連の流れを実行
        Initialize(argc, argv);
        Execute();
        Finalize();
    }
    catch (const std::exception& e) {
        // 実行中に発生したエラーを標準エラー出力に表示
        std::cerr << "エラー: " << e.what() << std::endl;
    }
}

void AssetGenerator::Initialize(int argc, char* argv[]) {
    std::setlocale(LC_ALL, "japanese");

    fs::path rootPath;

    if (argc > 1) {
        // ビルド前イベントから渡された "$(SolutionDir)..\"
        rootPath = fs::absolute(argv[1]).lexically_normal();
    } else {
        // VS等での直接実行デバッグ用 (generated/outputs/Debug から 3つ遡る)
        rootPath = fs::absolute(fs::current_path() / "../../../").lexically_normal();
    }

    rootPath = fs::absolute(rootPath).lexically_normal();

    std::cout << "[AssetGenerator] Root: " << rootPath.string() << std::endl;

    // 1. リソース（データの入出力）
    resourceRoot_ = rootPath / "Resources";
    outputDir_ = resourceRoot_ / "Data/Resource";
    rulesJsonPath_ = outputDir_ / "Rules.json";
    assetInfoPath_ = rootPath / "subproject/AssetGenerator/engine/generator";

    // 念のため、書き出し先のフォルダが存在するかチェックし、なければ作る
    if (!fs::exists(assetInfoPath_)) {
        fs::create_directories(assetInfoPath_);
    }
    // 各機能コンポーネントをインスタンス化
    ruleLoader_ = std::make_unique<RuleLoader>(resourceRoot_, outputDir_);
    assetLoader_ = std::make_unique<AssetInfoLoader>();
}

void AssetGenerator::Execute() {
    // 1. ルール設定の読み込み
    // どのフォルダをスキャンし、どのJSONに出力するかを決定する
    if (!ruleLoader_->Load(rulesJsonPath_)) {
        std::cerr << "エラー: Rules.json の読み込みに失敗しました。パス: " << rulesJsonPath_ << std::endl;
        return;
    }

    // 2. ディレクトリ走査と中間JSONの更新
    // 物理ファイルの状態を確認し、Textures.json 等を最新状態にする
    ResourceMapper mapper(resourceRoot_, outputDir_);
    mapper.UpdateByRules(ruleLoader_->GetRules());

    // 3. ID定義の集計
    // 手順2で生成・更新された各JSONを巡回し、プログラムで使用するIDリストをメモリ上に構築する
    assetLoader_->Clear(); // 既存のリストをクリア
    for (const auto& rule : ruleLoader_->GetRules()) {
        // ルールに基づき、出力されたJSONファイル（例：Models.json）を特定
        fs::path generatedJsonPath = outputDir_ / rule.output;

        // 階層構造を持つJSONを解析し、AssetDefinitionとして蓄積
        assetLoader_->LoadGeneratedJson(generatedJsonPath);
    }

    // 4. C++コード（ヘッダファイル）の自動生成
    // 全てのアセット情報から、プログラムで利用するID定数群を出力する
    CodeGenerator::GenerateResourceIDHeader(assetLoader_->GetAssets(), assetInfoPath_ / "LoadResourceID.h");
}

void AssetGenerator::Finalize() {
    std::cout << "処理が完了しました。" << std::endl;
}