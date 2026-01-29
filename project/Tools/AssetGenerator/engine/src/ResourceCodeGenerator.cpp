#include "ResourceCodeGenerator.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <iostream>
#include <set>

std::map<std::string, std::string> ResourceCodeGenerator::ParseExistingHeader(const std::string& filePath) {
    std::map<std::string, std::string> existingData;
    std::ifstream ifs(filePath);
    if (!ifs) return existingData;

    std::string line;
    // 正規表現を修正: "= "path"" の部分を非キャプチャグループ (?:...)? にして、あってもなくてもマッチさせる
    // 1: 変数名 (\w+), 2: パス ([^"]+)
    std::regex re(R"(constexpr const char\*\s+(\w+)(?:\s*=\s*\"([^\"]+)\")?\s*;)");
    std::smatch match;

    while (std::getline(ifs, line)) {
        if (std::regex_search(line, match, re)) {
            std::string varName = match[1].str();
            std::string path = match[2].matched ? match[2].str() : "__UNDEFINED__";

            // パスをキーにして変数名を保存（パス不明の場合は __UNDEFINED__ をキーにする）
            // ただし、同じパスが複数あると上書きされるため、手動定義が多い場合は注意
            existingData[path] = varName;
        }
    }
    return existingData;
}

void ResourceCodeGenerator::GenerateHeader(const std::string& outputPath, const std::vector<ResourceEntry>& entries) {
    // 1. 既存の定義を読み込む
    auto existingData = ParseExistingHeader(outputPath);

    // 既存の {変数名 : 元のパス} の逆引きマップを作成
    std::map<std::string, std::string> nameToPath;
    for (auto const& [path, name] : existingData) {
        nameToPath[name] = path;
    }

    std::stringstream ss;
    ss << "#pragma once\n\n";
    ss << "namespace LoadResourceID {\n\n";

    // どのグループでも出力されなかった既存変数名を追跡
    std::set<std::string> allUsedNames;

    auto writeGroup = [&](const std::string& groupName, auto filterFunc) {
        ss << "    namespace " << groupName << " {\n";
        std::set<std::string> usedInThisGroup;

        // A. 現存するファイルに基づく生成
        for (const auto& e : entries) {
            std::string currentPath = filterFunc(e);
            if (currentPath.empty()) continue;

            // 整理用フィルタ
            if (currentPath.find("Textures") == std::string::npos &&
                currentPath.find("Models") == std::string::npos) {
                continue;
            }

            std::string varName;
            if (existingData.count(currentPath)) {
                varName = existingData[currentPath];
            } else {
                varName = ToUpperCamel(e.name);
            }

            ss << "        constexpr const char* " << varName << " = \"" << currentPath << "\";\n";
            usedInThisGroup.insert(varName);
            allUsedNames.insert(varName);
        }

        // B. 手動追加された変数や、ファイルが消えた変数の nullptr 出力
        for (auto const& [name, path] : nameToPath) {
            // すでにこのグループで出力済みなら飛ばす
            if (usedInThisGroup.count(name)) continue;

            // 1. パスが特定のフォルダを含む場合、そのグループで nullptr 出力
            // 2. パスが "__UNDEFINED__" (手動で Title2; と書いた) の場合、
            //    とりあえず texture namespace などで最初に見つけた時に出す、
            //    または namespace 判定を簡易化して出力
            bool isTargetGroup = false;
            if (path == "__UNDEFINED__") {
                // 手動定義の場合、変数名のヒント（"Title"が含まれるならtextureなど）
                // または元の namespace の場所を特定するロジックが必要ですが、
                // 今回は「最初にマッチしたグループ」で出す簡易仕様にします
                if (groupName == "texture") isTargetGroup = true;
            } else if (ContainsFolder(path, groupName)) {
                isTargetGroup = true;
            }

            if (isTargetGroup && allUsedNames.find(name) == allUsedNames.end()) {
                ss << "        constexpr const char* " << name << " = nullptr; // // [Error: File not found or name mismatch] \n";
                allUsedNames.insert(name);
            }
        }
        ss << "    }\n\n";
        };

    // 1. Textureグループ: パスに "Textures" が含まれる場合のみ反映
    writeGroup("texture", [](const ResourceEntry& e) {
        if (!e.texture.empty() && e.texture.find("Textures") != std::string::npos) {
            return e.texture;
        }
        return std::string("");
        });

    // 2. Modelグループ: パスに "Models" が含まれる場合のみ反映
    writeGroup("model", [](const ResourceEntry& e) {
        if (!e.model.empty() && e.model.find("Models") != std::string::npos) {
            return e.model;
        }
        return std::string("");
        });

    // 3. Audioグループ: 基本的にそのまま反映
    writeGroup("audio", [](const ResourceEntry& e) {
        return e.audio;
        });

    ss << "} // namespace LoadResourceID\n";

    // --- 保存処理（変更なし） ---
    std::string newContent = ss.str();
    if (IsContentChanged(outputPath, newContent)) {
        std::ofstream ofs(outputPath);
        ofs << newContent;
        // 差分があるなら、新しい内容でファイルを上書き保存
        std::cout << "[Update] Header file updated." << std::endl;
    } else {
        // 内容が同じなら、書き込みをスキップ（既存ファイルを保持）
        std::cout << "[Skip] No changes detected." << std::endl;
    }
}

bool ResourceCodeGenerator::IsContentChanged(const std::string& filePath, const std::string& newContent) {
    std::ifstream ifs(filePath);
    // ファイルが開けない（存在しない）場合は、新規作成が必要なため「変更あり」とみなす
    if (!ifs) return true;

    std::stringstream oldContent;
    oldContent << ifs.rdbuf();
    return oldContent.str() != newContent;
}

std::string ResourceCodeGenerator::ToUpperCamel(const std::string& name) {
    std::string res;
    bool nextUpper = true; // 最初の文字や区切り後の文字を大文字にするフラグ
    for (char c : name) {
        // 区切り文字（アンダースコア、スペース、ハイフン）を検出
        if (c == '_' || c == ' ' || c == '-') {
            nextUpper = true;
            continue;
        }
        if (nextUpper) {
            // 大文字に変換して追加
            res += (char)toupper(static_cast<unsigned char>(c));
            nextUpper = false;
        } else {
            // 小文字に変換して追加
            res += (char)tolower(static_cast<unsigned char>(c));
        }
    }
    return res;
}