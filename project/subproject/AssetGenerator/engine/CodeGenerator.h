#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include "AssetInfoLoader.h"

/// <summary>
/// C++コード（ヘッダファイル）の自動生成を担当するクラス
/// </summary>
class CodeGenerator {
public:
    /// <summary>
    /// AssetInfoLoaderのデータから LoadResourceID.h を生成する
    /// </summary>
    /// <param name="assets">生成元となるアセット定義リスト</param>
    /// <param name="outputPath">出力するヘッダのパス</param>
    static void GenerateResourceIDHeader(const std::vector<AssetDefinition>& assets, const std::filesystem::path& outputPath);
};