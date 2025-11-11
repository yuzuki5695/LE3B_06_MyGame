#pragma once
#include <vector>
#include <memory>
#include <Camera.h>
#include <Vector3.h>
#include <CurveJsonLoader.h>
#include<Quaternion.h>
#include <Object3d.h>

enum class ViewType  {
    Main,  // メイン(レールカメラ)
    Sub,    // サブ(固定または追従カメラ)
    Transition // 切り替え中
};
enum class SubCamType {
    Fixed,      // 定点カメラ
    Follow      // 対象追従カメラ
};

///====================================================
/// GameCameraクラス
/// <summary>
/// ベジェ曲線をもとにカメラを自動移動させるクラス
/// カーブデータ（制御点）はJSONファイルから読み込む
/// </summary>
///====================================================
class GameCamera {
public:
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize();
    /// <summary>
    /// 更新処理
    /// </summary>
    void Update();
    Vector3 LookAtRotation(const Vector3& forward);

    Quaternion ForwardToQuaternion(const Vector3& forward);
    Vector3 Slerp(const Vector3& v0, const Vector3& v1, float t);

    // 移動停止・再開の確認
    bool CheckAndResumeMovement();
    void UpdateBezierMovement();
    void UpdateCameraRotation();

    void SwitchView(ViewType  targetType); // ← カメラ切り替えを開始する
    // イージング移動
    void UpdateTransition();

    ViewType GetCurrentView() const { return mode_; }

    void SetCurrentView(ViewType view) { mode_ = view; }

    void UpdateSubCameraFollow(const Vector3& targetPos, const Vector3& offset);

    Vector3 CatmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t);

private: // メンバ変数
    CurveJsonLoader* Jsondata = nullptr;         // ベジェ制御点を読み込むローダー
    std::unique_ptr<Camera> maincamera_ = nullptr;  // 実際に描画で使用されるカメラインスタンス  
    std::unique_ptr<Camera> subcamera_ = nullptr; // サブカメラ用インスタンス
    Vector3 bezierPos_;                          // 現在のベジェ曲線上の位置
    std::vector<BezierPoint> bezierPoints;       // 移動に使う制御点データ
    bool movefige;                               // ベジェ曲線に沿って移動するフラグ      	
    float speed;                             // 移動速度
    Vector3 prevForward = { 0,0,1 };        // 前フレームの向きベクトル（回転補間用）        
    int currentSegment = 0; // レール上の現在のセグメントインデックス

    ViewType  mode_ = ViewType::Main;   // 現在のモード
    ViewType  transitionTarget_ = ViewType::Main;

    Vector3 subPosition_ = { 0, 0, 0 };    // サブカメラ固定位置
    Vector3 subTarget_ = { 0, 0, 0 };    // サブカメラが見る対象
    bool subFollow_ = false;               // 対象追従フラグ
    bool recoveringFromSub_ = false;
    float recoverTimer_ = 0.0f;
    float recoverDuration_ = 1.0f; // 1秒間かけて復帰

    Vector3 startPos_;
    Vector3 startRot_;
    Vector3 endPos_;
    Vector3 endRot_;

    float transitionTimer_ = 0.0f;
    float transitionDuration_ = 1.0f; // 1秒で切り替え

    Object3d* followTarget_ = nullptr; // 追従対象
    Vector3 subOffset_ = { 0, 5, -10 }; // サブカメラのオフセット位置
    bool followMode_ = true; // 追従モード or 固定モード

    float yaw;
    float pitch;
    bool followInitialized_;
    float t_ = 0.0f;

public: // アクセッサ（Getter / Setter）
    // getter 
    Camera* GetMainCamera() { return maincamera_.get(); }
    Camera* GetSubCamera() { return subcamera_.get(); }

    bool Getmovefige() { return movefige; }
    Vector3 GetbezierPos() { return bezierPos_; }
    // setter
    void Setmovefige(bool value) { movefige = value; }
    void SetbezierPos(Vector3 pos) { bezierPos_ = pos; }
    // 前方ベクトルを取得
    Vector3 GetForward() const {
        // 回転行列を使わずforwardを保持している場合はこちら
        return prevForward;
    }
    // BezierPoint の取得
    std::vector<BezierPoint>& GetBezierPoints() { return bezierPoints; }
    // すべての passed を ON/OFF
    void SetAllPassed(bool flag) { for (auto& point : bezierPoints) { point.passed = flag; } }
    // 一度通過した扱いにする（すべて true）
    void MarkAllAsPassed() { for (auto& point : bezierPoints) { if (!point.passed) point.passed = true; } }
    // プレイヤーなど追従対象をセット
    void SetFollowTarget(Object3d* target) {
        followTarget_ = target;
        // ターゲットが変わったら初期化フラグを立てる（次回Subモードでリセットされる）      
        followInitialized_ = false;
    }
    ViewType GetMode() const { return mode_; }
    Camera* GetActiveCamera() {
        switch (mode_) {
        case ViewType::Sub:
            return subcamera_.get();
        case ViewType::Main:
            return maincamera_.get();
        case ViewType::Transition:
            // Transition中は maincamera_ をベースに更新していることが多い
            return maincamera_.get();
        }
    }
};