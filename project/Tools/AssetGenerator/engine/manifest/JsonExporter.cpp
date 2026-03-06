#include "JsonExporter.h"
#include <fstream>

void JsonExporter::Save(
    const std::string& path,
    const nlohmann::json& data)
{
    // JSONのインデント幅（マジックナンバー排除）
    const int kIndentSize = 4;

    // --- ファイル書き出し ---
    // 指定パスのファイルをオープン（出力ストリーム）
    std::ofstream ofs(path);

    // 可読性のためにインデントを付けてシリアライズし、書き込む
    ofs << data.dump(kIndentSize);
}