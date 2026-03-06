#pragma once
#include <string>

/// <summary>
/// パス文字列の正規化を行うユーティリティクラス
/// OS間の区切り文字の差異（\ と /）を解消する責務を持つ。
/// </summary>
class PathNormalizer
{
public:
    /// <summary>
    /// パス内のバックスラッシュをスラッシュに置換して統一する
    /// </summary>
    /// <param name="path">変換前のパス文字列</param>
    /// <returns>正規化されたパス文字列</returns>
    static std::string Normalize(const std::string& path);
};