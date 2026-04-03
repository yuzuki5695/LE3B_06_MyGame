//#pragma once
//#include <filesystem>
//#include <vector>
//#include <string>
//
///// <summary>
///// アセットの情報を保持する構造体
///// </summary>
//struct AssetEntry {
//    std::wstring path;      // Resources からの相対パス
//    std::wstring fileName;  // ファイル名
//    bool isDirectory;       // ディレクトリかどうか
//    std::wstring extension; // 拡張子
//};
//
///// <summary>
///// Resources 内のファイルを走査し、アセットリストを作成するクラス
///// </summary>
//class ResourceScanner {
//public: // メンバ関数
//    /// <summary>
//    /// 指定されたルートディレクトリ以下を再帰的にスキャンする
//    /// </summary>
//    /// <param name="root">スキャンを開始するルート（Resources）のパス</param>
//    /// <param name="excludeFileName">走査対象から除外するファイル名</param>
//    std::vector<AssetEntry> Scan(const std::filesystem::path& root, const std::wstring& excludeFileName);
//};