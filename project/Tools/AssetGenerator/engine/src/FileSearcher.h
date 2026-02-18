#pragma once
#include <string>
#include <vector>

/// <summary>
/// 指定したルートフォルダ以下を再帰的に走査し、
/// ファイルパス一覧を取得するためのユーティリティクラス
/// </summary>
class FileSearcher {
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="root">
    /// 検索の起点となるフォルダパス（例: "Resources"）
    /// </param>
    explicit FileSearcher(const std::string& root) : root_(root) {}
    /// <summary>
    /// ルートフォルダ以下を再帰的にスキャンし、
    /// 除外ディレクトリを考慮したファイル一覧を返す
    /// </summary>
    /// <param name="excludeDirs">
    /// 除外したいディレクトリ名のリスト
    /// （例: ".git", "Temp", "EditorTemp" など）
    /// </param>
    /// <returns>
    /// root からの相対パスで構成されたファイルパス一覧
    /// </returns>
    std::vector<std::string> GetAllFiles(const std::vector<std::string>& excludeDirs);

private:
    // 検索の基準となるルートディレクトリ
    std::string root_;
};