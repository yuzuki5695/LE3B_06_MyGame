#include "AssetManifestGenerator.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <chrono>
#include <set>

namespace fs = std::filesystem;

static std::string ToLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
    return s;
}

AssetManifestGenerator::AssetManifestGenerator(const std::string& resourceRoot, const std::string& outputJson)
    : resourceRoot_(resourceRoot), outputJson_(outputJson)
{
    // デフォルト除外
    excludeDirs_ = { ".git", "Temp", "obj", "bin" };

    // デフォルト拡張子
    textureExts_ = { ".png", ".jpg", ".jpeg", ".tga", ".dds", ".bmp", ".hdr" };
    modelExts_   = { ".obj", ".fbx", ".gltf", ".glb" };
    audioExts_   = { ".wav", ".mp3", ".ogg" };
}

void AssetManifestGenerator::AddExcludeDirectory(const std::string& dirName) {
    excludeDirs_.push_back(dirName);
}

void AssetManifestGenerator::SetTextureExtensions(const std::vector<std::string>& exts) {
    textureExts_ = exts;
}
void AssetManifestGenerator::SetModelExtensions(const std::vector<std::string>& exts) {
    modelExts_ = exts;
}
void AssetManifestGenerator::SetAudioExtensions(const std::vector<std::string>& exts) {
    audioExts_ = exts;
}

bool AssetManifestGenerator::IsAssetExtension(const std::string& ext) const {
    std::string e = ToLower(ext);
    for (const auto& x : textureExts_) if (x == e) return true;
    for (const auto& x : modelExts_) if (x == e) return true;
    for (const auto& x : audioExts_) if (x == e) return true;
    return false;
}

std::string AssetManifestGenerator::MakeIdFromRelativePath(const std::string& rel) const {
    std::string id = rel;
    // バックスラッシュ -> スラッシュ
    std::replace(id.begin(), id.end(), '\\', '/');
    // 拡張子削除
    size_t dot = id.find_last_of('.');
    if (dot != std::string::npos) id = id.substr(0, dot);
    // 小文字化
    id = ToLower(id);
    return id;
}

std::string AssetManifestGenerator::DetermineCategory(const std::string& relPath, const std::string& ext) const {
    // relPath は generic_string() で渡されている想定（つまりスラッシュ区切り）
    std::string lowRel = ToLower(relPath);

    // フォルダ名ヒント（優先）
    if (lowRel.find("/texture/") != std::string::npos || lowRel.find("/textures/") != std::string::npos
        || lowRel.find("/tex/") != std::string::npos) {
        return "textures";
    }
    if (lowRel.find("/model/") != std::string::npos || lowRel.find("/models/") != std::string::npos
        || lowRel.find("/mesh/") != std::string::npos) {
        return "models";
    }
    if (lowRel.find("/audio/") != std::string::npos || lowRel.find("/sound/") != std::string::npos
        || lowRel.find("/sfx/") != std::string::npos || lowRel.find("/music/") != std::string::npos) {
        return "audio";
    }

    // 拡張子ベースの判定（フォールバック）
    std::string e = ToLower(ext);
    for (const auto& x : textureExts_) if (x == e) return "textures";
    for (const auto& x : modelExts_)   if (x == e) return "models";
    for (const auto& x : audioExts_)   if (x == e) return "audio";

    // その他
    return "others";
}

