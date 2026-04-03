#pragma once
#include <BaseLoader.h>
#include <unordered_map>
#include <string>
#include <vector>
#include <json.hpp>

namespace AssetGen {
    /// <summary>
    /// リソースの個別の定義情報を表す構造体
    /// </summary>
    struct AssetDefinition {
        std::string id;                         ///< 生成されるID名（例: "EnemyBullet"）
        std::string path;                       ///< ファイルパス（例: "Bullet/EnemyBullet/EnemyBullet.obj"）
        std::string category;                   ///< ルートカテゴリ（例: "Models", "Textures"）
        std::vector<std::string> subFolders;    ///< 中間フォルダ階層（例: {"Bullet", "EnemyBullet"}）
    };

    /// <summary>
    /// アセット定義JSON（IDとパスの対応情報）を読み込むローダー
    /// また、生成済みJSON（Textures.json等）から自動的にアセット情報を構築する
    /// </summary>
    class AssetInfoLoader : public BaseLoader {
    public: // メンバ関数
        /// <summary>
        /// デフォルトコンストラクタ
        /// </summary>
        AssetInfoLoader() = default;

        /// <summary>
        /// アセット定義JSONを読み込む
        /// </summary>
        /// <param name="jsonPath">読み込むJSONのパス</param>
        /// <returns>成功したらtrue</returns>
        bool Load(const std::filesystem::path& jsonPath) override;

        /// <summary>
        /// 保持しているアセット情報をクリアする
        /// </summary>
        void Clear() override;

        /// <summary>
        /// 読み込んだアセット定義の一覧を取得する
        /// </summary>
        /// <returns>AssetDefinition の配列への const 参照</returns>
        const std::vector<AssetDefinition>& GetAssets() const { return assets_; }

        /// <summary>
        /// 生成済みの各リソースJSONを読み込み、アセット定義を蓄積する
        /// </summary>
        /// <param name=\"jsonPath\">Textures.json などのパス</param>
        /// <returns>成功したらtrue</returns>
        bool LoadGeneratedJson(const std::filesystem::path& jsonPath);
    private: // 内部関数
        /// <summary>
        /// JSONの階層構造を再帰的に解析し、アセット情報を抽出する内部関数
        /// </summary>
        /// <param name="j">解析対象のJSONノード</param>
        /// <param name="category">現在のカテゴリ（Textures / Models など）</param>
        /// <param name="hierarchy">現在までのフォルダ階層</param>
        void ParseRecursive(const nlohmann::json& j, const std::string& category, std::vector<std::string> hierarchy);
    private: // メンバ変数
        std::vector<AssetDefinition> assets_; /// 読み込まれたアセット定義のリスト
    };
}