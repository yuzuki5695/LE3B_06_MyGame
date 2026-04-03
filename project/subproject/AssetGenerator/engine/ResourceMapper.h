#pragma once
#include "Scanner.h"
#include "ResourceNode.h"
#include <filesystem>

/// <summary>
/// リソース全体をスキャンし、構造化してJSONとして出力する管理クラス
/// Scanner（取得）→ ResourceNode（構造化）→ JSON出力 の流れを制御する
/// </summary>
class ResourceMapper {
public: // メンバ関数

    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="root">Resourcesフォルダのルートパス</param>
    /// <param name="output">出力先JSONファイルパス</param>
    ResourceMapper(const std::filesystem::path& root, const std::filesystem::path& output);

    /// <summary>
    /// テクスチャ一覧をスキャンし、JSONを生成・更新する
    /// </summary>
    void UpdateTextureList();

    void UpdateSingle(const std::filesystem::path& targetDir, const std::vector<std::wstring>& extensions, const std::filesystem::path& outputPath);

private: // 内部関数

    /// <summary>
    /// 内容に変更があった場合のみファイルを保存する
    /// </summary>
    /// <param name="newContent">新しいファイル内容</param>
    /// <returns>更新された場合true</returns> 
    void SaveIfChanged(const std::string& content, const std::filesystem::path& path);


    /// <summary>
    /// ファイルを全読み込みする
    /// </summary>
    /// <param name="path">読み込むファイルパス</param>
    /// <returns>ファイル内容（失敗時は空文字）</returns>
    std::string ReadAllText(const std::filesystem::path& path);
private: // メンバ変数
    std::filesystem::path resourceRoot; /// Resourcesフォルダ
    std::filesystem::path outputPath;   /// 出力先JSONファイル
};