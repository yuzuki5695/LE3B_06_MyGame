#pragma once
#include <vector>
#include <json.hpp>
#include <ResourceId.h>
#include <ResourceEntry.h>

/// <summary>
/// Manifest.json を解析し、プログラム内で利用しやすい ResourceEntry のリストに変換するクラス
/// </summary>
class ResourceIdGenerator {
public:
    /// <summary>
    /// Manifest.json を読み込んで ResourceEntry の一覧を構築する
    /// </summary>
    /// <param name="jsonPath">読み込む JSON ファイルのパス</param>
    /// <returns>読み込みと解析に成功すれば true</returns>
    bool LoadFromJson(const std::string& jsonPath);
    /// <summary>
    /// 解析済みのリソースエントリリストを取得する
    /// </summary>
    /// <returns>ResourceEntry のベクタへの定数参照</returns>
    const std::vector<ResourceEntry>& GetResources() const { return resourceids_; }
private:
    /// <summary>
    /// JSON 内のキー文字列を ResourceType 列挙型に変換する
    /// </summary>
    /// <param name="key">"texture", "model", "audio" などの文字列</param>
    /// <returns>対応する ResourceType</returns>
    ResourceType ToResourceType(const std::string& key);
private:
    // 解析されたリソース情報の格納用リスト
    std::vector<ResourceEntry> resourceids_;
};