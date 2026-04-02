#include "MessageService.h"
#include <fstream>
#include <cassert>
#include <regex>

namespace MyEngine {

    using namespace Editor;
    
    // ファイルから読み込んだデータを格納する変数
    nlohmann::json MessageService::data_;
    // static constをstaticに修正
    std::string MessageService::fullpath;
    bool MessageService::isLoaded_ = false;
    Editor::Language MessageService::currentLanguage_ = Editor::Language::Japanese;

    void MessageService::Initialize() {
        // すでにロード済みなら何もしない
        if (isLoaded_) return;
        // 言語ファイルをロード
        Load("Resources/Data/Languages/Localization.json");
    }

    void MessageService::Load(const std::string& filePath) {
        // ファイルを開く
        fullpath = filePath; // フルパスを保存
        std::ifstream file(fullpath);
        if (file.fail()) {
            // 開けなかった場合はアサート
            assert(false && "Localization.json open failed");
            return;
        }
        // JSONとして読み込む
        file >> data_;
        // JSONルートがオブジェクトであることを確認
        assert(data_.is_object());
        // 読み込み完了フラグを立てる
        isLoaded_ = true;
    }

    std::string MessageService::LangToKey() {
        // enumごとにをキー文字列に変換
        switch (currentLanguage_) {
        case Language::Japanese: return "jp";
        case Language::English:  return "en";
        default:
            assert(false && "Unsupported Language");
            return "en";
        }
    }

    std::string MessageService::GetText(const std::string& key) {
        // まだロードされていない場合
        if (!isLoaded_)
            return "Language Data Not Loaded";
        try {
            // 入力されたキーの頭に "Editor." を自動で付ける
            // "Editor + key" とキーを指定できる
            std::string fullKey = "Editor." + key;

            // ドット区切りをJSONポインタ形式に変換
            std::string path = "/" + std::regex_replace(fullKey, std::regex("\\."), "/");

            nlohmann::json::json_pointer ptr(path);

            return data_.at(ptr).at(LangToKey()).get<std::string>();
        }
        catch (...) {
            return "Key Not Found: " + key; // デバッグ用に送られたキーを表示
        }
    }
}
