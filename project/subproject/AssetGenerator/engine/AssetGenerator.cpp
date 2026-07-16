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
		// 例外が発生した場合はエラーメッセージを表示して終了
        std::cerr << "[AssetGenerator] Error: " << e.what() << std::endl;
    }
}

bool AssetGenerator::NeedsUpdate(const fs::path& resourceRoot, const fs::path& headerPath) {
    // 出力先のファイル（JSONやヘッダー）が存在しないなら、必ず更新が必要
    if (!fs::exists(headerPath)) return true;
    // スキャン対象のリソースフォルダが存在しない場合は処理できないのでスキップ
    if (!fs::exists(resourceRoot)) return false;
    // 出力ファイルの最終更新日時を取得
    std::filesystem::file_time_type lastHeaderUpdate = fs::last_write_time(headerPath);
    // Resources内のフォルダ自身や、中のファイルを全検査
    try {
        for (const std::filesystem::directory_entry& entry : fs::recursive_directory_iterator(resourceRoot)) {
            // フォルダ内のどれか1つでも、出力ファイルより新しいタイムスタンプを持っていれば「更新が必要」
            if (fs::last_write_time(entry.path()) > lastHeaderUpdate) {
                return true;
            }
        }
    }
    catch (const std::exception& e) {
		// ディレクトリの走査中にエラーが発生した場合は更新が必要とみなす
        std::cerr << "Warning: Exception during directory scan: " << e.what() << std::endl;
        return true;
    }
    // すべてのファイルが出力ファイルより古い＝変更なし
    return false;
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

    // パス初期化
    resourceRoot_ = rootPath / "project" / "Resources";
    outputDir_ = resourceRoot_ / "Data" / "Resource";
    rulesJsonPath_ = outputDir_ / "Rules.json";   
    assetInfoPath_ = rootPath / "project" / "Subproject" / "AssetGenerator" / "Engine" / "Generator";
    // 各機能コンポーネントをインスタンス化
    ruleLoader_ = std::make_unique<RuleLoader>(resourceRoot_, outputDir_);
    assetLoader_ = std::make_unique<AssetInfoLoader>();
}

void AssetGenerator::Execute() {
    // ルール設定の読み込み
    if (!ruleLoader_->Load(rulesJsonPath_)) {
        std::cerr << "Error: Failed to load rule file: " << rulesJsonPath_ << std::endl;
        return;
    }

    // 全体として更新が必要かここで初めてチェックする
    fs::path headerPath = assetInfoPath_ / "LoadResourceID.h";
    bool headerExists = fs::exists(headerPath);

    // 全体の更新が必要ない、かつヘッダーが既に存在していれば、各JSONのチェックまで安全にスキップして終了できる
    if (!NeedsUpdate(resourceRoot_, headerPath)) {
        std::cout << "[AssetGenerator] Everything is up-to-date. Skipping process." << std::endl;
        return;
    }

    if (!fs::exists(assetInfoPath_)) {
        fs::create_directories(assetInfoPath_);
    }

    // ディレクトリ走査と中間JSONの更新
    ResourceMapper mapper(resourceRoot_, outputDir_);

    std::cout << "[AssetGenerator] Checking for resource changes..." << std::endl;

    for (const ExportRule& rule : ruleLoader_->GetRules()) {
        // rule.dir: スキャン対象フォルダ (例: Resources/Textures)
        // rule.output: 出力先JSON (例: generated/Textures.json)

        // 出力ファイルがすでに存在し、かつ対象フォルダ内にそれより新しいファイルがなければスキップ
        if (!NeedsUpdate(rule.dir, rule.output)) {
            std::cout << " -> Skipped (No changes): " << rule.dir.filename().string() << " -> " << rule.output.filename().string() << std::endl;
            continue;
        }

        // 変更があった場合、またはJSONが存在しない場合のみ処理を実行
        std::cout << " -> Changes detected (Updating JSON): " << rule.dir.filename().string() << std::endl;
        mapper.UpdateSingle(rule.dir, rule.extensions, rule.output);
    }

    // ID定義
    // 手順2で生成・更新された各JSONを巡回し、プログラムで使用するIDリストをメモリ上に構築する
    assetLoader_->Clear(); // 既存のリストをクリア
    for (const ExportRule& rule : ruleLoader_->GetRules()) {
        // 生成されたJSONを読み込む処理(LoadGeneratedJson)を行う
        if (fs::exists(rule.output)) {
            assetLoader_->LoadGeneratedJson(rule.output);
        }
    }

    // ヘッダファイの自動生成
    // 全てのアセット情報から、プログラムで利用するID定数群を出力する
    CodeGenerator::GenerateResourceIDHeader(assetLoader_->GetAssets(), assetInfoPath_ / "LoadResourceID.h");
}

void AssetGenerator::Finalize() {
    std::cout << "[AssetGenerator] Process completed." << std::endl;
}