#include "ManifestBuilder.h"

using json = nlohmann::json;

json ManifestBuilder::Build(const std::vector<FileInfo>& files) {
    json manifest;

    for (const auto& f : files)
    {
        std::string typeStr;

        // AssetTypeを文字列に変換（JSONキー用）
        switch (f.type)
        {
        case AssetType::Texture: typeStr = "texture"; break;
        case AssetType::Model:   typeStr = "model";   break;
        case AssetType::Audio:   typeStr = "audio";   break;
        default: continue; // 不明なタイプは除外
        }

        // manifest[グループ][アセット名][タイプ] = パスの階層構造で格納
        manifest[f.group][f.name][typeStr] = f.path;
    }

    return manifest;
}