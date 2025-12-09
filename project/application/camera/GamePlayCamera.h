#pragma once
#include <Vector3.h>
#include <CurveJsonLoader.h>
#include<Quaternion.h>
#include <Object3d.h>
#include<SceneCameraBase.h>

///====================================================
/// GamePlayCameraクラス
/// <summary>
/// ベジェ曲線をもとにカメラを自動移動させるクラス
/// カーブデータ（制御点）はJSONファイルから読み込む
/// </summary>
///====================================================
class GamePlayCamera : public SceneCameraBase {
public:
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize();
    /// <summary>
    /// 更新処理
    /// </summary>
    void Update();

    //Quaternion ForwardToQuaternion(const Vector3& forward);

    // 移動停止・再開の確認
    bool CheckAndResumeMovement();
    void UpdateBezierMovement();
    void UpdateCameraRotation();

    //void SwitchView(ViewType  targetType); // ← カメラ切り替えを開始する
    // イージング移動
    void UpdateTransition();

    // ViewType GetCurrentView() const { return mode_; }

   //  void SetCurrentView(ViewType view) { mode_ = view; }

    void UpdateSubCameraFollow(const Vector3& targetPos, const Vector3& offset);

    Vector3 CatmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t);

    Camera* GetActiveCamera();

    // サブカメラの追加
    void AddSubCamera(const CameraTransform& trans) override;
    // サブカメラの追加（複数登録に対応）
    void AddSubCameras(const std::vector<CameraTransform>& transforms) override;

    //void StartCameraTransition(ViewCameraType targetType, const std::string& subName);

    void UpdateSubCameraFollow();

private: // メンバ変数
    CurveJsonLoader* Jsondata = nullptr;         // ベジェ制御点を読み込むローダー

    Vector3 bezierPos_;                          // 現在のベジェ曲線上の位置
    std::vector<BezierPoint> bezierPoints;       // 移動に使う制御点データ
    bool movefige;                               // ベジェ曲線に沿って移動するフラグ      	
    float speed;                             // 移動速度
    Vector3 prevForward = { 0,0,1 };        // 前フレームの向きベクトル（回転補間用）        
    int currentSegment = 0; // レール上の現在のセグメントインデックス
    float t_ = 0.0f;

    Vector3 forward_ = { 0,0,1 };
    Vector3 up_ = { 0,1,0 };
    Vector3 right_ = { 1,0,0 };

    bool followInitialized_;

    float transitionTimer_ = 0.0f;
    float transitionDuration_ = 0.25f;

    Vector3 startPos_;
    Vector3 startRot_;
    Vector3 endPos_;
    Vector3 endRot_;

    std::string transitionSubCameraName_;



    Object3d* followTarget_ = nullptr; // 追従対象 
    Vector3 subOffset_ = { 0, 5, -10 }; // サブカメラのオフセット位置


    bool isMovingToTarget_ = false;

    Vector3 moveStartPos_;
    Vector3 moveTargetPos_;

    Vector3 rotStart_;
    Vector3 rotTarget_;

    float moveTimer_ = 0.0f;
    float moveDuration_ = 1.0f;  // 何秒で移動するか

    bool transitionStarted_ = false;

    bool isTransitioning_ = false;

    Vector3 mainStartPos_;
    Vector3 mainTargetPos_;

    Vector3 subStartPos_;
    Vector3 subTargetPos_;

    Vector3 mainStartRot_;
    Vector3 mainTargetRot_;


public: // アクセッサ（Getter / Setter）
    // getter 
    bool Getmovefige() { return movefige; }
    Vector3 GetbezierPos() { return bezierPos_; }
    // setter
    void Setmovefige(bool value) { movefige = value; }
    void SetbezierPos(Vector3 pos) { bezierPos_ = pos; }
    // BezierPoint の取得
    std::vector<BezierPoint>& GetBezierPoints() { return bezierPoints; }
    // すべての passed を ON/OFF
    void SetAllPassed(bool flag) { for (auto& point : bezierPoints) { point.passed = flag; } }
    // 一度通過した扱いにする（すべて true）
    void MarkAllAsPassed() { for (auto& point : bezierPoints) { if (!point.passed) point.passed = true; } }


    Vector3 GetBezierPos() const {
        return bezierPos_;
    }

    Vector3 GetForward() const {
        return forward_; // UpdateCameraRotation() 内で更新済み
    }

    Vector3 GetUp() const {
        return up_;      // これもカメラ更新で求めておく
    }

    Vector3 GetRight() const {
        return right_;
    }

    // 追従ターゲット設定
    void SetFollowTarget(Object3d* target) {
        followTarget_ = target;
    }
};