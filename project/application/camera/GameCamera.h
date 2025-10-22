#pragma once
#include <vector>
#include <memory>
#include <Camera.h>
#include <Vector3.h>
#include <CurveJsonLoader.h>

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
    /// <summary>
	/// ベジェ曲線の制御点を設定
    /// 4つの制御点（p0～p3）から、パラメータtに応じたベジェ曲線上の座標を算出する。
    /// <param name="p0">始点（ベジェ曲線の開始位置）</param>
    /// <param name="p1">第1制御点（曲線の始まり側の方向と強さを決める）</param>
    /// <param name="p2">第2制御点（曲線の終わり側の方向と強さを決める）</param>
    /// <param name="p3">終点（ベジェ曲線の終了位置）</param>
    /// <param name="t">補間係数（0.0f ～ 1.0f の範囲）</param>
    /// </summary>
    Vector3 BezierInterpolate(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t);
    /// <summary>
    /// 曲線距離から補間係数tを求める
    /// 累積距離テーブルから指定した距離に対応するt値を取得する。
    /// 曲線上を一定速度で移動させるために使用。
    /// </summary>
    /// <param name="distance">現在の進行距離</param>
    /// <returns>距離に対応するt値（0.0f～1.0f）</returns>
    float GetTForDistance(float distance) const;
    /// <summary> 
    /// 曲線全体の長さを計算
    /// ベジェ曲線を細分割し、各点間の距離を加算して
    /// 全体の長さを算出する。
    /// 累積距離テーブル（curveLengths）も同時に構築。
    /// </summary> 
    void CalculateCurveLength();
    /// <summary> 
    /// カメラ位置をベジェ曲線に沿って更新 
    /// 現在の進行距離に基づきt値を求め、
    /// ベジェ曲線上の座標を計算してカメラ位置を更新する。
    /// </summary>
    void UpdateObjectPosition();
private: // メンバ変数
    CurveJsonLoader* Jsondata = nullptr;         // ベジェ制御点を読み込むローダー
    Camera* camera_ = nullptr;                   // 実際に描画で使用されるカメラインスタンス
    Vector3 bezierPos_;                          // 現在のベジェ曲線上の位置
    bool useFollowCamera_ = false;               // カメラモード切替用フラグ
    std::vector<BezierPoint> bezierPoints;       // JSONから読み込んだベジェ制御点群
    float t = 0.0f;                              // 現在の補間係数
	bool movefige;                               // ベジェ曲線に沿って移動するフラグ     
    int segmentIndex = 0;                        // 今は1つだけと仮定、複数ならループ管理
    bool addedInitialOffset_ = false;            // 現在のベジェ区間インデックス
    std::vector<float> curveLengths;             // 曲線を細分割した累積距離テーブル
    float totalCurveLength = 0.0f;               // 曲線全体の長さ
    float distanceAlongCurve = 0.0f;             // 現在の距離位置
	float speed = 0.4f;                          // 移動の速さ
public: // アクセッサ（Getter / Setter）
    // getter 
	Camera* Getcamera() { return camera_; }
    bool Getmovefige() { return movefige; }
    Vector3 GetbezierPos() { return bezierPos_; } 
    // setter
    void Setmovefige(bool value) { movefige = value; }
    void SetbezierPos(Vector3 pos) { bezierPos_ = pos; }
};