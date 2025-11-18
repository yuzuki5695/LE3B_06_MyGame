#pragma once
#include <memory>
#include <Camera.h>
#include <Object3d.h>

enum class FollowMode {
    UseInitialOffset,   // 初期オフセットを１回だけ保存して追従
    FreeOffset,         // 毎フレ更新（今のコードの "追加保存なし" に近い）
    FixedLookAt         // カメラを固定位置に置き、対象だけ注視
};

class ClearCamera {
public:
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize();
    /// <summary>
    /// 更新処理
    /// </summary>
    void Update();

private: // メンバ変数
    FollowMode followMode_ = FollowMode::FreeOffset;
    std::unique_ptr<Camera> maincamera_ = nullptr;  // 実際に描画で使用されるカメラインスタンス  

    // 追従対象
    Object3d* target_ = nullptr;

    // 最初のオフセットを保存するフラグ
    bool addedInitialOffset_ = false;
    bool saveInitialOffsetFlag_ = false;
    Vector3 offset_; // ターゲットとの相対位置

public:

    // 追従ターゲットをセット
    void SetTarget(Object3d* target) { target_ = target; }

    // モード切り替え
    void SetFollowMode(FollowMode mode) { followMode_ = mode; }
    FollowMode GetFollowMode() const { return followMode_; }
    Camera* GetActiveCamera() const { return maincamera_.get(); }

    void SetSaveInitialOffset(bool flag) { addedInitialOffset_ = flag; }
    bool GetSaveInitialOffset() const { return addedInitialOffset_; }


    void SetSavesaveInitialOffsetFlag(bool flag) { saveInitialOffsetFlag_ = flag; }
    bool GetSavesaveInitialOffsetFlag() const { return saveInitialOffsetFlag_; }


};