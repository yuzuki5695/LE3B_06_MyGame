//#pragma once
//#include <filesystem>
//#include <string>
//
//// 前方宣言
//struct AssetEntry;
//
///// <summary>
///// 取得したアセット情報等を JSON 形式でファイルに書き出すためのエクスポートクラス
///// </summary>
//class JsonExporter {
//public: // メンバ関数
//    /// <summary>
//    /// アセットリストを Group/ID 構造に統合し、指定されたパスに JSON を出力する
//    /// </summary>
//    /// <param name="path">出力先のファイルパス</param>
//    /// <param name="date">manifest 内に記録する生成日の文字列</param> 
//    /// <param name="duration">計測された処理時間（秒）</param>
//    /// <param name="assets">Scanner によって取得された生のアセットデータリスト</param>
//    void Export(const std::filesystem::path& path, const std::string& date, double duration, const std::vector<AssetEntry>& assets); 
//    /// <summary>
//    /// Windows形式のパス区切り '\' を、JSONで扱いやすい '/' に置換し、UTF-8へ変換する
//    /// </summary>
//    /// <param name="wstr">変換対象のワイド文字列（パス）</param>
//    /// <returns>変換後の UTF-8 文字列</returns>
//    std::string NormalizePath(const std::wstring& wstr);    
//    /// <summary>
//    /// 拡張子に基づいて、JSON上のキー名（texture / model / audio）を決定する
//    /// </summary>
//    /// <param name="ext">判定対象の拡張子（ドットを含む）</param>
//    /// <returns>アセット種別を表す文字列。対象外なら "unknown"</returns>
//    std::string GetTypeKey(const std::wstring& ext);
//};