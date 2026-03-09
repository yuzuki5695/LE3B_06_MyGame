#include "HeaderGenerator.h"
#include "ResourceScanner.h"
#include <fstream>
#include <algorithm>
#include <set>
#include <map>

std::string HeaderGenerator::Sanitize(const std::string& str) {
    std::string res = str;
    // ハイフンやスペースをアンダースコアに
    std::replace(res.begin(), res.end(), '-', '_');
    std::replace(res.begin(), res.end(), ' ', '_');
    std::replace(res.begin(), res.end(), '.', '_');
    if (!res.empty() && std::isdigit(res[0])) res = "_" + res;
    return res;
}

void HeaderGenerator::Generate(const std::filesystem::path& outputPath, const std::vector<AssetEntry>& assets) {
    std::ofstream file(outputPath);
    if (!file.is_open()) return;

    file << "#pragma once\n#include <string>\n\n";
    file << "namespace resource {\n";

    // データの階層管理用：[大分類(texture/model)][中間パス][ID] = 相対パス
    std::map<std::string, std::map<std::vector<std::string>, std::map<std::string, std::string>>> tree;

    for (const AssetEntry& asset : assets) {
        if (asset.isDirectory) continue;

        std::filesystem::path p(asset.path);
        auto it = p.begin();
        if (it == p.end()) continue;

        std::string rootFolder = Sanitize(it->string());
        std::string category = "";

        // ルートフォルダ名で namespace を決定
        if (rootFolder == "Textures") category = "texture";
        else if (rootFolder == "Models") category = "model";
        else if (rootFolder == "Audio") category = "audio";
        else continue; // 今回の条件（Textures/Models以外）はスキップ

        std::vector<std::string> subFolders;
        it++; // Rootの次から
        for (; std::next(it) != p.end(); ++it) {
            subFolders.push_back(Sanitize(it->string()));
        }

        std::string id = Sanitize(p.stem().string());
        // 相対パス（正規化済み）を値として保持
        std::string fullPath = "";
        for (auto& part : p) {
            if (!fullPath.empty()) fullPath += "/";
            fullPath += part.string();
        }

        tree[category][subFolders][id] = fullPath;
    }

    // コード生成
    for (auto const& [catName, folders] : tree) {
        file << "    namespace " << catName << " {\n";
        
        // フォルダ階層を管理して出力（※簡易的な1階層ネスト例。必要に応じて再帰化）
        for (auto const& [folderPath, items] : folders) {
            std::string indent = "        ";
            for (const auto& f : folderPath) {
                file << indent << "namespace " << f << " {\n";
                indent += "    ";
            }

            for (auto const& [id, path] : items) {
                file << indent << "static const std::string " << id << " = \"" << path << "\";\n";
            }

            for (size_t i = 0; i < folderPath.size(); ++i) {
                indent = indent.substr(0, indent.length() - 4);
                file << indent << "}\n";
            }
        }
        file << "    }\n";
    }

    file << "}\n";
}