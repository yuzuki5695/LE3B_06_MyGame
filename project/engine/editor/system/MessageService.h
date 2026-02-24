#pragma once
#include <string>
#include <EditorTypes.h>
#include <unordered_map>
#include <json.hpp>

/// <summary>
/// 多言語メッセージ管理クラス
/// </summary>
class MessageService {
public: // メンバ関数
    static const std::string kDefaultBaseDirectory;    /// ファイルのデフォルトディレクトリ
    static const std::string kExtension;               /// 拡張子
    /// <summary>
    /// JSONファイルを読み込む
    /// </summary>
    /// <param name="fileName">拡張子なしのファイル名</param>
    static void Load(const std::string& filePath);
    /// <summary>
    /// 文字列キーでメッセージを取得
    /// 例: GetText("Editor.System.EngineInit", Language::Japanese);
    /// </summary>
    /// <param name="key">JSON内のキー（ドット区切り）</param>
    /// <returns>該当する文字列</returns>
    static std::string GetText(const std::string& key);
    /// <summary>
    /// MessageId を使ってメッセージ取得
    /// 例: FetchMessage(MessageId::EngineInitialized, Language::Japanese);
    /// </summary>
    /// <param name="id">メッセージID</param>
    /// <returns>該当する文字列</returns>
    static std::string FetchMessage(Editor::MessageId id);
private: // 内部関数
    /// <summary>
    /// Language enum を JSONキーに変換
    /// Japanese → "jp"
    /// English  → "en"
    /// </summary>
    static std::string LangToKey();
private: // メンバ変数
    static nlohmann::json data_; // 読み込んだデータ本体
    static bool isLoaded_; // 読み込まれたかどうか
    static const std::unordered_map<Editor::MessageId, std::string> messageKeyTable_; //     /// MessageId と キーの対応テーブル
    static Editor::Language currentLanguage_;
public: // アクセッサ
    // Setter
    static void SetLanguage(Editor::Language lang) { currentLanguage_ = lang; }
};