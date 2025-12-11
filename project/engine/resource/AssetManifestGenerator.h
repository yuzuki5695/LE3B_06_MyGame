#pragma once
#include <string>
#include <vector>

class AssetManifestGenerator {
public:
    // resourceRoot: "Resources" など。 outputJson: "manifest.json"など
    AssetManifestGenerator(const std::string& resourceRoot = "Resources",
                           const std::string& outputJson = "manifest.json");

    // 強制生成。既存の manifest を無視して作り直す
    bool Generate(bool overwrite = false);

    // 条件付き生成:
    // - manifest が存在しない場合は作る
    // - または overwriteIfOlder==true の場合、manifest が最終更新日時より古ければ再生成
    bool GenerateIfNeeded(bool overwriteIfOlder = true);

    // 除外ディレクトリ（例: ".git", "Temp"）を追加
    void AddExcludeDirectory(const std::string& dirName);

    // 拡張子フィルタの上書き（省略可）
    void SetTextureExtensions(const std::vector<std::string>& exts);
    void SetModelExtensions(const std::vector<std::string>& exts);
    void SetAudioExtensions(const std::vector<std::string>& exts);

private:
    std::string resourceRoot_;
    std::string outputJson_;
    std::vector<std::string> excludeDirs_;
    std::vector<std::string> textureExts_;
    std::vector<std::string> modelExts_;
    std::vector<std::string> audioExts_;

    // internal helpers
    bool IsAssetExtension(const std::string& ext) const;
    std::string MakeIdFromRelativePath(const std::string& rel) const;

    // New: determine category for an asset (textures/models/audio/others)
    std::string DetermineCategory(const std::string& relPath, const std::string& ext) const;
};
