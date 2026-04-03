#pragma once
#include <filesystem>
#include <vector>
#include <RuleLoader.h>
#include <AssetInfoLoader.h>

namespace AssetGen {
    /// <summary>
    /// アセット生成プロセスの全体を制御するメインクラス
    /// </summary>
    class AssetGenerator {
	public: // メンバ関数    
        /// <summary>
        /// プログラムのエントリポイントから呼び出される実行関数
        /// </summary>
        /// <param name="argc">コマンドライン引数の数</param>
        /// <param name="argv">コマンドライン引数の配列</param>
        void Run(int argc, char* argv[]);
	private: // 内部関数
        /// <summary>
        /// パスの設定や各ローダーの初期化を行う
        /// </summary>
        void Initialize(int argc, char* argv[]);

        /// <summary>
        /// ルールの読み込み、スキャン、コード生成のメインロジックを実行する
        /// </summary>
        void Execute();

        /// <summary>
        /// 終了メッセージの表示などの後処理を行う
        /// </summary>
        void Finalize();
	private: // メンバ変数
        // --- パス関連 ---
        std::filesystem::path resourceRoot_;      ///< 素材ファイルが存在するルートディレクトリ
        std::filesystem::path outputDir_;         ///< JSONやヘッダファイルを出力するディレクトリ
        std::filesystem::path rulesJsonPath_;     ///< スキャンルールが記述されたJSONのパス
		std::filesystem::path assetInfoPath_; ///< アセットヘッダー情報の出力先のパス
        
        // --- ロジック・データ保持 ---
        std::unique_ptr<RuleLoader> ruleLoader_;      ///< rules.jsonの読み込み管理
        std::unique_ptr<AssetInfoLoader> assetLoader_; ///< 生成されたJSON群からのID情報読み込み管理

        std::vector<ExportRule> rules_;               ///< 適用されるルールのリスト
    };
}