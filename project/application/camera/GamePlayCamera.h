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
       
        void BuildRailInfo(const std::vector<BezierPoint>& curve);
        float CalculateSegmentLength(const std::vector<BezierPoint>& curve, int index);
        MyEngine::Vector3 GetCatmullRomPoint(const std::vector<BezierPoint>& curve, int index, float t);

        MyEngine::Vector3 CatmullRom(const MyEngine::Vector3& p0, const MyEngine::Vector3& p1, const MyEngine::Vector3& p2, const MyEngine::Vector3& p3, float t);

        void UpdateBezier(MyEngine::Camera* camera);
        void ConvertDistanceToSegment(float distance);
        void ConvertDistanceToSegmentInternal(float distance, int& outIndex, float& outT);

    private: // メンバ変数
        std::unique_ptr<CurveJsonLoader> Jsondata_ = nullptr;         // ベジェ制御点を読み込むローダー 
        std::vector<std::vector<BezierPoint>> bezierPoints;       // 移動に使う制御点データ
        MyEngine::Vector3 bezierPos_;                          // 現在のベジェ曲線上の位置
        float speed = 0.3f;                             // 移動速度
        RailInfo railInfo_;
        float distance_ = 0.0f;   // 現在の移動距離

        int currentIndex_ = 0;   // 今いる区間（p1）
        float t_ = 0.0f;        // 区間内の進行度

    public:
        float GetProgress() const override {
            if (railInfo_.totalLength <= 0.0f) return 0.0f;
            return distance_ / railInfo_.totalLength;
        }
    };
}