#pragma once
#include <string>
#include "FileInfo.h"

/// <summary>
/// ファイル拡張子からアセットの種類を判定する静的クラス
/// </summary>
class AssetTypeDetector {
public: // メンバ関数
    /// <summary>
    /// 拡張子文字列から対応するAssetTypeを判定する
    /// </summary>
    /// <param name="extension">判定対象の拡張子（例: ".png"）</param>
    /// <returns>判定されたAssetType（不明な場合はUnknown）</returns>
    static AssetType Detect(const std::string& extension);
};