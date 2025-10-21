#pragma once
#include <memory>
#include <Object3d.h>

struct OBB {
    Vector3 center;     // 中心位置
    Vector3 halfSize;   // 各軸方向の半径（スケールの半分）
    Vector3 axis[3];    // ローカル軸（回転を反映させたX, Y, Zベクトル）
};

/// <summary>
/// ゲームキャラクターの基底クラス
/// 初期化・更新・描画のインターフェイスを定義
/// </summary>
class BaseCharacter {
public:
    /// <summary>
    /// デストラクタ
    /// </summary> 
    virtual ~BaseCharacter() = default;
    /// <summary> 
    /// 初期化処理 
    /// </summary>
    virtual void Initialize() = 0;
    /// <summary>
    /// 更新処理
    /// </summary>
    virtual void Update() = 0;
    /// <summary>
    /// 描画処理
    /// </summary>
    virtual void Draw() = 0;
};