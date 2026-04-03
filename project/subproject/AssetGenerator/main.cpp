#include "ResourceMapper.h"
#include <iostream>
#include <filesystem>
#include <RuleLoader.h>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    // 1. パスの設定
    // 実行環境に合わせてリソースのルートと出力先を定義します
    fs::path current = fs::current_path();
    // スキャン対象のルートディレクトリ
    fs::path resourceRoot = current / "../../Resources";
    // JSONの出力先ディレクトリ
    fs::path outputDir = current / "../../Resources/Data/Resource";
    // ルール設定ファイルのパス
    fs::path rulesJsonPath = current / "../../Resources/Data/Resource/Rules.json"; // main.cppと同じ階層にある場合
    // コマンドライン引数がある場合は出力先を上書き
    if (argc > 1) {
        outputDir = fs::path(argv[1]);
    }

    // 最終的な出力ファイルパス
    fs::path outputPath = outputDir / "TextureList.json";

    try {
        std::cout << "ルール読み込み中: " << rulesJsonPath.string() << std::endl;
        std::vector<ExportRule> rules = RuleLoader::LoadFromJson(rulesJsonPath, resourceRoot, outputDir);
        // 2. ResourceMapperのインスタンス化
        // 責務を分離したことで、main側は「どこをスキャンしてどこに出すか」を伝えるだけになります
        ResourceMapper mapper(resourceRoot, outputPath);

        // 3. 実行
        std::cout << "スキャン開始: " << resourceRoot.string() << std::endl;

        mapper.UpdateByRules(rules);
 
        // 完了メッセージなどはMapper側、もしくは成功後にここで出す
        std::cout << "処理が完了しました。" << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "エラーが発生しました: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}