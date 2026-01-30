#include "AssetManifestGenerator.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <chrono>
#include <set>
#include <sstream>
#include <json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

static std::string ToLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
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
        || lowRel.find("/tex/") != std::string::npos || lowRel.find("/ui/") != std::string::npos
        || lowRel.find("/sprite/") != std::string::npos) {
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

// ---------------- simple Generate (full rewrite) ----------------
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
        std::string json = BuildJsonStringFromCategorized(categorized);

        // Atomic write (tmp -> rename)
        std::string tmpPath = outputJson_ + ".tmp";
        {
            std::ofstream ofsTmp(tmpPath, std::ios::out | std::ios::trunc | std::ios::binary);
            if (!ofsTmp) {
                std::cerr << "AssetManifestGenerator: failed to open tmp output: " << tmpPath << '\n';
                return false;
            }
            ofsTmp << json;
            ofsTmp.close();
        }
        try {
            fs::rename(tmpPath, outputJson_);
        } catch (const std::exception& e) {
            std::cerr << "AssetManifestGenerator: rename failed: " << e.what() << '\n';
            // fallback: try writing directly
            std::ofstream ofs(outputJson_, std::ios::out | std::ios::trunc | std::ios::binary);
            if (!ofs) return false;
            ofs << json;
            ofs.close();
        }

        size_t total = globalIdToPath.size();
        std::cout << "AssetManifestGenerator: generated " << outputJson_ << " (" << total << " assets, "
                  << categorized.size() << " categories)\n";
        return true;
    } catch (const std::exception& e) {
        std::cerr << "AssetManifestGenerator: exception: " << e.what() << '\n';
        return false;
    }
}

// ---------------- GenerateIfNeeded ----------------
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

// Replace previous LoadSimpleManifest with this implementation
bool AssetManifestGenerator::LoadSimpleManifest(const std::string& path, std::unordered_map<std::string, std::string>& outMap) const {
    outMap.clear();
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if (!ifs) {
        std::cerr << "AssetManifestGenerator: LoadSimpleManifest - failed to open: " << path << '\n';
        return false;
    }

    json j;
    try {
        ifs >> j;
    } catch (const std::exception& e) {
        std::cerr << "AssetManifestGenerator: LoadSimpleManifest - json parse error: " << e.what() << '\n';
        return false;
    }

    // Expect top-level object with "assets"
    if (!j.is_object() || !j.contains("assets") || !j["assets"].is_object()) {
        std::cerr << "AssetManifestGenerator: LoadSimpleManifest - json missing 'assets' object\n";
        return false;
    }

    json assets = j["assets"];

    // Two possible shapes:
    // 1) categorized: assets: { "textures": { "id":"path", ... }, "models": {...}, ... }
    // 2) flat:      assets: { "id":"path", ... }
    // Detect which by checking whether values under assets are objects.

    bool anyLoaded = false;

    for (auto it = assets.begin(); it != assets.end(); ++it) {
        const std::string catKey = it.key();
        const json& val = it.value();
        if (val.is_object()) {
            // category object: iterate inner id->path
            for (auto inner = val.begin(); inner != val.end(); ++inner) {
                if (!inner.value().is_string()) continue;
                const std::string id = inner.key();
                const std::string rel = inner.value().get<std::string>();
                outMap.emplace(id, rel);
                anyLoaded = true;
            }
        } else if (val.is_string()) {
            // flat case unlikely here (assets contains a string directly) - handle defensively
            // e.g. "assets": { "some/id": "path" } would have been an id=>string pair where key is id and value is string.
            // But the above loop won't reach that because val would be string only if assets had keys whose value is string.
            // So we skip here.
            continue;
        }
    }

    // Additionally, handle the flat case where assets directly maps id->path
    // (this happens if the assets values are strings rather than objects; check first child)
    if (!anyLoaded) {
        for (auto it2 = assets.begin(); it2 != assets.end(); ++it2) {
            if (it2.value().is_string()) {
                outMap.emplace(it2.key(), it2.value().get<std::string>());
                anyLoaded = true;
            }
        }
    }

    if (!anyLoaded) {
        std::cerr << "AssetManifestGenerator: LoadSimpleManifest - no id/path pairs found in manifest\n";
        // return false to signal caller to do full scan fallback
        return false;
    }

    return true;
}


