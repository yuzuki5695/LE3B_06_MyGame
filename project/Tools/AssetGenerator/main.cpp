#include <iostream>
#include <filesystem>
#include <FileSearcher.h>
#include <ManifestExporter.h>
#include <ResourceIdGenerator.h>
#include <ResourceCodeGenerator.h>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    try { std::locale::global(std::locale("japanese")); } catch (...) {}

    // --- 1. 絶対パスの確定 ---
    fs::path projectRoot;
    
    // 実行ファイルの場所を取得
    fs::path exePath = fs::absolute(argv[0]);
    fs::path current = exePath.parent_path();
    bool found = false;

    // 「Resources」フォルダが見つかるまで、最大15階層親へ遡る
    for (int i = 0; i < 15; ++i) {
        if (fs::exists(current / "Resources")) {
            projectRoot = current;
            found = true;
            break;
        }
        if (!current.has_parent_path()) break;
        current = current.parent_path();
    }

    // 万が一見つからなかった場合は引数やデフォルトに頼る
    if (!found) {
        projectRoot = fs::absolute((argc > 1) ? argv[1] : "../../");
    }

    // 2. パスを強制固定
    fs::path resourceDir = projectRoot / "Resources";
    fs::path jsonOutputDir = resourceDir / "levels"; 
    fs::path jsonPath = jsonOutputDir / "Manifest.json"; 

    // 重要：今どこをルートにしているか、ビルドログに出力して確認できるようにする
    std::cout << "[AssetGenerator] Exe Path: " << exePath << std::endl;
    std::cout << "[AssetGenerator] Project Root: " << projectRoot << std::endl;
    std::cout << "[AssetGenerator] Target Path: " << jsonPath << std::endl;

    // 3. フォルダの強制作成
    if (!fs::exists(jsonOutputDir)) {
        fs::create_directories(jsonOutputDir);
    }

    try {
        // ① 検索
        FileSearcher searcher(resourceDir.string());
        // levelsフォルダ内を除外
        std::vector<std::string> exclude = { ".git", ".vs", "levels", "Manifest.json", "manifest.json" };
        auto files = searcher.GetAllFiles(exclude);

        // ② 書き出し
        ManifestExporter exporter;
        // 絶対パス jsonPath を使うことで、exeと同じ場所にできる不具合を物理的に防ぐ
        exporter.Export(jsonPath.string(), files);

        // ③ ヘッダー生成
        ResourceIdGenerator idGen;
        if (idGen.LoadFromJson(jsonPath.string())) {
            fs::path headerPath = projectRoot / "Tools/AssetGenerator/engine/math/LoadResourceID.h";
            if(!fs::exists(headerPath.parent_path())) fs::create_directories(headerPath.parent_path());
            
            ResourceCodeGenerator codeGen;
            codeGen.GenerateHeader(headerPath.string(), idGen.GetResources());
        }
        std::cout << "[Success] Generation complete." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "[Error] " << e.what() << std::endl;
        return 1;
    }
    return 0;
}