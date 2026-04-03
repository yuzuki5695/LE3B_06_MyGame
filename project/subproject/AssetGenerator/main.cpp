#include <iostream>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

// ファイル全体を文字列として読む関数
std::string ReadAllText(const fs::path& path) {
    std::ifstream ifs(path);
    if (!ifs) return "";

    std::stringstream buffer;
    buffer << ifs.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {

    // 実行ファイルのカレントディレクトリ
    fs::path current = fs::current_path();

    // 出力先のフォルダ
    fs::path resourcePath = current / "../../Resources/Data/Resource";

    // 引数があれば上書き
    if (argc > 1) {
        resourcePath = fs::path(argv[1]);
    }

    // 出力先の名称
    fs::path outputPath = resourcePath / "Test.json";

    // ディレクトリが無ければ作る
    fs::create_directories(outputPath.parent_path());

    std::string newJson = R"({
    "name": "Player",
    "hp": 100,
    "atk": 50
})";

    // 既存ファイルがあれば読む
    std::string oldJson;
    if (fs::exists(outputPath)) {
        oldJson = ReadAllText(outputPath);
    }

    // 同じならスキップ
    if (oldJson == newJson) {
        std::cout << "変更なしのためスキップ: " << outputPath << std::endl;
        return 0;
    }

    // 書き込み
    std::ofstream ofs(outputPath);
    if (!ofs) {
        std::cerr << "ファイル作成失敗: " << outputPath << std::endl;
        return 1;
    }

    ofs << newJson;
    ofs.close();

    std::cout << "更新しました: " << outputPath << std::endl;
    std::cout << "出力成功: " << outputPath << std::endl;

    return 0;
}