// ---------------- build JSON string from categorized map ----------------
std::string AssetManifestGenerator::BuildJsonStringFromCategorized(const std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& categorized) const {
    std::ostringstream oss;
    oss << "{\n  \"assets\": {\n";

    bool firstCategory = true;
    // 優先順
    std::vector<std::string> order = { "textures", "models", "audio", "others" };
    for (const auto& kv : categorized) {
        if (std::find(order.begin(), order.end(), kv.first) == order.end()) order.push_back(kv.first);
    }

    for (const auto& cat : order) {
        auto it = categorized.find(cat);
        if (it == categorized.end() || it->second.empty()) continue;
        if (!firstCategory) oss << ",\n";
        firstCategory = false;

        oss << "    \"" << cat << "\": {\n";

        // sort keys for deterministic output
        std::vector<std::string> keys;
        keys.reserve(it->second.size());
        for (const auto& kvp : it->second) keys.push_back(kvp.first);
        std::sort(keys.begin(), keys.end());

        bool first = true;
        for (const auto& id : keys) {
            if (!first) oss << ",\n";
            first = false;
            const std::string& rel = it->second.at(id);
            oss << "      \"" << id << "\": \"" << rel << "\"";
        }
        oss << "\n    }";
    }

    oss << "\n  }\n}\n";
    return oss.str();
}

