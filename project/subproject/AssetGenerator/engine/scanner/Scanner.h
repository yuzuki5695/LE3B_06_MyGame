#pragma once
#include <filesystem>
#include <vector>
#include <string>
#include <functional>

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
/// ディレクトリを走査してアセット情報を収集するクラス
/// 
/// 【責務】
/// ・ファイルシステムの走査
/// ・AssetEntryの生成
/// ・簡単なフィルタ（拡張子など）
/// 
/// ファイル収集のみを責務とし、分類や出力は行わない
/// </summary>
class Scanner {
public: // メンバ関数
    /// <summary>
    /// 指定ディレクトリ以下を再帰的に走査し、
    /// 各ファイル・ディレクトリの情報をコールバックで返す
    /// 
    /// 最も汎用的な関数で、他の関数はこれを利用する
    /// </summary>
    /// <param name="rootDirectory">スキャン開始ディレクトリ</param>
    /// <param name="onAssetFound">各アセットに対して実行される処理</param>
    void ResourceScan(const std::filesystem::path& rootDirectory, const std::function<void(const AssetEntry&)> onAssetFound);

    /// <summary>
    /// 指定ディレクトリ以下の全アセットを収集する
    /// フィルタなし
    /// </summary>
    /// <param name="rootDirectory">走査開始ディレクトリ</param>
    /// <param name="outEntries">取得したアセット一覧の出力先</param>
    void ScanAll(const std::filesystem::path& rootDirectory, std::vector<AssetEntry>& outEntries);

    /// <summary>
    /// 指定拡張子のファイルのみを収集する
    /// ディレクトリは除外される
    /// </summary>
    /// <param name="rootDirectory">走査開始ディレクトリ</param>
    /// <param name="extension">対象拡張子（例: L".png"）</param>
    /// <param name="outEntries">取得結果の格納先</param>
    void ScanByExtension(const std::filesystem::path& rootDirectory, const std::wstring& extension, std::vector<AssetEntry>& outEntries);
};