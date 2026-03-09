#include "JsonExporter.h"
#include <fstream>

void JsonExporter::Export(const std::filesystem::path& path) {
    // 指定パスのファイルを開く
    std::ofstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing.");
    }

    // JSONデータの書き込み
    file << "{\n";
    file << "  \"status\": \"success\",\n";
    file << "  \"msg\": \"Class-based generation with summary comments\"\n";
    file << "}\n";

    file.close();
}