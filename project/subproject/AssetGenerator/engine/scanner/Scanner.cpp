#include "Scanner.h"

namespace fs = std::filesystem;

/// <summary>
/// ディレクトリを再帰的に走査し、各ファイルをコールバックで返す
/// </summary>
void Scanner::ResourceScan(const fs::path& root, std::function<void(const AssetEntry&)> func) {
    for (const auto& entry : fs::recursive_directory_iterator(root)) {
        AssetEntry asset;

        // Resourcesからの相対パス
        asset.path = fs::relative(entry.path(), root).wstring();

        // ファイル名
        asset.fileName = entry.path().filename().wstring();

        // ディレクトリ判定
        asset.isDirectory = entry.is_directory();

        // 拡張子取得
        asset.extension = entry.path().extension().wstring();

        // コールバック実行
        func(asset);
    }
}

void Scanner::ScanAll(const std::filesystem::path& root, std::vector<AssetEntry>& out) {
    ResourceScan(root, [&](const AssetEntry& asset)
        {
            out.push_back(asset);
        });
}

void Scanner::ScanByExtension(const std::filesystem::path& root, const std::wstring& ext, std::vector<AssetEntry>& out) {
    // 既存の汎用スキャンを利用
    ResourceScan(root, [&](const AssetEntry& asset) {
        // ディレクトリは除外
        if (asset.isDirectory) return;

        // 拡張子一致チェック
        if (asset.extension == ext) {
            out.push_back(asset);
        }
        });
}