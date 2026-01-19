#include"ManifestExporter.h"
#include <fstream>
#include <unordered_map>
#include <json.hpp>

void ManifestExporter::Export(const std::string& outputPath, const std::vector<std::string>& fileList) {
    nlohmann::json manifest;

    for (const auto& file : fileList) {
        std::string normalized = NormalizePath(file);
        std::string ext = std::filesystem::path(normalized).extension().string();

        AssetType type = GetAssetType(ext);
        if (type == AssetType::Unknown) continue;

        std::string group = GetGroup(normalized);     // Bullet
        std::string name  = CreateId(normalized, type); // enemybullet

        // type → "model" / "texture"
        std::string typeKey = ToString(type);

        manifest[group][name][typeKey] = normalized;
    }

    SaveIfChanged(outputPath, manifest.dump(4));
}

void ManifestExporter::SaveIfChanged(const std::string& path, const std::string& content) {
    std::ifstream ifs(path, std::ios::binary);
    if (ifs) {
        std::string existing((std::istreambuf_iterator<char>(ifs)),
                               std::istreambuf_iterator<char>());
        if (existing == content) {
            return; // 変更なし
        }
    }

    std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
    ofs << content;
}

std::string ManifestExporter::CreateId(std::string path, AssetType type) {
    std::filesystem::path p(path);
    std::string id = p.stem().string();
    std::transform(id.begin(), id.end(), id.begin(), ::tolower);
    return id; // ← _model / _texture を付けない
}

std::string ManifestExporter::GetCategory(std::string ext) {
    // 小文字化
    std::transform(ext.begin(), ext.end(), ext.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".tga" || ext == ".dds") {
        return "textures";
    }
    if (ext == ".fbx" || ext == ".obj" || ext == ".gltf" || ext == ".glb") {
        return "models";
    }
    if (ext == ".wav" || ext == ".mp3" || ext == ".ogg") {
        return "audio";
    }

    return ""; // ← 不明な拡張子は無視
}


std::string ManifestExporter::NormalizePath(std::string path) {
    std::replace(path.begin(), path.end(), '\\', '/');
    return path;
}

std::string ManifestExporter::GetGroup(const std::string& path) {
    std::filesystem::path p(path);

    if (p.has_parent_path()) {
        return p.begin()->string();
    }
    return "Common";
}


AssetType ManifestExporter::GetAssetType(const std::string& ext) {
    std::string lowerExt = ext;

    std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(), ::tolower);

    if (lowerExt == ".png" || lowerExt == ".jpg" || lowerExt == ".jpeg"
        || lowerExt == ".tga" || lowerExt == ".dds")
        return AssetType::Texture;

    if (lowerExt == ".fbx" || lowerExt == ".obj"
        || lowerExt == ".gltf" || lowerExt == ".glb")
        return AssetType::Model;

    if (lowerExt == ".wav" || lowerExt == ".mp3" || lowerExt == ".ogg")
        return AssetType::Audio;

    return AssetType::Unknown;
}

std::string ManifestExporter::ToString(AssetType type) {
    switch (type) {
    case AssetType::Texture: return "texture";
    case AssetType::Model:   return "model";
    case AssetType::Audio:   return "audio";
    default: return "";
    }
}
