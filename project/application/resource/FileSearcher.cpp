#include"FileSearcher.h"
#include <filesystem>

std::vector<std::string> FileSearcher::GetAllFiles(const std::vector<std::string>& excludeDirs) {
    std::vector<std::string> files;
    // ルートディレクトリ以下を再帰的に走査
    for (const auto& entry : std::filesystem::recursive_directory_iterator(root_)) {
        // ファイル以外（ディレクトリ等）はスキップ
        if (!entry.is_regular_file()) { continue; }

        // フルパス文字列を取得
        std::string pathStr = entry.path().string();
        // 除外ディレクトリに該当するかチェック
        bool skip = false;
        for (const auto& ex : excludeDirs) {
            // パス文字列に除外ディレクトリ名が含まれていればスキップ
            if (pathStr.find(ex) != std::string::npos) {
                skip = true;
                break;
            }
        }
        // 除外対象でなければ、root からの相対パスを追加
        if (!skip) {
            files.push_back(std::filesystem::relative(entry.path(), root_).string());
        }
    }
    // 検索結果を返す
    return files;
}