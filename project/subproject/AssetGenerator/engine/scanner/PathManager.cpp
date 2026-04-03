//#include "PathManager.h"
//#include <Windows.h>
//
//std::filesystem::path PathManager::GetExePath() {
//    // Windows API を使用してモジュールのフルパスを取得
//    wchar_t path[MAX_PATH];
//    GetModuleFileNameW(NULL, path, MAX_PATH);
//    return std::filesystem::path(path);
//}
//
//std::filesystem::path PathManager::FindResourcesRoot(int argc, char* argv[]) {
//    // コマンドライン引数（$(ProjectDir)Resources 等）が渡されている場合は最優先で使用
//    if (argc > 1) {
//        return std::filesystem::absolute(argv[1]);
//    }
//    // 引数がない場合は実行ファイルの場所から遡って自動探索
//    return SearchRecursive(GetExePath());
//}
//
//std::filesystem::path PathManager::SearchRecursive(std::filesystem::path current) {
//    current = std::filesystem::absolute(current);
//    if (std::filesystem::is_regular_file(current)) {
//        current = current.parent_path(); // ファイルパスならディレクトリパスへ変換
//    }
//
//    while (current.has_parent_path()) {
//        // 現在のディレクトリ名が対象(Resources)と一致するか確認
//        if (current.filename() == L"Resources") return current;
//
//        // 子階層に直接 Resources が含まれるケース（プロジェクトルートからの検索）を確認
//        std::filesystem::path check = current / L"Resources";
//        if (std::filesystem::exists(check)) return check;
//
//        // エンジン固有の project フォルダ構造を考慮したパス解決
//        if (current.filename() == L"project") {
//            if (std::filesystem::exists(current / L"Resources")) {
//                return current / L"Resources";
//            }
//        }
//
//        // 上位階層へ遡り、ルート（C:\等）に達したらループを抜ける
//        if (current == current.parent_path()) break;
//        current = current.parent_path();
//    }
//    return "";
//}