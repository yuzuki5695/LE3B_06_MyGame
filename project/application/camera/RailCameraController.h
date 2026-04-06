//#pragma once
//#include <CurveJsonLoader.h>
//
//namespace MyGame {
//
//    enum class MoveType {
//        Linear,     // 等速
//        EaseInOut   // イージング
//    };
//
//    class RailCameraController {
//    public:
//        void Initialize(const std::vector<BezierPoint>& points);
//        void Update(float deltaTime);
//
//       MyEngine::Vector3 GetPosition() const;
//       MyEngine::Vector3 GetForward() const;
//           
//        MyEngine::Vector3 CatmullRom(const  MyEngine::Vector3& p0, const  MyEngine::Vector3& p1, const  MyEngine::Vector3& p2, const  MyEngine::Vector3& p3, float t);
//
//    private:
//        // パス
//        std::vector<BezierPoint> points_;
//
//        // 状態
//        int currentSegment_ = 0;
//        float t_ = 0.0f;
//        float speed_ = 0.3f;
//        bool moving_ = true;
//
//        // 補間タイプ（セグメントごとに設定可能）
//        std::vector<MoveType> moveTypes_;
//
//        // 出力
//        MyEngine::Vector3 position_;
//        MyEngine::Vector3 prevPosition_;
//        MyEngine::Vector3 forward_;
//    };
//}
