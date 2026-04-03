#pragma once
#include <BaseLoader.h>
#include <unordered_map>
#include <string>
#include <vector>

/// <summary>
/// リソースの個別の定義情報
/// </summary>
struct AssetDefinition {
    std::string id;             // プログラムから参照するID (例: "Player")
    std::string path;           // 実際のファイルパス (例: "Models/Player/Player.obj")
    std::string category;       // カテゴリ (Texture, Model, Se, etc...)
};

/// <summary>
/// アセット定義JSON（IDとパスの紐付け）を読み込むローダー
/// </summary>
class AssetInfoLoader : public BaseLoader {
public:
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
    /// 読み込んだアセット定義のリストを取得する
    /// </summary>
    const std::vector<AssetDefinition>& GetAssets() const { return assets_; }

    /// <summary>
    /// AssetInfoLoaderのデータから LoadResourceID.h を生成する
    /// </summary>
    /// <param name="loader">解析済みのAssetInfoLoader</param>
    /// <param name="outputPath">出力するヘッダのパス</param>
    void GenerateResourceIDHeader(const AssetInfoLoader& loader, const std::string& outputPath);

private:
    std::vector<AssetDefinition> assets_;
};