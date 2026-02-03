#include"ManifestExporter.h"
#include <fstream>
#include <unordered_map>
#include <json.hpp>

///=====================================================
/// Export
///-----------------------------------------------------
/// fileList を走査し、
/// 同一 Group / Name の Asset を統合しながら JSON を構築する
///
/// 出力形式:
/// {
///   "Bullet": {
///     "enemybullet": {
///       "model": "...",
///       "texture": "..."
///     }
///   }
/// }
///=====================================================

void ManifestExporter::Export(const std::string& outputPath, const std::vector<std::string>& fileList) { 
    // 出力用変数 
    nlohmann::json manifest;

   for (const auto& file : fileList) {
        // パス正規化
        std::string normalized = NormalizePath(file);

        // 拡張子取得
        std::string ext = std::filesystem::path(normalized).extension().string();

        // AssetType 判定
        AssetType type = GetAssetType(ext,normalized);
        if (type == AssetType::Unknown) continue;

        // Group（最上位フォルダ）
        std::string group = GetGroup(normalized);

        // Asset 論理名（拡張子無し. 同名のmodel/texture/audio を1つに束ねるために使用
        std::string name = CreateId(normalized, type);

        // JSONキー名（model / texture / audio）
        std::string typeKey = ToString(type);

        // 同一 Group/Name の中に model/texture/audioを統合して格納する
        manifest[group][name][typeKey] = normalized;
    }
    // 差分がある場合のみ保存し無駄なファイル更新を防ぐ
    SaveIfChanged(outputPath, manifest.dump(4));
}

void ManifestExporter::SaveIfChanged(const std::string& path, const std::string& content) {
    // 既存ファイルを読み込み
    std::ifstream ifs(path, std::ios::binary);
    if (ifs) {
        std::string existing((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        // 内容が同一なら書き込みをスキップ
        if (existing == content) {
            return;
        }
    }

    // 差分がある場合のみ上書き保存
    std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
    ofs << content;
}

std::string ManifestExporter::CreateId(std::string path, AssetType type) {
    std::filesystem::path p(path);
    // 拡張子を除いたファイル名を取得
    std::string id = p.stem().string();
    // 小文字化（キーの揺れ防止） _model / _texture は付けない
    std::transform(id.begin(), id.end(), id.begin(), ::tolower);
    return id;
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
    // フォルダを持たない場合のフォールバック
    return "Common";
}


AssetType ManifestExporter::GetAssetType(const std::string& ext, const std::string& fullPath) {
    std::string lowerExt = ext;
    std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(), ::tolower);

    // 正規化したパスで判定するため、一時的に小文字化
    std::string lowerPath = fullPath;
    std::transform(lowerPath.begin(), lowerPath.end(), lowerPath.begin(), ::tolower);

    // --- モデル判定 ---
    // モデル拡張子、または「Modelsフォルダ内のDDS」をモデルとして扱う
    if (lowerExt == ".fbx" || lowerExt == ".obj" || lowerExt == ".gltf" ||
        (lowerExt == ".dds" && lowerPath.find("models/") != std::string::npos)) { // ← ここがポイント
        return AssetType::Model;
    }

    // --- テクスチャ判定 ---
    // Modelsフォルダ外にあるDDS、またはその他の画像形式
    if (lowerExt == ".png" || lowerExt == ".jpg" || lowerExt == ".jpeg" ||
        lowerExt == ".tga" || lowerExt == ".dds") {
        return AssetType::Texture;
    }

    // --- 音声判定 ---
    if (lowerExt == ".wav" || lowerExt == ".mp3") {
        return AssetType::Audio;
    }

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
