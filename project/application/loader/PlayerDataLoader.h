#pragma once
#include <string>
#include <PlayerData.h>
#include <json.hpp>

namespace MyGame {
    /// <summary>
	/// PlayerDataをJSONファイルから読み込むクラス
    /// </summary>
    class PlayerDataLoader {
    public:// メンバ関数
        /// <summary>
        /// 指定したファイル名のJSONからPlayerDataを読み込む
        /// </summary>
        /// <param name="path"></param>
        /// <returns></returns>
        static PlayerData Load(const std::string& path);
        static MyEngine::Transform LoadTransform(const nlohmann::json& transform);
    private: // 内部変数
        /// <summary>
        /// JSONオブジェクトに対して、指定したキーが存在するかを検査
        /// </summary>
        /// <param name="j">検査対象のJSONオブジェクト</param>
        /// <param name="keys">存在を検査するキーのリスト</param>
        /// <param name="filePath">エラーメッセージに含めるファイルパス</param>
        static void ValidateRequiredKeys(const nlohmann::json& j, const std::initializer_list<std::string>& keys, const std::string& filePath);
    private: // メンバ変数
		static const std::string kDefaultBaseDirectory;   /// データファイルの基本ディレクトリ
		static const std::string kExtension;              /// データファイルの拡張子
    };
}