#pragma once
#include <string>
#include <json.hpp>
#include <FileInfo.h>

/// <summary>
/// JSONデータをファイルとして外部へ書き出すエクスポートクラス
/// </summary>
class JsonExporter
{
public:
    /// <summary>
    /// 指定したパスにJSONデータを整形して保存する
    /// </summary>
    /// <param name="path">保存先のファイルフルパス</param>
    /// <param name="data">書き出すJSONオブジェクト</param>
    static void Save(const std::string& path, const nlohmann::json& data);

    /// <summary>
    /// アセット情報のリストをJSON形式のマニフェストに変換する
    /// </summary>
    /// <param name="files">スキャン済みのファイル情報リスト</param>
    /// <returns>構築されたJSONデータ</returns>
    nlohmann::json Build(const std::vector<FileInfo>& files);

};