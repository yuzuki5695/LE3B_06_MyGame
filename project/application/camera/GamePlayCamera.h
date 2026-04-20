#pragma once
#include <ISceneCameraBehavior.h>
#include <CurveJsonLoader.h>
#include <memory>
#include <Player.h>
#include <RailPath.h>
#include <RailSampler.h>

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
		/// ベジェ曲線に沿ってカメラを移動させる更新処理
        /// </summary>
        /// <param name="camera"></param>
        void UpdateBezier(MyEngine::Camera* camera);

        void SetPlayer(Player* player) {
            player_ = player;
        }

    private: // メンバ変数
		RailPath railPath_;       // 距離計算用
		RailSampler railSampler_; // 座標取得用
        
        std::unique_ptr<CurveJsonLoader> Jsondata_ = nullptr;         // ベジェ制御点を読み込むローダー 
        std::vector<std::vector<BezierPoint>> bezierPoints;       // 移動に使う制御点データ
        MyEngine::Vector3 bezierPos_;                          // 現在のベジェ曲線上の位置
        float speed = 0.3f;                             // 移動速度

        float distance_ = 0.0f;   // 現在の移動距離
        MyEngine::Vector3 forward_{};
        int currentIndex_ = 0;   // 今いる区間（p1）
        float t_ = 0.0f;        // 区間内の進行度 
        Player* player_ = nullptr;


    public:
        MyEngine::Vector3 GetForward() const {
            return forward_;
        }
    };
}