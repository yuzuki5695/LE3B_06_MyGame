#pragma once
#include <BaseLoader.h>
#include <vector>

namespace AssetGen {
    /// <summary>
    /// アセットの書き出しルールを表す構造体
    /// </summary>
    struct ExportRule {
        std::filesystem::path dir;                  ///< スキャン対象ディレクトリ
        std::vector<std::wstring> extensions;       ///< 対象の拡張子
        std::filesystem::path output;               ///< 出力先ディレクトリ
    };
    /// <summary>
    /// rules.json を読み込み、ExportRuleとして保持するローダー
    /// </summary>
    class RuleLoader : public BaseLoader {
    public: // メンバ関数
        /// <summary>
        /// コンストラクタ
        /// </summary>
        /// <param name="resourceRoot">リソースのルートディレクトリ</param>
        /// <param name="outputDir">出力先ディレクトリ</param>
        RuleLoader(const std::filesystem::path& resourceRoot, const std::filesystem::path& outputDir);

        /// <summary>
        /// rules.json を読み込み、ExportRule の配列に変換する
        /// </summary>
        /// <param name="jsonPath">rules.json のパス</param>
        /// <returns>読み込み成功なら true、失敗なら false</returns>
        bool Load(const std::filesystem::path& jsonPath) override;

        /// <summary>
        /// 保持しているルールを破棄する
        /// </summary>
        void Clear() override;

        /// <summary>
        /// 読み込んだルールの一覧を取得する
        /// </summary>
        /// <returns>ExportRule の配列への const 参照</returns>
        const std::vector<ExportRule>& GetRules() const { return rules_; }
    private: // メンバ変数
        std::vector<ExportRule> rules_;                ///< 読み込んだルールのリスト
        std::filesystem::path resourceRoot_;           ///< リソースルートのパス
        std::filesystem::path outputDir_;              ///< 出力先ディレクトリのパス
    };
}