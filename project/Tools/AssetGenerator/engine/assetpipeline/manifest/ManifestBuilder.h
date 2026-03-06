#pragma once
#include <vector>
#include <FileInfo.h>
#include <json.hpp>

/// <summary>
/// 収集されたアセット情報を基に、エンジンが読み込めるマニフェスト（JSON）を構築するクラス
/// </summary>
class ManifestBuilder {
public: // メンバ関数
    /// <summary>
    /// アセット情報のリストをJSON形式のマニフェストに変換する
    /// </summary>
    /// <param name="files">スキャン済みのファイル情報リスト</param>
    /// <returns>構築されたJSONデータ</returns>
    nlohmann::json Build(const std::vector<FileInfo>& files);
};