bool AssetManifestGenerator::Generate(bool overwrite) {
    try {
        if (!fs::exists(resourceRoot_) || !fs::is_directory(resourceRoot_)) {
            std::cerr << "AssetManifestGenerator: resource root not found: " << resourceRoot_ << "\n";
            return false;
        }

        // カテゴリ別マップ: category -> (id -> relative path)
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> categorized;
        std::unordered_map<std::string, std::string> globalIdToPath; // 重複チェック

        for (auto it = fs::recursive_directory_iterator(resourceRoot_); it != fs::recursive_directory_iterator(); ++it) {
            try {
                const fs::directory_entry& entry = *it;
                if (!entry.is_regular_file()) continue;

                // 親ディレクトリが除外対象ならスキップ（パス全体に含まれているかでチェック）
                std::string parent = entry.path().parent_path().generic_string();
                bool skip = false;
                for (const auto& ex : excludeDirs_) {
                    if (parent.find(ex) != std::string::npos) { skip = true; break; }
                }
                if (skip) continue;

                std::string ext = ToLower(entry.path().extension().string());
                if (!IsAssetExtension(ext)) continue;

                // Resources/ からの相対パス（常に '/' を使う）
                std::string rel = fs::relative(entry.path(), resourceRoot_).generic_string();
                std::string id = MakeIdFromRelativePath(rel);

                // 重複IDチェック（プロジェクト全体でユニークでなければ警告してスキップ）
                if (globalIdToPath.find(id) != globalIdToPath.end()) {
                    std::cerr << "Warning: duplicate asset id: " << id
                              << " (skipping " << rel << ", already mapped to " << globalIdToPath[id] << ")\n";
                    continue;
                }

                std::string category = DetermineCategory(rel, ext);
                categorized[category].emplace(id, rel);
                globalIdToPath.emplace(id, rel);
            } catch (const std::exception& e) {
                std::cerr << "AssetManifestGenerator: error iterating: " << e.what() << '\n';
                // 続行
            }
        }

        // 既存ファイルチェック
        if (!overwrite && fs::exists(outputJson_)) {
            std::cerr << "AssetManifestGenerator: output exists and overwrite==false: " << outputJson_ << '\n';
            return true;
        }

        // JSON をシンプルな形式で出力（外部 lib 不使用）
        std::ofstream ofs(outputJson_, std::ios::out | std::ios::trunc);
        if (!ofs) {
            std::cerr << "AssetManifestGenerator: failed to open output: " << outputJson_ << '\n';
            return false;
        }

        ofs << "{\n  \"assets\": {\n";

        bool firstCategory = true;
        // 出力順を安定させる（textures, models, audio, others, ...）
        std::vector<std::string> order = { "textures", "models", "audio", "others" };
        // さらに他に存在するカテゴリがあれば追加
        for (const auto& kv : categorized) {
            if (std::find(order.begin(), order.end(), kv.first) == order.end()) order.push_back(kv.first);
        }

        for (const auto& cat : order) {
            auto found = categorized.find(cat);
            if (found == categorized.end() || found->second.empty()) continue;
            if (!firstCategory) ofs << ",\n";
            firstCategory = false;

            ofs << "    \"" << cat << "\": {\n";
            bool first = true;
            for (const auto& [id, rel] : found->second) {
                if (!first) ofs << ",\n";
                first = false;
                // rel は generic_string()（スラッシュ）なのでそのまま出力する
                ofs << "      \"" << id << "\": \"" << rel << "\"";
            }
            ofs << "\n    }";
        }

        ofs << "\n  }\n}\n";
        ofs.close();

        size_t total = globalIdToPath.size();
        std::cout << "AssetManifestGenerator: generated " << outputJson_ << " (" << total << " assets, "
                  << categorized.size() << " categories)\n";
        return true;
    } catch (const std::exception& e) {
        std::cerr << "AssetManifestGenerator: exception: " << e.what() << '\n';
        return false;
    }
}

bool AssetManifestGenerator::GenerateIfNeeded(bool overwriteIfOlder) {
    fs::path out(outputJson_);
    if (!fs::exists(out)) {
        return Generate(true);
    }
    if (!overwriteIfOlder) return true;

    // manifestの最終更新時刻
    auto manifestTime = fs::last_write_time(out);

    // assets 内の最終更新時刻を調べる
    fs::file_time_type newest = fs::file_time_type::min();

    for (auto& it : fs::recursive_directory_iterator(resourceRoot_)) {
        try {
            if (!it.is_regular_file()) continue;

            std::string ext = ToLower(it.path().extension().string());
            if (!IsAssetExtension(ext)) continue;

            auto t = fs::last_write_time(it.path());
            if (t > newest) newest = t;
        }
        catch (...) {}
    }

    if (newest > manifestTime) {
        return Generate(true);
    }

    return true;
}
