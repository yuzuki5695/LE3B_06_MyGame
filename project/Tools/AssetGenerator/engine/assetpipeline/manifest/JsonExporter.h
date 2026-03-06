#pragma once
#include <string>
#include <json.hpp>

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
    static void Save(const std::string& path,
                     const nlohmann::json& data);
};