//#pragma once
//#include <filesystem>
//#include <string>
//#include <vector>
//
//// 前方宣言
//struct AssetEntry;
//
///// <summary>
///// アセットパスから C++ で利用可能な ID（enum）を定義したヘッダーファイルを生成する
///// </summary>
//class HeaderGenerator {
//public: // メンバ関数
//    /// <summary>
//    /// アセットリストを解析し、ResourceID.h を生成する
//    /// </summary>
//    void Generate(const std::filesystem::path& outputPath, const std::vector<AssetEntry>& assets);
//
//private: // プライベートメンバ関数
//    // C++の識別子として使えない文字を置換する
//    std::string Sanitize(const std::string& str);
//};