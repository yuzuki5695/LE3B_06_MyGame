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
    private: // 内部関
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
        /// 未来位置取得
        /// </summary>
        MyEngine::Vector3 CalculateFuturePosition(const std::vector<BezierPoint>& points) const;
    private: // メンバ変数
        // プレイヤーへの参照ポインタ（移動の相対座標を取得するために使用）
        Player* player_ = nullptr;
        std::unique_ptr<CurveJsonLoader> Jsondata_ = nullptr;    // ベジェ制御点を読み込むローダー 
        std::unique_ptr<BezierData> bezierdata_;                 // 移動に使う制御点データ    
        MyEngine::Vector3 bezierPos_{};                          // 現在のベジェ曲線上の位置
        MyEngine::Vector3 forward_{};                            //
        MyEngine::Vector3 prevForward_;                          //  
        uint32_t currentSegment_;      // 
        float speed_;                  // 移動速度
        float lookAheadDistance_;      // 未来を見る距離
        float rotateSmooth_;           // 回転の追従速度
        float pitchInfluence_;         // ピッチ影響率
        float totalRailLength_;        // レール全体の長さ
        bool isFinished_ = false;      // 
    public: // アクセッサ
        // getter
        const MyEngine::Vector3& GetForward() const { return forward_; }
        const MyEngine::Vector3& GetRailPosition() const { return bezierPos_; }
        const bool GetFinished() const { return isFinished_; }
        float GetProgress() const;
        // setter
        void SetPlayer(Player* player) { player_ = player; }

        MyEngine::Vector3 GetRailEndPosition() const {
            // データ未読み込み対策
            if (!bezierdata_ || bezierdata_->points.empty()) { return {}; }
            // 最後の制御点(end)を返す
            return bezierdata_->points.back().controlPoint.controlPoint;
        }
    };
}