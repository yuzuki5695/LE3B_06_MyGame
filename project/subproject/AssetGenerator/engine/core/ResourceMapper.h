#pragma once
#include "Scanner.h"
#include "ResourceNode.h"
#include <RuleLoader.h>

/// <summary>
/// リソース処理の全体フローを管理するクラス（司令塔）
/// 
/// 【役割】
/// ・ルールに従って処理を実行する
/// ・各処理（スキャン / フィルタ / 構造化 / 出力）を順に呼び出す
/// 
/// 【責務】
/// ・処理の流れの制御（オーケストレーション）
/// 
/// 【責務外】
/// ・個別処理の詳細実装（Scanner / ResourceNodeなどに委譲）
/// 
/// 【処理フロー】
/// Scanner → フィルタ → ResourceNode → JSON → 保存
/// </summary>
class ResourceMapper {
public: // メンバ関数

    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="resourceRoot">Resourcesフォルダのルート</param>
    /// <param name="defaultOutputPath">デフォルト出力先</param>
    ResourceMapper(const std::filesystem::path& resourceRoot, const std::filesystem::path& defaultOutputPath);
    
    /// <summary>
    /// 単一ルールに基づいてリソースを処理する
    /// 
    /// 【処理内容】
    /// ・対象ディレクトリをスキャン
    /// ・拡張子でフィルタ
    /// ・ツリー構造を構築
    /// ・JSONに変換
    /// ・差分があれば保存
    /// </summary>
    void UpdateSingle(const std::filesystem::path& targetDir, const std::vector<std::wstring>& extensions, const std::filesystem::path& outputPath);

    /// <summary>
    /// 複数ルールを順番に処理する
    /// </summary>
    void UpdateByRules(const std::vector<ExportRule>& rules);

private: // 内部関数
    /// <summary>
    /// 内容に変更があった場合のみファイルを保存する
    /// </summary>
    /// <param name="content">書き込む文字列</param>
    /// <param name="path">出力先ファイルパス</param>
    void SaveIfChanged(const std::string& content, const std::filesystem::path& path);

    /// <summary>
    /// ファイルをすべて読み込む
    /// </summary>
    /// <param name="path">読み込むファイルパス</param>
    std::string ReadAllText(const std::filesystem::path& path);
private: // メンバ変数
    std::filesystem::path resourceRoot; /// Resourcesフォルダ
    std::filesystem::path outputPath;   /// 出力先JSONファイル
};