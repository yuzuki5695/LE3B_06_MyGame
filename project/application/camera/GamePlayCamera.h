#pragma once
#include <ISceneCameraBehavior.h>
#include <CurveJsonLoader.h>
#include <memory>
//#include <RailCameraController.h>

namespace MyGame {

    struct RailInfo {
        float totalLength = 0.0f;
        std::vector<float> segmentLengths;
    };

    class GamePlayCamera : public ISceneCameraBehavior {
    public: // メンバ関数
        void Initialize(MyEngine::Camera* camera) override;
        void Update(MyEngine::Camera* camera) override;
        //  void UpdateBezierMovement();

         // MyEngine::Vector3 CatmullRom(const MyEngine::Vector3& p0, const MyEngine::Vector3& p1, const MyEngine::Vector3& p2, const MyEngine::Vector3& p3, float t);
        
        void UpdateBezier(MyEngine::Camera* camera);
        void UpdateCameraRotation(MyEngine::Camera* camera);

    private: // メンバ変数
        std::unique_ptr<CurveJsonLoader> Jsondata_ = nullptr;         // ベジェ制御点を読み込むローダー 
        std::vector<std::vector<BezierPoint>> bezierPoints;       // 移動に使う制御点データ
        MyEngine::Vector3 bezierPos_;                          // 現在のベジェ曲線上の位置
          MyEngine::Vector3 prevForward = { 0,0,1 };        // 前フレームの向きベクトル（回転補間用）
        //  float speed = 0.3f;                             // 移動速度

        //  int currentSegment = 0; // レール上の現在のセグメントインデックス
          MyEngine::Vector3 forward_ = { 0,0,1 };
          MyEngine::Vector3 up_ = { 0,1,0 };
          MyEngine::Vector3 right_ = { 1,0,0 };

        //  RailInfo railInfo_;
        //  float currentRailLength_ = 0.0f;
        //  MyEngine::Vector3 prevPos_;
        //  
        ////  RailCameraController rail_;
        //     float t_ = 0.0f;
        //  float deltaTime_;
        //  bool movefige = true;







        int currentSegment_ = 0;   // 今どの区間か
        float t_ = 0.0f;          // 区間内の進行度（0～1）
        float speed_ = 0.01f;     // 進む速さ
        int currentCurve_ = 0;    // どのカーブ使うか

    public:


    };
}