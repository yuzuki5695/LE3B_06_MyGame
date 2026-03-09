#include <iostream>
#include <filesystem>
#include <fstream>

int main() {
    // 出力先パス
    std::filesystem::path outputPath = "engine/test.json";

    // engineフォルダが存在しない場合作成
    std::filesystem::create_directories(outputPath.parent_path());

    // JSONファイル書き込み
    std::ofstream file(outputPath);

    if (!file)
    {
        std::cout << "ファイルを開けませんでした\n";
        return -1;
    }

    file << "{\n";
    file << "  \"name\": \"AssetGenerator\",\n";
    file << "  \"version\": 1,\n";
    file << "  \"test\": true\n";
    file << "}\n";

    file.close();

    std::cout << "engine/test.json を生成しました\n";





    return 0;
}