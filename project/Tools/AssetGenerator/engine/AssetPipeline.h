#pragma once
#include <memory>
#include <JsonExporter.h>

/// <summary>
/// アセット生成プロセスの初期化、実行、終了処理を統括するパイプラインクラス。
/// エンジンの Framework クラスと同様の役割を持つ。
/// </summary>
class AssetPipeline {
public: // メンバ関数
    /// <summary>
    /// パイプラインの一連の処理を実行する
    /// </summary>
    /// <param name="argc">コマンドライン引数の数</param>
    /// <param name="argv">コマンドライン引数の配列</param>
    void Run(int argc, char* argv[]);
private: // プライベートメンバ関数
    /// <summary>
    /// ロケールの設定、パスの解決、内部コンポーネントの初期化を行う
    /// </summary>
    void Initialize(int argc, char* argv[]);
    /// <summary>
    /// 実際のアセットスキャンやファイル書き出し処理を実行する
    /// </summary>
    void Execute();
    /// <summary>
    /// リソースの解放処理を行う
    /// </summary>
    void Finalize();
    // メンバ変数
    std::wstring manifestFileName_;          // 出力ファイル名(.json)
    std::filesystem::path resourcesRoot_;    // .JSON出力先のルートとなるフォルダのパス 
    std::unique_ptr<JsonExporter> exporter_; // .JSON出力用インスタンス

    std::wstring resourceIDFileName_;        // ヘッダーファイル名
    std::filesystem::path generatedDir_;     // ヘッダー出力先のルートとなるフォルダのパス
};