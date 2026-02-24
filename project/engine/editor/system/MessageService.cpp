#include "MessageService.h"
#include <fstream>
#include <cassert>
#include <regex>

using namespace Editor;

// ファルルから読み込んだデータを格納する変数
const std::string MessageService::kDefaultBaseDirectory = "Resources/Data/Languages/";
const std::string MessageService::kExtension = ".json";

bool MessageService::isLoaded_ = false; // 読み込まれたかどうか
nlohmann::json MessageService::data_;
Editor::Language MessageService::currentLanguage_ = Editor::Language::Japanese; // 静的変数の初期化（デフォルトは日本語など）
const std::unordered_map<MessageId, std::string>
MessageService::messageKeyTable_ = { // MessageId と JSON内のキーの対応テーブル
    { MessageId::EngineInitialized, "Editor.System.EngineInit" },
    { MessageId::SaveCompleted,     "Editor.System.SaveComplete" },
    { MessageId::LangChanged,       "Editor.System.LangChanged" }
};

void MessageService::Load(const std::string& filePath) {
    // フルパス生成
    const std::string fullpath = kDefaultBaseDirectory + filePath + kExtension;

    // ファイルを開く
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

std::string MessageService::GetText(const std::string& key) {
    // まだロードされていない場合
    if (!isLoaded_)
        return "Language Data Not Loaded";

    try {
        // ドット区切りをJSONポインタ形式に変換
        // "Editor.System.EngineInit"
        // → "/Editor/System/EngineInit"
        std::string path = "/" + std::regex_replace(key, std::regex("\\."), "/");

        // JSONポインタ作成
        nlohmann::json::json_pointer ptr(path);

        // 該当ノードから言語キー("jp"/"en")を取得
        return data_.at(ptr)
            .at(LangToKey())
            .get<std::string>();

    }
    catch (...) {
        // キーが存在しない場合
        return "Key Not Found";
    }
}

std::string MessageService::FetchMessage(MessageId id) {
    // 対応テーブルを検索
    auto it = messageKeyTable_.find(id);
    if (it == messageKeyTable_.end()) return "Unknown MessageID";

    return GetText(it->second);
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
