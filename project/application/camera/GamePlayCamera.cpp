#include "GamePlayCamera.h"
#include <CameraManager.h>
#include <MathUtil.h>
#include <MatrixVector.h>
#include <Easing.h>
#include <Player.h>
#include <numbers>
#include <LineRenderer.h>
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

using namespace MyEngine;
using namespace CameraDefs;
using namespace MathUtil;
using namespace MatrixVector;
using namespace Easing;

namespace MyGame {

    ///====================================================
    /// 初期化処理
    ///====================================================
    void GamePlayCamera::Initialize(Camera* camera) {
        // カメラのステートを設定
        statedata_.type = CameraType::Main;
        statedata_.state = CameraState::Default;
        // ベジェカーブのJSONデータ読み込み
        jsonLoader_ = std::make_unique<CurveJsonLoader>();
        // ベジェ制御点の読み込み
        bezierdata_ = jsonLoader_->LoadFile("bezier");
        // サブカメラ生成
        std::unique_ptr<MyEngine::Camera> subCam = std::make_unique<MyEngine::Camera>();
        // サブカメラ登録
        CameraManager::GetInstance()->GetCameraSet().AddSubCamera("Sub", std::move(subCam));

        // メンバ変数の初期化
        speed_ = 0.3f;
        currentsegment_ = 0;
        previousforward_ = { 0,0,1 };
        lookaheaddistance_ = 180.0f;
        rotatesmooth_ = 0.025f;
        pitchInfluence_ = 0.45f;

        // 全制御点を通過可能に
        for (BezierPoint& point : bezierdata_->points) {
            point.passed = true;
        }

        // レール全長を計算
        totalrailLength_ = 0.0f;
        for (size_t i = 0; i + 1 < bezierdata_->points.size(); ++i) {
            const Vector3& p0 = bezierdata_->points[i].controlPoint.controlPoint;
            const Vector3& p1 = bezierdata_->points[i + 1].controlPoint.controlPoint;

            totalrailLength_ += Length(p1 - p0);
        }

        // start地点をセット
        bezierposition_ = bezierdata_->points[0].controlPoint.controlPoint;
        camera->SetTranslate(bezierposition_);
        
        previousbezierposition_ = bezierposition_;
        railvelocity_ = {};
    }
    ///====================================================
    /// 更新処理
    ///====================================================
    void GamePlayCamera::Update(Camera* camera) {
        if (!camera) return;

        switch (statedata_.state) {
        case CameraState::Default: // 待機状態

            break;
        case CameraState::Follow:     // レールカメラの更新
            // レールカメラの更新
            UpdateRailCamera(camera);
            break;
        case CameraState::LockOn: // サブカメラの更新(プレイヤーへロックオン)
            // カメラのステートをサブカメラに切り替え
            statedata_.type = CameraType::Sub;
            CameraManager::GetInstance()->SetActiveSubCamera("Sub");
            // サブカメラの更新
            UpdateSubCamera();
            break;
        }
        ///------------------------------------------
        /// デバッグ描画
        ///------------------------------------------
#ifdef _DEBUG
        if (camera) {
            LineRenderer::GetInstance()->AddRailLine(GetRailPoints(), { 1.0f, 1.0f, 0.0f, 1.0f });
            for (const auto& point : GetRailPoints()) {
                LineRenderer::GetInstance()->AddAABB(point.controlPoint.controlPoint, { 2.0f,2.0f,2.0f }, { 1,0,0,1 });
            }
        }
#endif
    }
    ///====================================================
    /// レールカメラをプレイヤーの相対移動に合わせて更新する処理
    ///====================================================
    void GamePlayCamera::UpdateRailCamera(Camera* camera) {
        // レール更新可能か判定
        if (!CanUpdateRail()) { return; }

        // 移動更新
        UpdateRailMovement();
        
        // レール速度更新
        railvelocity_ =bezierposition_ - previousbezierposition_;
 
        // 回転更新
        UpdateRailRotation();

        // カメラへ反映
        float yaw = atan2f(forward_.x, forward_.z);
        float pitch = -asinf(forward_.y);
        // 上下傾き弱める
        pitch *= pitchInfluence_;

        // カメラの位置と回転を更新
        camera->SetTranslate(bezierposition_);
        camera->SetRotate({ pitch, yaw, 0.0f });
    }
    ///====================================================
    /// レール更新可能かどうか判定
    ///====================================================
    bool GamePlayCamera::CanUpdateRail() const {
        // ベジェデータがない
        if (!bezierdata_) { return false; }
        // 制御点が2つ未満なら処理しない
        if (bezierdata_->points.size() < 2) { return false; }
        // すでにendに到達している
        if (isfinished_) { return false; }
        // それ以外は更新可能
        return true;
    }
    ///==================================================== 
    /// レール移動更新
    ///====================================================
    void GamePlayCamera::UpdateRailMovement() {
        // 現在のセグメントの終点を取得
        const auto& points = bezierdata_->points;
        // 現在のセグメントの終点
        Vector3 end = points[currentsegment_ + 1].controlPoint.controlPoint;
        // 現在位置から終点へのベクトル
        Vector3 dir = end - bezierposition_;
        // 終点までの距離
        float distance = Length(dir);

        // 終点までの距離がspeed以下なら、終点に到達したとみなして次のセグメントへ
        if (distance <= speed_) {
            // 終点に到達
            bezierposition_ = end;
            // 次のセグメントへ
            currentsegment_++;
            // 終点
            if (currentsegment_ >= points.size() - 1) {
                // 最終セグメントに到達
                isfinished_ = true;
            }
        } else {
            // まだ終点に到達していないので、speed分だけ移動
            dir = Normalize(dir);
            bezierposition_ += dir * speed_;
        }
    }
    ///====================================================  
    /// レールの未来位置を計算する
    ///==================================================== 
    Vector3 GamePlayCamera::CalculateFuturePosition(const std::vector<BezierPoint>& points) const {
        // 現在のセグメントの終点を取得
        Vector3 currentEnd = points[currentsegment_ + 1].controlPoint.controlPoint;
        // 未来位置の初期値は現在のセグメントの終点
        Vector3 futurePos = currentEnd;
        // 現在位置から終点までの距離
        float remainDist = Length(currentEnd - bezierposition_);
        // 未来位置の計算
        if (remainDist < lookaheaddistance_ && currentsegment_ + 2 < points.size()) {
            // 次のセグメントの終点
            Vector3 next = points[currentsegment_ + 2].controlPoint.controlPoint;
            // 現在位置から次のセグメントの終点までの距離
            float blend = 1.0f - (remainDist / lookaheaddistance_);
            // ブレンド値を0.0f～1.0fの範囲にクランプ
            blend = std::clamp(blend, 0.0f, 1.0f);
            // ブレンド値をイージングして滑らかにする
            blend = blend * blend * (3.0f - 2.0f * blend);
            // 現在のセグメントの終点と次のセグメントの終点をブレンドして未来位置を計算
            futurePos = currentEnd * (1.0f - blend) + next * blend;
        }
        // 未来位置を返す
        return futurePos;
    }
    ///==================================================== 
    /// レール回転更新
    ///==================================================== 
    void GamePlayCamera::UpdateRailRotation() {
        // ベジェデータから現在のセグメントの制御点を取得
        const auto& points = bezierdata_->points;
        // 目標の前方ベクトルを計算
        Vector3 targetForward = previousforward_;
        // 未来位置を考慮して目標の前方ベクトルを計算
        if (currentsegment_ < points.size() - 1) {
            // 未来位置を計算
            Vector3 futurePos = CalculateFuturePosition(points);
            // 現在位置から未来位置へのベクトルを正規化して目標の前方ベクトルとする
            targetForward = Normalize(futurePos - bezierposition_);
        }
        // 現在の前方ベクトルと目標の前方ベクトルをSlerpで滑らかに補間して新しい前方ベクトルを計算
        forward_ = Normalize(Slerp(previousforward_, targetForward, rotatesmooth_));
        // 現在の前方ベクトルを保存して次回の更新で使用する
        previousforward_ = forward_;
    }
    ///==================================================== 
    /// サブカメラをプレイヤーの相対移動に合わせて更新する処理 
    ///==================================================== 
    void GamePlayCamera::UpdateSubCamera() {
        Camera* subCam = CameraManager::GetInstance()->GetActiveCamera();
        if (!subCam || !player_) return;
        // プレイヤーの位置を取得
        Vector3 targetPos = player_->GetObject3d()->GetTranslate();
        // プレイヤー前方に配置
        Vector3 desiredPos = targetPos + forward_ * 20.0f + Vector3{ 0, 3, 0 };
        // イージング
        float ease = 0.1f;
        Vector3 now = subCam->GetTranslate();
        Vector3 newPos = now + (desiredPos - now) * ease;
        // カメラの位置を更新
        subCam->SetTranslate(newPos);
        // プレイヤーを見る
        Vector3 dir = targetPos - newPos;
        if (Length(dir) > 0.0001f) {
            dir = Normalize(dir);
            float yaw = atan2f(dir.x, dir.z);
            float pitch = -asinf(dir.y);
            subCam->SetRotate({ pitch, yaw, 0.0f });
        }
    }
    ///====================================================
    /// レールの進行率を取得する
    ///====================================================
    float GamePlayCamera::GetProgress() const {
        if (!bezierdata_ || bezierdata_->points.size() < 2 || totalrailLength_ <= 0.0f) {
            return 0.0f;
        }

        const auto& points = bezierdata_->points;
        float traveledDistance = 0.0f;

        // すでに通過したセグメント距離
        for (uint32_t i = 0; i < currentsegment_; ++i) {
            const Vector3& p0 = points[i].controlPoint.controlPoint;
            const Vector3& p1 = points[i + 1].controlPoint.controlPoint;
            traveledDistance += Length(p1 - p0);
        }

        // 現在セグメント内の進行距離
        if (currentsegment_ < points.size() - 1) {
            const Vector3& start = points[currentsegment_].controlPoint.controlPoint;
            traveledDistance += Length(bezierposition_ - start);
        }

        // 0～1に正規化
        return std::clamp(traveledDistance / totalrailLength_, 0.0f, 1.0f);
    }
}