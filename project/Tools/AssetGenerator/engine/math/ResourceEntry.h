#pragma once
#include <string>

/// <summary>
/// 各アセットの情報を保持する構造体
/// </summary>
struct ResourceEntry {
    std::string name;    //!< アセットの論理名（例: player_idle）
    std::string model;   //!< モデルファイルのパス
    std::string texture; //!< テクスチャファイルのパス
    std::string audio;   //!< 音源ファイルのパス
};