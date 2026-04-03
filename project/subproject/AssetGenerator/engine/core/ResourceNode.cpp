#include "ResourceNode.h"
#include <algorithm>
#include <Windows.h>

void ResourceNode::AddFile(const std::vector<std::wstring>& pathParts, const std::wstring& fullPath) {
    ResourceNode* current = this;
    // パスを順に辿る
    for (size_t i = 0; i < pathParts.size(); ++i) {
        // 最後の要素ならファイルとして登録
        if (i == pathParts.size() - 1) {
            current->files.push_back(fullPath);
        } else {
            // ディレクトリノードを取得（なければ生成）
            current = &current->children[pathParts[i]];
        }
    }
}

nlohmann::json ResourceNode::serialize() const {
    nlohmann::json j = nlohmann::json::object();

    // 子ディレクトリを再帰処理
    for (const std::pair<const std::wstring, ResourceNode>& pair : children) {
        const std::wstring& name = pair.first;   // ディレクトリ名
        const ResourceNode& child = pair.second; // 子ノード

        j[NormalizePath(name)] = child.serialize();
    }

    // ファイル一覧があれば追加
    if (!files.empty()) {
        std::vector<std::string> normalizedFiles;

        for (const std::wstring& f : files) {
            normalizedFiles.push_back(NormalizePath(f));
        }

        j = normalizedFiles;
    }

    return j;
}

std::string ResourceNode::NormalizePath(const std::wstring& wstr) {
    // 空文字チェック
    if (wstr.empty()) return "";

    // 必要なバッファサイズを取得
    uint32_t sizeNeeded = WideCharToMultiByte(
        CP_UTF8,                    // [in] 変換後の文字コード（UTF-8）
        0,                          // [in] フラグ（通常は0）
        &wstr[0],                   // [in] 変換元文字列（UTF-16）
        (uint32_t)wstr.size(),      // [in] 変換する文字数
        NULL,                       // [out] 出力バッファ（NULLでサイズ取得のみ）
        0,                          // [in] 出力バッファサイズ（0でサイズ計算）
        NULL,                       // [in] 既定文字（未使用）
        NULL                        // [out] 既定文字使用フラグ（未使用）
    );

    // UTF-8文字列用のバッファ確保
    std::string result(sizeNeeded, 0);
    
    // 実際の変換処理
    WideCharToMultiByte(
        CP_UTF8,                    // [in] 変換後の文字コード（UTF-8）
        0,                          // [in] フラグ
        &wstr[0],                   // [in] 変換元文字列
        (uint32_t)wstr.size(),      // [in] 文字数
        &result[0],                 // [out] 出力先バッファ
        sizeNeeded,                 // [in] 出力バッファサイズ
        NULL,                       // [in] 既定文字（未使用）
        NULL                        // [out] 既定文字使用フラグ（未使用）
    );
 
    // パス区切りを統一（Windows → 汎用）
    std::replace(result.begin(), result.end(), '\\', '/');

    return result;
}