#pragma once
#include <string>

/// <summary>
/// アセットの種類を定義する列挙型
/// </summary>
enum class AssetType {
    Texture, // 画像リソース
    Model,   // 3Dモデルリソース
    Audio,   // 音声リソース
    Unknown  // 判別不能な形式
};

/// <summary>
/// 個別のファイル情報を保持する構造体
/// スキャンしたアセットのパスや種類、グループ情報を格納する。
/// </summary>
struct FileInfo {
    std::string path;      // Resourcesからの相対パス
    std::string name;      // ファイル名(拡張子なし)
    std::string extension; // 拡張子
    AssetType type;        // アセット種類
    std::string group;     // 最上位フォルダ（アセットのカテゴリ分けに使用）
};