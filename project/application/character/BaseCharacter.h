#pragma once
#include <memory>
#include <Object3d.h>

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