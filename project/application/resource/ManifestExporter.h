#pragma once
#include <vector>
#include <string>

/// リソースの種類を表す列挙体
enum class AssetType {
    Texture,   // テクスチャ
    Model,     // 3Dモデル
    Audio,     // 音源
    Unknown    // 非対応拡張子
};

/// <summary>
/// ManifestExporter
/// ファイルパス一覧から Asset Manifest(JSON) を生成するクラス
/// </summary>
class ManifestExporter {
private: // メンバ関数
    /// <summary>
    /// 拡張子から AssetType を判定する
    /// 未対応の拡張子は Unknown を返す
    /// </summary>
    AssetType GetAssetType(const std::string& ext);
    /// <summary>
    /// ファイルパスから Asset の論理名を生成する
    /// 拡張子を除いたファイル名を小文字化したものを使用
    /// </summary>
    std::string CreateId(std::string path, AssetType type);
    /// <summary>
    /// AssetType を JSON のキー文字列へ変換する
    /// </summary>
    std::string ToString(AssetType type); 
    /// <summary>
    /// パスの最上位ディレクトリを Group 名として取得する
    /// 例: "Bullet/EnemyBullet.obj" → "Bullet"
    /// </summary>
    std::string GetGroup(const std::string& path);
    /// <summary>
    /// パス区切りを '/' に正規化する
    /// Windows / Unix 混在対策用のユーティリティ
    /// </summary>
    static std::string NormalizePath(std::string path);
    /// <summary>
    /// 既存ファイルと内容を比較し,差分がある場合のみ上書き保存する
    /// </summary>
    void SaveIfChanged(const std::string& path, const std::string& content); 
public: // メンバ関数
    /// <summary>
    /// ファイル出力処理
    /// ファイルリストを受け取ってJSON化出力。内容に差分がある場合のみファイルへ書き出す 
    /// </summary> 
    void Export(const std::string& outputPath, const std::vector<std::string>& fileList);
};