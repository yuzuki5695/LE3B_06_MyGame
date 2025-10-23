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
    Vector3 LookAtRotation(const Vector3& forward);

private: // メンバ変数
    CurveJsonLoader* Jsondata = nullptr;         // ベジェ制御点を読み込むローダー
    Camera* camera_ = nullptr;                   // 実際に描画で使用されるカメラインスタンス
    Vector3 bezierPos_;                          // 現在のベジェ曲線上の位置
    std::vector<BezierPoint> bezierPoints;       // JSONから読み込んだベジェ制御点群
	bool movefige;                               // ベジェ曲線に沿って移動するフラグ      	
    float speed;                          // 移動の速さ
public: // アクセッサ（Getter / Setter）
    // getter 
	Camera* Getcamera() { return camera_; }
    bool Getmovefige() { return movefige; }
    Vector3 GetbezierPos() { return bezierPos_; } 
    // setter
    void Setmovefige(bool value) { movefige = value; }
    void SetbezierPos(Vector3 pos) { bezierPos_ = pos; }
};