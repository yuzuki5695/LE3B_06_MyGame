#pragma once
#include <vector>
#include <string>
#include "FileInfo.h"

/// <summary>
/// 指定されたディレクトリ内のファイルを走査し、アセット情報を収集するクラス
/// </summary>
class ResourceScanner {
public: // メンバ関数
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="root">スキャンを開始するルートディレクトリのパス</param>
    ResourceScanner(const std::string& root);
    /// <summary>
    /// ディレクトリを再帰的に走査し、有効なアセットの一覧を返す
    /// </summary>
    /// <returns>見つかったアセット情報のリスト</returns>
    std::vector<FileInfo> ScanFile();
    /// <summary>
    /// パス内のバックスラッシュをスラッシュに置換して統一する
    /// </summary>
    /// <param name="path">変換前のパス文字列</param>
    /// <returns>正規化されたパス文字列</returns>
    static std::string Normalize(const std::string& path);
    /// <summary>
    /// 拡張子文字列から対応するAssetTypeを判定する
    /// </summary>
    /// <param name="extension">判定対象の拡張子（例: ".png"）</param>
    /// <returns>判定されたAssetType（不明な場合はUnknown）</returns>
    AssetType Detect(const std::filesystem::path& ext);
private: // メンバ変数
    // --- 設定・状態 ---
    std::string root_; // 走査対象のルートパス
};