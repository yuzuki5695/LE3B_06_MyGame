#include "PathNormalizer.h"
#include <algorithm>

std::string PathNormalizer::Normalize(const std::string& path)
{
    std::string result = path;

    // --- 文字列置換処理 ---
    // Windows形式の区切り文字 '\' を環境に依存しない '/' に置換する
    std::replace(result.begin(), result.end(), '\\', '/');

    return result;
}