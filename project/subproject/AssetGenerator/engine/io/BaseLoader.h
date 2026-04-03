#pragma once
#include <string>
#include <vector>
#include <filesystem>

namespace AssetGen {
    /// <summary>
    /// 全てのローダーの基底となる抽象クラス
    /// </summary>
    class BaseLoader {
    public: // メンバ関数 
        /// <summary>
        /// 仮想デストラクタ
        /// </summary>
        virtual ~BaseLoader() = default;

        /// <summary>
        /// ファイルを読み込み、内部データに変換する
        /// </summary>
        /// <param name="filePath">読み込むファイルのパス</param>
        /// <returns>成功したらtrue</returns>
        virtual bool Load(const std::filesystem::path& filePath) = 0;

        /// <summary>
        /// 読み込み後のデータをクリアする
        /// </summary>
        virtual void Clear() = 0;
    };
}