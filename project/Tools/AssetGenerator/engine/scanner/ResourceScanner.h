#pragma once
#include <vector>
#include <string>
#include "FileInfo.h"

/// <summary>
/// 指定されたディレクトリ内のファイルを走査し、アセット情報を収集するクラス
/// </summary>
class ResourceScanner　 {
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
    std::vector<FileInfo> Scan();

private: // メンバ変数
    // --- 設定・状態 ---
    std::string root_; // 走査対象のルートパス
};