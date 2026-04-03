#pragma once
#include <map>
#include <vector>
#include <string>
#include <json.hpp>

class ResourceNode {
public:
    void addFile(const std::vector<std::wstring>& pathParts, const std::wstring& fullPath);
    
    // 文字列ではなく json オブジェクトを返すように変更
    nlohmann::json serialize() const;

private:
    std::map<std::wstring, ResourceNode> children;
    std::vector<std::wstring> files;

    static std::string NormalizePath(const std::wstring& wstr);
};