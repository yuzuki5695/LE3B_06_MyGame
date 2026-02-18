#pragma once
#include <string>
#include <vector>
#include <map>
#include <ResourceIdGenerator.h>

/// <summary>
/// リソース定義コード（C++ヘッダー）を自動生成するクラス。
/// 既存の定義を解析し、定数名の一貫性を保ちつつ最新の状態に更新する。
/// </summary>
class ResourceCodeGenerator {
public:
    /// <summary>
    /// リソースエントリのリストから C++ ヘッダーファイルを生成する
    /// </summary>
    /// <param name="outputPath">出力するヘッダーファイルのパス</param>
    /// <param name="entries">ResourceIdGeneratorによって解析されたリソース情報リスト</param>
    void GenerateHeader(
        const std::string& outputPath,
        const std::vector<ResourceEntry>& entries
    );
    /// <summary>
    /// 文字列をアッパーキャメルケースに変換する（例: "enemy_bullet" -> "EnemyBullet"）
    /// </summary>
    /// <param name="name">変換元の文字列</param>
    /// <returns>変換後の文字列</returns>
    std::string ToUpperCamel(const std::string& name);
    /// <summary>
    /// 指定されたパスの中に特定のフォルダ名が含まれているか判定する
    /// </summary>
    /// <param name="path">対象のファイルパス</param>
    /// <param name="folder">検索するフォルダ名</param>
    /// <returns>含まれていれば true</returns>
    static bool ContainsFolder(const std::string& path, const std::string& folder) {
        return path.find(folder + "/") != std::string::npos;
    };
    /// <summary>
    /// 指定されたパスが特定のフォルダ直下（または配下）にあるか判定する
    /// </summary>
    bool IsUnderFolder(const std::string& path, const std::string& folder) {
        return path.find(folder + "/") != std::string::npos;
    };
private:
    /// <summary>
    /// 既存のヘッダーファイルを読み込み、定義済みの変数名とパスの対応表を作成する
    /// </summary>
    /// <param name="filePath">既存のヘッダーパス</param>
    /// <returns>パスをキー、変数名を値としたマップ</returns>
    std::map<std::string, std::string> ParseExistingHeader(const std::string& filePath);
    /// <summary>
    /// 新しく生成した内容が現在のファイル内容と異なるかチェックする
    /// </summary>
    /// <param name="filePath">比較対象のファイルパス</param>
    /// <param name="newContent">新しく生成したコード文字列</param>
    /// <returns>変更があれば true、同じであれば false</returns>
    bool IsContentChanged(const std::string& filePath, const std::string& newContent);
};