// ---------------- GenerateMerged (差分マージ + スキップ最適化 + atomic write) ----------------
bool AssetManifestGenerator::GenerateMerged(bool forceMerge) {
    try {
        if (!fs::exists(resourceRoot_) || !fs::is_directory(resourceRoot_)) {
            std::cerr << "AssetManifestGenerator: resource root not found: " << resourceRoot_ << "\n";
            return false;
        }

        fs::path outPath(outputJson_);
        bool outExists = fs::exists(outPath);

        // 既存 manifest をロード（flat map: id -> path）
        std::unordered_map<std::string, std::string> existingMap;
        fs::file_time_type manifestTime = fs::file_time_type::min();
        if (outExists) {
            bool ok = LoadSimpleManifest(outputJson_, existingMap);
            try {
                manifestTime = fs::last_write_time(outPath);
            }
            catch (...) {
                manifestTime = fs::file_time_type::min();
            }
            if (!ok || existingMap.empty()) {
                std::cerr << "AssetManifestGenerator: warning - failed to parse existing manifest or manifest empty -> performing full scan\n";
                manifestTime = fs::file_time_type::min();
                existingMap.clear();
            }
        }

        if (outExists && !forceMerge) {
            // 上書きしないモード（従来と同じ）
            std::cerr << "AssetManifestGenerator: output exists and forceMerge==false: " << outputJson_ << '\n';
            return true;
        }

        // global map starts from existing entries (so unchanged entries are preserved)
        std::unordered_map<std::string, std::string> globalIdToPath = existingMap;
        std::set<std::string> existingIds;
        for (const auto& kv : existingMap) existingIds.insert(kv.first);

        // スキャンして新規/更新を検出（manifestTime より新しいファイルのみ重点的に扱う）
        fs::file_time_type newest = fs::file_time_type::min();
        for (auto it = fs::recursive_directory_iterator(resourceRoot_); it != fs::recursive_directory_iterator(); ++it) {
            try {
                const fs::directory_entry& entry = *it;
                if (!entry.is_regular_file()) continue;

                // 除外ディレクトリチェック
                std::string parent = entry.path().parent_path().generic_string();
                bool skip = false;
                for (const auto& ex : excludeDirs_) {
                    if (parent.find(ex) != std::string::npos) { skip = true; break; }
                }
                if (skip) continue;

                std::string ext = ToLower(entry.path().extension().string());
                if (!IsAssetExtension(ext)) continue;

                auto fileTime = fs::last_write_time(entry.path());
                if (fileTime > newest) newest = fileTime;

                // 既存でかつ更新されていない場合はスキップ（ただし existingMap にパスが無ければ新規として扱う）
                bool consider = true;
                if (outExists && manifestTime != fs::file_time_type::min() && fileTime <= manifestTime) {
                    std::string rel = fs::relative(entry.path(), resourceRoot_).generic_string();
                    std::string id = MakeIdFromRelativePath(rel);
                    auto itEx = existingMap.find(id);
                    if (itEx != existingMap.end() && itEx->second == rel) {
                        consider = false;
                    }
                }
                if (!consider) continue;

                // 新規/更新対象
                std::string rel = fs::relative(entry.path(), resourceRoot_).generic_string();
                std::string id = MakeIdFromRelativePath(rel);

                auto foundGlobal = globalIdToPath.find(id);
                if (foundGlobal != globalIdToPath.end() && foundGlobal->second != rel) {
                    std::cerr << "Warning: duplicate asset id during merge: " << id
                              << " (existing: " << foundGlobal->second << ", new: " << rel << " -- skipping new)\n";
                    continue;
                }

                // reflect change
                globalIdToPath[id] = rel;
                existingIds.erase(id);
            } catch (const std::exception& e) {
                std::cerr << "AssetManifestGenerator: error iterating during merge: " << e.what() << '\n';
            }
        }

        // existingIds に残っている ID は実ファイルが見つからなかった（削除された）ため manifest から取り除く（ポリシー）
        for (const auto& id : existingIds) {
            globalIdToPath.erase(id);
            std::cout << "AssetManifestGenerator: manifest entry removed (file missing): " << id << "\n";
        }

        // globalIdToPath をカテゴリ別に振り分けて最終 JSON を作成
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> finalCategorized;
        for (const auto& kv : globalIdToPath) {
            const std::string& id = kv.first;
            const std::string& rel = kv.second;
            std::string ext = ToLower(fs::path(rel).extension().string());
            std::string cat = DetermineCategory(rel, ext);
            finalCategorized[cat].emplace(id, rel);
        }

        std::string newJson = BuildJsonStringFromCategorized(finalCategorized);

        // 既存と同じなら書き込みスキップ
        if (outExists) {
            std::ifstream ifs(outputJson_, std::ios::binary);
            if (ifs) {
                std::ostringstream ss;
                ss << ifs.rdbuf();
                if (ss.str() == newJson) {
                    std::cout << "AssetManifestGenerator: no changes detected, skip writing manifest.\n";
                    return true;
                }
            }
        }

        // Atomic write (tmp -> rename)
        std::string tmpPath = outputJson_ + ".tmp";
        {
            std::ofstream ofsTmp(tmpPath, std::ios::out | std::ios::trunc | std::ios::binary);
            if (!ofsTmp) {
                std::cerr << "AssetManifestGenerator: failed to open tmp output: " << tmpPath << '\n';
                return false;
            }
            ofsTmp << newJson;
            ofsTmp.close();
        }
        try {
            fs::rename(tmpPath, outputJson_);
        } catch (const std::exception& e) {
            std::cerr << "AssetManifestGenerator: rename failed: " << e.what() << '\n';
            // fallback: try writing directly
            std::ofstream ofs(outputJson_, std::ios::out | std::ios::trunc | std::ios::binary);
            if (!ofs) return false;
            ofs << newJson;
            ofs.close();
        }

        std::cout << "AssetManifestGenerator: merged and wrote " << outputJson_ << " (" << globalIdToPath.size() << " assets)\n";
        return true;
    } catch (const std::exception& e) {
        std::cerr << "AssetManifestGenerator: exception during GenerateMerged: " << e.what() << '\n';
        return false;
    }
}
