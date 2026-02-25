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
    /// <summary>
    /// 初期化処理
    /// </summary>
    static void  Initialize(); 
    /// <summary>
    /// JSONファイルを読み込む
    /// </summary>
    /// <param name="fileName">拡張子なしのファイル名</param>
    static void Load(const std::string& filePath);
    /// <summary>
    /// 文字列キーでメッセージを取得
    /// 例: GetText("Editor.System.EngineInit", Language::Japanese);
    /// </summary>
    /// <param name="key">JSON内のキー（キーの頭Editorを除きドット区切り）</param>
    /// <returns>該当する文字列</returns>
    static std::string GetText(const std::string& key);
private: // 内部関数
    /// <summary>
    /// Language enum を JSONキーに変換
    /// Japanese → "jp"
    /// English  → "en"
    /// </summary>
    static std::string LangToKey();
private: // メンバ変数
    static nlohmann::json data_;                           /// データ本体
	static std::string fullpath;                     /// ファイルのフルパス
    static bool isLoaded_;                                 /// 読み込まれたかどうかの判定フラグ
	static Editor::Language currentLanguage_;              /// 現在の言語設定
public: // アクセッサ
	// Getter
    static Editor::Language GetCurrentLanguage() { return currentLanguage_; }    // 現在の言語を取得する関数
    // Setter
    static void SetLanguage(Editor::Language lang) { currentLanguage_ = lang; }
};