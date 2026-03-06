#include "AssetTypeDetector.h"
#include <algorithm>

AssetType AssetTypeDetector::Detect(const std::string& extension)
{
    // 判定のために小文字に変換
    std::string ext = extension;
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    // 画像アセットの判定
    if (ext == ".png" || ext == ".jpg" || ext == ".dds")
        return AssetType::Texture;

    // モデルアセットの判定
    if (ext == ".obj" || ext == ".fbx" || ext == ".gltf")
        return AssetType::Model;

    // 音声アセットの判定
    if (ext == ".wav" || ext == ".mp3")
        return AssetType::Audio;

    // 対応外の拡張子
    return AssetType::Unknown;
}