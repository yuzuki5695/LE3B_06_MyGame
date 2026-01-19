#pragma once
#include <vector>
#include <string>

// Resourceの種類
enum class AssetType {
    Texture,   // テクスチャ
    Model,     // 3Dモデル
    Audio,     // 音楽
    Unknown    //
};

class ManifestExporter {
public:
    // ファイルリストを受け取ってJSON化し、差分があれば保存する
    void Export(const std::string& outputPath, const std::vector<std::string>& fileList);

    static std::string NormalizePath(std::string path);
    std::string GetGroup(const std::string& path);

    AssetType GetAssetType(const std::string& ext);
    
    std::string ToString(AssetType type);

private:
    std::string CreateId(std::string path, AssetType type);
    std::string GetCategory(std::string ext);
    void SaveIfChanged(const std::string& path, const std::string& content);
};