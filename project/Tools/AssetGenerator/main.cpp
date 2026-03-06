#include <iostream>
#include "ResourceScanner.h"
#include "ManifestBuilder.h"
#include "JsonExporter.h"

/// <summary>
/// アセット生成ツールのメインエントリーポイント
/// リソースの走査、マニフェストの構築、ファイル出力の一連の流れを制御する。
/// </summary>
/// <returns>終了ステータス（成功時 0）</returns>
int main()
{
    // --- 設定定数 ---
    const std::string kResourceDirectory = "Resources";      // 走査対象のルートディレクトリ
    const std::string kOutputManifestPath = "Resources/fest.json"; // 出力先パス

    // 1. リソースの走査
    // 指定されたディレクトリからファイル一覧とアセット情報を取得する
    ResourceScanner scanner(kResourceDirectory);
    auto files = scanner.Scan();

    // 2. マニフェストデータの構築
    // 取得したファイル情報をエンジンが解釈可能な階層構造（JSON）に変換する
    ManifestBuilder builder;
    auto manifest = builder.Build(files);

    // 3. ファイルへの書き出し
    // 構築されたJSONデータを指定のパスに保存する
    JsonExporter::Save(kOutputManifestPath, manifest);

    // 完了通知
    std::cout << "Asset Manifest successfully generated at: " << kOutputManifestPath << std::endl;

    return 0;
}