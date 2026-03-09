#pragma once
#include <filesystem>

/// <summary>
/// 取得したアセット情報等を JSON 形式でファイルに書き出すためのエクスポートクラス
/// </summary>
class JsonExporter {
public: // メンバ関数
    /// <summary>
    /// 指定されたパスに対して、アセットリスト（現在はテスト用データ）を書き出す
    /// </summary>
    /// <param name="path">書き出し先のファイルパス（例: manifest.json）</param>
    void Export(const std::filesystem::path& path);
};