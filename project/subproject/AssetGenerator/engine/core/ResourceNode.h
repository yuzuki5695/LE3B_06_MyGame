#pragma once
#include <map>
#include <vector>
#include <string>
#include <json.hpp>

namespace AssetGen {
    /// <summary>
    /// リソースの階層構造（ディレクトリツリー）を表現するノードクラス
    /// 
    /// 【役割】
    /// ・ディレクトリ構造をツリーとして保持する
    /// ・ファイルを階層ごとに分類する
    /// ・JSON形式へ変換する
    /// 
    /// 【構造】
    /// ・children : サブディレクトリ（名前 → 子ノード）
    /// ・files    : このディレクトリ直下にあるファイル一覧
    /// 【注意】
    /// ・パスはUTF-8に変換され、'\\' は '/' に正規化される
    /// ・ディレクトリ名がJSONキーになる
    /// </summary>
    class ResourceNode {
    public: // メンバ関数
        /// <summary>
        /// ファイルパスをツリー構造に追加する
        /// </summary>
        /// <param name="pathParts">パスを分割した要素配列</param>
        /// <param name="fullPath">フルパス（相対パス）</param>
        void AddFile(const std::vector<std::wstring>& pathParts, const std::wstring& fullPath);

        /// <summary>
        /// 現在のノード以下をJSON形式に変換する
        /// </summary>
        nlohmann::json serialize() const;
    private: // 内部関数
        /// <summary>
        /// パス文字列をJSON出力用に正規化する
        /// wstringをUTF-8文字列へ変換し、パス表記を正規化する
        /// 【用途】
        /// ・JSON出力時の文字コード統一
        /// ・Windows依存の '\\' を排除する
        /// </summary>
        /// <param name="wstr">変換対象のUTF-16文字列</param>
        /// <returns>UTF-8に変換された文字列</returns>
        static std::string NormalizePath(const std::wstring& wstr);
    private: // メンバ変数
        /// <summary>
        /// 子ディレクトリ一覧（ディレクトリ名 → ノード）
        /// </summary>
        std::map<std::wstring, ResourceNode> children;
        /// <summary>
        /// このディレクトリ直下のファイル一覧（フルパス）
        /// </summary>
        std::vector<std::wstring> files;
    };
}