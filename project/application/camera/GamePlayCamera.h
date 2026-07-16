#pragma once
#include <ISceneCameraBehavior.h>
#include <CurveJsonLoader.h>
#include <memory>
#include <Player.h>

namespace MyGame {
    /// <summary>
    /// ゲームプレイシーンのカメラ挙動クラス
    /// </summary>
    class GamePlayCamera : public ISceneCameraBehavior {
    public: // メンバ関数
        /// <summary>
        /// 初期化処理 
        /// </summary>
        /// <param name="camera"></param>
        void Initialize(MyEngine::Camera* camera) override;
        /// <summary>
        /// 更新処理
        /// </summary>
        /// <param name="camera"></param>
        void Update(MyEngine::Camera* camera) override;
        /// <summary>
        /// レールカメラをプレイヤーの相対移動に合わせて更新する処理
        /// </summary>
        /// <param name="camera"></param>
        void UpdateRailCamera(MyEngine::Camera* camera);
        /// <summary>
        /// サブカメラをプレイヤーの相対移動に合わせて更新する処理 
        /// </summary>
        void UpdateSubCamera();
    private: // 内部関数
        /// <summary>
        /// レール更新可能かどうか判定
        /// </summary>
        bool CanUpdateRail() const;
        /// <summary>
        /// レール移動更新
        /// </summary>
        void UpdateRailMovement();
        /// <summary>
        /// レール回転更新
        /// </summary>
        void UpdateRailRotation();
        /// <summary>
        /// レールの未来位置を計算する
        /// </summary>
        /// <param name="points"></param>
        /// <returns></returns>
        MyEngine::Vector3 CalculateFuturePosition(const std::vector<BezierPoint>& points) const;
    private: // メンバ変数
        Player* player_ = nullptr;                               // プレイヤーへの参照ポインタ
        std::unique_ptr<CurveJsonLoader> jsonLoader_ = nullptr;  // ベジェ制御点を読み込むローダー 
        std::unique_ptr<BezierData> bezierdata_;                 // 移動に使う制御点データ    
        /// レール情報
        MyEngine::Vector3 bezierposition_{};                     // 現在のベジェ曲線上の位置
        MyEngine::Vector3 forward_{};                            // 現在の進行方向
        MyEngine::Vector3 previousbezierposition_{};             // 前フレーム位置
        MyEngine::Vector3 previousforward_;                      // 前フレームのレール座標  
        MyEngine::Vector3 railvelocity_{};                       // レール移動速度
        /// レール制御パラメータ
        uint32_t currentsegment_;      // 現在のレールセグメント 
        float speed_;                  // 移動速度
        float lookaheaddistance_;      // 未来を見る距離
        float rotatesmooth_;           // 回転の追従速度
        float pitchInfluence_;         // ピッチ影響率
        float totalrailLength_;        // レール全体の長さ
        bool isfinished_ = false;      // レール終点へ到達したかどうか
    public: // アクセッサ
        // getter
        const MyEngine::Vector3& GetForward() const { return forward_; }
        const MyEngine::Vector3& GetRailPosition() const { return bezierposition_; }
        const bool GetFinished() const { return isfinished_; }
        const MyEngine::Vector3& GetRailVelocity() const { return railvelocity_; }
        float GetProgress() const;
        MyEngine::Vector3 GetRailEndPosition() const {
            // データ未読み込み対策
            if (!bezierdata_ || bezierdata_->points.empty()) { return {}; }
            // 最後の制御点(end)を返す
            return bezierdata_->points.back().controlPoint.controlPoint;
        }
        const std::vector<BezierPoint>& GetRailPoints() const {
            static std::vector<BezierPoint> empty;
            if (!bezierdata_) { return empty; }
            return bezierdata_->points;
        }
        // setter
        void SetPlayer(Player* player) { player_ = player; }
    };
}