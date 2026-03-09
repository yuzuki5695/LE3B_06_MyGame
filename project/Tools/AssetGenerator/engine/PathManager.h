#pragma once
#include <filesystem>
#include <string>

/// <summary>
/// 実行環境やプロジェクト構成に合わせて、適切なディレクトリパスを特定・管理するクラス
/// </summary>
class PathManager {
public: // メンバ関数
    /// <summary>
    /// 実行ファイル（.exe）自体の絶対パスを取得する
    /// </summary>
    /// <returns>実行ファイルの絶対パス</returns>
    static std::filesystem::path GetExePath();

    /// <summary>
    /// リソースの出力先となる "Resources" フォルダのルートを特定する
    /// </summary>
    /// <param name="argc">コマンドライン引数の数</param>
    /// <param name="argv">コマンドライン引数の配列</param>
    /// <returns>特定された Resources フォルダのパス（見つからない場合は空）</returns>
    static std::filesystem::path FindResourcesRoot(int argc, char* argv[]);

private: // プライベートメンバ関数
    /// <summary>
    /// 指定されたパスから親ディレクトリを遡り、再帰的に Resources フォルダを探索する
    /// </summary>
    /// <param name="current">探索を開始する現在のパス</param>
    /// <returns>見つかった Resources フォルダのパス</returns>
    static std::filesystem::path SearchRecursive(std::filesystem::path current);
};