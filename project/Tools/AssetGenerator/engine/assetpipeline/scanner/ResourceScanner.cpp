#include "ResourceScanner.h"
#include <filesystem>
#include <set>
#include <iostream>

namespace fs = std::filesystem;

ResourceScanner::ResourceScanner(const std::string& root) : root_(root) {}

std::vector<FileInfo> ResourceScanner::ScanFile() {
    std::vector<FileInfo> result;

    if (!fs::exists(root_))
        return result;

    for (const auto& entry : fs::recursive_directory_iterator(root_))
    {
        if (!entry.is_regular_file())
            continue;

        fs::path relative = fs::relative(entry.path(), root_);

        // 1階層目（Textures / Models / Audio）
        std::string group = (*relative.begin()).string();

        if (group != "Textures" &&
            group != "Models" &&
            group != "Audio")
        {
            continue;
        }

        AssetType type = Detect(entry.path().extension());

        if (type == AssetType::Unknown)
            continue;

        FileInfo info;
        info.path = relative;
        info.type = type;

        result.push_back(info);
    }

    return result;
}

std::string ResourceScanner::Normalize(const std::string& path)
{
    std::string result = path;
    // 確実に '文字' として置換
    std::replace(result.begin(), result.end(), '\\', '/'); 
    return result;
}

AssetType ResourceScanner::Detect(const std::filesystem::path& ext)
{
    std::string e = ext.string();
    std::transform(e.begin(), e.end(), e.begin(), ::tolower);

    if (e == ".png" || e == ".jpg" || e == ".dds")
        return AssetType::Texture;

    if (e == ".obj" || e == ".fbx" || e == ".gltf")
        return AssetType::Model;

    if (e == ".wav" || e == ".mp3")
        return AssetType::Audio;

    return AssetType::Unknown;
}