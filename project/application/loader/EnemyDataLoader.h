#pragma once
#include <BaseCharacterJsonLoader.h>
#include <EnemyData.h>

class EnemyDataLoader : public BaseCharacterJsonLoader {
public:// メンバ関数  
    /// <summary>
    /// JSONファイルからEnemyDataを読み込む
    /// </summary>
    /// <param name="fileName">拡張子なしのファイル名</param>
    /// <returns>EnemyData構造体</returns>
    static EnemyData Load(const std::string& fileName);

private: // メンバ変数
    static const std::string kDefaultBaseDirectory;   /// 格納ディレクトリ
    static const std::string kExtension;              /// ファイル拡張子
};