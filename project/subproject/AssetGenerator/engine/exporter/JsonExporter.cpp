//#include "JsonExporter.h"
//#include <ResourceScanner.h>
//#include <fstream>
//#include <iomanip>
//#include <Windows.h>
//#include <algorithm>
//#include <map>
//#include <json.hpp>
//
//std::string JsonExporter::NormalizePath(const std::wstring& wstr) {
//    if (wstr.empty()) return "";
//    
//    // Windows環境のワイド文字を UTF-8 マルチバイト文字列へ変換
//    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
//    std::string strTo(sizeNeeded, 0);
//    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], sizeNeeded, NULL, NULL);
//
//    // JSONや各種ツールで扱いやすいよう、パス区切りを '/' に統一
//    std::replace(strTo.begin(), strTo.end(), '\\', '/');
//    return strTo;
//}
//
//std::string JsonExporter::GetTypeKey(const std::wstring& ext) {
//    // 拡張子を小文字に統一して判定（大文字小文字の揺れを吸収）
//    std::wstring lowerExt = ext;
//    std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(), ::tolower);
//
//    if (lowerExt == L".png" || lowerExt == L".jpg" || lowerExt == L".tga" || lowerExt == L".dds") return "texture";
//    if (lowerExt == L".fbx" || lowerExt == L".obj" || lowerExt == L".gltf") return "model";
//    if (lowerExt == L".wav" || lowerExt == L".mp3" || lowerExt == L".ogg") return "audio";
//    return "unknown";
//}
//
//void JsonExporter::Export(const std::filesystem::path& path, const std::string& date, double duration, const std::vector<AssetEntry>& assets) {
//    // 1. JSON オブジェクトの宣言
//    nlohmann::json root;
//
//    // 2. 基本情報の記録
//    root["info"]["date"] = date;
//    root["info"]["sec"] = duration;
//
//    // 3. アセットリストを階層構造に組み込む
//    for (const AssetEntry& asset : assets) {
//        // フォルダ自体のエントリは記録不要（ファイルへのパスが階層を作るため）
//        if (asset.isDirectory) continue;
//
//        std::string typeKey = GetTypeKey(asset.extension);
//        if (typeKey == "unknown") continue;
//
//        // パスを要素ごとに分解して処理
//        // 例: "Textures/Ui/Arrow.png" -> ["Textures", "Ui", "Arrow.png"]
//        std::filesystem::path p(asset.path);
//
//        // 階層を辿るための参照
//        nlohmann::json* current = &root;
//
//        for (std::filesystem::path::iterator it = p.begin(); it != p.end(); ++it) {
//            std::string part = NormalizePath(it->wstring());
//
//            if (std::next(it) == p.end()) {
//                // --- ファイル（末尾）の処理 ---
//                // ファイル名（拡張子なし）をキーにする
//                std::string id = NormalizePath(std::filesystem::path(*it).stem().wstring());
//                std::transform(id.begin(), id.end(), id.begin(), ::tolower);
//
//                // パスを正規化して格納
//                (*current)[id][typeKey] = NormalizePath(asset.path);
//            } else {
//                // --- 中間フォルダの処理 ---
//                // current を一段深く進める。存在しなければ自動的にオブジェクト "{}" が作られる
//                current = &((*current)[part]);
//            }
//        }
//    }
//
//    // 4. ファイル書き出し
//    std::ofstream file(path);
//    if (file.is_open()) {
//        // インデント4スペースで整形して出力
//        file << root.dump(4) << std::endl;
//    }
//}