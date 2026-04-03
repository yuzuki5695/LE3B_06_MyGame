#include "ResourceNode.h"
#include <algorithm>
#include <Windows.h>

void ResourceNode::addFile(const std::vector<std::wstring>& pathParts, const std::wstring& fullPath) {
    ResourceNode* current = this;
    for (size_t i = 0; i < pathParts.size(); ++i) {
        if (i == pathParts.size() - 1) {
            current->files.push_back(fullPath);
        } else {
            current = &current->children[pathParts[i]];
        }
    }
}

nlohmann::json ResourceNode::serialize() const {
    nlohmann::json j = nlohmann::json::object();

    // フォルダ階層を再帰的にシリアライズ
    for (const auto& [name, child] : children) {
        j[NormalizePath(name)] = child.serialize();
    }

    // ファイルリストを追加
    if (!files.empty()) {
        std::vector<std::string> normalizedFiles;
        for (const auto& f : files) {
            normalizedFiles.push_back(NormalizePath(f));
        }
        j["files"] = normalizedFiles;
    }

    return j;
}

std::string ResourceNode::NormalizePath(const std::wstring& wstr) {
    if (wstr.empty()) return "";
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], sizeNeeded, NULL, NULL);
    std::replace(strTo.begin(), strTo.end(), '\\', '/');
    return strTo;
}