#include <iostream>
#include <filesystem>
#include <FileSearcher.h>
#include <ManifestExporter.h>
#include <ResourceIdGenerator.h>
#include <ResourceCodeGenerator.h>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    try {
        // 日本語環境がある場合のみ設定（CI対策）
        std::locale::global(std::locale("japanese"));
    }
    catch (...) {
        // 失敗しても何もしない（CI環境など）
        std::cout << "Locale setting failed. Proceeding with default locale." << std::endl;
    }
    // ...
    // 1. プロジェクトルートパスの決定
    // 引数がない場合は実行環境からの相対パスを使用
    std::string pathStr = (argc > 1) ? argv[1] : "../../";

    // Windowsの末尾引用符バグ対策 ---
    if (!pathStr.empty() && pathStr.back() == '\"') {
        pathStr.pop_back(); // 混入した " を削除
    }
    // 末尾が \. で終わるように指定された場合の対策
    if (!pathStr.empty() && pathStr.back() == '.') {
        pathStr.pop_back();
    }
    // ------------------------------------------
    fs::path projectRoot = fs::absolute(pathStr); // 絶対パスに変換して安定させる
    // 2. 入出力ディレクトリ・ファイルパスの定義
    fs::path resourceDir = projectRoot / "Resources";
    fs::path jsonPath = resourceDir / "Manifest.json";

    fs::path headerDir = projectRoot / "Tools/AssetGenerator/engine/math";
    fs::path headerPath = headerDir / "LoadResourceID.h";

    // 出力先ディレクトリの自動作成
    if (!fs::exists(headerDir)) {
        fs::create_directories(headerDir);
    }

    try {
        std::cout << "--- Asset Generator Start ---" << std::endl;

        // ① リソースファイルの検索
        // Manifest.json 自体は検索対象から除外する
        FileSearcher searcher(resourceDir.string());
        std::vector<std::string> exclude = { ".git", ".vs", "Manifest.json" };
        auto files = searcher.GetAllFiles(exclude);

        // ② アセット目録 (Manifest.json) の書き出し
        // 変更がない場合はファイル更新（タイムスタンプ変更）を行わない
        ManifestExporter exporter;
        exporter.Export(jsonPath.string(), files);
        std::cout << "[Success] JSON created at: " << jsonPath << std::endl;

        // ③ JSONの解析とC++定数定義ヘッダーの自動生成
        ResourceIdGenerator idGen;
        if (idGen.LoadFromJson(jsonPath.string())) {
            ResourceCodeGenerator codeGen;
            codeGen.GenerateHeader(headerPath.string(), idGen.GetResources());
            std::cout << "[Success] Header created at: " << headerPath << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[Error] " << e.what() << std::endl;
        return 1;
    }
    return 0;
}