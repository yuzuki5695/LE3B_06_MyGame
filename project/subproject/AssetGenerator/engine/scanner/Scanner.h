#pragma once
#include <filesystem>
#include <vector>
#include <string>
#include <functional>

struct ExportRule {
    std::filesystem::path targetDir;           // スキャン対象
    std::vector<std::wstring> extensions;      // 対象拡張子
    std::filesystem::path outputPath;          // 出力ファイル
};

/// <summary>
/// アセットの情報を保持する構造体
/// Resources フォルダ内のファイル・ディレクトリの情報を1件分保持する
/// </summary>
struct AssetEntry {
    std::wstring path;      /// Resources からの相対パス
    std::wstring fileName;  /// ファイル名（拡張子込み）
    bool isDirectory;       /// ディレクトリかどうか
    std::wstring extension; /// 拡張子（例: .png）
};

/// <summary>
/// Resources ディレクトリを走査し、アセット情報を取得するクラス
/// ファイル収集のみを責務とし、分類や出力は行わない
/// </summary>
class Scanner {
public: // メンバ関数

    /// <summary>
    /// 指定ディレクトリ以下を再帰的に走査し、各ファイル情報をコールバックで返す
    /// </summary>
    /// <param name="root">スキャン開始ディレクトリ（通常はResources）</param>
    /// <param name="func">各アセットに対して実行される処理</param>
    void ResourceScan(const std::filesystem::path& root, std::function<void(const AssetEntry&)> func);

    void ScanAll(const std::filesystem::path& root, std::vector<AssetEntry>& out);

    void ScanByExtension(const std::filesystem::path& root, const std::wstring& ext, std::vector<AssetEntry>& out);
};