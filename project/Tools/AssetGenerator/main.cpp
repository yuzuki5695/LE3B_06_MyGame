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
        std::cout << "Locale setting failed. Proceeding with default locale." << std::endl;
    }

    // 1. プロジェクトルートパスの決定
    std::string pathStr = (argc > 1) ? argv[1] : "../../";

    // Windowsの末尾引用符バグ対策
    if (!pathStr.empty() && pathStr.back() == '\"') pathStr.pop_back();
    if (!pathStr.empty() && pathStr.back() == '.') pathStr.pop_back();

    fs::path projectRoot = fs::absolute(pathStr); 
    
    // 2. 入出力ディレクトリの定義
    fs::path resourceDir = projectRoot / "Resources";
    fs::path jsonOutputDir = resourceDir / "levels"; // 出力先をlevelsに変更
    fs::path jsonPath = jsonOutputDir / "Manifest.json";

    fs::path headerDir = projectRoot / "Tools/AssetGenerator/engine/math";
    fs::path headerPath = headerDir / "LoadResourceID.h";

    // 3. ディレクトリの自動作成
    if (!fs::exists(jsonOutputDir)) fs::create_directories(jsonOutputDir);
    if (!fs::exists(headerDir)) fs::create_directories(headerDir);

    try {
        std::cout << "--- Asset Generator Start ---" << std::endl;

        // ① リソースファイルの検索
        FileSearcher searcher(resourceDir.string());
        // "levels" フォルダそのものを検索対象から除外（自分自身をスキャンしない）
        std::vector<std::string> exclude = { ".git", ".vs", "Manifest.json", "levels" };
        auto files = searcher.GetAllFiles(exclude);

        // ② アセット目録 (Manifest.json) の書き出し
        ManifestExporter exporter;
        exporter.Export(jsonPath.string(), files);
        std::cout << "[Success] JSON: " << jsonPath << std::endl;

        // ③ ヘッダーの自動生成
        ResourceIdGenerator idGen;
        if (idGen.LoadFromJson(jsonPath.string())) {
            ResourceCodeGenerator codeGen;
            codeGen.GenerateHeader(headerPath.string(), idGen.GetResources());
            std::cout << "[Success] Header: " << headerPath << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[Error] " << e.what() << std::endl;
        return 1;
    }
    return 0;
}