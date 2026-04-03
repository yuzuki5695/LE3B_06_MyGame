//#include "ResourceScanner.h"
//
//namespace fs = std::filesystem;
//
//std::vector<AssetEntry> ResourceScanner::Scan(const fs::path& root, const std::wstring& excludeFileName) {
//    std::vector<AssetEntry> assets;
//
//    // 指定されたルートディレクトリが存在しない場合は、空のリストを返して安全に終了する
//    if (!fs::exists(root)) {
//        return assets;
//    }
//    // サブフォルダの中身まで含めてすべて巡回する
//    for (const fs::directory_entry& entry : fs::recursive_directory_iterator(root)) {
//        AssetEntry asset;
//
//        // --- パスとファイル情報の抽出 ---
//
//        // ルートディレクトリからの相対パスを取得（ファイル内での一意な識別子として使用）
//        asset.path = fs::relative(entry.path(), root).wstring();
//
//        // 拡張子を含む純粋なファイル名を取得
//        asset.fileName = entry.path().filename().wstring();
//
//        // 現在のエントリがディレクトリ（フォルダ）かファイルかを判定
//        asset.isDirectory = entry.is_directory();
//
//        // ファイルの拡張子（.png, .obj など）を取得。後のアセットタイプ判定に使用
//        asset.extension = entry.path().extension().wstring();
//
//        // 除外フィルタリング 
//        if (asset.fileName == excludeFileName) {
//            continue;
//        }
//        // 抽出した情報をリストに追加
//        assets.push_back(asset);
//    }
//
//    return assets;
//}