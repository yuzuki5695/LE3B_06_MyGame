#include <iostream>
#include <vector>
#include "ResourceScanner.h"
#include "JsonExporter.h"
#include "FileInfo.h"

/// <summary>
/// アセット生成ツールのメインエントリーポイント
/// </summary>
/// <param name="argc">引数の数</param>
/// <param name="argv">argv[1]: ルートディレクトリパス, argv[2]: 出力ファイルパス</param>
int main(int argc, char* argv[])
{
    // --- 1. 引数チェックとパスの設定 ---
    // 引数が足りない場合はデフォルト値を使用
    std::string resourceDir = (argc > 1) ? argv[1] : "Resources";
    std::string outputJson  = (argc > 2) ? argv[2] : "Resources/manifest.json";

    std::cout << "[AssetGenerator] Scanning directory: " << resourceDir << std::endl;

    // --- 2. リソースの走査 ---
    // 指定されたディレクトリ配下を再帰的にスキャンし、FileInfoのリストを取得
    ResourceScanner scanner(resourceDir);
    std::vector<FileInfo> files = scanner.ScanFile();

    if (files.empty()) {
        std::cout << "[AssetGenerator] Warning: No valid assets found." << std::endl;
    }

    // --- 3. マニフェストの構築と書き出し ---
    // JsonExporter クラスを使用して、ファイルリストを JSON 構造に変換し保存する
    JsonExporter exporter;
    
    // 内部でパスの分解や階層構造の構築を行う
    nlohmann::json manifest = exporter.Build(files);

    // インデントを付けてファイルへ保存
    JsonExporter::Save(outputJson, manifest);

    std::cout << "[AssetGenerator] Success! " << files.size() << " assets indexed." << std::endl;
    std::cout << "[AssetGenerator] Output: " << outputJson << std::endl;

    return 0;
}