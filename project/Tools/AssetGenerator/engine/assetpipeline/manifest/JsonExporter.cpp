#include "JsonExporter.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>
#include <algorithm>

using json = nlohmann::json;

json JsonExporter::Build(const std::vector<FileInfo>& files) {
    json manifest = json::object();

    for (const auto& file : files) {
        const std::filesystem::path& p = file.path;
        json* current_node = &manifest;

        // 1. パスのコンポーネントをベクトルに分解する
        std::vector<std::string> parts;
        for (const auto& part : p) {
            parts.push_back(part.string());
        }

        // 2. 中間フォルダ階層を構築 (最後の要素はファイル名なので parts.size() - 1 まで)
        if (parts.size() > 1) {
            for (size_t i = 0; i < parts.size() - 1; ++i) {
                const std::string& folder_name = parts[i];
                // カレントディレクトリなどの特殊なパスを除外
                if (!folder_name.empty() && folder_name != "." && folder_name != "..") {
                    current_node = &((*current_node)[folder_name]);
                }
            }
        }

        // 3. 最後の要素（ファイル名）をキーとして処理
        // 拡張子なしのファイル名を取得
        std::string asset_name = p.stem().string();
        // 小文字化
        for (auto& c : asset_name) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }

        // 4. アセット種別に応じたキー設定
        std::string type_key;
        switch (file.type) {
            case AssetType::Texture: type_key = "texture"; break;
            case AssetType::Model:   type_key = "model";   break;
            case AssetType::Audio:   type_key = "audio";   break;
            default:                 type_key = "unknown"; break;
        }

        // 5. パスを保存 (generic_string でスラッシュに統一)
        (*current_node)[asset_name][type_key] = p.generic_string();
    }

    return manifest;
}


void JsonExporter::Save(const std::string& path, const nlohmann::json& data) {
    // JSONのインデント幅（マジックナンバー排除）
    const int kIndentSize = 4;

    // --- ファイル書き出し ---
    // 指定パスのファイルをオープン（出力ストリーム）
    std::ofstream ofs(path);

    // 可読性のためにインデントを付けてシリアライズし、書き込む
    ofs << data.dump(kIndentSize);
}