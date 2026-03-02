#pragma once
#include <memory>
#include <vector>
#include <Sprite.h>

/// <summary>
/// UIの基底クラス
/// 初期化・更新・描画のインターフェースを定義
/// </summary>
class BaseUI {
public:
    /// デストラクタ
    virtual ~BaseUI() = default;

    /// 初期化
    virtual void Initialize() = 0;

    /// 更新
    virtual void Update() = 0;

    /// 描画
    virtual void Draw() = 0;

    /// 表示フラグ操作
    void SetActive(bool isActive) { isActive_ = isActive; }
    bool IsActive() const { return isActive_; }
    // どのシーン用のUIか設定・取得
    void SetTargetSceneName(const std::string& name) { targetSceneName_ = name; }
    const std::string& GetTargetSceneName() const { return targetSceneName_; }

protected:
    bool isActive_ = true;   // 表示・更新を行うかどうか
    std::string targetSceneName_; // 追加：所属シーン名
};